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

class OneGraphicWithSliders {
  constructor(
    /** @type {ElementWithScripts} */
    owner,
    /** @type {string} */
    serialization
  ) {
    this.owner = owner;
    this.serialization = serialization;
    this.graphics = null;
    /** @type {CanvasThreeD|CanvasTwoD|null} */
    this.canvas = null;
  }

  /** 
   * Computes the graphics.
   */
  computeFromSerialization() {
    this.graphics = JSON.parse(this.serialization);
    let canvasName = this.graphics[pathnames.urlFields.result.canvasName];
    let controlsName = this.graphics[pathnames.urlFields.result.controlsName];
    let canvases = this.owner.element.querySelectorAll(`[name="${canvasName}"]`);
    let controls = this.owner.element.querySelectorAll(`[name="${controlsName}"]`);
    if (canvases.length < 1) {
      throw "Unexpected missing canvas.";
    }
    this.canvas = graphicsSerialization.fromJSON(
      this.graphics,
      canvases[0],
      controls[0],
      null,
      this.owner.sliders,
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
    /** @type {Object.<string,Array.<MathNode>>} */
    this.mathNodesAssociatedWithSliders = {}; 
    /** @type {Array.<OneGraphicWithSliders>} */
    this.graphicsWithSliders = [];
  }

  accountOneScript(
    /** @type {HTMLElement} */
    script,
  ) {
    let content = script.textContent;
    let scriptType = script.getAttribute("scriptType");
    if (scriptType in this.scriptContents) {
      this.scriptContents[scriptType].push(content);
    } else {
      throw `Unrecognized script type ${scriptType}.`;
    }
    script.setAttribute("scriptType", "processed");
  }

  bootstrapAllScripts(
    /** @type {HTMLElement} */
    element,
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
    /** @type {string} */
    content,
  ) {
    let parsed = JSON.parse(content);
    graphicsNDimensions.createGraphicsFromObject(parsed);
  }

  bootstrapDisplayTransportLayerSecurity() {
    let annotations = this.scriptContents["displayTransportLayerSecurity"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneDisplayTransportLayerSecurity(annotations[i]);
    }
  }

  bootstrapOneDisplayTransportLayerSecurity(
    /** @type {string} */
    content,
  ) {
    let parsed = JSON.parse(content);
    crypto.displayTransportLayerSecurity(parsed["id"], parsed["content"]);
  }

  bootstrapDisplaySSLRecord() {
    let annotations = this.scriptContents["displaySSLRecord"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneDisplaySSLRecord(annotations[i]);
    }
  }

  bootstrapOneDisplaySSLRecord(
    /** @type {string} */
    content,
  ) {
    let parsed = JSON.parse(content);
    crypto.displaySSLRecord(parsed["id"], parsed["content"]);
  }

  bootstrapAbstractSyntaxNotationScripts() {
    let annotations = this.scriptContents["abstractSyntaxNotationAnnotate"];
    for (let i = 0; i < annotations.length; i++) {
      this.bootstrapOneAbstractSyntaxNotation(annotations[i]);
    }
  }

  bootstrapOneAbstractSyntaxNotation(
    /** @type {string} */
    content,
  ) {
    let parsed = JSON.parse(content);
    crypto.abstractSyntaxNotationAnnotate(parsed[0], parsed[1], parsed[2]);
  }

  /** 
   * Bootstraps all form inputs in typeset math.
   * 
   * @param {EquationEditor} editor the equation editor object 
   * @param {HTMLElement} unused the ambient html element that contains 
   * the graphics and the equations 
   */
  bootstrapFormInputs(
    editor, unused,
  ) {
    this.processMathNodesRecursive(editor.rootNode);
  }

  bootstrapLatexWithCopyButtons() {
    let domElements = this.element.getElementsByClassName("latexWithCopyButton");  
    for (let i = 0; i < domElements.length; i++) {
      let element = domElements[i];
      this.bootstrapOneLatexWithCopyButtons( element);
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

  processMathNodesRecursive(
    /** @type {MathNode} */
    node,
  ) {
    for (let i = 0; i < node.children.length; i++) {
      this.processMathNodesRecursive(node.children[i]);
    }
    this.processOne(node);
  }

  processOne(
    /** @type {MathNode} */
    node,
  ) {
    if (node.type.type !== knownTypes.formInput.type) {
      return;
    }
    let name = node.name;
    node.element.addEventListener("input", () => {
      this.updateSlidersAndFormInputsOneKeyValuePair(name, node.element.value);
    });
    if (!(name in this.mathNodesAssociatedWithSliders)) {
      this.mathNodesAssociatedWithSliders[name] = [];
    }
    this.mathNodesAssociatedWithSliders[name].push(node);
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
      graphic.redraw();
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
    let mathNodes = this.mathNodesAssociatedWithSliders[name];
    for (let i = 0; i < mathNodes.length; i++) {
      let node = mathNodes[i];
      if (node.element !== null) {
        node.element.value = value;
      }
    }
    let slider = this.sliders.sliders[name];
    slider.value = value;
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
      {
        fontSize: "20px",
        fontFamily: "Times New Roman",
        display: "inline-block"
      },
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
  ) {
    let elementWithScripts = new ElementWithScripts();
    elementWithScripts.bootstrapAllScripts(element);
    return elementWithScripts;
  }
}

let dynamicJavascript = new DynamicJavascript();

module.exports = {
  dynamicJavascript,
};