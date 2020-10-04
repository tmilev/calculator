"use strict";

var module;
if (module === undefined) {
  module = {
    exports: null,
  };
}

class MathNodeType {

  normalizeConstructorInput(input) {
    for (let label in knownTypeDefaults) {
      if (!(label in input)) {
        input[label] = knownTypeDefaults[label];
      }
    }
    if (input["arrows"] === undefined) {
      input["arrows"] = knownTypeDefaultsArrows;
      return;
    }
    for (let label in knownTypeDefaultsArrows) {
      if (!(label in input["arrows"])) {
        input["arrows"][label] = knownTypeDefaultsArrows[label];
      }
    }
  }

  constructor(input) {
    this.normalizeConstructorInput(input);
    this.arrows = input["arrows"];
    this.type = input["type"];
    this.minHeightScale = input["minHeightScale"];
    this.width = input["width"];
    this.height = input["height"];
    this.display = input["display"];
    this.flexDirection = input["flexDirection"];
    this.justifyContent = input["justifyContent"];
    this.alignContent = input["alignContent"];
    this.alignItems = input["alignItems"];
    this.verticalAlign = input["verticalAlign"];
    this.outline = input["outline"];
    this.fontSize = input["fontSize"];
    this.whiteSpace = input["whiteSpace"];
    this.textAlign = input["textAlign"];
    this.float = input["float"];
    this.boxSizing = input["boxSizing"];
    this.position = input["position"];
    // Padding.
    this.paddingBottom = input["paddingBottom"];
    this.padding = input["padding"];
    // Margins.
    this.margin = input["margin"];
    this.marginBottom = input["marginBottom"];
    // Borders
    this.borderStyle = input["borderStyle"];
    this.borderColor = input["borderColor"];
    this.borderBottom = input["borderBottom"];
  }
}

const knownTypeDefaults = {
  "display": "inline-block",
  "minHeightScale": 0,
  "flexDirection": "",
  "justifyContent": "",
  "alignContent": "",
  "alignItems": "",
  "verticalAlign": "",
  "fontSize": "",
  "whiteSpace": "",
  "float": "",
  "textAlign": "",
  "boxSizing": "border-box",
  "position": "absolute",
  // Padding
  "paddingBottom": "",
  "padding": "",
  // Margins
  "marginBottom": "",
  "margin": "",
  // Borders.
  "borderStyle": "",
  "borderColor": "",
  "borderBottom": "",
};

class ArrowMotionTypes {
  constructor() {
    this.parentForward = "parentForward";
    this.firstAtomToTheLeft = "firstAtomToTheLeft";
    this.firstAtomToTheRight = "firstAtomToTheRight";
  }
}

let arrowMotion = new ArrowMotionTypes();

const knownTypeDefaultsArrows = {
  "ArrowUp": arrowMotion.parentForward,
  "ArrowDown": arrowMotion.parentForward,
  "ArrowLeft": arrowMotion.firstAtomToTheLeft,
  "ArrowRight": arrowMotion.firstAtomToTheRight,
};

const defaultFractionScale = 0.9;
const atomPad = `${0.02}em`;

const knownTypes = {
  root: new MathNodeType({
    "type": "root",
    "borderStyle": "1px solid black",
    "padding": "2px",
    "margin": "2px",
  }),
  // A math expression with no children such as "x", "2".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    "type": "atom",
    "padding": atomPad,
    "outline": "0px solid transparent",
    "width": "auto",
    "height": "auto",
  }),
  // A non-editable math expression/operator such as "+" or "-".
  atomImmutable: new MathNodeType({
    "type": "atomImmutable",
    "padding": `${0.02}em`,
    "width": "auto",
    "height": "auto",
  }),
  // Left delimiter (parentheses, bracket, ...)
  leftDelimiter: new MathNodeType({
    "type": "leftDelimiter",
    "width": "auto",
    "height": "auto",
    "position": "absolute",
  }),
  // Right delimiter (parentheses, bracket, ...)
  rightDelimiter: new MathNodeType({
    "type": "rightDelimiter",
  }),
  // Horizontally laid out math such as "x+2". 
  // The example "x+2" consists of the three elements "x", "+" and 2.
  // Not allowed to contain other horizontally laid out math elements.
  horizontalMath: new MathNodeType({
    "type": "horizontalMath",
    "whiteSpace": "nowrap",
    "width": "100%",
    "textAlign": "center",
  }),
  // Represents expressions such as "x/y" or "\frac{x}{y}".
  fraction: new MathNodeType({
    "type": "fraction",
  }),
  // Represents the numerator x of a fraction x/y.
  numerator: new MathNodeType({
    "type": "numerator",
    "borderBottom": "2px solid black",
    "fontSize": defaultFractionScale,
    "minHeightScale": defaultFractionScale,
    "arrows": {
      "ArrowUp": arrowMotion.firstAtomToTheLeft,
      "ArrowDown": arrowMotion.firstAtomToTheRight,
    },
    "whiteSpace": "nowrap",
    "textAlign": "center",
  }),
  // Represents the denominator y of a fraction x/y.
  denominator: new MathNodeType({
    "type": "denominator",
    "fontSize": defaultFractionScale,
    "minHeightScale": defaultFractionScale,
    "arrows": {
      "ArrowUp": arrowMotion.firstAtomToTheLeft,
      "ArrowDown": arrowMotion.firstAtomToTheRight,
    },
    "textAlign": "center",
    // "whiteSpace": "nowrap",
  }),
  baseWithExponent: new MathNodeType({
    "type": "baseWithExponent",
  }),
  exponent: new MathNodeType({
    "type": "exponent",
    "fontSize": 0.75,
    "minHeightScale": 0.75,
  }),
  parentheses: new MathNodeType({
    "type": "parentheses",
    "whiteSpace": "nowrap",
  }),
};

class MathNodeFactory {
  horizontalMath(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content
  ) {
    const result = new MathNode(equationEditor, knownTypes.horizontalMath);
    if (content === null) {
      content = this.atom(equationEditor, "");
    }
    result.appendChild(content);
    return result;
  }
  rootMath(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    const result = new MathNode(equationEditor, knownTypes.root);
    result.appendChild(this.horizontalMath(equationEditor, null));
    return result;
  }
  atom(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    initialContent
  ) {
    const result = new MathNode(equationEditor, knownTypes.atom);
    result.initialContent = initialContent;
    return result;
  }
  atomImmutable(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    operator,
  ) {
    const result = new MathNode(equationEditor, knownTypes.atomImmutable);
    result.initialContent = operator;
    return result;
  }
  /** @returns {MathNode} */
  fractionEmptyDenominator(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode}*/
    numeratorContent,
  ) {
    const fraction = new MathNode(equationEditor, knownTypes.fraction);
    const numerator = new MathNode(equationEditor, knownTypes.numerator);
    const denominator = new MathNode(equationEditor, knownTypes.denominator);
    numerator.appendChild(this.horizontalMath(equationEditor, numeratorContent));
    denominator.appendChild(this.horizontalMath(equationEditor, null));
    fraction.appendChild(numerator);
    fraction.appendChild(denominator);
    return fraction;
  }
  /** @returns {MathNode} */
  baseWithExponent(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode}*/
    base,
  ) {
    // Horizontal math wrapper for the exponent.
    const exponent = new MathNode(equationEditor, knownTypes.exponent);
    exponent.appendChild(this.horizontalMath(equationEditor, null));
    // Horizontal math wrapper for the base.
    const baseHorizontal = new MathNode(equationEditor, knownTypes.horizontalMath);
    baseHorizontal.appendChild(this.horizontalMath(equationEditor, base));
    baseHorizontal.normalizeHorizontalMath();
    // The base with the exponent.
    const baseWithExponent = new MathNode(equationEditor, knownTypes.baseWithExponent);
    baseWithExponent.appendChild(baseHorizontal);
    baseWithExponent.appendChild(exponent);
    return baseWithExponent;
  }
  /** @returns {MathNode} */
  parentheses(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode}*/
    content,
  ) {
    const leftParentheses = new MathNode(equationEditor, knownTypes.leftDelimiter);
    leftParentheses.initialContent = "(";
    const rightParentheses = new MathNode(equationEditor, knownTypes.rightDelimiter);
    rightParentheses.initialContent = ")";
    const wrappedContent = this.horizontalMath(equationEditor, content);
    wrappedContent.normalizeHorizontalMath();
    const result = new MathNode(
      equationEditor, knownTypes.parentheses,
    );
    result.appendChild(leftParentheses);
    result.appendChild(wrappedContent);
    result.appendChild(rightParentheses);
    return result;
  }
}

var mathNodeFactory = new MathNodeFactory();
/** @type {EquationEditor} */
var lastCreatedEquationEditor = null;

class AtomWithPosition {
  constructor(
    /** @type {MathNode|null} */
    element,
    /** @type {number} */
    position,
  ) {
    this.element = element;
    this.position = position;
  }
}

class EquationEditor {
  constructor(
    /** @type{HTMLElement} */
    container,
  ) {
    this.container = container;
    this.container.innerHTML = "";
    this.container.style.display = "inline-block";
    this.container.style.position = "relative";
    this.rootNode = mathNodeFactory.rootMath(this);
    this.rootNode.externalDOM = this.container;
    this.rootNode.updateDOM();
    lastCreatedEquationEditor = this;
  }
  toString() {
    return this.rootNode.toString(0);
  }
  sendKeys(
    /**@type {string[]} */
    keys,
  ) {
    if (keys.length === 0) {
      return;
    }
    let key = keys[0];
    let focused = this.rootNode.findFirstFocusedChild();
    this.dispatchKey(focused, key);

    setTimeout(() => {
      this.sendKeys(keys.slice(1));
    }, 400);
  }

  dispatchKey(
    /**@type{MathNode}*/ focused,

    /**@type{string} */ key,
  ) {
    if (focused === null) {
      console.log('DEBUG: focused should not be null!');
      return;
    }
    let specialKeyCodes = {
      "+": null,
      "-": null,
      "*": null,
      "/": null,
      "^": null,
      "(": null,
      ")": null,
      "ArrowLeft": 37,
      "ArrowRight": 39,
      "ArrowUp": 38,
      "ArrowDown": 40,
      "Backspace": null,
    };
    if (!(key in specialKeyCodes)) {
      console.log("DEBUG: simulate non-special key:" + key);
      focused.element.textContent += key;
      focused.focus(1);
      return;
    }
    if (key === "ArrowLeft" && focused.positionCaretBeforeKeyEvents > 0) {
      console.log('DEBUG: simulate arrow left');
      focused.focus(0);
      return;
    }
    const eventsToSend = [
      "keydown",
      "keyup",
      "keypress",
    ];
    for (let i = 0; i < eventsToSend.length; i++) {
      let event = new KeyboardEvent(
        eventsToSend[i], {
        key: key,
      });
      focused.element.dispatchEvent(event);
    }
  }

  updateAlignment() {
    this.rootNode.computeBoundingBox();
    this.rootNode.doAlign();
    let boundingRectangle = this.rootNode.element.getBoundingClientRect();
    this.container.style.height = boundingRectangle.height;
    this.container.style.width = boundingRectangle.width;
  }
}

class BoundingBox {
  constructor() {
    this.left = 0;
    this.top = 0;
    this.width = 0;
    this.height = 0;
    this.fractionLineHeight = 0;
    this.stretchFactor = 1;
    this.transform = "";
  }
};

class MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNodeType} */
    type
  ) {
    if (!(equationEditor instanceof EquationEditor)) {
      throw (`Unexpected equation editor ${equationEditor}`);
    }
    /** @type {Array<MathNode>} */
    this.children = [];
    /** @type {MathNodeType} */
    this.type = type;
    /** @type {HTMLElement} */
    this.element = null;
    /** @type {MathNode} */
    this.parent = null;
    /** @type {number} */
    this.indexInParent = - 1;
    /** @type {number} */
    this.fractionLineHeight = 0;
    /** @type {number} */
    this.scale = 0;
    /** @type {HTMLElement} */
    this.externalDOM = null;
    /** @type {number} */
    this.positionCaretBeforeKeyEvents = - 1;
    /** @type {number} */
    this.desiredCaretPosition = -1;
    /** @type {string} */
    this.initialContent = "";
    /** @type {boolean} */
    this.focused = false;
    this.equationEditor = equationEditor;
    this.boundingBox = new BoundingBox();
  }

  createDOMElementIfMissing() {
    if (this.element !== null) {
      // Element already created;
      return;
    }
    this.element = document.createElement("div");
    const fontSize = 20;
    if (this.type.type === knownTypes.atom.type) {
      this.element.contentEditable = true;
    }
    if (this.type.paddingBottom !== "") {
      this.element.style.paddingBottom = this.type.paddingBottom;
    }
    // Padding.
    if (this.type.padding !== "") {
      this.element.style.padding = this.type.padding;
    }
    // Margins
    if (this.type.margin !== "") {
      this.element.style.margin = this.type.margin;
    }
    if (this.type.marginBottom !== "") {
      this.element.style.marginBottom = this.type.marginBottom;
    }
    // Borders.
    if (this.type.borderStyle !== "") {
      this.element.style.border = this.type.borderStyle;
    }
    if (this.type.borderColor !== "") {
      this.element.style.borderColor = this.type.borderColor;
    }
    if (this.type.borderBottom !== "") {
      this.element.style.borderBottom = this.type.borderBottom;
    }
    this.element.style.width = this.type.width;
    this.element.style.height = this.type.height;
    this.element.style.display = this.type.display;
    if (this.type.minHeightScale !== 0) {
      this.element.style.minWidth = this.type.minHeightScale * fontSize / 1.6;
    }
    this.element.style.verticalAlign = this.type.verticalAlign;
    this.element.style.outline = this.type.outline;
    if (this.type.textAlign !== "") {
      this.element.style.textAlign = this.type.textAlign;
    }
    if (this.type.whiteSpace !== "") {
      this.element.style.whiteSpace = this.type.whiteSpace;
    }
    this.scale = 1;
    if (this.type.fontSize !== "") {
      this.element.style.fontSize = `${this.type.fontSize * 100}%`;
      this.scale = this.type.fontSize;
    }
    if (this.type.flexDirection !== "") {
      this.element.style.flexDirection = this.type.flexDirection;
    }
    if (this.type.justifyContent !== "") {
      this.element.style.justifyContent = this.type.justifyContent;
    }
    if (this.type.alignContent !== "") {
      this.element.style.alignContent = this.element.style.alignContent;
    }
    if (this.type.alignItems !== "") {
      this.element.style.alignItems = this.element.style.alignItems;
    }
    this.element.innerHTML = "";
    if (this.initialContent !== "") {
      this.element.textContent = this.initialContent;
    }
    if (this.type.float !== "") {
      this.element.style.cssFloat = this.type.float;
    }
    if (this.type.boxSizing !== "") {
      this.element.style.boxSizing = this.type.boxSizing;
    }
    if (this.type.position !== "") {
      this.element.style.position = this.type.position;
    }
    this.fractionLineHeight = 0;
    this.element.setAttribute("mathTagName", this.type.type);
    /*    this.element.addEventListener("keyup", (e) => {
          this.handleKeyUp(e);
        });*/
    this.element.addEventListener("keydown", (e) => {
      this.handleKeyDown(e);
    });
    this.element.addEventListener("focus", (e) => {
      this.handleFocus(e);
    });
    this.element.addEventListener("blur", (e) => {
      this.handleBlur(e);
    });
  }

  /** @returns {MathNode} */
  findFirstFocusedChild() {
    if (this.focused) {
      return this;
    }
    for (let i = 0; i < this.children.length; i++) {
      let candidate = this.children[i].findFirstFocusedChild();
      if (candidate !== null) {
        return candidate;
      }
    }
    return null;
  }

  updateDOM() {
    this.updateDOMRecursive(0);
    this.equationEditor.updateAlignment();
  }

  /** @returns {boolean} */
  isAtomic() {
    return (
      this.type.type === knownTypes.atom.type ||
      this.type.type === knownTypes.atomImmutable.type
    );
  }
  /** @returns {boolean} */
  isAtomEditable() {
    return this.type.type === knownTypes.atom.type;
  }
  hasHorizontalLayout() {
    if (this.type.type === knownTypes.fraction.type) {
      return false;
    }
    return true;
  }

  updateDOMRecursive(/** @type {number} */ depth) {
    if (
      this.type.type === knownTypes.atom.type
    ) {
      this.createDOMElementIfMissing();
      return;
    }
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.updateDOMRecursive(depth + 1);
    }
    const oldElement = this.element;
    this.element = null;
    this.createDOMElementIfMissing();
    for (let i = 0; i < this.children.length; i++) {
      this.element.appendChild(this.children[i].element);
    }
    if (depth === 0) {
      this.updateParentDOM(oldElement);
    }
  }

  computeBoundingBox() {
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.computeBoundingBox();
    }
    this.computeDimensions();
  }

  computeDimensionsAtomic() {
    this.boundingBox.width = this.element.getBoundingClientRect().width;
    this.boundingBox.height = this.element.getBoundingClientRect().height;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  computeDimensionsFraction() {
    let numerator = this.children[0];
    let denominator = this.children[1];
    this.boundingBox.fractionLineHeight = numerator.boundingBox.height;
    this.boundingBox.height = numerator.boundingBox.height + denominator.boundingBox.height;
    this.boundingBox.width = Math.max(numerator.boundingBox.width, denominator.boundingBox.width);
    numerator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.top = numerator.boundingBox.height;
    numerator.computeBoundingBoxLeftSingleChild();
    denominator.computeBoundingBoxLeftSingleChild();
  }

  computeDimensionsBaseWithExponent() {
    let base = this.children[0];
    let exponent = this.children[1];
    let overlap = base.boundingBox.height * 0.4;
    this.boundingBox.height = exponent.boundingBox.height + base.boundingBox.height - overlap;
    if (exponent.boundingBox.height > this.boundingBox.height) {
      this.boundingBox.height = exponent.boundingBox.height;
    }
    base.boundingBox.top = exponent.boundingBox.height - overlap;
    base.boundingBox.left = 0;
    this.boundingBox.width = base.boundingBox.width + exponent.boundingBox.width;
    exponent.boundingBox.left = base.boundingBox.width;
    this.boundingBox.fractionLineHeight = base.boundingBox.top + base.boundingBox.fractionLineHeight;
  }

  computeBoundingBoxLeftSingleChild() {
    let child = this.children[0];
    child.boundingBox.left = (this.boundingBox.width - child.boundingBox.width) / 2;
  }

  computeDimensionsDelimiter() {
    this.boundingBox.width = this.element.getBoundingClientRect().width;
    this.boundingBox.height = this.element.getBoundingClientRect().height;
  }

  verticallyStretch(
    /** @type {BoundingBox}*/
    boxToEnclose,
  ) {
    this.element.style.transform = '';
    let heightToStretchTo = boxToEnclose.height * 1.25;
    this.computeDimensionsDelimiter();
    if (heightToStretchTo !== 0) {
      this.boundingBox.stretchFactor = heightToStretchTo / this.boundingBox.height;
    }
    // this.boundingBox.stretchFactor = 1;
    this.boundingBox.fractionLineHeight = boxToEnclose.fractionLineHeight;
    let translateVertical = - this.boundingBox.stretchFactor * this.boundingBox.height / 5;
    this.boundingBox.transform = `matrix(1,0,0,${this.boundingBox.stretchFactor}, 0, ${translateVertical})`;
  }

  computeDimensionsParentheses() {
    let contentBox = this.children[1].boundingBox;
    let left = this.children[0];
    let right = this.children[2];
    left.verticallyStretch(contentBox);
    right.verticallyStretch(contentBox);
    right.boundingBox.left = left.boundingBox.width + contentBox.width;
    left.element.style.transformOrigin = "0 0";
    left.element.style.transform = left.boundingBox.transform;
    right.element.style.transformOrigin = "0 0";
    right.element.style.transform = right.boundingBox.transform;
    this.computeDimensionsStandard();
  }

  computeDimensionsNumerator() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height + 1;
  }

  computeDimensions() {
    if (this.isAtomic()) {
      this.computeDimensionsAtomic();
      return;
    }
    if (this.type.type === knownTypes.fraction.type) {
      this.computeDimensionsFraction();
      return;
    }
    if (this.type.type === knownTypes.numerator.type) {
      this.computeDimensionsNumerator();
    }
    if (this.type.type === knownTypes.baseWithExponent.type) {
      this.computeDimensionsBaseWithExponent();
      return;
    }
    if (this.type.type === knownTypes.parentheses.type) {
      this.computeDimensionsParentheses();
      return;
    }
    this.computeDimensionsStandard();
  }

  computeDimensionsStandard() {
    this.boundingBox.width = 0;
    // Compute present element width and offsets of children.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      this.boundingBox.fractionLineHeight = Math.max(
        this.boundingBox.fractionLineHeight,
        child.boundingBox.fractionLineHeight,
      );
      child.boundingBox.left = this.boundingBox.width;
      this.boundingBox.width += child.boundingBox.width;
    }
    // Compute present element height.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let heightFromChild = child.boundingBox.height - child.boundingBox.fractionLineHeight + this.boundingBox.fractionLineHeight;
      this.boundingBox.height = Math.max(this.boundingBox.height, heightFromChild);
    }
    // Compute top offsets of children.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.top = this.boundingBox.fractionLineHeight - child.boundingBox.fractionLineHeight;
    }
  }

  doAlign() {
    if (this.element.style.width !== 'auto') {
      this.element.style.width = this.boundingBox.width;
    }
    if (this.element.style.height !== 'auto') {
      this.element.style.height = this.boundingBox.height;
    }
    this.element.style.left = this.boundingBox.left;
    this.element.style.top = this.boundingBox.top;
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].doAlign();
    }
  }

  updateParentDOM(/** @type {HTMLElement|null} */ oldElement) {
    let parentElement = null;
    if (this.type.type === knownTypes.root.type) {
      parentElement = this.externalDOM;
    } else {
      if (oldElement !== null) {
        parentElement = oldElement.parentElement;
      }
    }
    if (parentElement === null) {
      return;
    }
    if (parentElement.contains(oldElement)) {
      parentElement.replaceChild(this.element, oldElement);
    } else {
      parentElement.appendChild(this.element);
    }
  }

  handleFocus(e) {
    this.element.style.background = "#f0f0f0";
    this.focused = true;
  }

  handleBlur(e) {
    this.element.style.background = "";
    this.focused = false;
  }

  handleKeyDown(
    /** @type {KeyboardEvent} */
    event,
  ) {
    this.storePositionCaret();
    event.stopPropagation();
    if (this.handleKeyDownCases(event)) {
      event.preventDefault();
    }
    setTimeout(() => {
      this.equationEditor.updateAlignment();
      writeDebugInfo();
    }, 0);
  }

  /** @returns {boolean} whether default should be prevented. */
  handleKeyDownCases(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let key = event.key;
    console.log("DEBUG: handle key down for: " + key);
    switch (key) {
      case "/":
        this.makeFractionNumerator();
        return true;
      case "*":
      case "+":
      case "-":
        this.makeHorizontalOperator(key);
        return true;
      case "^":
        this.makeBaseWithExponent();
        return true;
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        return this.arrow(key);
      case "(":
        this.makeParenthesesLeft();
        return true;
      case "Backspace":
        return this.backspace(event);
      default:
        return false;
    }
  }

  arrowRight(
    /** @type {string} */
    key,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      this.arrow(key);
      return;
    }
    if (this.isAtomWithRightmostCursor()) {
      this.arrow(key);
      return;
    }
  }

  /** @returns{boolean} whether the default should be prevented. */
  arrow(
    /** @type {string} */
    key,
  ) {
    if (this.arrowAbsorbedByAtom(key)) {
      return false;
    }
    /** @type {AtomWithPosition} */
    const toFocus = this.getAtomToFocus(key);
    if (toFocus.element !== null) {
      toFocus.element.focus(toFocus.position);
    }
    return true;
  }

  arrowAbsorbedByAtom(
    /** @type {string} */
    key
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    if (key === "ArrowLeft") {
      return this.positionCaretBeforeKeyEvents !== 0;
    }
    if (key === "ArrowRight") {
      return this.positionCaretBeforeKeyEvents !== this.element.textContent.length;
    }
    return false;
  }

  /** @returns {AtomWithPosition} */
  getAtomToFocus(/** @type {string} */ key) {
    return this.getAtomToFocusFromAction(key, this.type.arrows[key]);
  }

  /** @returns {AtomWithPosition} */
  getAtomToFocusFromAction(
    /** @type {string} */ key,
    /** @type {string} */ arrowType,
  ) {
    if (arrowType === arrowMotion.parentForward) {
      if (this.parent === null) {
        return this;
      }
      return this.parent.getAtomToFocus(key);
    }
    if (arrowType === arrowMotion.firstAtomToTheLeft) {
      return new AtomWithPosition(this.firstAtomToTheLeft(), 1);
    }
    if (arrowType === arrowMotion.firstAtomToTheRight) {
      return new AtomWithPosition(this.firstAtomToTheRight(), - 1);
    }
    return new AtomWithPosition(null, - 1);
  }

  // Returns first MathNode atom that lies to the right of 
  // the present element or null if there is no such element. 
  firstAtomToTheRight() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.firstAtomToTheRightOf(this.indexInParent);
  }

  firstAtomToTheRightOf(/** @type{number}*/ childIndex) {
    for (let i = childIndex + 1; i < this.children.length; i++) {
      let candidate = this.children[i].leftmostAtomChild();
      if (candidate !== null) {
        return candidate;
      }
    }
    return this.firstAtomToTheRight();
  }

  firstAtomToTheLeft() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.firstAtomToTheLeftOf(this.indexInParent);
  }

  firstAtomToTheLeftOf(/** @type {number} */ childIndex) {
    for (let i = childIndex - 1; i >= 0; i--) {
      let candidate = this.children[i].rightmostAtomChild(i);
      if (candidate !== null) {
        return candidate;
      }
    }
    return this.firstAtomToTheLeft();
  }

  leftmostAtomChild() {
    if (this.type.type === knownTypes.atom.type) {
      return this;
    }
    for (let i = 0; i < this.children.length; i++) {
      let candidate = this.children[i].leftmostAtomChild();
      if (candidate !== null) {
        return candidate;
      }
    }
    return null;
  }

  rightmostAtomChild() {
    if (this.type.type === knownTypes.atom.type) {
      return this;
    }
    for (let i = this.children.length - 1; i >= 0; i--) {
      let candidate = this.children[i].rightmostAtomChild();
      if (candidate !== null) {
        return candidate;
      }
    }
    return null;
  }

  storePositionCaret() {
    if (this.type.type !== knownTypes.atom.type) {
      this.positionCaretBeforeKeyEvents = - 1;
      return;
    }
    let range = window.getSelection().getRangeAt(0);
    let selected = range.toString().length;
    let rangeClone = range.cloneRange();
    rangeClone.selectNodeContents(this.element);
    rangeClone.setEnd(range.endContainer, range.endOffset);
    this.positionCaretBeforeKeyEvents = rangeClone.toString().length - selected;
    // window.getSelection().removeAllRanges();
    // window.getSelection().addRange(rangeClone);
  }

  appendChild(/** @type{MathNode} */ child) {
    let needsNormalization = false;
    if (
      child.type.type === knownTypes.horizontalMath.type &&
      this.type.type === knownTypes.horizontalMath.type
    ) {
      needsNormalization = true;
    }
    if (needsNormalization) {
      // Horizontally laid-out math not allowed to contain other 
      // horizontally laid-out math.
      // As an example, we give the LaTeX expression a+b+{c+d},
      // in which the curly braces are not rendered, but give an 
      // "invisible" grouping. That would not be allowed here;
      // instead, that expression would have to be normalized to 
      // a+b+c+d.
      for (let i = 0; i < child.children.length; i++) {
        this.appendChild(child.children[i]);
      }
      return;
    }
    if (child.parent !== null && child.indexInParent >= 0) {
      child.parent.children[child.indexInParent] = null;
    }
    child.parent = this;
    child.indexInParent = this.children.length;
    this.children.push(child);
  }

  normalizeHorizontalMath() {
    if (!this.type.type === knownTypes.horizontalMath.type) {
      return false;
    }
    let found = false;
    while (
      this.normalizeHorizontalMathInHorizontalMathOnce() ||
      this.normalizeHorizontalMathAtomNextToAtomOnce()
    ) {
      found = true;
    }
    return found;
  }

  normalizeHorizontalMathInHorizontalMathOnce() {
    let normalizedChildren = [];
    let found = false;
    for (let i = 0; i < this.children.length; i++) {
      let current = this.children[i];
      if (current.type.type === knownTypes.horizontalMath.type) {
        found = true;
        for (let j = 0; j < current.children.length; j++) {
          normalizedChildren.push(current.children[j]);
        }
      } else {
        normalizedChildren.push(current);
      }
    }
    if (!found) {
      return false;
    }
    this.replaceChildRangeWithChildren(0, this.children.length - 1, normalizedChildren);
  }

  normalizeHorizontalMathAtomNextToAtomOnce() {
    for (let i = 0; i < this.children.length - 1; i++) {
      let current = this.children[i];
      let next = this.children[i + 1];
      if (
        current.type.type !== knownTypes.atom.type ||
        next.type.type != knownTypes.atom.type
      ) {
        continue;
      }
      current.desiredCaretPosition = current.element.textContent.length;
      current.element.textContent += next.element.textContent;
      this.removeChildRange(i + 1, i + 1);
      return true;
    }
    return false;
  }

  replaceChildAtPositionWithChildren(
    /** @type {number} */
    index,
    /** @type {MathNode[]} */
    newChildren,
  ) {
    this.replaceChildRangeWithChildren(index, index, newChildren);
  }

  replaceChildAtPosition(
    /** @type {number} */
    index,
    /** @type {MathNode} */
    child,
  ) {
    this.replaceChildRangeWithChildren(index, index, [child]);
  }

  insertChildAtPosition(
    /** @type {number} */
    index,
    /** @type {MathNode} */
    child,
  ) {
    this.replaceChildRangeWithChildren(index, index - 1, [child]);
  }

  /** @returns {MathNode} */
  removeChildReplaceWithNull(childIndex) {
    let result = this.children[childIndex];
    result.parent = null;
    result.indexInParent = - 1;
    this.children[childIndex] = null;
    return result;
  }

  removeChildRange(
    /** @type {number} */
    fromIndex,
    /** @type {number} */
    toIndex,
  ) {
    return this.replaceChildRangeWithChildren(fromIndex, toIndex, []);
  }

  replaceChildRangeWithChildren(
    /** @type {number} */
    fromIndex,
    /** @type {number} */
    toIndex,
    /** @type {Array<MathNode>} */
    inputChildren,
  ) {
    // Please do not modify removed as removed can
    // be in use as a member of a sub-tree of the inputChildren.
    let toBeShiftedDown = [];
    for (let i = toIndex + 1; i < this.children.length; i++) {
      toBeShiftedDown.push(this.removeChildReplaceWithNull(i));
    }
    this.children.length = fromIndex;
    for (let i = 0; i < inputChildren.length; i++) {
      this.appendChild(inputChildren[i]);
    }
    for (let i = 0; i < toBeShiftedDown.length; i++) {
      this.appendChild(toBeShiftedDown[i]);
    }
  }

  isAtomWithLeftmostCursor() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    let offset = window.getSelection().getRangeAt(0).startOffset;
    return offset === 0;
  }

  isAtomWithRightmostCursor() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    let offset = window.getSelection().getRangeAt(0).startOffset;
    return offset === this.element.textContent.length;
  }

  /** @returns{boolean} whether the default should be prevented. */
  backspace(/** @type {KeyboardEvent} */ event) {
    if (
      this.positionCaretBeforeKeyEvents !== 0 ||
      this.type.type !== knownTypes.atom.type
    ) {
      return false;
    }
    this.applyBackspace();
    return true;
  }

  /** @returns {boolean} whether input was reduced */
  applyBackspaceHorizontalMathParent() {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    if (this.indexInParent === 0) {
      parent.applyBackspace();
      return true;
    }
    let indexPrevious = this.indexInParent - 1;
    let previous = parent.children[indexPrevious];
    if (previous.type.type === knownTypes.atomImmutable.type) {
      parent.removeChildRange(indexPrevious, indexPrevious);
      parent.normalizeHorizontalMath();
      parent.updateDOM();
      if (indexPrevious - 1 >= 0) {
        parent.children[indexPrevious - 1].focus(1);
      } else {
        parent.focus(-1);
      }
    }
    return previous.applyBackSpaceNonHorizontalMath();
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceFraction() {
    if (this.type.type === knownTypes.horizontalMath.type) {
      if (
        this.parent.type.type === knownTypes.denominator.type ||
        this.parent.type.type === knownTypes.numerator.type
      ) {
        return this.parent.applyBackspaceFraction();
      }
    }
    if (
      this.type.type === knownTypes.denominator.type ||
      this.type.type === knownTypes.numerator.type
    ) {
      return this.parent.applyBackspaceFraction();
    }
    if (this.type.type !== knownTypes.fraction.type) {
      return false;
    }
    let indexInParent = this.indexInParent;
    let horizontal = this.children[0].children[0];
    horizontal.appendChild(this.children[1].children[0]);
    horizontal.normalizeHorizontalMath();
    this.parent.replaceChildAtPosition(indexInParent, horizontal);
    this.parent.normalizeHorizontalMath();
    this.parent.updateDOM();
    this.parent.focus(0);
    return true;
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceExponent() {
    if (this.type.type === knownTypes.horizontalMath.type) {
      if (
        this.parent.type.type === knownTypes.exponent.type
      ) {
        return this.parent.applyBackspaceExponent();
      }
    }
    if (this.type.type !== knownTypes.exponent.type) {
      return false;
    }
    let baseWithExponent = this.parent;
    let indexBaseWithExponent = baseWithExponent.indexInParent;
    let base = baseWithExponent.children[0];
    base.appendChild(this.children[0]);
    base.normalizeHorizontalMath();
    let parent = baseWithExponent.parent;
    parent.replaceChildAtPosition(indexBaseWithExponent, base);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.children[indexBaseWithExponent].focus(0);
    return true;
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceParentheses() {
    if (this.type.type === knownTypes.horizontalMath.type) {
      if (
        this.parent.type.type === knownTypes.parentheses.type
      ) {
        return this.parent.applyBackspaceParentheses();
      }
    }
    if (this.type.type !== knownTypes.parentheses.type) {
      return false;
    }
    let indexInParent = this.indexInParent;
    let horizontal = this.children[1];
    this.parent.replaceChildAtPosition(indexInParent, horizontal);
    this.parent.normalizeHorizontalMath();
    this.parent.updateDOM();
    this.parent.focus(0);
    return true;
  }

  applyBackspace() {
    if (this.parent === null) {
      console.log('Unexpected null parent while updating backspace.');
      return;
    }
    if (this.applyBackspaceHorizontalMathParent()) {
      return;
    }
    this.applyBackSpaceNonHorizontalMath();
  }

  applyBackSpaceNonHorizontalMath() {
    if (this.applyBackspaceFraction()) {
      return;
    }
    if (this.applyBackspaceExponent()) {
      return;
    }
    if (this.applyBackspaceParentheses()) {
      return;
    }
  }

  /** Returns the position of the operator.
   * 
   * - 1 stands for start ("+11"), 0 for middle ("1+1"), 1 for end ("11+"). 
   */
  computePositionOfOperator() {
    let positionOperator = 1;
    if (this.positionCaretBeforeKeyEvents === 0 && this.element.textContent.length > 0) {
      positionOperator = - 1;
    } else if (
      this.positionCaretBeforeKeyEvents > 0 &&
      this.positionCaretBeforeKeyEvents < this.element.textContent.length
    ) {
      positionOperator = 0;
    }
    return positionOperator;
  }

  /** @returns {ParentWithIndex} */
  computeHorizontalMathParent() {
    let result = new ParentWithIndex(this.parent, this.indexInParent);
    while (result.parent !== null) {
      if (result.parent.type.type === knownTypes.horizontalMath.type) {
        return result;
      }
      result.parent = result.parent.parent;
      result.index = result.parent.indexInParent;
    }
    return result;
  }

  makeHorizontalOperator(
    /** @type {string} */
    key,
  ) {
    this.storePositionCaret();
    let positionOperator = this.computePositionOfOperator();
    // Find closest ancestor node that's of type horizontal math.
    let parentAndIndex = this.computeHorizontalMathParent();
    if (parentAndIndex.parent === null) {
      // No ancestor is of type horizontal math. 
      console.log('Warning: could not find ancestor of type horizontal math.');
      return;
    }
    let oldIndexInParent = parentAndIndex.indexInParent;
    let parent = parentAndIndex.parent;
    if (positionOperator === 1) {
      parent.insertChildAtPosition(oldIndexInParent + 1, mathNodeFactory.atomImmutable(this.equationEditor, key));
      parent.insertChildAtPosition(oldIndexInParent + 2, mathNodeFactory.atom(this.equationEditor, ""));
    } else if (positionOperator === 0) {
      let leftContent = this.element.textContent.slice(0, this.positionCaretBeforeKeyEvents);
      let rightContent = this.element.textContent.slice(this.positionCaretBeforeKeyEvents, this.element.textContent.length);
      parent.replaceChildAtPosition(oldIndexInParent, mathNodeFactory.atom(this.equationEditor, leftContent));
      parent.insertChildAtPosition(oldIndexInParent + 1, mathNodeFactory.atomImmutable(this.equationEditor, key));
      parent.insertChildAtPosition(oldIndexInParent + 2, mathNodeFactory.atom(this.equationEditor, rightContent));
    }
    parent.updateDOM();
    parent.children[oldIndexInParent + 2].focus(- 1);
  }

  makeParenthesesLeft() {
    let positionOperator = this.computePositionOfOperator();
    // Find closest ancestor node that's of type horizontal math.
    let parentAndIndex = this.computeHorizontalMathParent();
    if (parentAndIndex.parent === null) {
      // No ancestor is of type horizontal math. 
      console.log('Warning: could not find ancestor of type horizontal math.');
      return;
    }
    let oldIndexInParent = parentAndIndex.indexInParent;
    let parent = parentAndIndex.parent;
    if (parent.children[oldIndexInParent] !== this) {
      parent.children[oldIndexInParent].doMakeParenthesesLeft(positionOperator);
    } else {
      this.doMakeParenthesesLeft(positionOperator);
    }
  }

  doMakeParenthesesLeft(positionOperator) {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      console.log('Warning: making parentheses in non-horizontal math.');
    }
    let oldIndexInParent = this.indexInParent;
    if (positionOperator === 1) {
      let parenthesesContent = mathNodeFactory.horizontalMath(this.equationEditor, mathNodeFactory.atom(this.equationEditor, ""));
      for (let i = oldIndexInParent + 1; i < parent.children.length; i++) {
        parenthesesContent.children.push(parent.children[i]);
      }
      parenthesesContent.normalizeHorizontalMath();
      let parentheses = mathNodeFactory.parentheses(
        this.equationEditor,
        parenthesesContent,
      );
      parent.replaceChildRangeWithChildren(
        oldIndexInParent + 1,
        parent.children.length,
        [parentheses],
      );
      parent.ensureEditableAtomBothSides();
      parent.updateDOM();
      parent.children[oldIndexInParent + 1].focus(- 1);
    } else if (positionOperator === 0) {
      let leftContent = this.element.textContent.slice(0, this.positionCaretBeforeKeyEvents);
      let rightContent = this.element.textContent.slice(this.positionCaretBeforeKeyEvents, this.element.textContent.length);
      parent.replaceChildAtPosition(oldIndexInParent, mathNodeFactory.atom(this.equationEditor, leftContent));
      let parentheses = mathNodeFactory.parentheses(this.equationEditor, mathNodeFactory.atom(this.equationEditor, rightContent));
      parent.insertChildAtPosition(oldIndexInParent + 1, parentheses);
      parent.ensureEditableAtomBothSides();
      parent.updateDOM();
      parent.children[oldIndexInParent + 1].focus(- 1);
    } else {
      let parentheses = mathNodeFactory.parentheses(this.equationEditor, this);
      parent.replaceChildAtPosition(oldIndexInParent, parentheses);
      parent.ensureEditableAtomBothSides();
      parent.updateDOM();
      parentheses.focus(- 1);
    }
  }

  ensureEditableAtomBothSides() {
    if (this.type.type !== knownTypes.horizontalMath.type) {
      console.log('Warning: call ensureEditableAtomToTheRight on non-horizontal math.');
      return;
    }
    if (this.children.length === 0) {
      this.insertChildAtPosition(0, mathNodeFactory.atom(this.equationEditor, ""));
      return;
    }
    if (!this.children[this.children.length - 1].isAtomEditable()) {
      this.insertChildAtPosition(this.children.length, mathNodeFactory.atom(this.equationEditor, ""));
    }
    if (!this.children[0].isAtomEditable()) {
      this.insertChildAtPosition(0, mathNodeFactory.atom(this.equationEditor, ""));
    }
  }

  makeFractionNumerator() {
    if (this.parent === null) {
      return;
    }
    const oldParent = this.parent;
    const oldIndexInParent = this.indexInParent;
    const fraction = mathNodeFactory.fractionEmptyDenominator(this.equationEditor, this);

    oldParent.replaceChildAtPosition(oldIndexInParent, mathNodeFactory.atom(this.equationEditor, ""));
    oldParent.insertChildAtPosition(oldIndexInParent + 1, fraction);
    oldParent.insertChildAtPosition(oldIndexInParent + 2, mathNodeFactory.atom(this.equationEditor, ""));
    fraction.parent.updateDOM();
    fraction.children[1].focus(- 1);
  }

  makeBaseWithExponent() {
    if (this.parent === null) {
      return;
    }
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, this);
    originalParent.replaceChildAtPosition(originalIndexInParent, baseWithExponent);
    originalParent.updateDOM();
    baseWithExponent.children[1].children[0].focus(- 1);
  }

  /** Focuses the HTMLElement that belongs to the math node.
   * 
   * The endToFocus parameter denotes where the focus should occur.
   * At the moment, negative endToFocus indicates the caret should
   * be on the element's left, and positive endToFocus indicates the caret 
   * should go on the right. Zero indicates to focus at this.positionCaretBeforeKeyEvents.
   */
  focus(endToFocus) {
    if (this.type.type === knownTypes.atom.type) {
      if (this.element !== null) {
        this.element.focus(null);
        let position = 0;
        if (endToFocus === 0) {
          position = this.desiredCaretPosition;
        }
        if (endToFocus > 0) {
          position = this.element.textContent.length;
        }
        if (position < 0) {
          position = 0;
        }
        if (position > this.element.textContent.length) {
          position = this.element.textContent.length;
        }
        this.setCursorPosition(position);
      }
      return true;
    }
    if (this.children.length < 1) {
      return false;
    }
    if (endToFocus === 1) {
      for (let i = this.children.length - 1; i >= 0; i--) {
        if (this.children[i].focus(endToFocus)) {
          return true;
        }
      }
    } else {
      for (let i = 0; i < this.children.length; i++) {
        if (this.children[i].focus(endToFocus)) {
          return true;
        }
      }
    }
    return false;
  }

  setCursorPosition(/** @type {number}*/ position) {
    var range = document.createRange();
    var selection = window.getSelection();
    selection.removeAllRanges();
    let start = true;
    if (position >= this.element.textContent.length && position > 0) {
      range.selectNodeContents(this.element);
      range.selectNodeContents(this.element);
      start = false;
    } else {
      range.setStart(this.element, position);
    }
    range.collapse(start);
    selection.removeAllRanges();
    selection.addRange(range);
    this.positionCaretBeforeKeyEvents = position;
  }

  toString(indentationLevel) {
    const indentation = "-".repeat(indentationLevel);
    const result = [];
    let content = `${indentation}${this.type.type}`;
    if (this.element !== null) {
      content += `[${this.element.textContent}]`;
    }
    if (this.boundingBox.width !== 0) {
      content += `, w: ${this.boundingBox.width}`;
    }
    if (this.boundingBox.height !== 0) {
      content += `, h: ${this.boundingBox.height}`;
    }
    if (this.boundingBox.left !== 0) {
      content += `, l: ${this.boundingBox.left}`;
    }
    if (this.boundingBox.top !== 0) {
      content += `, t: ${this.boundingBox.top}`;
    }
    if (this.boundingBox.fractionLineHeight !== 0) {
      content += `, fl: ${this.boundingBox.fractionLineHeight}`;
    }
    if (this.type.type === knownTypes.atom.type) {
      content += `, caret: ${this.positionCaretBeforeKeyEvents}, `;
      content += `desiredCaret: ${this.desiredCaretPosition}`;
    }
    result.push(content);
    for (let i = 0; i < this.children.length; i++) {
      result.push(this.children[i].toString(indentationLevel + 1));
    }
    return result.join("\n<br>\n");
  }

  toLatexFraction() {
    let result = `\\frac{${this.children[0].toLatex()}}{${this.children[1].toLatex()}}`;
    if (this.children.length <= 2) {
      return result;
    }
    // This is not expected to happen: a fraction should have exactly two children.
    result += "[";
    for (let i = 2; i < this.children.length; i++) {
      result += this.children[i].toLatex();
    }
    result += "]";
    return result;
  }

  toLatexAtom() {
    return this.element.textContent;
  }

  toLatex() {
    if (this.type.type === knownTypes.fraction.type) {
      return this.toLatexFraction();
    }
    if (
      this.type.type === knownTypes.atom.type ||
      this.type.type === knownTypes.atomImmutable.type
    ) {
      return this.toLatexAtom();
    }
    let toJoin = [];
    for (let i = 0; i < this.children.length; i++) {
      toJoin.push(this.children[i].toLatex());
    }
    return toJoin.join("");
  }
}

class ParentWithIndex {
  constructor(
    /** @type{MathNode} */
    parent,
    /**@type{number} */
    indexInParent,
  ) {
    /** @type{MathNode} */
    this.parent = parent;
    /** @type{number} */
    this.indexInParent = indexInParent;
  }
}


function writeDebugInfo() {
  document.getElementById("latex").innerHTML = lastCreatedEquationEditor.rootNode.toLatex();
  document.getElementById("debug").innerHTML = lastCreatedEquationEditor.toString();
}

/** @type {EquationEditor|null} */
var defaultEquationEditorLocal = null;

function initialize() {
  defaultEquationEditorLocal = new EquationEditor(document.getElementById('equation-editor'));
  MathQuill.getInterface(2).MathField(document.getElementById('mq-editor'));
}

function testEquationEditor() {
  defaultEquationEditorLocal.rootNode.focus(- 1);
  setTimeout(() => {
    defaultEquationEditorLocal.sendKeys([
      //"2", "^", "3", "ArrowLeft", "ArrowLeft", "ArrowLeft", "(",
      //"1", "+", "1", "Backspace", "Backspace", "2",
      // "(", "1", "/", "1",
      //"1", "/", "1", "ArrowUp", "ArrowUp", "ArrowUp", "(",
      /*      "1", "+", "1",
            "1", "+", "1", "/", "1", "+", "1", "/",
            "1", "+", "1", "/", "1",
            "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp",
            "+", "1", "/", "1",*/
    ]);
  }, 300);
}

module.exports = {
  defaultEquationEditorLocal,
  testEquationEditor,
  EquationEditor,
  initialize
};