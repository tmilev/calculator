"use strict";

const EquationEditor = require("./equation_editor/src/equation_editor");
const CanvasTwoD = require("./graphics").CanvasTwoD;
const CanvasThreeD = require("./graphics").Canvas;
const Graphics = require("./graphics");

class SliderUpdater {
  constructor(
    /** @type {function(Object.<string,number>)} */
    inputSlideAndFormUpdateFunction,
  ) {
    /** @type {Object.<string, HTMLElement>} */
    this.sliders = {};
    /** @type {Object.<string, HTMLElement>} */
    this.inputsForms = {};
    /** @type {function(string, string)} */
    this.slideAndFormUpdateFunction = inputSlideAndFormUpdateFunction;
  }

  updateTwoSlidersFromPoint(
    /** @type {number} */
    x,
    /** @type {number} */
    y,
    /** @type {string} */
    nameX,
    /** @type {string} */
    nameY,
    /** @type {CanvasTwoD} */
    canvas,
  ) {
    canvas.parameterValues[canvas.parameterNames[nameX]] = x;
    canvas.parameterValues[canvas.parameterNames[nameY]] = y;
    let nameMap = {};
    nameMap[nameX] = x;
    nameMap[nameY] = y;
    this.slideAndFormUpdateFunction(nameMap);
  }

  accountOneSlider(
    /** @type {HTMLInputElement} */
    slider,
  ) {
    let sliderName = slider.getAttribute("name");
    if (
      sliderName === "" ||
      sliderName === null ||
      sliderName === undefined
    ) {
      return;
    }
    if (slider.type !== "range") {
      if (sliderName in this.inputsForms) {
        console.log(
          `Input name ${sliderName} already present.`
        );
        return;
      }
      this.inputsForms[sliderName] = slider;
      return;
    }
    if (sliderName in this.sliders) {
      console.log(
        `Slider name ${sliderName} already present.`
      );
      return;
    }
    this.sliders[sliderName] = slider;
  }
}

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
      parametersOnTheGraphLetter: "parametersOnTheGraphLetter",
      parametersOnTheGraph: "parametersOnTheGraph",
      mandelbrotMode: "mandelbrotMode",
      points: "points",
      onePoint: "point",
      text: "text",
      coordinateFunctions: "coordinateFunctions",
      viewWindow: "viewWindow",
      manifoldImmersion: "manifoldImmersion",
      defaultLength: "defaultLength",
      layerLabel: "layerLabel",
    };
  }

  /** 
   * Constructs a canvas from JSON.
   *  
   * @return {CanvasTwoD|CanvasThreeD|null} 
   */
  fromJSON(
    input,
    /** @type {HTMLCanvasElement} */
    canvas,
    /** @type {HTMLElement} */
    controls,
    /** @type {HTMLElement} */
    messages,
    /** @type {SliderUpdater} */
    sliders,
    /** @type {HTMLElement|null|undefined} */
    layerContainer,
  ) {
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        return this.twoDimensionalGraphics(
          input,
          canvas,
          controls,
          messages,
          sliders,
          layerContainer,
        );
      case "threeDimensional":
        return this.threeDimensionalGraphics(
          input, canvas, controls, messages, sliders, layerContainer,
        );
      default:
        throw `Unknown graphics type ${graphicsType}.`;
    }
  }

  /** Redraws the canvas, without re-creating it. */
  redrawFromJSON(
    /** @type {CanvasTwoD|CanvasThreeD} */
    canvas,
    input,
    /** @type {SliderUpdater} */
    sliders,
  ) {
    let graphicsType = input["graphicsType"];
    switch (graphicsType) {
      case "twoDimensional":
        canvas.redraw();
        return;
      case "threeDimensional":
        canvas.clear();
        this.plotThreeDimensionalGraphics(canvas, input, sliders);
        return;
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
    /** @type {HTMLCanvasElement} */
    canvasElement,
    /** @type {HTMLElement} */
    controls,
    /** @type {HTMLElement} */
    messages,
    /** @type {SliderUpdater} */
    sliders,
    /** @type {HTMLElement|null|undefined} */
    layerContainer,
  ) {
    let drawLatexOnCanvas = null;
    /** @type {EquationEditor.EquationEditor|null} */
    let equationEditor = null;
    if (controls !== null && controls !== undefined) {
      controls.textContent = '';
      let editorContainer = document.createElement("span");
      equationEditor = new EquationEditor.EquationEditor(
        editorContainer,
        new EquationEditor.EquationEditorOptions({
          editable: false
        }),
      );
      editorContainer.style.display = "inline-block";
      editorContainer.style.maxHeight = "0";
      editorContainer.style.maxWidth = "0";
      editorContainer.style.width = "0";
      editorContainer.style.height = "0";
      editorContainer.style.overflow = "clip";
      controls.appendChild(editorContainer);
      drawLatexOnCanvas = (
        /** @type {number[]} */
        textLocation,
        /** @type {string} */
        latex,
      ) => {
        equationEditor.writeLatex(latex);
        equationEditor.drawOnCanvasAtLocation(textLocation);
      };
    }
    let canvas = new CanvasTwoD(
      canvasElement,
      controls,
      messages,
      drawLatexOnCanvas,
      layerContainer,
    );
    canvas.initialize();
    if (equationEditor !== null) {
      equationEditor.canvasTwoDContext = canvas.surface;
    }
    return this.plotTwoDimensionalGraphics(
      canvas,
      input,
      sliders,
    );
  }

  /**
   * Creates three-dimensional graphics.
   * 
   * @return {CanvasThreeD} 
   */
  threeDimensionalGraphics(
    input,
    /** @type {HTMLCanvasElement} */
    canvasElement,
    /** @type {HTMLElement} */
    controls,
    /** @type {HTMLElement} */
    messages,
    /** @type {SliderUpdater} */
    sliders,
    /** @type {HTMLElement|undefined} */
    layerContainer,
  ) {
    let canvas = new CanvasThreeD(
      canvasElement, controls, messages, layerContainer,
    );
    canvas.initialize();
    return this.plotThreeDimensionalGraphics(canvas, input, sliders);
  }

  /** 
   * Plots two dimensional graphics on an already created graphics object.
   * 
   * @return {CanvasTwoD} 
   */
  plotTwoDimensionalGraphics(
    /** @type {CanvasTwoD} */
    canvas,
    input,
    /** @type {SliderUpdater} */
    sliders,
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    this.writeParameters(input[this.labels.parameters], canvas, sliders);
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneTwoDimensionalObjectWithLabel(plotObjects[i], canvas, sliders);
    }
    canvas.redraw();
    return canvas;
  }

  writeParameters(
    /** @type {Array.<string>} */
    parameterNames,
    /** @type {CanvasTwoD|CanvasThreeD} */
    canvas,
    /** @type {SliderUpdater} */
    sliders,
  ) {
    for (let i = 0; i < parameterNames.length; i++) {
      let parameterName = parameterNames[i];
      canvas.parameterNames[parameterName] = i;
      if (parameterName in sliders.sliders) {
        canvas.parameterValues[i] = parseFloat(sliders.sliders[parameterName].value);
      } else {
        throw `Parameter value ${parameterName} not found.`;
      }
    }
  }

  /** 
   * Plots three dimensional graphics on an already created graphics object.
   * 
   * @return {CanvasThreeD}
   */
  plotThreeDimensionalGraphics(
    /** @type {CanvasThreeD} */
    canvas,
    input,
    /** @type {SliderUpdater} */
    sliders,
  ) {
    let plotObjects = input["plotObjects"];
    if (!Array.isArray(plotObjects)) {
      throw `Plot objects not an array.`;
    }
    this.writeParameters(input[this.labels.parameters], canvas, sliders);
    for (let i = 0; i < plotObjects.length; i++) {
      this.oneThreeDimensionalObject(plotObjects[i], canvas);
    }
    if (input["setBoundingBoxAsDefaultViewWindow"]) {
      canvas.setBoundingBoxAsDefaultViewWindow();
    }
    canvas.redraw();
    return canvas;
  }

  oneTwoDimensionalObjectWithLabel(
    plot,
    /** @type {CanvasTwoD} */
    canvas,
    /** @type {SliderUpdater} */
    sliderUpdater,
  ) {
    let objectsAtStart = canvas.drawObjects.length;
    this.oneTwoDimensionalObject(plot, canvas, sliderUpdater);
    /** @type {string} */
    let layer = plot[this.labels.layerLabel];
    if (layer === null || layer === undefined) {
      layer = "";
    }
    for (let i = objectsAtStart; i < canvas.drawObjects.length; i++) {
      canvas.layers.push(layer);
    }
  }

  oneTwoDimensionalObject(
    plot,
    /** @type {CanvasTwoD} */
    canvas,
    /** @type {SliderUpdater} */
    sliderUpdater,
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
    let variableAndParameterNames = this.getArguments(plot);

    let parameterNames = this.getParameters(plot);
    let parameterValues = canvas.parameterValues;

    let variableRanges = plot[this.labels.variableRanges];
    let manifoldImmersion = plot[this.labels.manifoldImmersion];
    let coordinateFunctions = plot[this.labels.coordinateFunctions];
    let parametersFromInputBoxes = plot[this.labels.parameters];
    let parametersOnTheGraph = plot[this.labels.parametersOnTheGraph];
    if (parametersOnTheGraph === undefined) {
      parametersOnTheGraph = [];
    }
    let leftEvaluated;
    let rightEvaluated;
    let segmentCount;
    let lineWidthEvaluated;
    switch (plotType) {
      case "plotFunction":
        let functionConstructed = this.functionFromObject(functionObject);
        leftEvaluated = this.interpretStringToNumberOrFunction(
          left, parameterNames, parameterValues,
        );
        rightEvaluated = this.interpretStringToNumberOrFunction(
          right, parameterNames, parameterValues,
        );
        segmentCount = this.interpretStringToNumberOrFunction(
          numberOfSegments, parameterNames, parameterValues,
        );
        lineWidthEvaluated = this.interpretStringToNumberOrFunction(
          lineWidth, parameterNames, parameterValues,
        );
        canvas.drawFunction(
          functionConstructed,
          leftEvaluated,
          rightEvaluated,
          segmentCount,
          color,
          lineWidthEvaluated,
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
        let immersion = this.functionFromObject(manifoldImmersion);
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
        leftEvaluated = this.interpretStringToNumberOrFunction(
          variableRanges[0], parameterNames, parameterValues,
        );
        rightEvaluated = this.interpretStringToNumberOrFunction(
          variableRanges[1], parameterNames, parameterValues,
        );
        segmentCount = this.interpretStringToNumberOrFunction(
          numberOfSegments, parameterNames, parameterValues,
        );
        canvas.drawCurve(
          coordinateFunctionArray,
          leftEvaluated,
          rightEvaluated,
          segmentCount,
          color,
          lineWidth,
        );
        return;
      case "points":
        canvas.drawPoints(
          this.interpretListListStringsAsNumbersOrFunctions(
            points, parameterNames, parameterValues,
          ), color
        );
        return;
      case "selectablePoint":
        let pointComputer = this.interpretListListStringsAsNumbersOrFunctions(
          points, parameterNames, parameterValues
        )[0];
        let nameX = parametersFromInputBoxes[0];
        let nameY = parametersFromInputBoxes[1];
        canvas.drawSelectablePoint(
          pointComputer,
          (x, y) => {
            sliderUpdater.updateTwoSlidersFromPoint(
              x, y, nameX, nameY, canvas,
            );
          }
        );
        return;
      case "pathFilled":
        canvas.drawPathFilled(points, color, colorFill);
      // no break is intentional: we want to execute the next case.
      case "path":
      case "pathParametric":
      case "segment":
      case "segmentParametric":
        let interpretedPoints = this.interpretListListStringsAsNumbersOrFunctions(
          points, parameterNames, parameterValues,
        );
        canvas.drawPath(interpretedPoints, color, lineWidth);
        return;
      case "label":
        let onePointComputer = this.interpretListStringsAsNumbersOrFunctions(
          onePoint,
          parameterNames,
          parameterValues,
        );
        canvas.drawText(onePointComputer, text, color);
        return;
      case "escapeMap":
        let functionX =
          this.functionFromBodyAndArguments(
            coordinateFunctions[0], variableAndParameterNames,
          );
        let functionY =
          this.functionFromBodyAndArguments(
            coordinateFunctions[1], variableAndParameterNames,
          );
        canvas.drawEscapeMap(
          functionX,
          functionY,
          parametersOnTheGraph,
          plot[this.labels.mandelbrotMode],
        );
        return;
      case "latex":
        canvas.drawLatex(
          this.interpretListStringsAsNumbersOrFunctions(
            onePoint, parameterNames, parameterValues,
          ),
          text,
        );
        return;
      default:
        throw `Unknown plot type: ${plotType}.`;
    }
  }

  oneThreeDimensionalObject(
    plot,
    /** @type {CanvasThreeD} */
    canvas
  ) {
    let plotType = plot[this.labels.plotType];
    let variableRanges = plot[this.labels.variableRanges];
    let numberOfSegments = plot[this.labels.numberOfSegments];
    let coordinateFunctions = plot[this.labels.coordinateFunctions];
    let colorFront = plot[this.labels.colorFront];
    let colorBack = plot[this.labels.colorBack];
    let colorContour = plot[this.labels.colorContour];
    let points = plot["points"];
    let color = plot["color"];
    let lineWidth = plot[this.labels.lineWidth];
    let point = plot["point"];
    let text = plot["text"];
    let variableNames = this.getArguments(plot);
    let parameterNames = this.getParameters(plot);
    let parameterValues = canvas.parameterValues;
    let layer = plot[this.labels.layerLabel];
    if (layer === undefined || layer === null) {
      layer = "";
    }
    switch (plotType) {
      case "segment":
        canvas.drawLineCreate(points[0], points[1], color, lineWidth, layer);
        return;
      case "label":
        canvas.drawTextCreate(point, text, color, layer);
        return;
      case "setProjectionScreen":
        canvas.screenBasisUserDefault = plot["projectionScreen"];
        return;
      case "parametricCurve":
        let coordinates = [
          this.functionFromBodyAndArguments(
            coordinateFunctions[0], this.getArguments(plot),
          ),
          this.functionFromBodyAndArguments(
            coordinateFunctions[1], this.getArguments(plot),
          ),
          this.functionFromBodyAndArguments(
            coordinateFunctions[2], this.getArguments(plot),
          ),
        ];
        let curveImmersion = (p, q) => {
          return [
            coordinates[0](p, q),
            coordinates[1](p, q),
            coordinates[2](p, q),
          ];
        }
        let leftEvaluated = this.interpretStringToNumberOrFunction(
          variableRanges[0], parameterNames, parameterValues,
        );
        let rightEvaluated = this.interpretStringToNumberOrFunction(
          variableRanges[1], parameterNames, parameterValues,
        );
        canvas.drawCurveCreate(
          new Graphics.CurveThreeD(
            curveImmersion,
            leftEvaluated,
            rightEvaluated,
            numberOfSegments,
            color,
            lineWidth,
          ),
        );
        return;
      case "surface":
        let convertedRanges = [[
          this.interpretStringToNumberOrFunction(
            variableRanges[0][0], parameterNames, parameterValues,
          ),
          this.interpretStringToNumberOrFunction(
            variableRanges[0][1], parameterNames, parameterValues,
          ),
        ], [
          this.interpretStringToNumberOrFunction(
            variableRanges[1][0], parameterNames, parameterValues,
          ),
          this.interpretStringToNumberOrFunction(
            variableRanges[1][1], parameterNames, parameterValues,
          ),
        ]];
        let convertedSegments = [
          this.interpretStringToNumberOrFunction(
            numberOfSegments[0], parameterNames, parameterValues,
          ),
          this.interpretStringToNumberOrFunction(
            numberOfSegments[1], parameterNames, parameterValues,
          ),
        ];
        let surfaceImmersion = this.functionFromBodyAndArguments(
          coordinateFunctions,
          variableNames,
          parameterValues,
        );
        canvas.drawSurfaceCreate(
          surfaceImmersion,
          convertedRanges,
          convertedSegments,
          colorFront,
          colorBack,
          colorContour,
          lineWidth,
          layer,
        );
        return;
      default:
        throw `Unknown plot type: ${plotType}.`;
    }
  }

  /** 
   * Extracts a number from javascript, or a function that
   * generates a number. The use case for a function is a 
   * graphics parameter that is tied to a slider or 
   * another user-controlled element.
   * 
   * @return {number|function(Array.<number>):number} 
   */
  interpretStringToNumberOrFunction(
    /** @type {string} */
    input,
    /** @type {string[]} */
    inputParameters,
    /** @type {Array.<number>} */
    parameterValues,
  ) {
    let definingFunction = this.functionFromBodyAndArguments(
      `"use strict"; return (${input});`, inputParameters,
    );
    if (inputParameters.length === 0) {
      return definingFunction(parameterValues);
    }
    return definingFunction;
  }

  /** 
   * Interprets a list of strings from javascript.
   * 
   * @return {number[]} 
   */
  interpretListStringsAsNumbersOrFunctions(
    /** @type {string[]} */
    input,
    /** @type {string[]} */
    inputArguments,
    /** @type {Array.<number>} */
    parameterValues,
  ) {
    let result = [];
    for (let i = 0; i < input.length; i++) {
      result.push(
        this.interpretStringToNumberOrFunction(
          input[i], inputArguments, parameterValues,
        ),
      );
    }
    return result;
  }

  /** 
   * Interprets a list of list of strings from javascript.
   * 
   * @return {Array.<Array.<number|function(Array.<number>):number>>} 
   */
  interpretListListStringsAsNumbersOrFunctions(
    /** @type {string[][]} */
    input,
    /** @type {string[]} */
    inputArguments,
    /** @type {Array.<number>} */
    parameterValues,
  ) {
    let result = [];
    for (let i = 0; i < input.length; i++) {
      result.push(
        this.interpretListStringsAsNumbersOrFunctions(
          input[i],
          inputArguments,
          parameterValues,
        )
      );
    }
    return result;
  }

  functionFromObject(input) {
    /** @type {string} */
    let body = input[this.labels.body];
    return this.functionFromBodyAndArguments(
      body, this.getArguments(input),
    );
  }

  /** 
   * Gets argument list from function specification. 
   * 
   * @return {Array.<string>} 
   */
  getArguments(input) {
    /** @type {string[]} */
    let result = [];
    if (this.labels.arguments in input) {
      result = result.concat(input[this.labels.arguments].slice());
    }
    return result.concat(
      this.getParameters(input)
    ).concat(
      this.getParametersOnGraph(input)
    );
  }

  /**
   * Gets parameters on graph list from function specification. 
   * 
   * @return {Array.<string>} 
   */
  getParametersOnGraph(input) {
    /** @type {string[]} */
    let result = [];
    let parametersOnTheGraphLetter = input[this.labels.parametersOnTheGraphLetter];
    if (
      parametersOnTheGraphLetter !== "" &&
      parametersOnTheGraphLetter !== undefined
    ) {
      result.push(parametersOnTheGraphLetter);
    }
    return result;
  }

  /**
   * Gets parameter list from function specification. 
   * 
   * @return {Array.<string>} 
   */
  getParameters(input) {
    /** @type {string[]} */
    let result = [];
    let parameterLetter = input[this.labels.parameterLetter];
    if (parameterLetter !== "" && parameterLetter !== undefined) {
      result.push(parameterLetter);
    }
    return result;
  }

  /** @return {number|function(Array.<number>):number>} */
  functionFromBodyAndArguments(
    /** @type {string|number} */
    body,
    /** @type {string[]} */
    inputArguments
  ) {
    if (typeof body === "number") {
      return body;
    }
    if (inputArguments.length === 0) {
      // Create a fake parameter so we can always call the function
      // with one parameter. If the inputArguments is empty,
      // then the body should have no mentions of any variable
      // and "p" is simply an unused argument.
      return Function(
        "p",
        body,
      );
    }
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
  SliderUpdater,
};