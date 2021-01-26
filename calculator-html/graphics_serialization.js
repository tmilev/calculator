"use strict";

const CanvasTwoD = require("./graphics").CanvasTwoD;

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

  oneTwoDimensionalObject(
    plot,
    /**@type{CanvasTwoD} */
    canvas,
  ) {
    let plotType = plot["plotType"];

    switch (plotType) {
      case "plotFunction":
        canvas.drawFunction(
          this.functionFromString(plot[this.labels.functionLabel]),
          this.interpretStringToNumber(plot[this.labels.left]),
          this.interpretStringToNumber(plot[this.labels.right]),
          this.interpretStringToNumber(plot[this.labels.numberOfSegments]),
          plot[this.labels.color],
          this.interpretStringToNumber(plot[this.labels.lineWidth]),
        );
        return;
      case "coordinateAxes":
        canvas.drawCoordinateAxes();
        return;
      case "computeViewWindow":
        canvas.computeViewWindow();
        return;
      default:
        throw `Uknown plot type: ${plotType}.`;
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