"use strict";

const CanvasTwoD = require("./graphics").CanvasTwoD;
const CanvasThreeD = require("./graphics").Canvas;

class GraphicsSerialization {
  constructor() {
    this.labels = {
      functionLabel: "function",
      left: "left",
      right: "right",
      numberOfSegments: "numberOfSegments",
      color: "color",
      lineWidth: "lineWidth",
      body: "body",
      arguments: "arguments",
      points: "points",
      onePoint: "point",
      text: "text",
    };
  }

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
        this.twoDimensionalGraphics(input, canvas, controls, messages);
        return;
      case "threeDimensional":
        this.threeDimensionalGraphics(input, canvas, controls, messages);
        return;
      default:
        throw `Unknown graphics type ${graphicsType}.`;
    }
  }

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
  }

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
  }

  oneTwoDimensionalObject(
    plot,
    /**@type{CanvasTwoD} */
    canvas,
  ) {
    let plotType = plot["plotType"];
    let functionString = plot[this.labels.functionLabel];
    let left = plot[this.labels.left];
    let right = plot[this.labels.right];
    let numberOfSegments = plot[this.labels.numberOfSegments];
    let color = plot[this.labels.color];
    let lineWidth = plot[this.labels.lineWidth];
    let points = plot[this.labels.points];
    let onePoint = plot[this.labels.onePoint];
    let text = plot[this.labels.text];
    switch (plotType) {
      case "plotFunction":
        canvas.drawFunction(
          this.functionFromString(functionString),
          this.interpretStringToNumber(left),
          this.interpretStringToNumber(right),
          this.interpretStringToNumber(numberOfSegments),
          color,
          this.interpretStringToNumber(lineWidth),
        );
        return;
      case "coordinateAxes":
        canvas.drawCoordinateAxes();
        return;
      case "computeViewWindow":
        canvas.computeViewWindow();
        return;
      case "path":
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
    let plotType = plot["plotType"];
    let points = plot["points"];
    let color = plot["color"];
    let lineWidth = plot["lineWidth"];
    let point = plot["point"];
    let text = plot["text"];
    switch (plotType) {
      case "segment":
        canvas.drawLine(points[0], points[1], color, lineWidth);
        return;
      case "label":
        canvas.drawText(point, text, color);
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

  functionFromString(
    input,
  ) {
    /**@type{string[]} */
    let inputArguments = input[this.labels.arguments];
    /**@type{string[]} */
    let body = input[this.labels.body];
    //return Function(inputArguments, body);
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
  }
}

let graphicsSerialization = new GraphicsSerialization();

module.exports = {
  graphicsSerialization,
};