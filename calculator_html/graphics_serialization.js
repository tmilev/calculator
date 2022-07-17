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
      points: "points",
      onePoint: "point",
      text: "text",
      coordinateFunctions: "coordinateFunctions",
      viewWindow: "viewWindow",
      manifoldImmersion: "manifoldImmersion",
      defaultLength: "defaultLength",
    };
  }

  /**@returns {CanvasTwoD|CanvasThreeD|null} */
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
  ) {
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        return this.twoDimensionalGraphics(input, canvas, controls, messages, sliders);
      case "threeDimensional":
        return this.threeDimensionalGraphics(input, canvas, controls, messages, sliders);
      default:
        throw `Unknown graphics type ${graphicsType}.`;
    }
  }

  /**@returns {CanvasTwoD} */
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
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    let canvas = new CanvasTwoD(canvasElement, controls, messages);
    canvas.initialize();
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneTwoDimensionalObject(plotObjects[i], canvas, sliders);
    }
    canvas.redraw();
    return canvas;
  }

  /**@returns{CanvasThreeD} */
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
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    let canvas = new CanvasThreeD(canvasElement, controls, messages,);
    canvas.initialize();
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneThreeDimensionalObject(plotObjects[i], canvas, sliders);
    }
    if (input["setBoundingBoxAsDefaultViewWindow"]) {
      canvas.setBoundingBoxAsDefaultViewWindow();
    }
    canvas.redraw();
    return canvas;
  }

  oneTwoDimensionalObject(
    plot,
    /**@type{CanvasTwoD} */
    canvas,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
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
        let functionConstructed = this.functionFromObject(functionObject, sliders);
        parameterValues = this.getSliderValuesFromInput(sliders, functionObject);
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
        let immersion = this.functionFromObject(manifoldImmersion, sliders);
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
        parameterValues = this.getSliderValuesFromInput(sliders, plot);
        let coordinateFunctionArray = [
          this.functionFromBodyAndArguments(
            coordinateFunctions[0], variableArguments, parameterValues,
          ),
          this.functionFromBodyAndArguments(
            coordinateFunctions[1], variableArguments, parameterValues,
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
        parameterValues = this.getSliderValuesFromInput(sliders, plot);
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
        parameterValues = this.getSliderValuesFromInput(sliders, plot);
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

  /**@returns {Object.<string,string>} */
  getSliderValuesFromInput(
    /**@type{Object.<string,HTMLInputElement>} */
    sliders,
    plot,
  ) {
    return this.getSliderValues(sliders, plot[this.labels.parameters]);
  }

  /**@returns {Object.<string,string>} */
  getSliderValues(
    /**@type{Object.<string,HTMLInputElement>} */
    sliders,
    /**@type{string[]} */
    parameters,
  ) {
    let parameterValues = {};
    for (let i = 0; i < parameters.length; i++) {
      let parameterName = parameters[i];
      if (parameterName in sliders) {
        parameterValues[parameterName] = sliders[parameterName].value;
      } else {
        throw `Parameter value ${parameterName} not found.`;
      }
    }
    return parameterValues;
  }

  oneThreeDimensionalObject(
    plot,
    /**@type{CanvasThreeD} */
    canvas,
    /**@type{Object.<string,HTMLElement>} */
    sliders,
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
        let parameterValues = this.getSliderValuesFromInput(sliders, plot);
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

  /**@return{number} */
  interpretStringToNumber(
    /**@type{string} */
    input,
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    let extraJavascript = this.getParametersJavascript(parameterValues);
    return Function(`"use strict"; ${extraJavascript} return (${input});`)();
  }

  /**@return{number[]} */
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

  /**@return{number[][]} */
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
  ) {
    /**@type{string[]} */
    let inputArguments = input[this.labels.arguments];
    /**@type{string[]} */
    let body = input[this.labels.body];
    let parameterValues = this.getSliderValues(sliders, input[this.labels.parameters]);
    return this.functionFromBodyAndArguments(body, inputArguments, parameterValues);
  }

  getParametersJavascript(
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    let parametersJavascript = [];
    for (let label in parameterValues) {
      parametersJavascript.push(`let ${label}=${parameterValues[label]};`);
    }
    return parametersJavascript.join("");
  }

  functionFromBodyAndArguments(
    /**@type{string} */
    body,
    /**@type{string[]} */
    inputArguments,
    /**@type{Object<string, string>} */
    parameterValues,
  ) {
    body = this.getParametersJavascript(parameterValues) + body;
    if (inputArguments.length === 1) {
      return Function(inputArguments[0], body);
    } else if (inputArguments.length === 2) {
      return Function(inputArguments[0], inputArguments[1], body);
    } else if (inputArguments.length === 3) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[2], body);
    } else if (inputArguments.length === 4) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[3], body);
    } else if (inputArguments.length === 5) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[3], inputArguments[4], body);
    } else if (inputArguments.length === 6) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[3], inputArguments[4], inputArguments[5], body);
    } else if (inputArguments.length === 7) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[3], inputArguments[4], inputArguments[5], inputArguments[6], body);
    } else if (inputArguments.length === 8) {
      return Function(inputArguments[0], inputArguments[1], inputArguments[3], inputArguments[4], inputArguments[5], inputArguments[6], inputArguments[7], body);
    }
    return null;
  }
}

let graphicsSerialization = new GraphicsSerialization();

module.exports = {
  graphicsSerialization,
};