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
 * @param {!Array.<number>} vector to be converted to string
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
 * @param {!Array.<number>} s the first vector
 * @param {!Array.<number>} t the second vector
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

/**
 * Multiplies a vector by a scalar in-place.
 * @param {!Array.<number>} vector vector modified in place
 * @param {number} scalar the scalar
 */
function vectorTimesScalar(vector, scalar) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] *= scalar;
  }
}

/**
 * Adds two vectors. The vectors must have the same dimension.
 * @param {!Array.<number>} left the left summand
 * @param {!Array.<number>} right the right summand
 * @returns{!Array.<number>}
 */
function vectorPlusVector(left, right) {
  let output = new Array(left.length);
  for (let i = 0; i < left.length; i++) {
    output[i] = left[i] + right[i];
  }
  return output;
}

/**
 * Computes the cross product of two vectors in 3d.
 * @param {!Array.<number>} left the vector on the left
 * @param {!Array.<number>} right the vector on the right
 * @returns {!Array.<number>}
 */
function vectorCrossVector(left, right) {
  return [
    left[1] * right[2] - left[2] * right[1],
    -left[0] * right[2] + left[2] * right[0],
    left[0] * right[1] - left[1] * right[0]
  ];
}

/**
 * Subtracts two vectors: left-right. The vectors must have the same dimension.
 * @param {!Array.<number>} left the minuend
 * @param {!Array.<number>} right the subtrahend
 * @returns{!Array.<number>}
 */
function vectorMinusVector(left, right) {
  let output = new Array(left.length);
  for (let i = 0; i < left.length; i++) {
    output[i] = left[i] - right[i];
  }
  return output;
}

/**
 * Multiplies a vector by a scalar and adds it to an output vector in-place.
 *
 * Formula: output = output + scalar*otherVector.
 *
 * @param {!Array.<number>} output the vector to which we add.
 * @param {!Array.<number>} otherVector the vector we use to modify the output
 * @param {number} scalar the scalar we multiply otherVector by
 */
function vectorAddVectorTimesScalar(output, otherVector, scalar) {
  for (let i = 0; i < output.length; i++) {
    output[i] += otherVector[i] * scalar;
  }
}

/**
 * The length of a vector = sqrt(vector dot product vector).
 *
 * @param {!Array.<number>} vector the input vector
 * @returns{number}
 */
function vectorLength(vector) {
  return Math.sqrt(vectorScalarVector(vector, vector));
}

/**
 * Rounds every coordinate of a vector to the nearest integer in place.
 *
 * @param {!Array.<number>} vector the vector to be modified in place
 */
function vectorRound(vector) {
  for (let i = 0; i < vector.length; i++) {
    vector[i] = Math.round(vector[i]);
  }
}

/**
 * Rescales in place a non-zero vector so it has a unit length.
 *
 * @param {!Array.<number>} vector the vector to be rescaled in place
 */
function vectorNormalize(vector) {
  vectorTimesScalar(vector, 1 / vectorLength(vector));
}

/**
 * Returns true if the vector is the zero vector,
 * false otherwise.
 * @param {!Array.<number>} vector the vector to be rescaled in place
 * @returns {boolean}
 */
function vectorIsZero(
    vector,
) {
  for (let i = 0; i < vector.length; i++) {
    if (vector[i] !== 0) {
      return false;
    }
  }
  return true;
}

/**
 * Enlarges a bounding box in-place to ensure it encloses a given point.
 *
 * @param {!Array.<number>} inputPoint point to be enclosed
 * @param {!Array.<!Array.<number>>} outputBox output bounding box
 */
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

/**
 * Offset the screen coordinates of a DOM element relative to the center of the
 * mathematical coordinate system in pixel coordinates.
 *
 * Returns a pair of (x,y)-coordinates.
 *
 * @param {!HTMLElement} object the DOM element
 * @param {number} cx the x coordinate of the center of the mathematical
 *     coordinate system
 * @param {number} cy the y coordinate of the center of the mathematical
 *     coordinate system
 *
 * @returns{!Array.<number>}
 */
function getXYPositionOfObject(object, cx, cy) {
  let rectangle = object.getBoundingClientRect();
  return [cx - rectangle.left, cy - rectangle.top];
}

/**
 * Converts an RGB number triple to a hex string starting with #.
 * @param {number} r red value.
 * @param {number} g green value.
 * @param {number} b blue value.
 *
 * @returns{string}
 */
function colorRGBToHex(r, g, b) {
  return '#' + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}

/**
 * Converts an RGB number triple to a string of the form rgb(r,g,b).
 *
 * For example, [100,50,50] will be converted to the string "rgb(100,50,50)".
 *
 * @param {!Array<number>} inputRGB an RGB-triple of numbers.
 *
 * @returns {string}
 */
function colorRGBToString(inputRGB) {
  return `rgb(${inputRGB[0].toString()},${inputRGB[1].toString()},${
      inputRGB[2].toString()})`;
}

/**
 * Rescales a color by multiplying its red,green,blue-coordinates by
 * the given scale.
 *
 * Values are rounded down to integers and truncated to the range [0,255].
 *
 * @param {!Array<number>} inputRGB an RGB-triple of numbers.
 * @param {number} scale the scale.
 *
 * @returns {!Array<number>}
 */
function colorScale(inputRGB, scale) {
  let result = [0, 0, 0];
  for (let i = 0; i < inputRGB.length; i++) {
    result[i] = Math.round(inputRGB[i] * scale);
    if (result[i] < 0) {
      result[i] = 0;
    }
    if (result[i] > 255) {
      result[i] = 255;
    }
  }
  return result;
}

/**
 * Extracts an RGB number triple from a color.
 *
 * @param {string|!Array.<number>} input color, named, rgb hex value or RGB
 *     number triple.
 *
 * @returns {!Array.<number>}
 */
function colorToRGB(input) {
  if (typeof input !== 'string') {
    return input;
  }
  let hex = colorToHex(input);
  return [
    parseInt(hex.slice(1, 3), 16),
    parseInt(hex.slice(3, 5), 16),
    parseInt(hex.slice(5, 7), 16),
  ];
}

/**
 * Converts a named color to a rgb hex string prepended with #.
 *
 * If the color name is not recognized, returns the input, unmodified.
 *
 * @param {string} color input named color.
 *
 * @returns {string}
 */
function colorToHex(color) {
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


/**
 * Converts a number or string to a number.
 *
 * @param {number|string} input the number or string to be normalized
 * @returns {number}
 */
function numberFromNumberOfInfinity(input) {
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
 * If the input is a string that indicates +/- infinity, returns a normalized
 * string representation. Otherwise returns the empty string.
 *
 * @param {number|string} input the input number or string.
 *
 * @returns {string}
 */
function infinityType(input) {
  if (typeof input !== 'string') {
    return '';
  }
  let inputString = /** @type{string} */ (input);
  if (inputString.toLowerCase() === 'infinity') {
    return 'infinity';
  }
  if (inputString.toLowerCase() === 'minusinfinity') {
    return 'minusInfinity';
  }
  return '';
}

/** A set of points in two dimensions. */
class PointsTwoD {
  /**
   * @param {!Array.<!Array.<number>>} inputPoints the points.
   * @param {string} inputColor desired point color.
   */
  constructor(
      inputPoints,
      inputColor,
  ) {
    this.location = inputPoints.slice();
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'points';
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    for (let i = 0; i < this.location.length; i++) {
      accountBoundingBox(this.location[i], inputOutputBox);
    }
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    for (let i = 0; i < this.location.length; i++) {
      surface.beginPath();
      surface.strokeStyle = colorRGBToString(this.color);
      surface.fillStyle = colorRGBToString(this.color);
      let coordinates = canvas.coordinatesMathToScreen(this.location[i]);
      surface.arc(coordinates[0], coordinates[1], 3, 0, Math.PI * 2);
      surface.fill();
    }
  }

  /**
   * Same as draw, required by interface.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {
    this.draw(canvas);
  }
}

/**
 * Parametric curve in two dimensions.
 *
 * A parametric curve is a curve in 2d given by:
 *
 * x=f(t)
 * y=g(t)
 *
 * for some functions f and g and some parameter range t\in [a,b].
 */
class CurveTwoD {
  /**
   * @param{!Array.<function(number):number>} inputCoordinateFunctions a pair of
   * coordinate functions.
   * @param{number} inputLeftPt lower bound for the parameter.
   * @param{number} inputRightPt higher bound for the parameter.
   * @param{number} inputNumSegments number of segments used to approximate the
   * curve.
   * @param{string} inputColor the color of the curve.
   * @param{number} inputLineWidth the width of the line.
   */
  constructor(
      inputCoordinateFunctions, inputLeftPt, inputRightPt, inputNumSegments,
      inputColor, inputLineWidth) {
    /** @type{!Array.<!Function>}*/
    this.coordinateFunctions = inputCoordinateFunctions;
    /** @type{number} */
    this.leftPt = inputLeftPt;
    /** @type{number} */
    this.rightPt = inputRightPt;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type{number} */
    this.numSegments = inputNumSegments;
    /** @type{number} */
    this.lineWidth = inputLineWidth;
    /** @type{string} */
    this.type = 'curve';
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
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

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    let surface = canvas.surface;
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    let coordinates = canvas.coordinatesMathToScreen([x, y]);
    let alreadyMoved = false;
    if (startByMoving) {
      alreadyMoved = true;
    }
    surface.lineWidth = this.lineWidth;
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
          console.log(`Curve point: [${x}, ${
              y}] is too large, skipping. Further errors suppressed.`);
        }
        skippedValues = true;
        continue;
      }
      coordinates = canvas.coordinatesMathToScreen([x, y]);
      if (!alreadyMoved) {
        alreadyMoved = true;
        surface.moveTo(coordinates[0], coordinates[1]);
      } else {
        surface.lineTo(coordinates[0], coordinates[1]);
      }
    }
  }
}

/**
 * A path in two dimenions.
 *
 * Here, "path" means a sequence of points connected by straight segments.
 */
class PathTwoD {
  /**
   * @param{!Array.<!Array.<number>>} inputPath a list of points in 2d.
   * @param{string} inputColor the color of the path.
   * @param{string} inputFillColor fill color of the path, used when isFilled is
   * set.
   * @param{number} inputLineWidth line width.
   */
  constructor(inputPath, inputColor, inputFillColor, inputLineWidth) {
    /** @type{!Array.<!Array.<number>>} */
    this.path = inputPath;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type{!Array.<number>} */
    this.colorFill = colorToRGB(inputFillColor);
    this.isFilled = false;
    /** @type{string} */
    this.type = 'path';
    /** @type{number} */
    this.lineWidth = inputLineWidth;
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    for (let i = 0; i < this.path.length; i++) {
      accountBoundingBox(this.path[i], inputOutputBox);
    }
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    if (this.path.length < 1) {
      return;
    }
    let surface = canvas.surface;
    let coordinates = canvas.coordinatesMathToScreen(this.path[0]);
    if (startByMoving) {
      surface.moveTo(coordinates[0], coordinates[1]);
    } else {
      surface.lineTo(coordinates[0], coordinates[1]);
    }
    surface.lineWidth = this.lineWidth;
    for (let i = 1; i < this.path.length; i++) {
      coordinates = canvas.coordinatesMathToScreen(this.path[i]);
      surface.lineTo(coordinates[0], coordinates[1]);
    }
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.colorFill);
    if (this.isFilled) {
      surface.fill();
    }
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    if (this.path.length < 1) {
      return;
    }
    let surface = canvas.surface;
    surface.beginPath();
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }
}

class CoordinateAxesTwoD {
  /**
   * @param {string} inputXAxisLabel string label of the x axis.
   * @param {string} inputYAxisLabel string label of the y axis.
   */
  constructor(inputXAxisLabel, inputYAxisLabel) {
    /** @type{string} */
    this.xAxisLabel = inputXAxisLabel;
    /** @type{string} */
    this.yAxisLabel = inputYAxisLabel;
    /** @type{string} */
    this.type = 'axis';
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    this.drawNoFinish(canvas, true);
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    accountBoundingBox([0, 1], inputOutputBox);
    accountBoundingBox([1, 0], inputOutputBox);
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    let lowLeft = canvas.coordsScreenToMathScreen([0, canvas.height]);
    let topRight = canvas.coordsScreenToMathScreen([canvas.width, 0]);
    let surface = canvas.surface;
    surface.strokeStyle = colorRGBToString([0, 0, 0]);
    surface.fillStyle = colorRGBToString([0, 0, 0]);
    surface.lineWidth = 1;
    let left = lowLeft[0];
    let right = topRight[0];
    let bottom = lowLeft[1];
    let top = topRight[1];
    surface.beginPath();
    let coordinates = canvas.coordsMathScreenToScreen([0, bottom]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = canvas.coordsMathScreenToScreen([0, top]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    surface.fillText(this.yAxisLabel, coordinates[0] - 10, coordinates[1] + 10);

    coordinates = canvas.coordsMathScreenToScreen([left, 0]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = canvas.coordsMathScreenToScreen([right, 0]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    surface.fillText(this.xAxisLabel, coordinates[0] - 10, coordinates[1] + 10);
    coordinates = canvas.coordsMathScreenToScreen([1, -0.1]);
    surface.moveTo(coordinates[0], coordinates[1]);
    coordinates = canvas.coordsMathScreenToScreen([1, 0.1]);
    surface.lineTo(coordinates[0], coordinates[1]);
    surface.stroke();
    coordinates = canvas.coordsMathScreenToScreen([1, 0]);
    surface.fillText('1', coordinates[0], coordinates[1] + 10);
  }
}

/** Grid lines of the coordinate system. Excludes the coordinate axes. */
class AxesGrid {
  constructor() {
    this.type = 'grid';
  }

  /**
   * Used to satisfy interface requirements
   *
   * @param {!Array.<!Array.<number>>} unused output bounding box.
   */
  accountBoundingBox(unused) {}

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    this.drawNoFinish(canvas, true);
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {
    let lowLeft = canvas.coordsScreenToMathScreen([0, canvas.height]);
    let topRight = canvas.coordsScreenToMathScreen([canvas.width, 0]);
    let surface = canvas.surface;
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
            (ceilRight - floorLeft) / (canvas.width / distanceBetweenMarks)));
    let DeltaVertical = Math.max(
        1,
        Math.floor(
            (ceilTop - floorBottom) / (canvas.height / distanceBetweenMarks)));
    let Delta = Math.max(DeltaHorizontal, DeltaVertical);
    surface.beginPath();
    for (let i = floorLeft; i <= ceilRight; i += Delta) {
      let coordinates = canvas.coordsMathScreenToScreen([i, bottom]);
      surface.moveTo(coordinates[0], coordinates[1]);
      coordinates = canvas.coordsMathScreenToScreen([i, top]);
      surface.lineTo(coordinates[0], coordinates[1]);
      surface.stroke();
    }
    for (let i = floorBottom; i <= ceilTop; i += Delta) {
      let coordinates = canvas.coordsMathScreenToScreen([left, i]);
      surface.moveTo(coordinates[0], coordinates[1]);
      coordinates = canvas.coordsMathScreenToScreen([right, i]);
      surface.lineTo(coordinates[0], coordinates[1]);
      surface.stroke();
    }
    surface.strokeStyle = colorRGBToString([0, 0, 0]);
    surface.fillStyle = colorRGBToString([0, 0, 0]);
    let counter = 0;
    for (let i = floorLeft; i <= ceilRight; i += Delta) {
      counter++;
      let coordinates = canvas.coordsMathScreenToScreen([i, 0]);
      surface.fillText(`${i}`, coordinates[0], coordinates[1] + 10);
    }
    counter = 0;
    for (let i = floorBottom; i <= ceilTop; i += Delta) {
      counter++;
      let coordinates = canvas.coordsMathScreenToScreen([0, i]);
      surface.fillText(`${i}`, coordinates[0] - 10, coordinates[1]);
    }
  }
}

/** Plot of a function in one variable. */
class PlotTwoD {
  /**
   * @param {function(number):number} inputFunction function to be plotted.
   * @param {number|string} leftPoint low x-coordinate or the string
   *     'minusInfinity'.
   * @param {number|string} rightPoint high x-coordinate or the string
   *     'infinity'.
   * @param {number} numberOfSegments number of segments to approximate the
   *     graph.
   * @param {string} inputColor color of the graph.
   * @param {number} inputLineWidth line width of the graph.
   */
  constructor(
      inputFunction, leftPoint, rightPoint, numberOfSegments, inputColor,
      inputLineWidth) {
    this.functionPlotted = inputFunction;
    /** @type{number} */
    this.leftPoint = numberFromNumberOfInfinity(leftPoint);
    /** @type{string} */
    this.leftInfinityType = infinityType(leftPoint);
    /** @type{number} */
    this.rightPoint = numberFromNumberOfInfinity(rightPoint);
    /** @type{string} */
    this.rightInfinityType = infinityType(rightPoint);
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    if (inputLineWidth === undefined) {
      this.lineWidth = 1;
    } else {
      this.lineWidth = inputLineWidth;
    }
    this.type = 'plotFunction';
    this.numberOfSegments = numberOfSegments;
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
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

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    let surface = canvas.surface;
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    let realLeft = this.leftPoint;
    let realRight = this.rightPoint;
    if (this.leftInfinityType === 'minusInfinity') {
      let lowLeft = canvas.coordsScreenToMathScreen([0, canvas.height]);
      realLeft = lowLeft[0];
    }
    if (this.rightInfinityType === 'infinity') {
      let topRight = canvas.coordsScreenToMathScreen([canvas.width, 0]);
      realRight = topRight[0];
    }
    let x = realLeft;
    let y = this.functionPlotted(x);
    let coordinates = canvas.coordinatesMathToScreen([x, y]);
    let alreadyMoved = false;
    if (startByMoving) {
      alreadyMoved = true;
    }
    surface.lineWidth = this.lineWidth;
    let skippedValues = false;
    for (let i = 0; i < this.numberOfSegments; i++) {
      let ratio = i / (this.numberOfSegments - 1);
      x = realLeft * (1 - ratio) + realRight * ratio;  //<- this way of
      // computing x this way introduces smaller numerical errors.
      // For example, suppose you plot sqrt(1-x^2) from - 1 to 1.
      // If not careful with rounding errors,
      // you may end up evaluating sqrt(1-x^2) for x =1.00000000000004
      // resulting in serious visual glitches.
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
      coordinates = canvas.coordinatesMathToScreen([x, y]);
      if (!alreadyMoved) {
        alreadyMoved = true;
        surface.moveTo(coordinates[0], coordinates[1]);
      } else {
        surface.lineTo(coordinates[0], coordinates[1]);
      }
    }
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }
}

/** A text label in two dimensions. */
class TextPlotTwoD {
  /**
   * @param {!Array.<number>} inputLocation location of the text.
   * @param {string} inputText text to draw.
   * @param {string} inputColor text color.
   */
  constructor(inputLocation, inputText, inputColor) {
    /** @type{!Array.<number>} */
    this.location = inputLocation;
    /** @type{string} */
    this.text = inputText;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'plotText';
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.location, inputOutputBox);
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line. Ignored.
   */
  drawNoFinish(canvas, startByMoving) {
    let surface = canvas.surface;
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    let coordinates = canvas.coordinatesMathToScreen(this.location);
    surface.fillText(this.text, coordinates[0], coordinates[1]);
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }
}

class EscapeMap { 
  /**
   * @param {!function(number, number):number} functionX.
   * @param {!function(number, number):number} functionY.
   */
  constructor(functionX, functionY) {
    this.functionX = functionX;
    this.functionY = functionY; 
    this.lastImageData = null;
    this.ignoreNextComputation = true;
    // The i^th position in this array gives the color
    // of a pixel that takes 33 - i steps to escape to infinity.
    // In other words, points with colors appearing earlier
    // take longer to escape to infinity.
    this.colorMap = [
      [31, 31, 60],
      [31, 31, 90],
      [31, 31, 120],
      [31, 31, 150],
      [31, 31, 180],
      [31, 31, 210],
      [31, 31, 219],
      [31, 31, 228],
      [31, 31, 237],
      [31, 31, 246],
      [31, 31, 255],
      //
      [31, 60, 31],
      [31, 90, 31],
      [31, 120, 31],
      [31, 150, 31],
      [31, 180, 31],
      [31, 210, 31],
      [31, 219, 31],
      [31, 228, 31],
      [31, 237, 31],
      [31, 246, 31],
      [31, 255, 31],
      //
      [60,  255, 255],
      [90,  255, 255],
      [120, 255, 255],
      [150, 255, 255],
      [180, 255, 255],
      [210, 255, 255],
      [219, 255, 255],
      [228, 255, 255],
      [237, 255, 255],
      [246, 255, 255],
      [255, 255, 255],
    ];
  }

  /** 
   * Required to satisfy interface.
   * @param {!Array.<!Array.<number>>} unused output bounding box.
   */
  accountBoundingBox(unused) { }
  
  /**
   * Same as draw but required to satisfy interface.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {
    this.draw(canvas);
  }

  /** 
   * Returns the number of iterations of a map 
   * 
   * f:{(x,y)} \to {(x,y)} 
   * 
   * needed to map a starting point (x,y) to 
   * infinity (maximum 32), or 0 if the point does not escape 
   * to infinity in 32 steps.
   * 
   * Here, map iteration is meant in the mathematical sense.
   * For example, iterating f 4 times amounts to computing:
   * 
   * f^4(x,y) = f(f(f(f(x,y))))
   *  
   * We declare a point to have escaped to infinity after n iterations 
   * if the magnitude of |f^{n}(x,y)|^2 is 50 times larger than the magniute |x,y|^2 
   * and the magnitue |x,y|^2 is larger than 50.
   * 
   * @param {number} x
   * @param {number} y
   */
  iterateMap(x0, y0) {
    let x = x0;
    let y = y0;
    for (let i = 1; i < 34; i ++) {
      let newX = this.functionX(x, y);
      let newY = this.functionY(x, y);
      x = newX;
      y = newY;
      let deltaX = x - x0;
      let deltaY = y - y0;
      let rho = deltaX * deltaX + deltaY * deltaY; 
      if (isNaN(rho)) {
        return i;
      }
      if (rho > 10000) {
        return i;
      }
    }
    return 0;
  }
  
  /**
   * Converts a number from 0 to 33 to a color.
   * 
   * The number records the number of steps required for a point
   * to escape to infinity or 0 if the point doesn't escape.
   * 
   * @param {number} input a number from 0 to 33.
   */
  mapEscapeCountToColor(input) {
    if (input == 0) {
      return [31, 31, 31];
    }
    return this.colorMap[33 - input];
  }

  /** 
   * Computes an escape color triple.
   * @param {number} x
   * @param {number} y
   */
  escapeColor(x, y) {
    let escapeSteps = this.iterateMap(x, y);
    return this.mapEscapeCountToColor(escapeSteps);
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    if (this.ignoreNextComputation > 0 && this.lastImageData !== null) {
      this.ignoreNextComputation = false;
      surface.putImageData(this.lastImageData, 0, 0);
      return;
    }
    this.lastImageData = surface.createImageData(canvas.width, canvas.height);
    for (let j = 0; j < canvas.width; j++) {
      for (let i = 0; i < canvas.height; i++) {
        let coordinates = canvas.coordsScreenToMathScreen([j, i]);
        coordinates = canvas.coordinatesMathScreenToMath(coordinates);
        let escapeColor = this.escapeColor(coordinates[0], coordinates[1]);
        this.lastImageData.data[i * canvas.width * 4 + j * 4 + 0] = escapeColor[0];
        this.lastImageData.data[i * canvas.width * 4 + j * 4 + 1] = escapeColor[1];
        this.lastImageData.data[i * canvas.width * 4 + j * 4 + 2] = escapeColor[2];
        this.lastImageData.data[i * canvas.width * 4 + j * 4 + 3] = 255;
      }
    }
    surface.putImageData(this.lastImageData, 0, 0);
  }

  /**
   * Plots the orbit of a single point. 
   * The input is in screen coordinates.
   * 
   * @param {CanvasTwoD} canvas
   * @param {number} x
   * @param {number} y
   */
  mouseMove(canvas, x, y) {
    if (this.lastImageData === null) {
      return;
    }
    this.ignoreNextComputation = true;
    canvas.redraw();
    let surface = canvas.surface;
    let coordinates = canvas.coordsScreenAbsoluteToMathScreen(x, y);
    coordinates = canvas.coordinatesMathScreenToMath(coordinates);
    let xPrevious = coordinates[0];
    let yPrevious = coordinates[1];
    let points = [canvas.coordinatesMathToScreen([xPrevious, yPrevious])];
    for (let i = 0; i < 32; i++) {
      let xNext = this.functionX(xPrevious, yPrevious);
      let yNext = this.functionY(xPrevious, yPrevious);
      xPrevious = xNext;
      yPrevious = yNext;
      let coordinates = canvas.coordinatesMathToScreen([xPrevious, yPrevious]);
      if (isNaN(coordinates[0]) || isNaN(coordinates[1])) {
        break;
      }
      if (Math.abs(coordinates[0]) > 1000 || Math.abs(coordinates[1]) > 1000) {
        break;
      } 
      points.push(coordinates);
    } 
    surface.beginPath();
    surface.strokeStyle = "red";
    surface.lineWidth = 3;
    surface.moveTo(points[0][0], points[0][1]);
    for (let i = 1; i < points.length; i++) {
      surface.lineTo(points[i][0], points[i][1]);
    }
    surface.stroke();
    surface.strokeStyle = "green";
    for (let i = 0; i < points.length; i++) {
      surface.beginPath();
      surface.arc(points[i][0], points[i][1], 3, 0, Math.PI * 2);
      surface.stroke();
    }
  }
}

/**
 * A plot of a vector field.
 *
 * A vector field is a map assigns a vectors to points in the plane. A vector
 * field is given by a pair of functions:
 *
 * [f(x,y), g(x,y)].
 *
 * This is visualized by selecting a "grid" of base points and drawing the
 * vector attached to each base point.
 */
class VectorFieldTwoD {
  /**
   * @param {!Function} inputField the vector field.
   * @param {boolean} inputIsDirectionField when set indicates that each vector
   *     must be drawn with equal length.
   * @param {!Array.<number>} inputLowLeft [x,y]-coordinate of the low left
   *     coordinate of the rectangle over which we draw the vector field.
   * @param {!Array.<number>} inputHighRight [x,y]-coordinate of the high right
   *     coordinate of the rectangle over which we draw the vector field.
   * @param {!Array.<number>} inputNumSegmentsXY a pair of numbers. The first
   *     number gives the number of x-coordinates of the sample points less one
   * the second number gives the number of y coordinates less one.
   * @param {number} inputDesiredLengthDirectionVectors lengths of the direction
   *     vectors; used when inputIsDirectionField is set.
   * @param {string} inputColor color of the vectors.
   * @param {number} inputLineWidth line width.
   */
  constructor(
      inputField,
      inputIsDirectionField,
      inputLowLeft,
      inputHighRight,
      inputNumSegmentsXY,
      inputDesiredLengthDirectionVectors,
      inputColor,
      inputLineWidth,
  ) {
    /** @type{!Function} */
    this.theField = inputField;
    /** @type{boolean} */
    this.isDirectionField = inputIsDirectionField;
    /** @type{!Array.<number>} */
    this.lowLeft = inputLowLeft;
    /** @type{!Array.<number>} */
    this.highRight = inputHighRight;
    this.numSegmentsXY = inputNumSegmentsXY;
    /** @type{number} */
    this.desiredLengthDirectionVectors = inputDesiredLengthDirectionVectors;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.lineWidth = inputLineWidth;
    this.type = 'vectorField';
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.lowLeft, inputOutputBox);
    accountBoundingBox(this.highRight, inputOutputBox);
  }

  /**
   * Same as draw but required to satisfy interface.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {
    this.draw(canvas);
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    surface.lineWidth = this.lineWidth;
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
        let headScreen = canvas.coordinatesMathToScreen(headMath);
        let tailScreen = canvas.coordinatesMathToScreen(tailMath);
        surface.moveTo(tailScreen[0], tailScreen[1]);
        surface.lineTo(headScreen[0], headScreen[1]);
        surface.stroke();
      }
    }
  }
}

/** A segment in two dimensions. */
class SegmentTwoD {
  /**
   * @param {!Array.<number>} inputLeftPt [x,y]-coordinates of the first point.
   * @param {!Array.<number>} inputRightPt [x,y]-coordinates of the second
   *     point.
   * @param {string} inputColor color of the segment.
   * @param {number} inputLineWidth line width.
   */
  constructor(inputLeftPt, inputRightPt, inputColor, inputLineWidth) {
    /** @type{!Array.<number>} */
    this.leftPt = inputLeftPt;
    /** @type{!Array.<number>} */
    this.rightPt = inputRightPt;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'segment';
    this.lineWidth = inputLineWidth;
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
  accountBoundingBox(inputOutputBox) {
    accountBoundingBox(this.leftPt, inputOutputBox);
    accountBoundingBox(this.rightPt, inputOutputBox);
  }

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    let surface = canvas.surface;
    let coordinates = canvas.coordinatesMathToScreen(this.leftPt);
    surface.lineWidth = this.lineWidth;
    if (startByMoving) {
      surface.moveTo(coordinates[0], coordinates[1]);
    } else {
      surface.lineTo(coordinates[0], coordinates[1]);
    }
    coordinates = canvas.coordinatesMathToScreen(this.rightPt);
    surface.lineTo(coordinates[0], coordinates[1]);
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }
}

/**
 * An object that indicates the start of a plot fill. All objects between this
 * one and the first occurence of the next 'plotFillFinish' object will be
 * plotted and filled.
 */
class PlotFillTwoD {
  /**
   * @param{!CanvasTwoD} inputCanvas the owner canvas.
   * @param{string} inputColor color of the plot fill.
   */
  constructor(inputCanvas, inputColor) {
    this.indexFillStart = inputCanvas.drawObjects.length;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    this.type = 'plotFillStart';
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} unused output bounding box.
   */
  accountBoundingBox(unused) {}

  /**
   * Same as draw but used to satisfy an interface.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {
    this.draw(canvas);
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    let theObs = canvas.drawObjects;
    let tempCounter = canvas.numDrawnObjects;
    surface.beginPath();
    let firstRun = true;
    for (tempCounter++; tempCounter < theObs.length; tempCounter++) {
      let currentO = theObs[tempCounter];
      if (currentO.type === 'plotFillFinish') {
        break;
      }
      currentO.drawNoFinish(canvas, firstRun);
      firstRun = false;
    }
    surface.fillStyle = colorRGBToString(this.color);
    surface.fill();
    for (canvas.numDrawnObjects++; canvas.numDrawnObjects < theObs.length;
         canvas.numDrawnObjects++) {
      let currentO = theObs[canvas.numDrawnObjects];
      if (currentO.type === 'plotFillFinish') {
        break;
      }
      currentO.draw(canvas);
    }
  }
}

/** An object that indicates the end of a plot fill. */
class PlotFillFinishTwoD {
  constructor() {
    this.type = 'plotFillFinish';
  }

  /**
   * Unused.
   * @param {!Array.<!Array.<number>>} unused used.
   */
  accountBoundingBox(unused) {}

  /**
   * Unused.
   * @param {!CanvasTwoD} canvas unused.
   * @param {boolean} unused unused.
   */
  drawNoFinish(canvas, unused) {}

  /**
   * Unused.
   * @param {!CanvasTwoD} canvas unused.
   */
  draw(canvas) {}
}

/**
 * @typedef {{ strokeStyle: string, fillStyle:string, moveTo:
 * function(number, number), lineTo: function(number, number), fillText:
 * function(string, number, number), beginPath: !Function, arc:
 * function(number, number, number, number, number), fill: !Function, stroke:
 * !Function, lineWidth: number, clearRect: function(number, number,
 * number, number)}}
 */
let Canvas2DContext;

/**
 * A two dimensional plot.
 *
 * A collection of 2d objects plotted on a 2d canvas in the web browser.
 */
class CanvasTwoD {
  /**
   * @param {!HTMLCanvasElement} inputCanvas the canvas on which to plot.
   * @param {?HTMLElement} controls generate input controls here.
   * @param {?HTMLElement|null} messages generae debug messages here.
   */
  constructor(inputCanvas, controls, messages) {
    /**
     * @type {!Array.<{
     *   draw: function(!CanvasTwoD),
     *   drawNoFinish: function(!CanvasTwoD, boolean),
     *   accountBoundingBox:function(!Array.<!Array.<number>>),
     *   type: string
     */
    this.drawObjects = [];

    /** @type {?Canvas2DContext} */
    this.surface = null;
    this.canvasContainer = inputCanvas;

    this.canvasId = null;
    this.screenBasisOrthonormal = [];
    /** @type{?HTMLElement} */
    this.spanMessages = messages;
    if (this.spanMessages === undefined) {
      this.spanMessages = null;
    }
    /** @type{?HTMLElement} */
    this.spanControls = controls;
    if (this.spanControls === undefined) {
      this.spanControls = null;
    }
    /** @type{number} */
    this.numDrawnObjects = 0;
    this.boundingBoxMath = [[-0.1, -0.1], [0.1, 0.1]];
    /** @type{number} */
    this.width = inputCanvas.width;
    /** @type{number} */
    this.height = inputCanvas.height;
    this.centerCanvasX = inputCanvas.width / 2;
    this.centerCanvasY = inputCanvas.height / 2;
    this.centerX = this.centerCanvasX;
    this.centerY = this.centerCanvasY;
    this.viewWindowDefault = [[-5, -5], [5, 5]];
    /** @type{number} */
    this.scale = 50;
    /** @type{!Array.<number>} */
    this.mousePosition = [];
    /** @type{!Array.<number>} */
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
    /**@type{!Array.<function(number, number)>} */
    this.additionalMouseMoveListeners = [];
  }

  /**
   * Draws points on the canvas.
   *
   * @param {!Array.<!Array.<number>>} inputPoints the points to plot.
   * @param {string} inputColor input color.
   */
  drawPoints(inputPoints, inputColor) {
    this.drawObjects.push(new PointsTwoD(inputPoints, inputColor));
  }

  /**
   * Draws a segment on the canvas.
   *
   * @param {!Array.<number>} inputLeftPt first endpoint.
   * @param {!Array.<number>} inputRightPt second endpoint.
   * @param {string} inputColor input color.
   * @param {number} inputLineWidth line width.
   */
  drawLine(inputLeftPt, inputRightPt, inputColor, inputLineWidth) {
    let newLine =
        new SegmentTwoD(inputLeftPt, inputRightPt, inputColor, inputLineWidth);
    this.drawObjects.push(newLine);
  }

  /** Adds a grid to the plot. */
  drawGrid() {
    this.flagShowAxesTicks = true;
    this.flagShowGrid = true;
    this.drawObjects.push(new AxesGrid());
  }

  /**
   * Adds a coordinate axes to the plot.
   * @param {string|undefined} xAxisLabel label of the x axis.
   * @param {string|undefined} yAxisLabel label of the y axis.
   */
  drawCoordinateAxes(xAxisLabel, yAxisLabel) {
    if (xAxisLabel === undefined) {
      xAxisLabel = '';
    }
    if (yAxisLabel === undefined) {
      yAxisLabel = '';
    }
    this.drawObjects.push(new CoordinateAxesTwoD(xAxisLabel, yAxisLabel));
  }

  /**
   * Draws a vector field on the canvas. See VectorFieldTwoD.
   *
   * @param {!Function} inputField the vector field.
   * @param {boolean} inputIsDirectionField when set indicates that each vector
   *     must be drawn with equal length.
   * @param {!Array.<number>} inputLowLeft [x,y]-coordinate of the low left
   *     coordinate of the rectangle over which we draw the vector field.
   * @param {!Array.<number>} inputHighRight [x,y]-coordinate of the high right
   *     coordinate of the rectangle over which we draw the vector field.
   * @param {!Array.<number>} inputNumSegmentsXY a pair of numbers. The first
   *     number gives the number of x-coordinates of the sample points less one
   * the second number gives the number of y coordinates less one.
   * @param {number} inputDesiredLengthDirectionVectors lengths of the direction
   *     vectors; used when inputIsDirectionField is set.
   * @param {string} inputColor color of the vectors.
   * @param {number} inputLineWidth line width.
   */
  drawVectorField(
      inputField,
      inputIsDirectionField,
      inputLowLeft,
      inputHighRight,
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
    this.drawObjects.push(newLine);
  }

  /**
   * Draws a path.
   *
   * @param{!Array.<!Array.<number>>} inputPath a list of points in 2d.
   * @param{string} inputColor the color of the path.
   * @param{number} inputLineWidth line width.
   */
  drawPath(inputPath, inputColor, inputLineWidth) {
    let newPath =
        new PathTwoD(inputPath, inputColor, inputColor, inputLineWidth);
    this.drawObjects.push(newPath);
  }

  /**
   * Draws a filled path.
   *
   * @param{!Array.<!Array.<number>>} inputPath a list of points in 2d.
   * @param{string} inputContourColor the color of the path.
   * @param{string} inputFillColor the fill color.
   */
  drawPathFilled(inputPath, inputContourColor, inputFillColor) {
    let newPath = new PathTwoD(inputPath, inputContourColor, inputFillColor, 1);
    newPath.isFilled = true;
    this.drawObjects.push(newPath);
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
   */
  drawFunction(
      toBePlotted, leftPoint, rightPoint, inputNumSegments, inputColor,
      inputLineWidth) {
    let newPlot = new PlotTwoD(
        toBePlotted, leftPoint, rightPoint, inputNumSegments, inputColor,
        inputLineWidth);
    this.drawObjects.push(newPlot);
  }

  /**
   * Plots an escape map [Julia set].
   *
   * @param{function(number, number):number} functionX the x-coordinate of the 
   * map we are iterating
   * @param{function(number, number):number} functionY the y-coordinate of the 
   * map we are iterating
   */
  drawEscapeMap(functionX, functionY) {
    let newPlot = new EscapeMap(functionX, functionY);
    this.drawObjects.push(newPlot);
    this.additionalMouseMoveListeners.push((x,y) => {
      newPlot.mouseMove(this, x, y);
    });
  }

  /**
   * Draws a parametric curve on the canvas
   * @param{!Array.<function(number):number>} inputCoordinateFunctions a pair
   * of coordinate functions.
   * @param{number} inputLeftPt lower bound for the parameter.
   * @param{number} inputRightPt higher bound for the parameter.
   * @param{number} inputNumSegments number of segments used to approximate the
   * curve.
   * @param{string} inputColor the color of the curve.
   * @param{number} inputLineWidth the width of the line.
   */
  drawCurve(
      inputCoordinateFunctions, inputLeftPt, inputRightPt, inputNumSegments,
      inputColor, inputLineWidth) {
    let newPlot = new CurveTwoD(
        inputCoordinateFunctions, inputLeftPt, inputRightPt, inputNumSegments,
        inputColor, inputLineWidth);
    this.drawObjects.push(newPlot);
  }

  /**
   * Draws a text on the canvas.
   *
   * @param {!Array.<number>} inputLocation location of the text.
   * @param {string} inputText text to draw.
   * @param {string} inputColor text color.
   */
  drawText(inputLocation, inputText, inputColor) {
    let newPlot = new TextPlotTwoD(inputLocation, inputText, inputColor);
    this.drawObjects.push(newPlot);
  }

  /**
   * Starts a plot fill sequence. All objects drawn after this command and
   * before the next plotFillStart() command will be filled.
   *
   * @param {string} inputColor fill color.
   */
  plotFillStart(inputColor) {
    this.drawObjects.push(new PlotFillTwoD(this, inputColor));
  }

  /** Ends a plot fill sequence. Newly drawn objecs will no longer be filled. */
  plotFillFinish() {
    this.drawObjects.push(new PlotFillFinishTwoD());
  }

  /** Computes a view window using the bounding boxes of the various objects. */
  computeViewWindow() {
    this.boundingBoxMath = [[-0.1, -0.1], [0.1, 0.1]];
    for (let i = 0; i < this.drawObjects.length; i++) {
      if (this.drawObjects[i].accountBoundingBox === undefined) {
        continue;
      }
      this.drawObjects[i].accountBoundingBox(this.boundingBoxMath);
    }
    vectorTimesScalar(this.boundingBoxMath[0], 1.05);
    vectorTimesScalar(this.boundingBoxMath[1], 1.05);
    this.setViewWindow(this.boundingBoxMath[0], this.boundingBoxMath[1]);
  };

  /**
   * Sets a custom view window.
   *
   * @param {!Array.<number>} leftLowPt low left corner of the view window.
   * @param {!Array.<number>} rightUpPt right up corner of the view window.
   */
  setViewWindow(leftLowPt, rightUpPt) {
    this.viewWindowDefault = [leftLowPt, rightUpPt];
    let leftLowScreen = this.coordinatesMathToScreen(leftLowPt);
    let rightUpScreen = this.coordinatesMathToScreen(rightUpPt);
    let desiredHeight = Math.abs(rightUpScreen[1] - leftLowScreen[1]);
    let desiredWidth = Math.abs(rightUpScreen[0] - leftLowScreen[0]);
    let candidateScaleHeight = this.scale * this.height / desiredHeight;
    let candidateScaleWidth = this.scale * this.width / desiredWidth;
    this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
    let centerViewWindowMath = vectorPlusVector(leftLowPt, rightUpPt);
    vectorTimesScalar(centerViewWindowMath, 0.5);
    let centerViewWindowScreen =
        this.coordinatesMathToScreen(centerViewWindowMath);
    this.centerX += this.centerCanvasX - centerViewWindowScreen[0];
    this.centerY += this.centerCanvasY - centerViewWindowScreen[1];
  }

  /** Clears the graphics. */
  clear() { 
    this.drawObjects = [];
    this.additionalMouseMoveListeners = [];
  }

  /** Redraws the entire two dimensional plot. */
  redraw() {
    this.textPerformance = '';
    this.redrawStart = new Date().getTime();
    let surface = this.surface;
    surface.clearRect(0, 0, this.width, this.height);
    for (this.numDrawnObjects = 0;
         this.numDrawnObjects < this.drawObjects.length;
         this.numDrawnObjects++) {
      this.drawObjects[this.numDrawnObjects].draw(this);
    }
    let redrawTime = new Date().getTime();
    if (this.flagShowPerformance) {
      this.textPerformance =
          'Redraw time (ms): ' + (redrawTime - this.redrawStart);
      this.showMessages();
    }
  }

  /** Initializes a mathematical basis for the coordinate system. */
  computeBasis() {
    this.screenBasisOrthonormal[0] = [1, 0];
    this.screenBasisOrthonormal[1] = [0, 1];
    this.textProjectionInfo = '';
    this.textProjectionInfo +=
        `<br>e1: ${this.screenBasisOrthonormal[0]} <br>e2: ${
            this.screenBasisOrthonormal[1]}`;
  }

  /**
   * Rescales the canvas on a mouse wheel event.
   *
   * @param {{preventDefault: !Function, stopPropagation: !Function, detail:
   *     (number|null|undefined), wheelDelta: number, clientX: number, clientY:
   *     number}} e the event.
   */
  mouseWheelHandler(e) {
    let inputs = drawing.mouseWheelCommon(e);
    this.mouseWheel(inputs.delta, inputs.x, inputs.y);
  }

  /** Stops panning the canvas sysem and redraws. */
  mouseUp() {
    this.selectEmpty();
    this.redraw();
  }

  /** Starts a canvas pan. */
  clickHandler(e) {
    this.canvasClick(e.clientX, e.clientY);
  }

  /**
   * Initializes the graphics.
   *
   * Must be called exactly once per plot and before any calls to redraw().
   * Creates the canvas 2d context.
   */
  initialize() {
    this.surface =
        /** @type {!Canvas2DContext} */ (this.canvasContainer.getContext('2d'));
    this.canvasContainer.addEventListener('DOMMouseScroll', (e) => {
      this.mouseWheelHandler(e);
    }, {passive: false});
    this.canvasContainer.addEventListener('mousewheel', (e) => {
      this.mouseWheelHandler(e);
    }, {passive: false});
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

    this.drawObjects = [];
    this.constructControls();
    this.computeBasis();
  }

  /** Resets the view window to the original one and redraws. */
  resetView() {
    if (this.surface === null) {
      // Graphics not intialized.
      return;
    }
    this.setViewWindow(this.viewWindowDefault[0], this.viewWindowDefault[1]);
    this.redraw();
  }

  /**
   * Contructs all button controls.
   *
   * As of writing, this is a single button that resets the view.
   *
   * @private
   */
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

  /**
   * Converts mathematical screen coordinates to mathmatical coordinates.
   *
   * @param {!Array.<number>} input mathematical screen coordinates.
   *
   * @returns {!Array.<number>}
   */
  coordinatesMathScreenToMath(input) {
    let output = this.screenBasisOrthonormal[0].slice();
    vectorTimesScalar(output, input[0]);
    vectorAddVectorTimesScalar(
        output, this.screenBasisOrthonormal[1], input[1]);
    return output;
  }

  /**
   * Converts mathematical screen coordinates to mathmatical coordinates.
   *
   * @param {!Array.<number>} vector mathematical coordinates.
   *
   * @returns {!Array.<number>}
   */
  coordinatesMathToMathScreen(vector) {
    return [
      vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.screenBasisOrthonormal[1])
    ];
  }

  /**
   * Converts mathematical coordinates to screen coordinates.
   *
   * @param {!Array.<number>} vector mathematical coordinates.
   *
   * @returns {!Array.<number>}
   *
   * @private
   */
  coordinatesMathToScreen(vector) {
    return [
      this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) +
          this.centerX,
      (-1) * this.scale *
              vectorScalarVector(vector, this.screenBasisOrthonormal[1]) +
          this.centerY
    ];
  }

  /**
   * Converts absolute screen coordinates to screen coordinates.
   *
   * @param {number} screenX absolute x coordinate.
   * @param {number} screenY absolute y coordinate.
   *
   * @returns {!Array.<number>}
   *
   * @private
   */
  coordsScreenAbsoluteToScreen(screenX, screenY) {
    return getXYPositionOfObject(this.canvasContainer, screenX, screenY);
  }

  /**
   * Converts screen coordinates to math screen coordinates.
   *
   * @param {!Array.<number>} screenPosition screen coordinates.
   *
   * @returns {!Array.<number>}
   *
   * @private
   */
  coordsScreenToMathScreen(screenPosition) {
    return [
      (screenPosition[0] - this.centerX) / this.scale,
      (this.centerY - screenPosition[1]) / this.scale
    ];
  }

  /**
   * Converts screen absolute coordinates to math screen coordinates.
   *
   * @param {number} screenX screen x coordinate.
   * @param {number} screenY screen y coordinate.
   *
   * @returns {!Array.<number>}
   */
  coordsScreenAbsoluteToMathScreen(screenX, screenY) {
    return this.coordsScreenToMathScreen(
        this.coordsScreenAbsoluteToScreen(screenX, screenY));
  }

  /**
   * Converts mathematical coordinates to screen coordinates.
   *
   * @param {!Array.<number>} coordinates mathematical coordinates.
   * @returns {!Array.<number>}
   *
   * @private
   */
  coordsMathScreenToScreen(coordinates) {
    return [
      this.scale * coordinates[0] + this.centerX,
      this.centerY - this.scale * coordinates[1]
    ];
  }

  /**
   * Zooms in and out based on the mouse wheel delta and the cursor position.
   * Keeps the cursor pointing to the same position in the graph.
   *
   * @param {number} wheelDelta change in wheel position.
   * @param {number} screenX absolute screen x coordinates.
   * @param {number} screenY absolute screen y coordinates.
   */
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
    this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
    this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
    this.redraw();
  }

  /**
   * Executes the default mouseMove handler as well as the additional 
   * listeners.
   *
   * @param {number} screenX absolute screen x coordinates.
   * @param {number} screenY absolute screen y coordinates.
   */
  mouseMove(screenX, screenY) {
    this.mouseMoveDefault(screenX, screenY);
    for (let i = 0; i < this.additionalMouseMoveListeners.length; i++) {
      this.additionalMouseMoveListeners[i](screenX, screenY);
    }
  }

  /**
   * If panning, moves the viewing window to follow the mouse motion.
   *
   * @param {number} screenX absolute screen x coordinates.
   * @param {number} screenY absolute screen y coordinates.
   */
  mouseMoveDefault(screenX, screenY){
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

  /**
   * If panning, moves the viewing window to follow the mouse motion.
   * @private
   */
  panAfterCursor() {
    let difference =
        vectorMinusVector(this.mousePosition, this.clickedPosition);
    this.centerX += difference[0] * this.scale;
    this.centerY -= difference[1] * this.scale;
    this.redraw();
  }

  /**
   * Determines whether a clicked point is within click tolerance distance to
   * another point.
   *
   * @param {!Array.<number>} leftXY [x,y]-coordinates of first point.
   * @param {!Array.<number>} rightXY [x,y]-coordinates of the second point.
   *
   * @returns {boolean}
   * @private
   */
  pointsWithinClickTolerance(leftXY, rightXY) {
    let squaredDistance =
        ((leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) +
         (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])) *
        this.scale;
    return squaredDistance < 1000;
  }

  /**
   * Determines whether a clicked point is within click tolerance distance to
   * another point.
   *
   * @param {number} screenX absolute screen X coordinate.
   * @param {number} screenY absolute screen Y coordinate.
   * @private
   */
  canvasClick(screenX, screenY) {
    this.clickedPosition =
        this.coordsScreenAbsoluteToMathScreen(screenX, screenY);
    this.mousePosition = [];
    this.selectedElement = 'origin';
    if (this.flagShowPerformance) {
      this.logStatus();
    }
  }

  /** Stops panning the plot. */
  selectEmpty() {
    this.selectedElement = '';
  }

  /**
   * Writes all debug information to a DOM component.
   *
   * @private
   */
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

  /**
   * Computes performance information.
   *
   * @private
   */
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

/** Parametric curve in three dimensions. */
class CurveThreeD {
  /**
   * @param {function(number):!Array.<number>} inputCoordinateFunctions the
   *     coordinate functions: a function that map a real number to a triple of
   *     numbers
   * @param {number} inputLeftPoint left (lower) bound for the parameter
   * @param {number} inputRightPoint right (high) bound for the parameter
   * @param {number} inputNumberOfSegments number of segments used to
   *     approximate the curve
   * @param {string} inputColor curve color
   * @param {number} inputLineWidth the line width of the curve.
   */
  constructor(
      inputCoordinateFunctions, inputLeftPoint, inputRightPoint,
      inputNumberOfSegments, inputColor, inputLineWidth) {
    /** @type{function(number):!Array.<number>} */
    this.coordinateFunctions = inputCoordinateFunctions;
    /** @type{number} */
    this.leftPt = inputLeftPoint;
    /** @type{number} */
    this.rightPt = inputRightPoint;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type{number} */
    this.numSegments = inputNumberOfSegments;
    this.lineWidth = inputLineWidth;
  }

  /**
   * Enlarges a bounding box in-place to ensure it encloses the object.
   *
   * @param {!Array.<!Array.<number>>} inputOutputBox output bounding box.
   */
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

  /**
   * Same as draw but does not complete the stroke command; use for plot fills.
   *
   * @param {!CanvasTwoD} canvas the canvas
   * @param {boolean} startByMoving whether to move to the initial point without
   * drawing a line
   */
  drawNoFinish(canvas, startByMoving) {
    let surface = canvas.surface;
    surface.strokeStyle = colorRGBToString(this.color);
    surface.fillStyle = colorRGBToString(this.color);
    let argumentT = this.leftPt;
    let x = this.coordinateFunctions[0](argumentT);
    let y = this.coordinateFunctions[1](argumentT);
    let coordinates = canvas.coordinatesMathToScreen([x, y]);
    if (startByMoving) {
      surface.moveTo(coordinates[0], coordinates[1]);
    } else {
      surface.lineTo(coordinates[0], coordinates[1]);
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
      coordinates = canvas.coordinatesMathToScreen([x, y]);
      surface.lineTo(coordinates[0], coordinates[1]);
    }
  }

  /**
   * Draws the object onto the given canvas.
   *
   * @param {!CanvasTwoD} canvas the canvas.
   */
  draw(canvas) {
    let surface = canvas.surface;
    surface.beginPath();
    this.drawNoFinish(canvas, true);
    surface.stroke();
  }
}

/**
 * Parametric surface in 3d.
 *
 * A parametric surface in 3d is an expression of the form:
 *
 * x=f(u,v)
 * y=g(u,v)
 * z=h(u,v)
 *
 * (u,v)\in X for some functions f,g,h and for the u,v-parameters in some
 * 2d-set. We presently support only rectangles for the (u,v)-parameter space.
 */
class Surface {
  /**
   * @param {function(number,number):!Array.<number>} inputXYZFunction a
   *     two-variable function that returns a the [x,y,z]-coordinates [f(u,v),
   *     h(u,v), g(u,v)].
   * @param{!Array.<!Array.<number>>} inputUVBox u,v-variable ranges, format:
   * [[uMin, uMax], [vMin, vMax]].
   * @param {!Array.<number>} inputPatchDimensions the pair of numbers.
   * [uPatchCount, vPathCount], where we subdivide the UV-rectangle in
   * uPatchCount times uPatchCount rectangles.
   * @param{string} colorFront color of the front of the patch.
   * @param{string} colorBack color of the back of the patch.
   * @param{string} colorContour color of the patch contour.
   * @param{number} inputContourWidth the line width of the contour.
   */
  constructor(
      inputXYZFunction,
      inputUVBox,
      inputPatchDimensions,
      colorFront,
      colorBack,
      colorContour,
      inputContourWidth,
  ) {
    this.xyzFun = inputXYZFunction;
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

/**
 * A class that represents a point in IIId.
 */
class Point {
  /**
   * @param {!Array.<number>} inputLocation [x,y,z]-coordinates of the input.
   * @param {string} inputColor input color.
   */
  constructor(inputLocation, inputColor) {
    this.location = inputLocation;
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
  }
}

/**
 * Patch, a 2d-parallelogram embedded in 3d.
 *
 * This is one of the basic building blocks for drawing surfaces.
 *
 * Given by a base point p and two edge vectors v_1, v_2. The points that belong
 * to the patch are the points in the set
 *
 * \{p+s*v_1+t*v_2 | s\in [0,1], t\in[0,1]\}
 */
class Patch {
  /**
   * @param{!Array.<number>} inputBase the base point of the patch.
   * @param{!Array.<number>} inputEdge1 the first edge vector.
   * @param{!Array.<number>} inputEdge2 the second edge vector.
   * @param{string} inputColorUV color of the "forward" side of the patch.
   * @param{string} inputColorVU color of the "back" side of the patch.
   */
  constructor(inputBase, inputEdge1, inputEdge2, inputColorUV, inputColorVU) {
    /** @type{!Array.<number>}*/
    this.base = inputBase.slice();
    /** @type{!Array.<number>}*/
    this.edge1 = inputEdge1.slice();
    /** @type{!Array.<number>}*/
    this.edge2 = inputEdge2.slice();
    this.colorUV = colorToRGB(inputColorUV);
    this.colorVU = colorToRGB(inputColorVU);
    /** @type{!Array.<number>}*/
    this.v1 = vectorPlusVector(this.base, this.edge1);
    this.v2 = vectorPlusVector(this.base, this.edge2);
    this.vEnd = vectorPlusVector(this.v1, this.edge2);
    this.internalPoint = this.base.slice();
    /** @type{!Array.<number>}*/
    this.normalScreen1 = [];
    /** @type{!Array.<number>}*/
    this.normalScreen2 = [];
    /** @type{!Array.<number>}*/
    this.normal = [];
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

/**
 * A parametric curve in 3d that is a boundary of a surface patch.
 *
 * Sections of the curve that are not visible are plotted with dashes.
 */
class Contour {
  /**
   * @param{!Array.<!Array<number>>} inputPoints the points on the contour.
   * @param{string|!Array.<number>} inputColor color of the contour.
   * @param{number} inputLineWidth line width of the contour.
   *
   */
  constructor(inputPoints, inputColor, inputLineWidth) {
    /** @type{!Array.<!Array<number>>} */
    this.points = inputPoints.slice();
    this.thePointsMathScreen = [];
    /** @type{!Array.<number>} */
    this.color = colorToRGB(inputColor);
    /** @type {!Object.<number,boolean>} */
    this.adjacentPatches = {};
    this.index = -1;
    this.lineWidth = inputLineWidth;
  }
}

/**
 * A text label in 3d.
 */
class TextInThreeD {
  /**
   * @param{!Array.<number>} location [x,y,z]-location of the label.
   * @param{string} text text of the label
   * @param{string} color color of the text.
   */
  constructor(location, text, color) {
    this.location = location;
    this.text = text;
    this.color = color;
  }
}

/**
 * Canvas used for software-generated mathematical 3d plots.
 *
 * The canvas uses a 2d context; all computations required to project the 3d
 * plot are computed here, in software (javascript). This comes with heavy
 * restrictions on performance, please use with appropriate caution.
 *
 * Graphics prorperties.
 * - Uses orthographic projections to project the 3d scene, as is the custom in
 * mathematical plots.
 * - Countours and curves that are not visible in the scene
 * are drawn by dashed lines, as usual in mathematical plots.
 * - Patches that are farther away (in terms of view depth from the screen) will
 * be drawn slightly darker.
 * - Surfaces are broken down into curvilinear patches.
 * -- Occlusions are computed by approximating each patch with a parallelogram.
 * -- Patches are drawn using higher curvilinear precision.
 * -- See comments below for the limitations and advangates of this approach.
 *
 * Any two straight-edge patches can be correctly superimposed on the scene by
 * drawing one of the patches first, and drawing the second patch after
 * (possibly partially occluding part of the original patch).
 *
 * The superimposition considerations above break for 3 or more patches with
 * circular overlaps 1>2>3>1. This results in visual glitches. The visual
 * glitches get smaller if the patches are reduced in size. Also, circularly
 * overlapping patches are rare in mathemaical problems, so this should not a
 * present a problem most educational applications.
 *
 * The superimposition considerations above also break for two curvilinear
 * patches that are highly curved. This issue can be alleviated by increasing
 * the patch precision, but at an increased computational cost.
 *
 * The strategy above is how we choose to paint our scene.
 *
 * Advantages.
 * - Since such graphics are not pixel based, they are easy and
 * inexpensive to draw to SVG/canvas/postscript[pdf, paper print context].
 * - Such graphics are extremely portable and do not require WebGL.
 *
 * Disadvantages.
 * - This scheme for generating plots has strict performance limitations.
 * Drawing more than 1000 patches on the screen is not recommended.
 * - The visual glitches described above can be a problem for complicated
 * geometry applications.
 */
class Canvas {
  /**
   * @param {!HTMLCanvasElement} inputCanvas The canvas element.
   * @param {!HTMLElement} controls A location in which to generate the plot
   *     controls.
   * @param {?HTMLElement} messages A Dom element in which to write debug
   *     information.
   */
  constructor(
      inputCanvas,
      controls,
      messages,
  ) {
    this.canvasContainer = inputCanvas;
    this.spanControls = controls;
    this.spanMessages = messages;
    this.all3dObjects = {
      /** @type {!Array.<!Patch>} */
      allPatches: [],
      allContours: [],
      points: [],
      /** @type {!Array.<!TextInThreeD>} */
      theLabels: [],
    };
    this.screenXY = [0, 0];
    this.flagShowPerformance = true;
    this.rotationModesAvailable = {
      'rotateAfterCursorDefaultGreatNormalCircle': () => {
        this.rotateAfterCursorDefaultGreatNormalCircle();
      },
    };
    this.rotationMode = 'rotateAfterCursorDefaultGreatNormalCircle';

    this.patchOrder = [];
    this.numAccountedPatches = 0;
    this.numCyclicallyOverlappingPatchTieBreaks = 0;
    this.numContourPoints = 0;
    this.numContourPaths = 0;
    this.patchIsAccounted = [];
    /** @type{?CanvasRenderingContext2D} */
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
    /** @type{boolean} */
    this.flagPaintZBuffer = false;
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
    /** @type{!Array.<number>} */
    this.mousePosition = [];
    /** @type{!Array.<number>} Position in math coordinates, 3d. */
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
    /** @type{!Array.<!Array.<number>>} */
    this.selectedScreenBasisOrthonormal = [];
    /** @type{!Array.<number>} */
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

  /**
   * Zooms the 3d scene following the mouse wheel.
   *
   * @param {{preventDefault: !Function, stopPropagation: !Function, detail:
   *     (number|null|undefined), wheelDelta: number, clientX: number, clientY:
   *     number}} e the mouse wheel event.
   */
  mouseWheelHandler(e) {
    let inputs = drawing.mouseWheelCommon(e);
    this.mouseWheel(inputs.delta, inputs.x, inputs.y);
  }

  /** Handles the mouse up event. */
  mouseUp() {
    this.selectEmpty();
    this.redraw();
  }

  /**
   * Handles a mouse click event.
   *
   * @param {{clientX: number, clientY: number, shiftKey: boolean}} e the click
   *     event.
   */
  clickHandler(e) {
    this.canvasClick(e.clientX, e.clientY, e);
  }

  /**
   * Initializes the 3d scene.
   *
   * Among other initialization steps, this also creates the 2d context canvas
   * and attaches all mouse events.
   */
  initialize() {
    this.clear();
    /** @type{!CanvasRenderingContext2D} */
    this.surface = /** @type{!CanvasRenderingContext2D} */ (
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
  }

  /** Clears the graphics. */
  clear() {
    this.all3dObjects.allPatches = [];
    this.all3dObjects.allContours = [];
    this.all3dObjects.points = [];
    this.all3dObjects.theLabels = [];
    this.constructControls();
    this.computeBasis();
    if (this.zBuffer.length === 0) {
      this.allocateZbuffer();
    }
  }

  /**
   * Draws a text at a location in the 3d scene.
   *
   * @param{!Array.<number>} location [x,y,z]-location of the label.
   * @param{string} text text of the label
   * @param{string} color color of the text.
   */
  drawText(location, text, color) {
    this.all3dObjects.theLabels.push(new TextInThreeD(location, text, color));
  }

  /**
   * Draws a curve in 3d. Curve sections that are in the background will be
   * drawn with a dashed line.
   *
   * @param{!CurveThreeD} curve [x,y,z]-location of the label.
   */
  drawCurve(curve) {
    let contourPoints = new Array(curve.numSegments + 1);
    for (let i = 0; i < curve.numSegments + 1; i++) {
      let ratio = i / curve.numSegments;
      let currentParam = curve.leftPt * (1 - ratio) + curve.rightPt * ratio;
      contourPoints[i] = curve.coordinateFunctions(currentParam);
    }
    this.all3dObjects.allContours.push(
        new Contour(contourPoints, curve.color, curve.lineWidth));
  }

  /**
   * Draws a 2d parallelogram in 3d.
   *
   * @param{!Array.<number>} base the base point of the patch.
   * @param{!Array.<number>} edge1 the first edge vector.
   * @param{!Array.<number>} edge2 the second edge vector.
   * @param{string} color color of the "forward" side of the patch.
   */
  drawPatchStraight(base, edge1, edge2, color) {
    this.all3dObjects.allPatches.push(
        new Patch(base, edge1, edge2, color, color));
    let patchIndex = this.all3dObjects.allPatches.length - 1;
    let patch = this.all3dObjects.allPatches[patchIndex];
    patch.index = patchIndex;
    let allContours = this.all3dObjects.allContours;
    patch.adjacentContours.push(this.drawLine(patch.base, patch.v1, color, 1));
    patch.traversalOrder.push(1);

    allContours[allContours.length - 1].adjacentPatches[patchIndex] = true;
    patch.adjacentContours.push(this.drawLine(patch.v1, patch.vEnd, color, 1));
    patch.traversalOrder.push(1);

    allContours[allContours.length - 1].adjacentPatches[patchIndex] = true;
    patch.adjacentContours.push(this.drawLine(patch.vEnd, patch.v2, color, 1));
    patch.traversalOrder.push(1);

    allContours[allContours.length - 1].adjacentPatches[patchIndex] = true;
    patch.adjacentContours.push(this.drawLine(patch.v2, patch.base, color, 1));
    patch.traversalOrder.push(1);

    allContours[allContours.length - 1].adjacentPatches[patchIndex] = true;
  }

  /**
   * Draws points on the scene. Points are always painted, even when occluded.
   *
   * @param{!Array.<!Array<number>>} inputPoints list of [x,y,z]-coordinates.
   * @param{string} inputColor color of the points.
   */
  drawPoints(inputPoints, inputColor) {
    for (let i = 0; i < inputPoints.length; i++) {
      this.all3dObjects.points.push(new Point(inputPoints[i], inputColor));
    }
  }

  /**
   * Draws a straight segment. Parts of the segment in the background with be
   * drawn with dashed lines. Returns the index of the newly added contour.
   *
   * @param{!Array.<number>} leftPt [x,y,z]-coordinates of the first point.
   * @param{!Array.<number>} rightPt [x,y,z]-coordinates of the first point.
   * @param{string} inputColor color of the points.
   * @param{number} inputLineWidth line width.
   *
   * @returns {number}
   */
  drawLine(leftPt, rightPt, inputColor, inputLineWidth) {
    let newContour = new Contour([], inputColor, inputLineWidth);
    let numSegments = this.defaultNumSegmentsPerContour;
    newContour.index = this.all3dObjects.allContours.length;
    let incrementVector = vectorMinusVector(rightPt, leftPt);
    if (vectorLength(incrementVector) * 10 > numSegments) {
      numSegments = Math.ceil(vectorLength(incrementVector) * 10);
    }
    newContour.points = new Array(numSegments);
    let incrementScalar = 1 / numSegments;
    vectorTimesScalar(incrementVector, incrementScalar);
    let currentPoint = leftPt.slice();
    for (let i = 0; i < numSegments + 1; i++) {
      newContour.points[i] = currentPoint;
      currentPoint = vectorPlusVector(currentPoint, incrementVector);
    }
    this.all3dObjects.allContours.push(newContour);
    return this.all3dObjects.allContours.length - 1;
  }

  /**
   * Computes internal information about a patch in place.
   *
   * @param{!Patch} patch the patch to be updated.
   * @private
   */
  computePatch(patch) {
    patch.normalScreen1 = vectorCrossVector(this.screenNormal, patch.edge1);
    patch.normalScreen2 = vectorCrossVector(this.screenNormal, patch.edge2);
    patch.normal = vectorCrossVector(patch.edge1, patch.edge2);
    patch.internalPoint = patch.base.slice();
    vectorAddVectorTimesScalar(patch.internalPoint, patch.edge1, 0.5);
    vectorAddVectorTimesScalar(patch.internalPoint, patch.edge2, 0.5);
  }

  /**
   * Computes internal information about a contour in place.
   *
   * @param{!Contour} contour the contour to be updated.
   * @private
   */
  computeContour(contour) {
    if (contour.thePointsMathScreen.length !== contour.points.length) {
      contour.thePointsMathScreen = new Array(contour.points.length);
    }
    for (let i = 0; i < contour.points.length; i++) {
      contour.thePointsMathScreen[i] =
          this.coordsProjectMathToMathScreen2d(contour.points[i]);
    }
  }

  /**
   * Computes the position of a point relative to a patch.
   * Returns 1 if the point is in front of the patch. Returns -1 if the patch
   * occludes the point. Returns 0 if the point and the patch do not overlap.
   *
   * @param{!Array.<number>} point [x,y,z]-coordinates of a point.
   * @param{!Patch} patch the patch.
   *
   * @returns{number}
   * @private
   */
  pointRelativeToPatch(point, patch) {
    if (vectorLength(patch.normalScreen1) < 0.00001 ||
        vectorLength(patch.normalScreen2) < 0.00001 ||
        vectorLength(patch.normal) < 0.00001) {
      return 0;
    }
    if (vectorScalarVector(
            vectorMinusVector(point, patch.base), patch.normalScreen1) *
            vectorScalarVector(
                vectorMinusVector(patch.internalPoint, patch.base),
                patch.normalScreen1) >
        0) {
      if (vectorScalarVector(
              vectorMinusVector(point, patch.vEnd), patch.normalScreen1) *
              vectorScalarVector(
                  vectorMinusVector(patch.internalPoint, patch.vEnd),
                  patch.normalScreen1) >
          0) {
        if (vectorScalarVector(
                vectorMinusVector(point, patch.base), patch.normalScreen2) *
                vectorScalarVector(
                    vectorMinusVector(patch.internalPoint, patch.base),
                    patch.normalScreen2) >
            0) {
          if (vectorScalarVector(
                  vectorMinusVector(point, patch.vEnd), patch.normalScreen2) *
                  vectorScalarVector(
                      vectorMinusVector(patch.internalPoint, patch.vEnd),
                      patch.normalScreen2) >
              0) {
            if (vectorScalarVector(
                    vectorMinusVector(point, patch.base), patch.normal) *
                    vectorScalarVector(this.screenNormal, patch.normal) <=
                0) {
              return -1;
            }
            if (vectorScalarVector(
                    vectorMinusVector(point, patch.base), patch.normal) *
                    vectorScalarVector(this.screenNormal, patch.normal) >=
                0) {
              return 1;
            }
          }
        }
      }
    }
    return 0;
  }

  /**
   * Computes whether a point is occluded by a patch.
   *
   * @param{!Array.<number>} point [x,y,z]-coordinates of a point.
   * @param{!Patch} patch the patch.
   *
   * @returns{boolean}
   * @private
   */
  pointIsBehindPatch(point, patch) {
    return this.pointRelativeToPatch(point, patch) === -1;
  }

  /**
   * Computes whether a point is in front of a patch.
   *
   * @param{!Array.<number>} point [x,y,z]-coordinates of a point.
   * @param{!Patch} patch the patch.
   *
   * @returns{boolean}
   */
  pointIsInFrontOfPatch(point, patch) {
    return this.pointRelativeToPatch(point, patch) === 1;
  }

  /**
   * Computes whether a point is in the foreground (not occluded by any patch).
   *
   * The point is compared only to the given patches. Patches can be optimized
   * out of this comparison using the z-buffer.
   *
   * @param{!Array.<number>} point [x,y,z]-coordinates of a point.
   * @param{!Object.<number,boolean>} containerPatches the patches that can
   * occlude the point.
   *
   * @returns{boolean}
   * @private
   */
  pointIsInForeGround(point, containerPatches) {
    let allPatches = this.all3dObjects.allPatches;
    for (let i = 0; i < allPatches.length; i++) {
      if (i in containerPatches) {
        continue;
      }
      if (this.pointIsBehindPatch(point, allPatches[i])) {
        return false;
      }
    }
    return true;
  }

  /**
   * Paints a visual indicator to aid with mouse rotation.
   *
   * The aid is a straight line connecting the selected point to the center of
   * the coordinate system.
   *
   * @private
   */
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

  /**
   * Paints one contour on the canvas.
   *
   * @param {!Contour} contour the contour.
   *
   * @private
   */
  paintOneContour(contour) {
    if (contour.points.length < 2) {
      return;
    }
    let surface = this.surface;
    let thePts = contour.points;
    let currentPt = this.coordinatesMathToScreen(thePts[0]);
    if (this.flagRoundContours) {
      vectorRound(currentPt);
    }
    let oldIsInForeGround =
        this.pointIsInForeGround(contour.points[0], contour.adjacentPatches);
    let newIsInForeground = true;
    let dashIsOn = false;
    surface.beginPath();
    surface.setLineDash([]);
    this.numContourPaths++;
    surface.strokeStyle = colorRGBToString(contour.color);
    if (contour.lineWidth !== undefined) {
      surface.lineWidth = contour.lineWidth;
    } else {
      surface.lineWidth = 1;
    }
    surface.moveTo(currentPt[0], currentPt[1]);
    this.numContourPoints += thePts.length;
    for (let i = 1; i < thePts.length; i++) {
      newIsInForeground =
          this.pointIsInForeGround(contour.points[i], contour.adjacentPatches);
      if (!newIsInForeground && !oldIsInForeGround && !dashIsOn) {
        if (i > 1) {
          surface.stroke();
          surface.setLineDash([4, 4]);
          surface.beginPath();
          this.numContourPaths++;
          surface.moveTo(currentPt[0], currentPt[1]);
        }
        surface.setLineDash([4, 4]);
        dashIsOn = true;
      } else if (dashIsOn && newIsInForeground) {
        surface.stroke();
        surface.setLineDash([]);
        surface.beginPath();
        this.numContourPaths++;
        surface.moveTo(currentPt[0], currentPt[1]);
        dashIsOn = false;
      }
      currentPt = this.coordinatesMathToScreen(thePts[i]);
      if (this.flagRoundContours) {
        vectorRound(currentPt);
      }
      surface.lineTo(currentPt[0], currentPt[1]);
      oldIsInForeGround = newIsInForeground;
    }
    surface.stroke();
    /////////////////
    if (this.flagDebugLabelPatches) {
      currentPt = this.coordinatesMathToScreen(thePts[4]);
      if (this.flagRoundContours) {
        vectorRound(currentPt);
      }
      surface.font = '9pt sans-serif';
      surface.fillStyle = 'black';
      surface.fillText(`${contour.index}`, currentPt[0], currentPt[1]);
    }
  }

  /**
   * Paints one patch on the canvas.
   *
   * @param {!Patch} patch the patch.
   *
   * @private
   */
  paintOnePatch(patch) {
    let surface = this.surface;
    let visibilityScalarProd =
        vectorScalarVector(this.screenNormal, patch.normal);
    let depthScalarProd =
        vectorScalarVector(this.screenNormal, patch.internalPoint);
    let depthRatio = (depthScalarProd - this.boundingSegmentZ[0]) /
            (this.boundingSegmentZ[1] - this.boundingSegmentZ[0]) -
        0.5;
    // depthRatio is a number between -0.5 and 0.5 measuring how deep is the
    // patch relative to the center of the picture
    let colorFactor = 1 + depthRatio * this.colorDepthFactor;
    if (visibilityScalarProd >= 0) {
      surface.fillStyle =
          colorRGBToString(colorScale(patch.colorUV, colorFactor));
    } else {
      surface.fillStyle =
          colorRGBToString(colorScale(patch.colorVU, colorFactor));
    }
    surface.beginPath();
    let first = true;
    for (let i = 0; i < patch.adjacentContours.length; i++) {
      let currentContour =
          this.all3dObjects.allContours[patch.adjacentContours[i]];
      for (let j = 0; j < currentContour.points.length; j++) {
        let theIndex = (patch.traversalOrder[i] === -1) ?
            j :
            currentContour.points.length - j - 1;
        let coordinates =
            this.coordinatesMathToScreen(currentContour.points[theIndex]);
        if (this.flagRoundPatches) {
          vectorRound(coordinates);
        }
        if (first) {
          surface.moveTo(coordinates[0], coordinates[1]);
        } else {
          surface.lineTo(coordinates[0], coordinates[1]);
        }
        first = false;
      }
    }
    surface.closePath();
    surface.fill();
  }

  /**
   * Paints text on the canvas.
   *
   * @param {!TextInThreeD} theText the text.
   *
   * @private
   */
  paintText(theText) {
    let surface = this.surface;
    let isInForeGround = this.pointIsInForeGround(theText.location, []);
    surface.beginPath();
    surface.strokeStyle = theText.color;
    let coordinates = this.coordinatesMathToScreen(theText.location);
    surface.font = '15pt sans-serif';
    surface.lineWidth = 1;
    if (isInForeGround) {
      surface.fillStyle = theText.color;
      surface.fillText(theText.text, coordinates[0], coordinates[1]);
    } else {
      surface.strokeStyle = theText.color;
      surface.strokeText(theText.text, coordinates[0], coordinates[1]);
    }
  }

  /**
   * Paints one point on the canvas.
   *
   * @param {!Point} point the point.
   *
   * @private
   */
  paintOnePoint(point) {
    let surface = this.surface;
    let isInForeGround = this.pointIsInForeGround(point.location, []);
    surface.beginPath();
    surface.strokeStyle = colorRGBToString(point.color);
    surface.fillStyle = colorRGBToString(point.color);
    let coordinates = this.coordinatesMathToScreen(point.location);
    surface.arc(coordinates[0], coordinates[1], 3, 0, Math.PI * 2);
    if (isInForeGround) {
      surface.fill();
    } else {
      surface.stroke();
    }
  }

  /**
   * Get the largest/smallest coordinate of a given index.
   *
   * @param {number} indexToCompareBy the index of the relevant coordinate.
   * @param {boolean} getLarger whether we seek the largest coordinate.
   * @param {!Array.<number>} pt1 the first point.
   * @param {!Array.<number>} pt2 the second point.
   * @param {!Array.<number>} pt3 the third point.
   * @param {!Array.<number>} pt4 the fourth point.
   *
   * @returns{number}
   * @private
   */
  getExtremePoint(indexToCompareBy, getLarger, pt1, pt2, pt3, pt4) {
    let result = pt1[indexToCompareBy];
    if (getLarger) {
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

  /**
   * Fetches the row index of the z-buffer patch for a given y coordinate.
   *
   * @param {number} input the y coordinate of the point.
   *
   * @returns{number}
   * @private
   */
  coordsMathScreenToBufferIndicesROWSFloat(input) {
    return this.zBufferRowCount * (input - this.boundingBoxMathScreen[0][1]) /
        (this.boundingBoxMathScreen[1][1] - this.boundingBoxMathScreen[0][1]);
  }

  /**
   * A less precise but faster version of
   * coordsMathScreenToBufferIndicesROWSFloat.
   *
   * @param {number} input the y coordinate of the point.
   *
   * @returns{number}
   * @private
   */
  coordsMathScreenToBufferIndicesROWS(input) {
    let result =
        Math.floor(this.coordsMathScreenToBufferIndicesROWSFloat(input));
    if (result >= this.zBufferRowCount) {
      result--;
    }
    return result;
  }

  /**
   * Fetches the column index of the z-buffer patch for a given x coordinate.
   *
   * @param {number} input the x coordinate of the point.
   *
   * @returns{number}
   * @private
   */
  coordsMathScreenToBufferIndicesCOLSFloat(input) {
    return (this.zBufferColCount) * (input - this.boundingBoxMathScreen[0][0]) /
        (this.boundingBoxMathScreen[1][0] - this.boundingBoxMathScreen[0][0]);
  }

  /**
   * A less precise but faster version of
   * coordsMathScreenToBufferIndicesCOLSFloat.
   *
   * @param {number} input the x coordinate of the point.
   *
   * @returns{number}
   * @private
   */
  coordsMathScreenToBufferIndicesCOLS(input) {
    let result =
        Math.floor(this.coordsMathScreenToBufferIndicesCOLSFloat(input));
    if (result >= this.zBufferColCount) {
      result--;
    }
    return result;
  }

  /**
   * Computes the row, column of the z-buffer patch that contains the given
   * point.
   *
   * @param {!Array.<number>} input the [x,y]-coordinates of an orthographic
   *     projection of a point in our 3d scene.
   *
   * @returns{!Array.<number>}
   * @private
   */
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

  /**
   * @param {number} row the row of the z-buffer.
   * @param {!Array.<number>} point [x,y]-coordinates of the orthographic
   *     projection of a point in the scene.
   *
   * @private
   */
  accountOnePointMathCoordsInBufferStrip(row, point) {
    if (row < 0 || row >= this.zBufferIndexStrip.length) {
      return;
    }
    let bufferCoords = this.coordsMathScreenToBufferIndices(
        this.coordsProjectMathToMathScreen2d(point));
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

  /**
   * @param {!Array.<number>} base the starting point of the edge.
   * @param {!Array.<number>} edgeVector the edge vector.
   *
   * @private
   */
  accountEdgeInBufferStrip(base, edgeVector) {
    let end = vectorPlusVector(base, edgeVector);
    let lowFloat = this.coordsMathScreenToBufferIndicesROWSFloat(
        this.coordsProjectMathToMathScreen2d(base)[1]);
    let highFloat = this.coordsMathScreenToBufferIndicesROWSFloat(
        this.coordsProjectMathToMathScreen2d(end)[1]);
    if (lowFloat > highFloat) {
      let minusEdge = edgeVector.slice();
      vectorTimesScalar(minusEdge, -1);
      this.accountEdgeInBufferStrip(end, minusEdge);
      return;
    }
    this.accountOnePointMathCoordsInBufferStrip(Math.floor(lowFloat), base);
    this.accountOnePointMathCoordsInBufferStrip(Math.floor(highFloat), end);
    if (lowFloat === highFloat) {
      return;
    }
    let currentPoint;
    for (let i = Math.ceil(lowFloat); i < highFloat; i++) {
      currentPoint = base.slice();
      vectorAddVectorTimesScalar(
          currentPoint, edgeVector, (i - lowFloat) / (highFloat - lowFloat));
      this.accountOnePointMathCoordsInBufferStrip(i, currentPoint);
      this.accountOnePointMathCoordsInBufferStrip(i - 1, currentPoint);
    }
  }

  /**
   * Accounts all z-buffer patches intersected by a given patch.
   *
   * @param {number} patchIndex the inded of the patch.
   *
   * @private
   */
  accountOnePatch(patchIndex) {
    let patch = this.all3dObjects.allPatches[patchIndex];
    let pt1 = this.coordsProjectMathToMathScreen2d(patch.base);
    let pt2 = this.coordsProjectMathToMathScreen2d(patch.v1);
    let pt3 = this.coordsProjectMathToMathScreen2d(patch.v2);
    let pt4 = this.coordsProjectMathToMathScreen2d(patch.vEnd);
    let lowFloat = this.getExtremePoint(1, false, pt1, pt2, pt3, pt4);
    let highFloat = this.getExtremePoint(1, true, pt1, pt2, pt3, pt4);
    let low = this.coordsMathScreenToBufferIndicesROWS(lowFloat);
    let high = this.coordsMathScreenToBufferIndicesROWS(highFloat);
    for (let i = low; i <= high; i++) {
      this.zBufferIndexStrip[i][0] = -1;
      this.zBufferIndexStrip[i][1] = -1;
    }
    this.accountEdgeInBufferStrip(patch.base, patch.edge1);
    this.accountEdgeInBufferStrip(patch.base, patch.edge2);
    this.accountEdgeInBufferStrip(patch.v1, patch.edge2);
    this.accountEdgeInBufferStrip(patch.v2, patch.edge1);
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

  /**
   * Accounts one point of the scene into the scene's 2d-bounding box.
   *
   * @param {!Array.<number>} input [x,y,z] coordinates of the point.
   *
   * @private
   */
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

  /**
   * Computes the 2d-bounding box of the scene.
   *
   * @private
   */
  computeBoundingBox() {
    let allPatches = this.all3dObjects.allPatches;
    let allContours = this.all3dObjects.allContours;
    let points = this.all3dObjects.points;
    for (let i = 0; i < allPatches.length; i++) {
      this.computeBoundingBoxAccountPoint(allPatches[i].base);
      this.computeBoundingBoxAccountPoint(allPatches[i].v1);
      this.computeBoundingBoxAccountPoint(allPatches[i].v2);
      this.computeBoundingBoxAccountPoint(allPatches[i].vEnd);
    }
    for (let i = 0; i < allContours.length; i++) {
      for (let j = 0; j < allContours[i].points.length; j++) {
        this.computeBoundingBoxAccountPoint(allContours[i].points[j]);
      }
    }
    for (let i = 0; i < points.length; i++) {
      this.computeBoundingBoxAccountPoint(points[i].location);
    }
  }

  /**
   * Computes all z-buffer patches.
   *
   * @private
   */
  computeBuffers() {
    let allPatches = this.all3dObjects.allPatches;
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
    for (let i = 0; i < allPatches.length; i++) {
      this.accountOnePatch(i);
    }
    this.computePatchOrder();
  }

  /**
   * Computes the relative position of a contour point to a patch. Takes into
   * account known incidence data: if the contour point belongs to a patch,
   * occlusion computations can be optimized out.
   *
   * @param {!Patch} patch the patch.
   * @param {!Contour} contour  the contour.
   * @param {number} ptIndex the index of the point in the contour.
   *
   * @private
   */
  computePatchOrderOneContourPoint(patch, contour, ptIndex) {
    let thePointMathScreen = contour.thePointsMathScreen[ptIndex];
    let point = contour.points[ptIndex];
    let theIndices = this.coordsMathScreenToBufferIndices(thePointMathScreen);
    let currentBuffer = this.zBuffer[theIndices[0]][theIndices[1]];
    let allPatches = this.all3dObjects.allPatches;
    for (let i = 0; i < currentBuffer.length; i++) {
      if (patch.index === currentBuffer[i]) {
        continue;
      }
      if (patch.patchesAboveMe.indexOf(currentBuffer[i]) >= 0 ||
          patch.patchesBelowMe.indexOf(currentBuffer[i]) >= 0) {
        continue;
      }
      let otherPatch = allPatches[currentBuffer[i]];
      let relativePosition = this.pointRelativeToPatch(point, otherPatch);
      if (relativePosition === -1) {
        otherPatch.patchesBelowMe.push(patch.index);
        patch.patchesAboveMe.push(currentBuffer[i]);
      } else if (relativePosition === 1) {
        otherPatch.patchesAboveMe.push(patch.index);
        patch.patchesBelowMe.push(currentBuffer[i]);
      }
    }
  }

  /**
   * Computes partial patch order of one patch relative to the other patches.
   *
   * @param {!Patch} patch the patch.
   *
   * @private
   */
  computePatchPartialOrderOnePatch(patch) {
    let allContours = this.all3dObjects.allContours;
    for (let i = 0; i < patch.adjacentContours.length; i++) {
      for (let j = 0; j < allContours[patch.adjacentContours[i]].points.length;
           j++) {
        this.computePatchOrderOneContourPoint(
            patch, allContours[patch.adjacentContours[i]], j);
      }
    }
  }

  /**
   * Computes the partial patch order.
   *
   * The order is not transitive, i.e., it is possible that
   * patch1>patch2>patch3>patch1 (cyclically overlapping patches).
   *
   * @private
   */
  computePatchPartialOrder() {
    let allPatches = this.all3dObjects.allPatches;
    for (let i = 0; i < allPatches.length; i++) {
      allPatches[i].patchesBelowMe = [];
      allPatches[i].patchesAboveMe = [];
    }
    for (let i = 0; i < allPatches.length; i++) {
      this.computePatchPartialOrderOnePatch(allPatches[i]);
    }
  }

  /**
   * Computes the paint order of the patches
   *
   * Breaks cyclically overlapping patches into a strict paint order.
   *
   * @private
   */
  computePatchOrder() {
    this.computePatchPartialOrder();
    let allPatches = this.all3dObjects.allPatches;
    if (this.patchOrder.length !== allPatches.length) {
      this.patchOrder = new Array(allPatches.length);
      this.patchIsAccounted = new Array(allPatches.length);
    }
    for (let i = 0; i < allPatches.length; i++) {
      this.patchOrder[i] = -1;
      this.patchIsAccounted[i] = 0;
    }
    this.numAccountedPatches = 0;
    for (let i = 0; i < allPatches.length; i++) {
      if (allPatches[i].patchesBelowMe.length === 0) {
        this.patchOrder[this.numAccountedPatches] = i;
        this.numAccountedPatches++;
        this.patchIsAccounted[i] = 1;
      }
    }
    this.numCyclicallyOverlappingPatchTieBreaks = 0;
    while (this.numAccountedPatches < allPatches.length) {
      let currentIndex = 0;
      while (currentIndex < this.numAccountedPatches) {
        let currentPatch = allPatches[this.patchOrder[currentIndex]];
        for (let i = 0; i < currentPatch.patchesAboveMe.length; i++) {
          let nextIndex = currentPatch.patchesAboveMe[i];
          if (this.patchIsAccounted[nextIndex] === 1) {
            continue;
          }
          let nextPatch = allPatches[nextIndex];
          let isGood = 1;
          for (let j = 0; j < nextPatch.patchesBelowMe.length; j++) {
            if (this.patchIsAccounted[nextPatch.patchesBelowMe[j]] !== 1) {
              isGood = 0;
              break;
            }
          }
          if (isGood === 1) {
            this.patchOrder[this.numAccountedPatches] = nextIndex;
            this.numAccountedPatches++;
            this.patchIsAccounted[nextIndex] = 1;
          }
        }
        currentIndex++;
      }
      if (this.numAccountedPatches < allPatches.length) {
        this.patchOverlapTieBreak();
      }
    }
  }

  /**
   * Breaks one collection of cyclically overlapping patches into a strict paint
   * order.
   *
   * @private
   */
  patchOverlapTieBreak() {
    // If we have cyclically overlapping patches we break ties
    // by selecting/painting first the patches whose internal point
    // has the highest (screen) depth.
    let allPatches = this.all3dObjects.allPatches;
    let deepestNonAccountedIndex = -1;
    let minDepth = 0;
    for (let i = 0; i < this.patchIsAccounted.length; i++) {
      if (this.patchIsAccounted[i] === 1) {
        continue;
      }
      if (deepestNonAccountedIndex === -1) {
        deepestNonAccountedIndex = i;
        minDepth =
            vectorScalarVector(this.screenNormal, allPatches[i].internalPoint);
      }
      let currentDepth =
          vectorScalarVector(this.screenNormal, allPatches[i].internalPoint);
      if (currentDepth < minDepth) {
        minDepth = currentDepth;
        deepestNonAccountedIndex = i;
      }
    }
    this.numCyclicallyOverlappingPatchTieBreaks++;
    this.patchOrder[this.numAccountedPatches] = deepestNonAccountedIndex;
    this.numAccountedPatches++;
    this.patchIsAccounted[deepestNonAccountedIndex] = 1;
  }

  /**
   * Allocates the initial z-buffer.
   *
   * @private
   */
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

  /**
   * Returns the low left, top right corner of a z-buffer patch.
   *
   * @param {number} row index of the row.
   * @param {number} col index of the column.
   *
   * @returns {!Array.<!Array<number>>}
   * @private
   */
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

  /**
   * Paints the z-buffer on the canvas.
   *
   * Use for visual debugging of the graphics.
   */
  paintZbuffer() {
    let surface = this.surface;
    surface.strokeStyle = 'gray';
    surface.fillStyle = 'black';
    surface.setLineDash([]);
    for (let i = 0; i < this.zBuffer.length; i++) {
      for (let j = 0; j < this.zBuffer[i].length; j++) {
        let bufferBox = this.getBufferBox(i, j);
        let bufferBoxLowLeft = this.coordsMathScreenToScreen(bufferBox[0]);
        let bufferBoxTopRight = this.coordsMathScreenToScreen(bufferBox[1]);
        surface.beginPath();
        surface.moveTo(bufferBoxLowLeft[0], bufferBoxLowLeft[1]);
        surface.lineTo(bufferBoxTopRight[0], bufferBoxLowLeft[1]);
        surface.lineTo(bufferBoxTopRight[0], bufferBoxTopRight[1]);
        surface.lineTo(bufferBoxLowLeft[0], bufferBoxTopRight[1]);
        surface.lineTo(bufferBoxLowLeft[0], bufferBoxLowLeft[1]);
        surface.stroke();
        if (this.zBuffer[i][j].length !== 0) {
          surface.fill();
        }
      }
    }
  }

  /**
   * Computes the basis for the screen onto which we project orthographically.
   *
   * @param {!Array.<number>} inputNormal the normal to the screen.
   */
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

  /**
   * Computes debug information the projection.
   *
   * @private
   */
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

  /**
   * Computes an orthonormal basis for the screen projection.
   *
   * @private
   */
  computeBasis() {
    this.screenBasisOrthonormal[0] = this.screenBasisUser[0].slice();
    this.screenBasisOrthonormal[1] = this.screenBasisUser[1].slice();
    let e1 = this.screenBasisOrthonormal[0];
    let e2 = this.screenBasisOrthonormal[1];
    vectorNormalize(e1);
    vectorAddVectorTimesScalar(e2, e1, -vectorScalarVector(e1, e2));
    vectorNormalize(e2);
    this.screenNormal = vectorCrossVector(e1, e2);
  }

  /**
   * Sets the bounding box as the default view window. This rescales the
   * graphics so as to "fit" the entire picture on the canvas.
   */
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
    this.scale = Math.min(candidateScaleHeight, candidateScaleWidth);
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

  /** Resets the graphics to its original view window. */
  resetView() {
    this.resetViewNoRedraw();
    this.redraw();
  }

  /**
   * Same as resetView() but doesn't repaint the canvas.
   *
   * @private
   */
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

  /**
   * Constructs controls for the graphics.
   *
   * As of writing, these consist of a single reset view button and
   * internationalization-friendly instructions how to rotate the scene.
   *
   * @private
   */
  constructControls() {
    if (this.spanControls === null) {
      return;
    }
    this.spanControls.textContent = '';
    let buttonElement = document.createElement('BUTTON');
    buttonElement.style.border = 'none';
    buttonElement.style.background = 'none';
    buttonElement.style.cursor = 'pointer';
    // Cirle arrow: "refresh button".
    buttonElement.textContent = '\u21BB';
    buttonElement.addEventListener('click', this.resetView.bind(this));
    let hintElement = document.createElement('SMALL');
    // "Shift+cursor=rotation [circle arrow]", written with utf-8 symbols.
    hintElement.textContent = ` \u21E7+\u279A=\u21BB`;
    this.spanControls.appendChild(buttonElement);
    this.spanControls.appendChild(hintElement);
  }

  /**
   * Injects a point given in screen 2d-coordinates as if the point was a
   * physical part of the 3d coordinate system.
   *
   * @param {!Array.<number>} coordinates the [x,y] coordinates in the 2d
   *     screen.
   *
   * @returns {!Array.<number>}
   * @private
   */
  coordsInjectMathScreen2dToMath3d(coordinates) {
    let output = this.screenBasisOrthonormal[0].slice();
    vectorTimesScalar(output, coordinates[0]);
    vectorAddVectorTimesScalar(
        output, this.screenBasisOrthonormal[1], coordinates[1]);
    return output;
  }

  /**
   * Projects a point in 3d onto the selected 2d math screen.
   *
   * A screen is "selected" when we got it while rotating the scene when
   * shift+clicking.
   *
   * @param {!Array.<number>} vector the [x,y,z] coordinates of the point
   *     in 3d.
   *
   * @returns {!Array.<number>}
   */
  coordsProjectMathToMathSelectedScreen2d(vector) {
    return [
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.selectedScreenBasisOrthonormal[1]),
    ];
  }

  /**
   * Projects a point in 3d onto the original (before we started rotating) 2d
   * math screen.
   *
   * @param {!Array.<number>} vector the [x,y,z] coordinates of the point
   *     in 3d.
   *
   * @returns {!Array.<number>}
   * @private
   */
  coordsProjectMathToMathScreen2d(vector) {
    return [
      vectorScalarVector(vector, this.screenBasisOrthonormal[0]),
      vectorScalarVector(vector, this.screenBasisOrthonormal[1]),
    ];
  }

  /**
   * Projects a vector in 3d to the mathematical screen, injected as a
   * two-dimensional plane in 3d.
   *
   * @param {!Array.<number>} vector the vector to be projected.
   *
   * @returns{!Array.<number>}
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
   * Same as coordsProjectToMathScreen3d but uses the selected screen.
   *
   * @param {!Array.<number>} vector the vector to be projected.
   *
   * @returns{!Array.<number>}
   */
  coordsProjectToSelectedMathScreen3d(vector) {
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

  /**
   * Converts 3d coordinates to screen coordinates (pixels).
   *
   * @param {!Array.<number>} vector the [x,y,z] coordinates of the vector.
   *
   * @returns{!Array.<number>}
   * @private
   */
  coordinatesMathToScreen(vector) {
    return [
      this.scale * vectorScalarVector(vector, this.screenBasisOrthonormal[0]) +
          this.centerX,
      (-1) * this.scale *
              vectorScalarVector(vector, this.screenBasisOrthonormal[1]) +
          this.centerY
    ];
  }

  /**
   * Sames coordinatesMathToScreen but uses the selected math screen.
   *
   * @param {!Array.<number>} vector the [x,y,z] coordinates of the vector.
   *
   * @returns{!Array.<number>}
   * @private
   */
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

  /**
   * Converts 2d screen coordinates to 2d mathematical screen coordinates.
   *
   * @param {!Array.<number>} screenPos the [x,y] coordinates on the screen.
   *
   * @returns{!Array.<number>}
   * @private
   */
  coordsScreenToMathScreen(screenPos) {
    return [
      (screenPos[0] - this.centerX) / this.scale,
      (this.centerY - screenPos[1]) / this.scale
    ];
  }

  /**
   * Converts 2d screen coordinates to 2d mathematical screen coordinates.
   *
   * @param {number} cx x coordinate of the center of the coordinae system.
   * @param {number} cy y coordinate of the center of the coordinae system.
   *
   * @returns{!Array.<number>}
   * @private
   */
  coordsScreenAbsoluteToScreen(cx, cy) {
    return getXYPositionOfObject(this.canvasContainer, cx, cy);
  }

  /**
   * Converts 2d absolute screen coordinates to 2d mathematical screen
   * coordinates.
   *
   * @param {number} screenX x coordinate of the center of the coordinae system.
   * @param {number} screenY y coordinate of the center of the coordinae system.
   *
   * @returns{!Array.<number>}
   * @private
   */
  coordsScreenAbsoluteToMathScreen(screenX, screenY) {
    return this.coordsScreenToMathScreen(
        this.coordsScreenAbsoluteToScreen(screenX, screenY));
  }

  /**
   * Converts math screen coordinates to screen coordinates.
   *
   * @param {!Array.<number>} coordinates mathematical [x,y]-coordinates.
   *
   * @returns{!Array.<number>}
   * @private
   */
  coordsMathScreenToScreen(coordinates) {
    return [
      this.scale * coordinates[0] + this.centerX,
      this.centerY - this.scale * coordinates[1]
    ];
  }

  /**
   * Partially rotates the input vector.
   *
   * The rotation is the component of larger rotation that is orthogonal to the
   * 3d injection of the mathematical screen.
   *
   * @param {!Array.<number>} input the point we are rotating.
   * @param {!Array.<number>} orthoBasis1 the first basis vector of the
   *     injection of the math screen.
   * @param {!Array.<number>} orthoBasis2 the second basis vector of the
   *     injection of the math screen.
   * @param {number} angle the angle of rotation.
   *
   * @returns{!Array.<number>}
   * @private
   */
  rotateOutOfPlane(input, orthoBasis1, orthoBasis2, angle) {
    let vComponent = input.slice();
    let scal1 = vectorScalarVector(orthoBasis1, input);
    let scal2 = vectorScalarVector(orthoBasis2, input);
    let projection = orthoBasis1.slice();
    vectorTimesScalar(projection, scal1);
    vectorAddVectorTimesScalar(projection, orthoBasis2, scal2);
    vectorAddVectorTimesScalar(vComponent, projection, -1);
    projection = orthoBasis1.slice();
    vectorTimesScalar(
        projection, Math.cos(angle) * scal1 - Math.sin(angle) * scal2);
    vectorAddVectorTimesScalar(
        projection, orthoBasis2,
        Math.sin(angle) * scal1 + Math.sin(angle) * scal2);
    return vectorPlusVector(vComponent, projection);
  }

  /**
   * Computes the difference between the clicked position and the mouse position
   * in mathemaical screen coordinates.
   *
   * @private
   */
  computePositionDelta() {
    let oldX = this.clickedPosition[0];
    let oldY = this.clickedPosition[1];
    let newX = this.mousePosition[0];
    let newY = this.mousePosition[1];
    this.positionDelta = [newX - oldX, newY - oldY];
  }

  /**
   * Rotates the scene so the selected point follows the mouse cursor.
   *
   * The rotation has two components.
   *
   * First, a radial rotation in the
   * mathematical screen plane. This is governed by the polar angle of the mouse
   * cursor position relative to the center of the mathematical screen
   * coordinate system.
   *
   * Second, a rotational component orthogonal to the injection of the
   * mathematical screen in 3d. This is governed by the shortening/enlarging the
   * polar radius of cursor position relative to the center of the mathematical
   * screen coordinate system.
   *
   * The angles and deltas are adjusted so that the point "selected" by the
   * mouse remains under the cursor as we move it around.
   *
   * @private
   */
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

  /**
   * A mechanism for boostrapping additional methods for rotation with the
   * mouse.
   *
   * TODO(tmilev): I haven't found a scheme for graphics rotation that matches
   * my intuition other than the "default" one.
   *
   * @private
   */
  rotateAfterCursorDefault() {
    if (this.mousePosition.length === 0) {
      return;
    }
    this.rotationModesAvailable[this.rotationMode]();
  }

  /**
   * Zooms the graphics in/out using the mouse wheel.
   *
   * @param {number} wheelDelta the mouse wheel change.
   * @param {number} screenX the screen x-coordinate of the mouse.
   * @param {number} screenY the screen y-coordinate of the mouse.
   */
  mouseWheel(wheelDelta, screenX, screenY) {
    let screenPos = this.coordsScreenAbsoluteToScreen(screenX, screenY);
    let mathScreenPos = this.coordsScreenToMathScreen(screenPos);
    this.scale += wheelDelta;
    if (this.scale <= 0) {
      this.scale = 1;
    }
    let intermediateScreenPos = this.coordsMathScreenToScreen(mathScreenPos);
    this.centerX = this.centerX + screenPos[0] - intermediateScreenPos[0];
    this.centerY = this.centerY + screenPos[1] - intermediateScreenPos[1];
    this.redraw();
  }

  /**
   * If rotation or panning is selected, rotates or pans the graphics.
   *
   * @param {number} screenX the screen x-coordinate of the mouse.
   * @param {number} screenY the screen y-coordinate of the mouse.
   */
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

  /**
   * Pans after the mouse cursor.
   *
   * @private
   */
  panAfterCursor() {
    let difference =
        vectorMinusVector(this.mousePosition, this.clickedPosition);
    this.centerX += difference[0] * this.scale;
    this.centerY -= difference[1] * this.scale;
    this.redraw();
  }

  /**
   * Checks wheher two points are within mouse click tolerance.
   *
   * @param {!Array.<number>} leftXY screen coordinates of the first point.
   * @param {!Array.<number>} rightXY screen coordinates of the second point.
   *
   * @returns {boolean}
   * @private
   */
  pointsWithinClickTolerance(leftXY, rightXY) {
    let squaredDistance =
        ((leftXY[0] - rightXY[0]) * (leftXY[0] - rightXY[0]) +
         (leftXY[1] - rightXY[1]) * (leftXY[1] - rightXY[1])) *
        this.scale;
    return squaredDistance < 7;
  }

  /**
   * Clicks the canvas: selects whether we rotate the coordinate system or
   * whether we are panning.
   *
   * @param {number} screenX screen x coordinate of the cursor.
   * @param {number} screenY screen y coordinate of the cursor.
   * @param {?{shiftKey: boolean}} event the mouse event.
   *
   * @private
   */
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

  /** Deselects everything: stops panning and rotation. */
  selectEmpty() {
    this.selectedElement = '';
    this.selectedScreenBasisOrthonormal = [];
    this.selectedVector = [];
    this.selectedPolarAngleChange = 0;
    this.angleNormal = 0;
  }

  /**
   * Computes which math screen vector was clicked by the mouse.
   *
   * @private
   */
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

  /**
   * Writes debug messages onto a DOM component, if available.
   *
   * @private
   */
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

  /**
   * Computes debug information about the patches.
   *
   * TODO(tmilev): this info used to be in html format, but was changed to
   * textual due to security concerns. Please cleanup the text from html tags
   * and adjust it for readability in plain text format.
   */
  infoPatchesCompute() {
    this.textPatchInfo = '';
    this.textPatchInfo += 'Z-depth: ' + this.boundingSegmentZ + '<br>';
    let allPatches = this.all3dObjects.allPatches;
    for (let i = 0; i < allPatches.length; i++) {
      let currentPatch = allPatches[i];
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
      if (i != allPatches.length - 1) {
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

  /**
   * Computes debug information about the cursor.
   *
   * TODO(tmilev): This is no longer in html format, ensure it's readable as
   * plain text.
   */
  infoMouseCompute() {
    if (!this.flagShowPerformance) {
      return;
    }
    this.textMouseInfo = '';
    let allPatches = this.all3dObjects.allPatches;
    if (this.numAccountedPatches < allPatches.length) {
      this.textMouseInfo += `<span style ='color:red'><b>Error: only ${
          this.numAccountedPatches} out of `;
      this.textMouseInfo += `${
          this.all3dObjects.allPatches
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

  /** Redraws the 3d scene. */
  redraw() {
    this.redrawStart = new Date().getTime();
    let allContours = this.all3dObjects.allContours;
    let allPatches = this.all3dObjects.allPatches;
    let points = this.all3dObjects.points;
    let theLabels = this.all3dObjects.theLabels;
    let surface = this.surface;
    surface.clearRect(0, 0, this.width, this.height);
    for (let i = 0; i < allPatches.length; i++) {
      this.computePatch(allPatches[i]);
    }
    let computePatchesTime = new Date().getTime();
    for (let i = 0; i < allContours.length; i++) {
      this.computeContour(allContours[i]);
    }
    let computeContoursTime = new Date().getTime();
    this.computeBuffers();
    let computeBuffersTime = new Date().getTime();
    if (this.flagPaintZBuffer) {
      this.paintZbuffer();
    }
    let paintBuffersTime = new Date().getTime();
    let numPainted = 0;
    for (let i = 0; i < this.patchOrder.length; i++) {
      if (this.patchOrder[i] !== -1) {
        this.paintOnePatch(allPatches[this.patchOrder[i]]);
        numPainted++;
      }
    }
    for (let i = 0; i < this.patchIsAccounted.length; i++) {
      if (this.patchIsAccounted[i] === 0) {
        this.paintOnePatch(allPatches[i]);
        numPainted++;
      }
    }
    let paintPatchTime = new Date().getTime();
    this.numContourPoints = 0;
    this.numContourPaths = 0;
    for (let i = 0; i < allContours.length; i++) {
      this.paintOneContour(allContours[i]);
    }
    let paintContourTime = new Date().getTime();
    for (let i = 0; i < points.length; i++) {
      this.paintOnePoint(points[i]);
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
          allContours.length + ' contour(s) with ';
      this.textPerformance += this.numContourPoints + ' points in ' +
          this.numContourPaths + ' sub-paths) + ';
      this.textPerformance +=
          (redrawTime - paintContourTime) + ' (paint all else). ';
    }
    this.showMessages();
  }

  /**
   * Draws a surface.
   *
   * @param {function(number,number):!Array.<number>} inputXYZFunction a
   *     two-variable function that returns a the [x,y,z]-coordinates [f(u,v),
   *     h(u,v), g(u,v)].
   * @param{!Array.<!Array.<number>>} inputUVBox u,v-variable ranges, format:
   * [[uMin, uMax], [vMin, vMax]].
   * @param {!Array.<number>} inputPatchDimensions the pair of numbers.
   * [uPatchCount, vPathCount], where we subdivide the UV-rectangle in
   * uPatchCount times uPatchCount rectangles.
   * @param{string} colorFront color of the front of the patch.
   * @param{string} colorBack color of the back of the patch.
   * @param{string} colorContour color of the patch contour.
   * @param{number} inputContourWidth the line width of the contour.
   */
  drawSurfaceCreate(
      inputXYZFunction,
      inputUVBox,
      inputPatchDimensions,
      colorFront,
      colorBack,
      colorContour,
      inputContourWidth,
  ) {
    this.drawSurface(new Surface(
        inputXYZFunction,
        inputUVBox,
        inputPatchDimensions,
        colorFront,
        colorBack,
        colorContour,
        inputContourWidth,
        ));
  }

  /**
   * Same as drawSurfaceCreate but assumes the user of the function allocated
   * the Surface object themselves.
   *
   * @param {!Surface} surface the surface.
   */
  drawSurface(surface) {
    let numUsegments = surface.patchDimensions[0];
    let numVsegments = surface.patchDimensions[1];
    let allPatches = this.all3dObjects.allPatches;
    let allContours = this.all3dObjects.allContours;
    let deltaU = surface.deltaU;
    let deltaV = surface.deltaV;
    let firstPatchIndex = allPatches.length;
    for (let i = 0; i < numUsegments; i++) {
      for (let j = 0; j < numVsegments; j++) {
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
        incomingPatch.index = allPatches.length;
        incomingPatch.internalPoint =
            surface.xyzFun(currentU + deltaU / 2, currentV + deltaV / 2);
        allPatches.push(incomingPatch);
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
        incomingContour.index = allContours.length;
        if (i > 0) {
          incomingContour
              .adjacentPatches[firstPatchIndex + numVsegments * (i - 1) + j] =
              true;
          allPatches[firstPatchIndex + numVsegments * (i - 1) + j]
              .adjacentContours[2] = allContours.length;
          allPatches[firstPatchIndex + numVsegments * (i - 1) + j]
              .traversalOrder[2] = -1;
        }
        if (i < numUsegments) {
          incomingContour
              .adjacentPatches[firstPatchIndex + numVsegments * i + j] = true;
          allPatches[firstPatchIndex + numVsegments * i + j]
              .adjacentContours[0] = allContours.length;
        }
        allContours.push(incomingContour);
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
        incomingContour.index = allContours.length;
        if (j > 0) {
          incomingContour
              .adjacentPatches[firstPatchIndex + numVsegments * i + j - 1] =
              true;
          allPatches[firstPatchIndex + numVsegments * i + j - 1]
              .adjacentContours[1] = allContours.length;
        }
        if (j < numVsegments) {
          incomingContour
              .adjacentPatches[firstPatchIndex + numVsegments * i + j] = true;
          allPatches[firstPatchIndex + numVsegments * i + j]
              .adjacentContours[3] = allContours.length;
          allPatches[firstPatchIndex + numVsegments * i + j].traversalOrder[3] =
              -1;
        }
        allContours.push(incomingContour);
      }
    }
  }
}

/** Convenience wrapper for a few functions and classes. */
class Drawing {
  constructor() {
    this.firstCriticalRunTimeError = '';
    this.firstCanvas = null;
    this.numberOfControlsConstructed = 0;
    this.plotUpdaters = {};
    this.Surface = Surface;
    this.CurveThreeD = CurveThreeD;
  }

  /**
   * Processes an error message.
   * 
   * @param{string} input
   */
  calculatorError(input) {
    console.log(input);
  }

  /**
   * Converts a mouse wheel event to a triple: delta (change in wheel position),
   * x and y (cursor position during the wheel event).
   *
   * @param {{preventDefault: !Function, stopPropagation: !Function, detail:
   *     (number|null|undefined), wheelDelta: number, clientX: number, clientY:
   *     number}} e the mouse wheel event.
   * @returns{{delta:number, x:number, y:number}}
   */
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
 *
 * Let and const would not be allowed here as they would shadow the externally
 * attached module variable. Var however does not shadow the external variable.
 */
/** @suppress {lintChecks} */
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
