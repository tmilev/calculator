const graphicsSerialization = require("./graphics_serialization").graphicsSerialization;
const crypto = require("./crypto");
const graphicsNDimensions = require("./graphics_n_dimensions");
const pathnames = require("./pathnames");

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
    /**@type{HTMLElement|null} */
    this.element = null;
    /**@type{Object.<string,HTMLInputElement>} */
    this.sliders = {};
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
    /**@type{string} */
    content,
  ) {
    let graphics = JSON.parse(content);
    let canvasName = graphics[pathnames.urlFields.result.canvasName];
    let controlsName = graphics[pathnames.urlFields.result.controlsName];
    console.log("DEBUG: bootstrap graphics: " + JSON.stringify(graphics));
    let canvases = this.element.querySelectorAll(`[name="${canvasName}"]`);
    let controls = this.element.querySelectorAll(`[name="${controlsName}"]`);
    if (canvases.length < 1) {
      throw "Unexpected missing canvas.";
    }
    let canvas = graphicsSerialization.fromJSON(graphics, canvases[0], controls[0], null);
    for (let label in this.sliders) {
      let current = this.sliders[label];
      current.addEventListener("input", () => {
        canvas.redraw();
      });
    }
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
}

class DynamicJavascript {
  constructor() {
  }

  bootstrapAllScripts(
    /**@type{HTMLElement} */
    element,
  ) {
    let elementWithScripts = new ElementWithScripts();
    elementWithScripts.bootstrapAllScripts(element);
  }
}

let dynamicJavascript = new DynamicJavascript();

module.exports = {
  dynamicJavascript,
};