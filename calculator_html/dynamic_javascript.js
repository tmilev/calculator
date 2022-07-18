const graphicsSerialization = require("./graphics_serialization").graphicsSerialization;
const crypto = require("./crypto");
const graphicsNDimensions = require("./graphics_n_dimensions");
const pathnames = require("./pathnames");
const typeset = require("./math_typeset");
const EquationEditor = require("./equation_editor").EquationEditor;
const MathNode = require("./equation_editor").MathNode;
const knownTypes = require("./equation_editor").knownTypes;

class OneGraphicWithSliders {
  constructor(
    /** @type{ElementWithScripts} */
    owner,
    /** @type{string} */
    serialization
  ) {
    this.owner = owner;
    this.serialization = serialization;
    this.graphics = null;
    /** @type{Canvas|CanvasTwoD|null} */
    this.canvas = null;
    this.usedSliders = {};
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
      this.usedSliders,
    );
    for (let label in this.usedSliders) {
      let current = this.owner.sliders[label];
      current.addEventListener("input", () => {
        this.owner.updateSlidersAndFormInputs(label, current.value);
      });
    }    
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
    /** @type{HTMLElement|null} */
    this.element = null;
    /** @type{Object.<string,HTMLInputElement>} */
    this.sliders = {};
    /** @type{Object.<string,Array.<MathNode>>} */
    this.mathNodesAssociatedWithSliders = {}; 
    /** @type{Array.<OneGraphicWithSliders>} */
    this.graphicsWithSliders = [];
  }

  accountOneScript(
    /**@type{HTMLElement} */
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

  accountOneSlider(
    /**@type{HTMLInputElement} */
    slider,
  ) {
    let sliderName = slider.getAttribute("name");
    if (sliderName === "" || sliderName === null || sliderName === undefined) {
      return;
    }
    if (sliderName in this.sliders) {
      console.log(`Slider name ${sliderName} already present in current element.`);
      return;
    }
    this.sliders[sliderName] = slider;
  }

  bootstrapAllScripts(
    /**@type{HTMLElement} */
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
      this.accountOneSlider(candidateSliders[i]);
    }
    this.bootstrapGraphics3d();
    this.bootstrapGraphics();
    this.bootstrapAbstractSyntaxNotationScripts();
    this.bootstrapDisplaySSLRecord();
    this.bootstrapDisplayTransportLayerSecurity();
    this.bootstrapGraphicsNDimensional();
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
    /** @type{string} */
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
    /**@type{string} */
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
    /**@type{string} */
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
    /**@type{string} */
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
    /**@type{string} */
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

  processMathNodesRecursive(
    /** @type{MathNode} */
    node,
  ) {
    for (let i = 0; i < node.children.length; i++) {
      this.processMathNodesRecursive(node.children[i]);
    }
    this.processOne(node);
  }

  processOne(
    /**@type{MathNode} */
    node,
  ) {
    if (node.type.type !== knownTypes.formInput.type) {
      return;
    }
    let name = node.name;
    node.element.addEventListener("input", () => {
      this.updateSlidersAndFormInputs(name, node.element.value);
    });
    if (!(name in this.mathNodesAssociatedWithSliders)) {
      this.mathNodesAssociatedWithSliders[name] = [];
    }
    this.mathNodesAssociatedWithSliders[name].push(node);
  }

  /** 
   * Updates all form inputs and sliders to the given value. 
   * 
   * @param {string} name name of the slider
   * @param {string} value value to update to
   */
  updateSlidersAndFormInputs(name, value) {
    let mathNodes = this.mathNodesAssociatedWithSliders[name];
    for (let i = 0; i < mathNodes.length; i++){
      let node = mathNodes[i];
      if (node.element !== null) {
        node.element.value = value;
      }
    }
    let slider = this.sliders[name];
    slider.value = value;
    for (let i = 0; i < this.graphicsWithSliders.length; i++) {
      let graphic = this.graphicsWithSliders[i];
      if (name in graphic.usedSliders) { 
        graphic.redraw();
      }      
    }
  }
}

class DynamicJavascript {
  constructor() {
  }

  typeset(
    /**@type{HTMLElement} */
    output,
    /**@type{Object<string, string>} */
    extraAttributes,
    /**@type{Function|null} */
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
   * @return{ElementWithScripts}
   */
  bootstrapAllScripts(
    /** @type{HTMLElement} */
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