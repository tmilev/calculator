const graphicsSerialization = require("./graphics_serialization").graphicsSerialization;
const SliderUpdater = require("./graphics_serialization").SliderUpdater;
const CanvasTwoD = require("./graphics").CanvasTwoD;
const CanvasThreeD = require("./graphics").Canvas;
const crypto = require("./crypto");
const graphicsNDimensions = require("./graphics_n_dimensions");
const pathnames = require("./pathnames");
const typeset = require("./math_typeset");
const EquationEditor = require("./equation_editor/src/equation_editor").EquationEditor;
const MathNode = require("./equation_editor/src/equation_editor").MathNode;
const knownTypes = require("./equation_editor/src/equation_editor").knownTypes;
const miscellaneous = require("./miscellaneous_frontend");

const scriptTypeLabel = "scriptType";

class OneGraphicWithSliders {
  constructor(
    /** @type {ElementWithScripts} */
    owner,
    /** @type {Object} */
    graphics,
  ) {
    this.owner = owner;
    this.graphics = graphics;
    /** @type {CanvasThreeD|CanvasTwoD|null} */
    this.canvas = null;
  }

  /** 
   * Computes the graphics.
   */
  computeFromSerialization() {
    let canvasName = this.graphics[pathnames.urlFields.result.canvasName];
    let controlsName = this.graphics[pathnames.urlFields.result.controlsName];
    let layerContainerName = this.graphics[pathnames.urlFields.result.layerContainerName];
    let canvases = this.owner.element.querySelectorAll(`[name="${canvasName}"]`);
    let controls = this.owner.element.querySelectorAll(`[name="${controlsName}"]`);
    let layerContainerArray = this.owner.element.querySelectorAll(`[name="${layerContainerName}"]`);
    let layerContainer = null;
    if (layerContainerArray.length > 0) {
      layerContainer = layerContainerArray[0];
      layerContainer.className = "layerContainer";
    }
    if (canvases.length < 1) {
      throw "Unexpected missing canvas.";
    }
    this.canvas = graphicsSerialization.fromJSON(
      this.graphics,
      canvases[0],
      controls[0],
      null,
      this.owner.sliders,
      layerContainer,
    );
  }

  redraw() {
    graphicsSerialization.redrawFromJSON(this.canvas, this.graphics, this.owner.sliders);
  }
}

class ElementWithScripts {
  constructor() {
    this.scriptContents = {
      "processed": [],
      "graphics": [],
      "graphics3d": [],
      "abstractSyntaxNotationAnnotate": [],
      "displaySSLRecord": [],
      "displayTransportLayerSecurity": [],
      "graphicsNDimensional": [],
    };
    /** @type {HTMLElement|null} */
    this.element = null;
    this.sliders = new SliderUpdater((
      /** @type {Object.<string,number> } */
      map
    ) => {
      this.updateSlidersAndFormInputs(map);
    });
    /** @type {Array.<OneGraphicWithSliders>} */
    this.graphicsWithSliders = [];
  }

  accountOneScript(
    /** @type {HTMLElement} */
    script,
  ) {
    let content = script.textContent;
    let contentParsed = JSON.parse(content);
    let scriptType = script.getAttribute(scriptTypeLabel);
    if (scriptType === null) {
      scriptType = contentParsed[scriptTypeLabel];
    }
    if (scriptType === undefined || scriptType === null) {
      throw `Unknown script type: ${scriptType}.`;
    }
    this.addOneScript(scriptType, contentParsed);
    script.setAttribute(scriptTypeLabel, "processed");
  }

  accountOneScriptObject(script) {
    let scriptType = script[scriptTypeLabel];
    this.addOneScript(scriptType, script);
  }

  addOneScript(scriptType, content) {
    if (scriptType in this.scriptContents) {
      this.scriptContents[scriptType].push(content);
    } else {
      throw `Unrecognized script type ${scriptType}.`;
    }
  }

  bootstrapAllScripts(
    /** @type {HTMLElement} */
    element,
    /** @type {Array.<Object>} */
    scripts,
  ) {
    this.element = element;
    let incomingScripts = this.element.getElementsByTagName("script");
    for (let i = 0; i < incomingScripts.length; i++) {
      this.accountOneScript(incomingScripts[i]);
    }
    let incomingScriptsInSpans = this.element.querySelectorAll(`[name="script"]`);
    for (let i = 0; i < incomingScriptsInSpans.length; i++) {
      this.accountOneScript(incomingScriptsInSpans[i]);
    }
    if (scripts !== undefined && scripts !== null) {
      for (let i = 0; i < scripts.length; i++) {
        this.accountOneScriptObject(scripts[i]);
      }
    }
    let candidateSliders = this.element.getElementsByTagName("input");
    for (let i = 0; i < candidateSliders.length; i++) {
      this.sliders.accountOneSlider(candidateSliders[i]);
    }
    this.bootstrapGraphics3d();
    this.bootstrapGraphics();
    this.bootstrapAbstractSyntaxNotationScripts();
    this.bootstrapDisplaySSLRecord();
    this.bootstrapDisplayTransportLayerSecurity();
    this.bootstrapGraphicsNDimensional();
    this.bootstrapSliders();
    this.bootstrapLatexWithCopyButtons();
  }

  bootstrapSliders() {
    for (let label in this.sliders.sliders) {
      let current = this.sliders.sliders[label];
      current.addEventListener("input", () => {
        this.updateSlidersAndFormInputsOneKeyValuePair(label, current.value);
      });
      this.bootstrapOneInput(label);
    }
  }

  bootstrapOneInput(
    /** @type {string} */
    name,
  ) {
    let inputs = document.getElementsByName(name);
    for (let i = 0; i < inputs.length; i++) {
      let current = inputs[i];
      if (current.type === "range") {
        continue;
      }
      current.addEventListener("input", () => {
        this.updateSlidersAndFormInputsOneKeyValuePair(name, current.value);
      });
    }
  }


  bootstrapGraphics3d() {
    let annotations = this.scriptContents["graphics3d"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneGraphic(annotations[i]);
    }
  }

  bootstrapGraphics() {
    let graphics = this.scriptContents["graphics"];
    for (let i = 0; i < graphics.length; i++) {
      this.bootstrapOneGraphic(graphics[i]);
    }
  }

  bootstrapOneGraphic(
    /** @type {string} */
    content,
  ) {
    let oneGraphic = new OneGraphicWithSliders(this, content);
    oneGraphic.computeFromSerialization();
    this.graphicsWithSliders.push(oneGraphic);
  }

  bootstrapGraphicsNDimensional() {
    let annotations = this.scriptContents["graphicsNDimensional"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneGraphicsNDimensional(annotations[i]);
    }
  }

  bootstrapOneGraphicsNDimensional(
    /** @type {Object} */
    parsedContent,
  ) {
    graphicsNDimensions.createGraphicsFromObject(parsedContent);
  }

  bootstrapDisplayTransportLayerSecurity() {
    let annotations = this.scriptContents["displayTransportLayerSecurity"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneDisplayTransportLayerSecurity(annotations[i]);
    }
  }

  bootstrapOneDisplayTransportLayerSecurity(
    /** @type {string} */
    parsedContent,
  ) {
    crypto.displayTransportLayerSecurity(parsedContent["id"], parsedContent["content"]);
  }

  bootstrapDisplaySSLRecord() {
    let annotations = this.scriptContents["displaySSLRecord"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneDisplaySSLRecord(annotations[i]);
    }
  }

  bootstrapOneDisplaySSLRecord(
    /** @type {string} */
    parsedContent,
  ) {
    crypto.displaySSLRecord(parsedContent["id"], parsedContent["content"]);
  }

  bootstrapAbstractSyntaxNotationScripts() {
    let annotations = this.scriptContents["abstractSyntaxNotationAnnotate"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneAbstractSyntaxNotation(annotations[i]);
    }
  }

  bootstrapOneAbstractSyntaxNotation(
    /** @type {Object} */
    parsedContent,
  ) {
    crypto.abstractSyntaxNotationAnnotate(parsedContent[0], parsedContent[1], parsedContent[2]);
  }

  /** 
   * Shows latex snippets with a copy button 
   * that is not rendered as math, 
   * possibly for being too large. 
   */
  bootstrapLatexWithCopyButtons() {
    let domElements = this.element.getElementsByClassName("latexWithCopyButton");
    for (let i = 0; i < domElements.length; i++) {
      let element = domElements[i];
      this.bootstrapOneLatexWithCopyButtons(element);
    }
  }

  bootstrapOneLatexWithCopyButtons(
    /** @type {HTMLElement} */
    element,
  ) {
    let content = element.textContent;
    element.textContent = "";
    let button = document.createElement("button");
    button.className = "buttonMQ";
    miscellaneous.writeHTML(button, "<tiny>L&#x1F4CB;</tiny>");
    button.addEventListener('click', () => {
      this.copyLatex(content, element);
    });
    element.appendChild(button);
  }

  copyLatex(
    /** @type {string} */
    content,
    /** @type {HTMLElement} */
    element,
  ) {
    navigator.clipboard.writeText(content);
    element.style.transition = "all 1s";
    element.style.backgroundColor = "lightgreen";
    setTimeout(() => {
      element.style.backgroundColor = "";
    }, 1000);
  }


  updateSlidersAndFormInputsOneKeyValuePair(key, value) {
    let map = {};
    map[key] = value;
    this.updateSlidersAndFormInputs(map);
  }

  /**
   * Updates all form inputs and sliders to all the given value, number pairs. 
   * 
   * @param {Object.<string, value>} map a collection of name-value pairs.
   */
  updateSlidersAndFormInputs(map) {
    let setToRedraw = new Set();
    for (let label in map) {
      let moreToRedraw = this.updateOneSliderAndFormInput(label, map[label]);
      for (let graphic of moreToRedraw) {
        setToRedraw.add(graphic);
      }
    }
    for (let graphic of setToRedraw) {
      if (graphic instanceof CanvasThreeD) {
        graphic.recomputeAndRedraw();
      } else {
        graphic.redraw();
      }
    }
  }

  /** 
   * Updates all form inputs and sliders to the given value. 
   * 
   * @param {name} name of of the input box.
   * @param {string} value value to update to.
   * @return a set of graphics to redraw.
   */
  updateOneSliderAndFormInput(name, value) {
    let result = new Set();
    let slider = this.sliders.sliders[name];
    slider.value = value;
    let inputForm = this.sliders.inputsForms[name];
    inputForm.value = value;
    for (let i = 0; i < this.graphicsWithSliders.length; i++) {
      let graphic = this.graphicsWithSliders[i];
      if (name in graphic.canvas.parameterNames) {
        let index = graphic.canvas.parameterNames[name];
        graphic.canvas.parameterValues[index] = parseFloat(value);
        result.add(graphic.canvas);
      }
    }
    return result;
  }
}

class DynamicJavascript {
  constructor() {
  }

  typeset(
    /** @type {HTMLElement} */
    output,
    /** @type {Object<string, string>} */
    extraAttributes,
    /** @type {Function|null} */
    typeSetCallback,
  ) {
    if (extraAttributes === undefined || extraAttributes === null) {
      extraAttributes = {};
    }
    typeset.typesetter.typesetSoft(
      output,
      null,
      (editor) => {
        if (typeSetCallback !== null && typeSetCallback !== undefined) {
          typeSetCallback(editor, output);
        }
      },
      extraAttributes,
    );
    this.flagTypeset = true;
  }

  /** 
   * Boostraps all scripts in a given html element and returns a handle
   * to the bootstrap element.
   * 
   * @return {ElementWithScripts}
   */
  bootstrapAllScripts(
    /** @type {HTMLElement} */
    element,
    /** @type {Array.<Object>} */
    scripts,
  ) {
    let elementWithScripts = new ElementWithScripts();
    elementWithScripts.bootstrapAllScripts(element, scripts);
    return elementWithScripts;
  }
}

let dynamicJavascript = new DynamicJavascript();

module.exports = {
  dynamicJavascript,
};