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
      points: "points",
      onePoint: "point",
      text: "text",
      coordinateFunctions: "coordinateFunctions",
      viewWindow: "viewWindow",
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
  ) {
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        return this.twoDimensionalGraphics(input, canvas, controls, messages);
      case "threeDimensional":
        return this.threeDimensionalGraphics(input, canvas, controls, messages);
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
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    let canvas = new CanvasTwoD(canvasElement, controls, messages);
    canvas.initialize();
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneTwoDimensionalObject(plotObjects[i], canvas);
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
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    let canvas = new CanvasThreeD(canvasElement, controls, messages);
    canvas.initialize();
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneThreeDimensionalObject(plotObjects[i], canvas);
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
    switch (plotType) {
      case "plotFunction":
        canvas.drawFunction(
          this.functionFromObject(functionObject),
          this.interpretStringToNumber(left),
          this.interpretStringToNumber(right),
          this.interpretStringToNumber(numberOfSegments),
          color,
          this.interpretStringToNumber(lineWidth),
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
      case "parametricCurve":
        let coordinateFunctions = [
          this.functionFromBodyAndArguments(
            plot.coordinateFunctions[0], variableArguments,
          ),
          this.functionFromBodyAndArguments(
            plot.coordinateFunctions[1], variableArguments,
          ),
        ];
        canvas.drawCurve(
          coordinateFunctions,
          this.interpretStringToNumber(variableRanges[0]),
          this.interpretStringToNumber(variableRanges[1]),
          numberOfSegments, color, lineWidth,
        );
        return;
      case "points":
      case "path":
      case "segment":
        canvas.drawPath(points, color, lineWidth);
        return;
      case "label":
        canvas.drawText(onePoint, text, color);
        return;
      default:
        throw `Unknown plot type: ${plotType}.`;
    }
  }

  oneThreeDimensionalObject(
    plot,
    /**@type{CanvasThreeD} */
    canvas,
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
      case "surface":
        let convertedRanges = [[
          this.interpretStringToNumber(variableRanges[0][0]),
          this.interpretStringToNumber(variableRanges[0][1]),
        ], [
          this.interpretStringToNumber(variableRanges[1][0]),
          this.interpretStringToNumber(variableRanges[1][1]),
        ]];
        canvas.drawSurfaceCreate(
          this.functionFromBodyAndArguments(coordinateFunctions, inputArguments),
          convertedRanges,
          numberOfSegments,
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

  interpretStringToNumber(
    /**@type{string} */
    input,
  ) {
    return Function(`"use strict"; return (${input});`)();
  }

  functionFromObject(
    input,
  ) {
    /**@type{string[]} */
    let inputArguments = input[this.labels.arguments];
    /**@type{string[]} */
    let body = input[this.labels.body];
    return this.functionFromBodyAndArguments(body, inputArguments);
  }

  functionFromBodyAndArguments(
    body, inputArguments,
  ) {
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