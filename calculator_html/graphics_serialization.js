"use strict";

const CanvasTwoD = require("./graphics").CanvasTwoD;
const CanvasThreeD = require("./graphics").Canvas;

class GraphicsSerialization {
  constructor() {
    this.labels = {
      plotType: "plotType",
      functionLabel: "function",
      variableRanges: "variableRange",
      left: "left",
      right: "right",
      numberOfSegments: "numberOfSegments",
      color: "color",
      colorFill: "colorFill",
      colorFront: "colorFront",
      colorBack: "colorBack",
      colorContour: "colorContour",
      lineWidth: "lineWidth",
      body: "body",
      arguments: "arguments",
      parameters: "parameters",
      parameterLetter: "parameterLetter",
      points: "points",
      onePoint: "point",
      text: "text",
      coordinateFunctions: "coordinateFunctions",
      viewWindow: "viewWindow",
      manifoldImmersion: "manifoldImmersion",
      defaultLength: "defaultLength",
    };
  }

  /** 
   * Constructs a canvas from JSON.
   *  
   * @returns {CanvasTwoD|CanvasThreeD|null} 
   */
  fromJSON(
    input,
    /**@type{HTMLCanvasElement} */
    canvas,
    /**@type{HTMLElement} */
    controls,
    /**@type{HTMLElement} */
    messages,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    if (outputUsedSliders === null || outputUsedSliders === undefined) {
      throw "Missing output sliders";
    }
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        return this.twoDimensionalGraphics(input, canvas, controls, messages, sliders, outputUsedSliders);
      case "threeDimensional":
        return this.threeDimensionalGraphics(input, canvas, controls, messages, sliders, outputUsedSliders);
      default:
        throw `Unknown graphics type ${graphicsType}.`;
    }
  }

  /** Redraws the canvas, without re-creating it. */
  redrawFromJSON(
    /**@type{CanvasTwoD|CanvasThreeD}*/
    canvas,
    input,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
  ) {
    canvas.clear();
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        return this.plotTwoDimensionalGraphics(canvas, input, sliders, {});
      case "threeDimensional":
        return this.plotThreeDimensionalGraphics(canvas, input, sliders, {});
      default:
        throw `Unknown graphics type ${graphicsType}.`;
    }
  }

  /** 
   * Creates two-dimensional graphics.
   * 
   * @return {CanvasTwoD} 
   */
  twoDimensionalGraphics(
    input,
    /**@type{HTMLCanvasElement}*/
    canvasElement,
    /**@type{HTMLElement} */
    controls,
    /**@type{HTMLElement} */
    messages,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let canvas = new CanvasTwoD(canvasElement, controls, messages);
    canvas.initialize();
    return this.plotTwoDimensionalGraphics(canvas, input, sliders, outputUsedSliders);
  }

  /**
   * Creates three-dimensional graphics.
   * 
   * @returns{CanvasThreeD} 
   */
  threeDimensionalGraphics(
    input,
    /**@type{HTMLCanvasElement}*/
    canvasElement,
    /**@type{HTMLElement} */
    controls,
    /**@type{HTMLElement} */
    messages,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let canvas = new CanvasThreeD(canvasElement, controls, messages);
    canvas.initialize();
    return this.plotThreeDimensionalGraphics(canvas, input, sliders, outputUsedSliders);
  }

  /** 
   * Plots two dimensional graphics on an already created graphics object.
   * 
   * @returns {CanvasTwoD} 
   */
  plotTwoDimensionalGraphics(
    /** @type{CanvasTwoD}*/
    canvas,
    input,
    /** @type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneTwoDimensionalObject(plotObjects[i], canvas, sliders, outputUsedSliders);
    }
    this.writeParameters(input[this.labels.parameters], canvas, sliders);
    canvas.redraw();
    return canvas;
  }

  writeParameters(
    /** @type{Array.<string>}*/
    parameterNames,
    /** @type{CanvasTwoD|CanvasThreeD} */
    canvas,
    /** @type{Object.<string,HTMLElement>} */
    sliders,
  ) {
    canvas.parameterNames = parameterNames;
    for (let i = 0; i < parameterNames.length; i++) {
      let parameterName = parameterNames[i];
      if (parameterName in sliders) {
        canvas.parameterValues[i] = parseFloat(sliders[parameterName].value);
      } else {
        throw `Parameter value ${parameterName} not found.`;
      }
    }
  }

  /** 
   * Plots three dimensional graphics on an already created graphics object.
   * 
   * @return{CanvasThreeD}
   */
  plotThreeDimensionalGraphics(
    /**@type{CanvasThreeD}*/
    canvas,
    input,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneThreeDimensionalObject(plotObjects[i], canvas, sliders, outputUsedSliders);
    }
    if (input["setBoundingBoxAsDefaultViewWindow"]) {
      canvas.setBoundingBoxAsDefaultViewWindow();
    }
    canvas.redraw();
    return canvas;    
  }

  accountUsedParameters(
    /** @type{Object.<string,string>|null} */
    parameters,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) { 
    if (parameters === null) {
      return;
    }
    for (let label in parameters){
      outputUsedSliders[label] = label;
    }
  }

  oneTwoDimensionalObject(
    plot,
    /**@type{CanvasTwoD} */
    canvas,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let plotType = plot[this.labels.plotType];
    let functionObject = plot[this.labels.functionLabel];
    let left = plot[this.labels.left];
    let right = plot[this.labels.right];
    let numberOfSegments = plot[this.labels.numberOfSegments];
    let color = plot[this.labels.color];
    let colorFill = plot[this.labels.colorFill];
    let lineWidth = plot[this.labels.lineWidth];
    let points = plot[this.labels.points];
    let onePoint = plot[this.labels.onePoint];
    let text = plot[this.labels.text];
    let viewWindow = plot[this.labels.viewWindow];
    let variableArguments = plot[this.labels.arguments];
    let variableRanges = plot[this.labels.variableRanges];
    let manifoldImmersion = plot[this.labels.manifoldImmersion];
    let coordinateFunctions = plot[this.labels.coordinateFunctions];
    let parameterValues = null;
    switch (plotType) {
      case "plotFunction":
        let functionConstructed = this.functionFromObject(functionObject, sliders, outputUsedSliders);
        canvas.drawFunction(
          functionConstructed,
          this.interpretStringToNumber(left, parameterValues),
          this.interpretStringToNumber(right, parameterValues),
          this.interpretStringToNumber(numberOfSegments, parameterValues),
          color,
          this.interpretStringToNumber(lineWidth, parameterValues),
        );
        return;
      case "plotFillStart":
        canvas.plotFillStart(colorFill);
        return;
      case "plotFillFinish":
        canvas.plotFillFinish();
        return;
      case "axesGrid":
        canvas.drawGrid();
        return;
      case "coordinateAxes":
        canvas.drawCoordinateAxes();
        return;
      case "setViewWindow":
        canvas.setViewWindow(viewWindow[0], viewWindow[1]);
        return;
      case "computeViewWindow":
        canvas.computeViewWindow();
        return;
      case "plotDirectionField":
        let immersion = this.functionFromObject(manifoldImmersion, sliders, outputUsedSliders);
        canvas.drawVectorField(
          immersion,
          true,
          variableRanges[0],
          variableRanges[1],
          numberOfSegments,
          plot[this.labels.defaultLength],
          color,
          lineWidth,
        );
        return;
      case "parametricCurve":
        let coordinateFunctionArray = [
          this.functionFromBodyAndArguments(
            coordinateFunctions[0], this.getArguments(plot),
          ),
          this.functionFromBodyAndArguments(
            coordinateFunctions[1], this.getArguments(plot),
          ),
        ];
        canvas.drawCurve(
          coordinateFunctionArray,
          this.interpretStringToNumber(variableRanges[0], parameterValues),
          this.interpretStringToNumber(variableRanges[1], parameterValues),
          numberOfSegments, color, lineWidth,
        );
        return;
      case "points":
        canvas.drawPoints(this.interpretListListStringsAsNumbers(points, parameterValues), color);
        return;
      case "pathFilled":
        canvas.drawPathFilled(points, color, colorFill);
      case "path":
      case "segment":
        canvas.drawPath(points, color, lineWidth);
        return;
      case "label":
        canvas.drawText(onePoint, text, color);
        return;
      case "escapeMap":
        let functionX =
           this.functionFromBodyAndArguments(
            coordinateFunctions[0], variableArguments, parameterValues,
          );
        let functionY =
          this.functionFromBodyAndArguments(
            coordinateFunctions[1], variableArguments, parameterValues,
          );
        canvas.drawEscapeMap(functionX, functionY);
        return;
      default:
        throw `Unknown plot type: ${plotType}.`;
    }
  }

  oneThreeDimensionalObject(
    plot,
    /**@type{CanvasThreeD} */
    canvas,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    let plotType = plot[this.labels.plotType];
    let variableRanges = plot[this.labels.variableRanges];
    let numberOfSegments = plot[this.labels.numberOfSegments];
    let coordinateFunctions = plot[this.labels.coordinateFunctions];
    let inputArguments = plot[this.labels.arguments];
    let colorFront = plot[this.labels.colorFront];
    let colorBack = plot[this.labels.colorBack];
    let colorContour = plot[this.labels.colorContour];
    let points = plot["points"];
    let color = plot["color"];
    let lineWidth = plot[this.labels.lineWidth];
    let point = plot["point"];
    let text = plot["text"];
    switch (plotType) {
      case "segment":
        canvas.drawLine(points[0], points[1], color, lineWidth);
        return;
      case "label":
        canvas.drawText(point, text, color);
        return;
      case "setProjectionScreen":
        canvas.screenBasisUserDefault = plot["projectionScreen"];
        return;
      case "surface":
        let convertedRanges = [[
          this.interpretStringToNumber(variableRanges[0][0], parameterValues),
          this.interpretStringToNumber(variableRanges[0][1], parameterValues),
        ], [
          this.interpretStringToNumber(variableRanges[1][0], parameterValues),
          this.interpretStringToNumber(variableRanges[1][1], parameterValues),
        ]];
        let convertedSegments = [
          this.interpretStringToNumber(numberOfSegments[0], parameterValues),
          this.interpretStringToNumber(numberOfSegments[1], parameterValues),
        ];
        canvas.drawSurfaceCreate(
          this.functionFromBodyAndArguments(coordinateFunctions, inputArguments, parameterValues),
          convertedRanges,
          convertedSegments,
          colorFront,
          colorBack,
          colorContour,
          lineWidth,
        );
        return;
      default:
        throw `Unknown plot type: ${plotType}.`;
    }
  }

  /** 
   * Extracts a number from javascript.
   * 
   * @return{number} 
   */
  interpretStringToNumber(
    /**@type{string} */
    input,
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    return Function(`"use strict"; return (${input});`)();
  }

  /** 
   * Interprets a list of strings from javascript.
   * 
   * @return{number[]} 
   */
  interpretListStringsAsNumbers(
    /**@type{string[]} */
    input,
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    let result = [];
    for (let i = 0; i < input.length; i++) {
      result.push(this.interpretStringToNumber(input[i], parameterValues));
    }
    return result;
  }

  /** 
   * Interprets a list of list of strings from javascript.
   * 
   * @return{number[][]} 
   */
  interpretListListStringsAsNumbers(
    /**@type{string[][]} */
    input,
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    let result = [];
    for (let i = 0; i < input.length; i++) {
      result.push(this.interpretListStringsAsNumbers(input[i], parameterValues));
    }
    return result;
  }

  functionFromObject(
    input,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
    /**@type{Object.<string,string>} */
    outputUsedSliders,
  ) {
    if (outputUsedSliders === null || outputUsedSliders === undefined) {
      throw "Missing output sliders";
    }
    /** @type{string} */
    let body = input[this.labels.body];
    return this.functionFromBodyAndArguments(body, this.getArguments(input));
  }

  /** 
   * Gets argument list from function specification. 
   * 
   * @return {Array.<string>} 
   */
  getArguments(input) {
    /** @type{string[]} */
    let result = input[this.labels.arguments].slice();
    let parameterLetter = input[this.labels.parameterLetter];
    if (parameterLetter !== "" && parameterLetter !== undefined) {
      result.push(parameterLetter);
    }
    return result;
  }

  functionFromBodyAndArguments(
    /** @type{string} */
    body,
    /** @type{string[]} */
    inputArguments
  ) {
    if (inputArguments.length === 1) {
      return Function(
        inputArguments[0],
        body,
      );
    } else if (inputArguments.length === 2) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        body,
      );
    } else if (inputArguments.length === 3) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        body,
      );
    } else if (inputArguments.length === 4) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        inputArguments[3],
        body,
      );
    } else if (inputArguments.length === 5) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        inputArguments[3],
        inputArguments[4],
        body,
      );
    } else if (inputArguments.length === 6) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        inputArguments[3],
        inputArguments[4],
        inputArguments[5],
        body,
      );
    } else if (inputArguments.length === 7) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        inputArguments[3],
        inputArguments[4],
        inputArguments[5],
        inputArguments[6],
        body,
      );
    } else if (inputArguments.length === 8) {
      return Function(
        inputArguments[0],
        inputArguments[1],
        inputArguments[2],
        inputArguments[3],
        inputArguments[4],
        inputArguments[5],
        inputArguments[6],
        inputArguments[7],
        body,
      );
    }
    return null;
  }
}

let graphicsSerialization = new GraphicsSerialization();

module.exports = {
  graphicsSerialization,
};