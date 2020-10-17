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
    this.colorImplied = input["colorImplied"];
    this.colorText = input["colorText"];
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
    this.borderTop = input["borderTop"];
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
  // Colors
  "colorText": "",
  "colorImplied": "",
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
  "borderTop": "",
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

const defaultFractionScale = 0.75;
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
    "minWidth": "4px",
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
    "colorImplied": "silver",
  }),
  // Right delimiter (parentheses, bracket, ...)
  rightDelimiter: new MathNodeType({
    "type": "rightDelimiter",
    "colorImplied": "silver",
  }),
  // Horizontally laid out math such as "x+2". 
  // The example "x+2" consists of the three elements "x", "+" and 2.
  // Not allowed to contain other horizontally laid out math elements.
  horizontalMath: new MathNodeType({
    "type": "horizontalMath",
    "whiteSpace": "nowrap",
    // "width": "100%",
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
  sqrt: new MathNodeType({
    "type": "sqrt",
    "width": "auto",
    "height": "auto",
    "margin": "2px",
  }),
  sqrtSign: new MathNodeType({
    "type": "sqrtSign",
  }),
  radicalExponentBox: new MathNodeType({
    "type": "radicalExponentBox",
    "fontSize": 0.55,
    "minHeightScale": 0.55,
  }),
  radicalUnderBox: new MathNodeType({
    "type": "radicalUnderBox",
    "borderTop": "2px solid black",
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
  fraction(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode|null}*/
    numeratorContent,
    /** @type{MathNode|null}*/
    denominatorContent,
  ) {
    const fraction = new MathNode(equationEditor, knownTypes.fraction);
    const numerator = new MathNode(equationEditor, knownTypes.numerator);
    const denominator = new MathNode(equationEditor, knownTypes.denominator);
    numerator.appendChild(this.horizontalMath(equationEditor, numeratorContent));
    denominator.appendChild(this.horizontalMath(equationEditor, denominatorContent));
    fraction.appendChild(numerator);
    fraction.appendChild(denominator);
    return fraction;
  }

  /** @returns {MathNode} */
  sqrtSign(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    let result = new MathNode(equationEditor, knownTypes.sqrtSign);
    result.initialContent = "\u221A";
    return result;
  }

  /** @returns {MathNode} */
  sqrt(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    underTheRadicalContent,
  ) {
    const sqrt = new MathNode(equationEditor, knownTypes.sqrt);
    const radicalExponentBox = new MathNode(equationEditor, knownTypes.radicalExponentBox);
    radicalExponentBox.appendChild(this.horizontalMath(equationEditor, null));
    sqrt.appendChild(radicalExponentBox);
    sqrt.appendChild(mathNodeFactory.sqrtSign(equationEditor));
    const underTheRadical = new MathNode(equationEditor, knownTypes.radicalUnderBox);
    underTheRadical.appendChild(mathNodeFactory.horizontalMath(equationEditor, underTheRadicalContent));
    sqrt.appendChild(underTheRadical);
    return sqrt;
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
  leftParenthesis(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {boolean} */
    implied,
  ) {
    const leftParentheses = new MathNode(equationEditor, knownTypes.leftDelimiter);
    leftParentheses.initialContent = "(";
    leftParentheses.implied = implied;
    return leftParentheses;
  }

  rightParenthesis(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {boolean} */
    implied,
  ) {
    const rightParentheses = new MathNode(equationEditor, knownTypes.rightDelimiter);
    rightParentheses.initialContent = ")";
    rightParentheses.implied = implied;
    return rightParentheses;
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
    /** @type {MathNode|null} */
    this.element = element;
    /** @type {number} */
    this.position = position;
    if (this.element === null) {
      this.position = -1;
    }
  }

  /** @returns {AtomWithPosition} */
  leftNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, -1);
    }
    if (this.position > 0) {
      return new AtomWithPosition(this.element, this.position - 1);
    }
    let resultElement = this.element.firstAtomToTheLeft();
    if (resultElement === null) {
      return new AtomWithPosition(null, - 1);
    }
    return new AtomWithPosition(resultElement, resultElement.element.textContent.length);
  }

  /** @returns {AtomWithPosition} */
  rightNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, -1);

    }
    if (this.position < this.element.element.textContent.length) {
      return new AtomWithPosition(this.element, this.position + 1);
    }
    return new AtomWithPosition(this.element.firstAtomToTheRight(), 0);
  }

  /** @returns {string} */
  toString() {
    if (this.element === null) {
      return `[null, ${this.position}]`;
    }
    return `[${this.element.toString()}, ${this.position}]`;
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
    /** @type{AtomWithPosition} */
    this.selectionStart = new AtomWithPosition(null, -1);
    this.selectionEnd = new AtomWithPosition(null, -1);
  }

  toHtml() {
    let result = `Latex: ${this.rootNode.toLatex()}`;
    result += `<br>Drawing: ${this.rootNode.toString()}`;
    result += `<br>Structure: ${this.rootNode.toHtml(0)}`;
    return result;
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

  /** @returns {SplitBySelectionResult|null} */
  splitAtomsBySelection() {
    if (this.selectionStart.element === null || this.selectionEnd.element === null) {
      return null;
    }
    let start = this.selectionStart;
    let end = this.selectionEnd;
    if (end.element.isToTheLeftOf(start.element)) {
      // Swap end and start
      let temporary = start;
      start = end;
      end = temporary;
    }
    let ancestor = start.element.commonAncestor(end.element);
    if (ancestor === null) {
      console.log(`Not expected: no common ancestor element betwen ${start} and ${end}. `);
      return null;
    }
    if (ancestor.type.type !== knownTypes.horizontalMath.type) {
      // Common ancestor is not of type horizontal math. Don't split.
      return null;
    }
    if (start.element === ancestor || end.element === ancestor) {
      // Start and end should are the same node, which is the ancestor.
      return null;
    }
    while (start.element.parent !== ancestor && start.element.parent !== null) {
      start.element = start.element.parent;
      start.position = - 1;
    }
    while (end.element.parent !== ancestor && end.element.parent !== null) {
      end.element = end.element.parent;
      end.position = - 1;
    }
    let startSplit = start.element.splitByPosition(start.position);
    let endSplit = end.element.splitByPosition(end.position);
    let result = new SplitBySelectionResult(ancestor);
    for (let i = 0; i < start.element.indexInParent; i++) {
      result.beforeSplit.push(ancestor.children[i]);
    }
    if (startSplit[0] !== null) {
      result.beforeSplit.push(startSplit[0]);
    }
    if (startSplit[1] !== null) {
      result.split.push(startSplit[1]);
    }
    for (let i = start.element.indexInParent + 1; i < end.element.indexInParent; i++) {
      result.split.push(ancestor.children[i]);
    }
    if (endSplit[0] !== null) {
      result.split.push(endSplit[0]);
    }
    if (endSplit[1] !== null) {
      result.afterSplit.push(endSplit[1]);
    }
    for (let i = end.element.indexInParent + 1; i < ancestor.children.length; i++) {
      result.afterSplit.push(ancestor.children[i]);
    }
    return result;
  }
}

class SplitBySelectionResult {
  constructor(
    /**@type{MathNode} */
    ancestor,
  ) {
    /**@type{MathNode[]} */
    this.beforeSplit = [];
    /**@type{MathNode[]} */
    this.afterSplit = [];
    /**@type{MathNode[]} */
    this.split = [];
    /**@type{MathNode} */
    this.ancestor = ancestor;
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
    this.transformOrigin = "";
  }
};

class MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNodeType} */
    type,
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
    /** @type {boolean} */
    this.focusDesired = false;
    /** @type {boolean} 
     * Indicates whether the node was explicitly entered by the user 
     * or has been implied by a user action, such as parentheses auto-balancing.
     */
    this.implied = false;
    /** @type {EquationEditor} */
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
    if (this.type.borderTop !== "") {
      this.element.style.borderTop = this.type.borderTop;
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
    this.element.addEventListener("keyup", (e) => {
      this.handleKeyUp(e);
    });
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
      this.type.type === knownTypes.atomImmutable.type ||
      this.type.type === knownTypes.leftDelimiter.type ||
      this.type.type === knownTypes.rightDelimiter.type ||
      this.type.type === knownTypes.sqrtSign.type
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
    if (this.implied) {
      this.element.style.color = this.type.colorImplied;
    } else {
      this.element.style.color = this.type.colorText;
    }
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

  verticallyStretchParenthesis(
    /** @type {number}*/
    heightToEnclose,
  ) {
    this.element.style.transformOrigin = "top left";
    this.element.style.transform = "";
    this.element.style.width = "";
    this.element.style.height = "";
    this.element.style.top = "";
    this.element.style.left = "";
    this.computeDimensionsAtomic();
    if (heightToEnclose !== 0) {
      heightToEnclose *= 1.3;
      this.boundingBox.stretchFactor = heightToEnclose / this.boundingBox.height;
      this.boundingBox.height = heightToEnclose;
    }
    // For many fonts, the parenteses's middle appears to be below 
    // the middle of the line. We therefore translate the parenthesis up 
    // (negative translation) with a % of its bounding box height.
    let translateUpPercent = 0.15; //0.1;
    this.boundingBox.fractionLineHeight = (1 - translateUpPercent) * this.boundingBox.height * 0.5;
    let translateVertical = - this.boundingBox.height * translateUpPercent;
    this.boundingBox.transformOrigin = "top left";
    this.boundingBox.transform = `matrix(1,0,0,${this.boundingBox.stretchFactor}, 0, ${translateVertical})`;
  }

  computeDimensionsNumerator() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height + 1;
  }

  verticallyStretchSqrt(
    /** @type {number}*/
    heightToEnclose,
  ) {
    this.element.style.transformOrigin = "top left";
    this.element.style.transform = "";
    this.element.style.width = "";
    this.element.style.height = "";
    this.element.style.top = "";
    this.element.style.left = "";
    this.computeDimensionsAtomic();
    if (heightToEnclose !== 0) {
      heightToEnclose *= 1.3;
      this.boundingBox.stretchFactor = heightToEnclose / this.boundingBox.height;
      this.boundingBox.height = heightToEnclose;
    }
    this.boundingBox.transformOrigin = "top left";
    this.boundingBox.transform = `matrix(1,0,0,${this.boundingBox.stretchFactor}, 0, 0)`;
  }

  computeDimensionsSqrt() {
    let radicalExponentBox = this.children[0];
    let sqrtSign = this.children[1];
    let underTheRadical = this.children[2];
    // The proportion of the width of the sqrt sign that overlaps with the radical exponent.
    let sqrtProportionOverlapped = 0.85;
    let extraWidth = Math.max(0, radicalExponentBox.boundingBox.width - sqrtSign.boundingBox.width * sqrtProportionOverlapped);
    // The top of the sqrt sign may not connect perfectly with the overline of the under-the-radical content.
    // The following variable compensates for that.
    sqrtSign.verticallyStretchSqrt(underTheRadical.boundingBox.height);
    let widthSqrtSign = 0.99;// 0.92;
    sqrtSign.boundingBox.left = extraWidth;
    underTheRadical.boundingBox.left = sqrtSign.boundingBox.left + sqrtSign.boundingBox.width * widthSqrtSign;
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = underTheRadical.boundingBox.height * 1.15;
    this.boundingBox.fractionLineHeight = underTheRadical.boundingBox.fractionLineHeight;
    this.boundingBox.width = underTheRadical.boundingBox.left + underTheRadical.boundingBox.width;
  }

  computeDimensionsRadicalUnderBox() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height + 1;
  }

  computeDimensions() {
    if (this.isAtomic()) {
      this.computeDimensionsAtomic();
      return;
    }
    if (this.type.type === knownTypes.sqrt.type) {
      this.computeDimensionsSqrt();
      return;
    }
    if (this.type.type === knownTypes.radicalUnderBox.type) {
      this.computeDimensionsRadicalUnderBox();
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
    if (this.type.type === knownTypes.horizontalMath) {
      this.computeDimensionsHorizontalMath();
    }
    this.computeDimensionsStandard();
  }

  computeDimensionsStandard() {
    this.computeDimensionsHorizontalMath();
  }

  computeDimensionsHorizontalMathParenthesesHeight() {
    // Compute parentheses height
    /** @type {number[]} */
    let enclosedHeights = [];
    /** @type {number[]} */
    let indicesOpenedParentheses = [];
    for (let i = 0; i < this.children.length; i++) {
      this.computeDimensionsHorizontalMathParenthesesAccountChild(
        i, enclosedHeights, indicesOpenedParentheses
      );
    }
  }

  computeDimensionsHorizontalMathParenthesesAccountChild(
    /** @type {number} */
    index,
    /** @type {number[]} */
    enclosedHeights,
    /** @type {number[]} */
    indicesOpenedParentheses,
  ) {
    let child = this.children[index];
    if (child.type.type === knownTypes.leftDelimiter.type) {
      enclosedHeights.push(0);
      indicesOpenedParentheses.push(index);
      return;
    }
    if (child.type.type === knownTypes.rightDelimiter.type) {
      if (enclosedHeights.length <= 0) {
        console.log("Warning: unbalanced right parenthesis.");
        return;
      }
      let currentHeight = enclosedHeights.pop();
      child.verticallyStretchParenthesis(currentHeight);
      let leftCounterpart = this.children[indicesOpenedParentheses.pop()];
      leftCounterpart.verticallyStretchParenthesis(currentHeight);
    }
    if (enclosedHeights.length > 0) {
      enclosedHeights[enclosedHeights.length - 1] = Math.max(
        child.boundingBox.height, enclosedHeights[enclosedHeights.length - 1]
      );
    }
  }

  computeDimensionsHorizontalMath() {
    this.boundingBox = new BoundingBox();
    this.computeDimensionsHorizontalMathParenthesesHeight();
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      this.boundingBox.fractionLineHeight = Math.max(
        this.boundingBox.fractionLineHeight,
        child.boundingBox.fractionLineHeight,
      );
      child.boundingBox.left = this.boundingBox.width;
      this.boundingBox.width += child.boundingBox.width;
    }
    // Compute top offsets of children.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.top = this.boundingBox.fractionLineHeight - child.boundingBox.fractionLineHeight;
    }
    // Compute present element height.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let heightFromChild = child.boundingBox.height - child.boundingBox.fractionLineHeight + this.boundingBox.fractionLineHeight;
      this.boundingBox.height = Math.max(this.boundingBox.height, heightFromChild);
    }
  }

  doAlign() {
    if (this.boundingBox.transform !== "") {
      this.element.style.transformOrigin = this.boundingBox.transformOrigin;
      this.element.style.transform = this.boundingBox.transform;
    } else {
      if (this.element.style.width !== "auto") {
        this.element.style.width = this.boundingBox.width;
      }
      if (this.element.style.height !== "auto") {
        this.element.style.height = this.boundingBox.height;
      }
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

  handleKeyUp(
    /** @type {KeyboardEvent} */
    event,
  ) {
    if (!event.shiftKey && event.key !== "Shift") {
      this.equationEditor.selectionStart = new AtomWithPosition(null, - 1);
      this.equationEditor.selectionEnd = new AtomWithPosition(null, - 1);
    }
  }

  /** @returns {boolean} whether default should be prevented. */
  handleKeyDownCases(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let key = event.key;
    let shiftHeld = event.shiftKey;
    console.log("DEBUG: handle key down for: " + key);
    console.log("DEBUG: shiftHeld: " + shiftHeld);
    switch (key) {
      case "/":
        this.makeFractionNumerator();
        return true;
      case "*":
      case "+":
      case "-":
        this.makeHorizontalOperatorCorrectInput(key);
        return true;
      case "^":
        this.makeBaseWithExponent();
        return true;
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        return this.arrow(key, shiftHeld);
      case "(":
        this.makeParenthesesLeft();
        return true;
      case ")":
        this.makeParenthesesRight();
        return true;
      case "Enter":
        return true;
      case "s":
        this.makeSqrt();
        return true;
      case "Delete":
        return this.deleteButton();
      case "Backspace":
        return this.backspace();
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

  setSelectionStart() {
    if (this.equationEditor.selectionStart.element !== null) {
      return;
    }
    this.equationEditor.selectionStart = new AtomWithPosition(this, this.positionCaretBeforeKeyEvents);
    this.equationEditor.selectionEnd = new AtomWithPosition(this, this.positionCaretBeforeKeyEvents);
  }

  /** @returns{number[]} The path needed to get from the current node to the root node.*/
  getPathToRoot() {
    let reversed = [];
    let current = this;
    while (current !== null) {
      if (current.indexInParent !== -1) {
        reversed.push(current.indexInParent);
      }
      current = current.parent;
    }
    return reversed.reverse();
  }

  /** @returns{MathNode|null} returns the common ancestor of two nodes. */
  commonAncestor(
    /**@type{MathNode} */
    other,
  ) {
    if (this === other || other === null) {
      return this;
    }
    if (this.equationEditor.rootNode !== other.equationEditor.rootNode) {
      return null;
    }
    let thisPath = this.getPathToRoot();
    let otherPath = other.getPathToRoot();
    let current = this.equationEditor.rootNode;
    for (let i = 0; i < thisPath.length; i++) {
      if (i >= otherPath.length) {
        return current;
      }
      if (thisPath[i] !== otherPath[i]) {
        return current;
      }
      current = current.children[thisPath[i]];
    }
    return current;
  }

  /** @returns{boolean} Given two atoms, decides whether this is to the left of the other atom. */
  isToTheLeftOf(
    /**@type{MathNode} */ other,
  ) {
    let thisPath = this.getPathToRoot();
    let otherPath = other.getPathToRoot();
    for (let i = 0; i < thisPath.length; i++) {
      if (i >= otherPath.length) {
        // The this element is contained in the other element: this 
        // is not expected as both elements are required to be atoms and cannot contain one another.
        console.log(`Element ${this.toString()} is contained in ${other.toString()}, which is not allowed in this function. `);
        return false;
      }
      if (thisPath[i] < otherPath[i]) {
        return true;
      }
      if (thisPath[i] > otherPath[i]) {
        return false;
      }
    }
    // The two elements are equal.
    return false;
  }

  selectBetween(
    /** @type{AtomWithPosition} */
    end1,
    /** @type{AtomWithPosition} */
    end2,
  ) {
    console.log(`DEBUG: select from: ${end1.toString()}, to ${end2.toString()}`);
    let shouldSwap = end2.element.isToTheLeftOf(end1.element);
    if (end1.element === end2.element) {
      shouldSwap = (end2.position < end1.position);
    }
    if (shouldSwap) {
      // Swap the two ends.
      let end2copy = end2;
      end2 = end1;
      end1 = end2copy;
      console.log(`DEBUG: selection corrected: ${end1.toString()}, to ${end2.toString()}`);
    }
    let newRange = document.createRange();
    end1.element.setRangeStart(newRange, end1.position);
    end2.element.setRangeEnd(newRange, end2.position);
    document.getSelection().removeAllRanges();
    console.log(`DEBUG: And the new selection: ${newRange}`);
    document.getSelection().addRange(newRange);
  }

  /** @returns{boolean} whether the default should be prevented. */
  setSelectionEnd(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    console.log("DEBUG: inside set selection end");
    if (!shiftHeld) {
      return false;
    }
    let newSelection = null;
    if (key === "ArrowLeft" || key === "ArrowUp") {
      newSelection = this.equationEditor.selectionEnd.leftNeighbor();
    } else {
      newSelection = this.equationEditor.selectionEnd.rightNeighbor();
    }
    if (newSelection.element === null) {
      return false;
    }
    this.equationEditor.selectionEnd = newSelection;
    this.selectBetween(
      this.equationEditor.selectionStart,
      this.equationEditor.selectionEnd,
    );

    return true;
  }

  /** @returns{boolean} whether the default should be prevented. */
  arrow(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    if (shiftHeld) {
      this.setSelectionStart(key, shiftHeld);
      this.setSelectionEnd(key, shiftHeld);
      return true;
    }
    if (this.arrowAbsorbedByAtom(key)) {
      return false;
    }
    /** @type {AtomWithPosition} */
    const toFocus = this.getAtomToFocus(key);
    if (toFocus.element !== null) {
      toFocus.element.positionCaretBeforeKeyEvents = toFocus.element.element.textContent.length + 1;
      toFocus.element.focus(toFocus.position);
    }
    return true;
  }

  arrowAbsorbedByAtom(
    /** @type {string} */
    key,
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
        return new AtomWithPosition(null, - 1);
      }
      return this.parent.getAtomToFocus(key);
    }
    if (arrowType === arrowMotion.firstAtomToTheLeft) {
      return (new AtomWithPosition(this, 0)).leftNeighbor();
    }
    if (arrowType === arrowMotion.firstAtomToTheRight) {
      return (new AtomWithPosition(this, this.element.textContent.length)).rightNeighbor();
    }
    return new AtomWithPosition(null, - 1);
  }

  /** @returns{MathNode|null} returns sibling, right one if direction is positive, left one otherwise. */
  siblingAtom(
    /**@type{number} */
    direction,
  ) {
    if (direction > 0) {
      return this.firstAtomToTheRight();
    } else {
      return this.firstAtomToTheLeft();
    }
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
    if (child.parent !== this && child.parent !== null && child.indexInParent >= 0) {
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

  mergeAtomContentToTheRight(
    /**@type{MathNode} */
    right,
  ) {
    let thisContent = this.initialContent;
    if (this.element !== null) {
      thisContent = this.element.textContent;
    }
    let rightContent = right.initialContent;
    if (right.element !== null) {
      rightContent = right.element.textContent;
    }
    if (right.desiredCaretPosition !== -1) {
      this.desiredCaretPosition = thisContent.length + right.desiredCaretPosition;
    }
    thisContent += rightContent;
    if (this.element === null) {
      this.initialContent = thisContent;
    } else {
      this.element.textContent = thisContent;
    }
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
      current.mergeAtomContentToTheRight(next);
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

  removeChild(
    /** @type {number} */
    index,
  ) {
    return this.replaceChildRangeWithChildren(index, index, []);
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
    // Disown all removed children.
    for (
      let i = fromIndex;
      i <= toIndex && i >= 0 && i < this.children.length;
      i++
    ) {
      if (this.children[i] === null) {
        continue;
      }
      this.children[i].parent = null;
    }
    // Store all children that need to be shifted down.
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
  deleteButton() {
    if (
      this.positionCaretBeforeKeyEvents !== this.element.textContent.length ||
      this.type.type !== knownTypes.atom.type
    ) {
      return false;
    }
    let sibling = this.siblingAtom(1);
    if (sibling === null) {
      return false;
    }
    this.positionCaretBeforeKeyEvents = - 1;
    sibling.positionCaretBeforeKeyEvents = 0;
    return sibling.backspace();
  }

  /** @returns{boolean} whether the default should be prevented. */
  backspace() {
    if (
      this.positionCaretBeforeKeyEvents !== 0 ||
      this.type.type !== knownTypes.atom.type
    ) {
      return false;
    }
    this.desiredCaretPosition = 0;
    return this.applyBackspaceToTheLeft();
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheLeftNumerator() {
    if (this.type.type !== knownTypes.numerator.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftNumeratorOrDenominator();
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheLeftDenominator() {
    if (this.type.type !== knownTypes.denominator.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftNumeratorOrDenominator();
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheLeftNumeratorOrDenominator() {
    let fraction = this.parent;
    let fractionIndexInParent = fraction.indexInParent;
    let horizontal = fraction.children[0].children[0];
    horizontal.appendChild(fraction.children[1].children[0]);
    horizontal.normalizeHorizontalMath();
    let fractionParent = fraction.parent;
    fractionParent.replaceChildAtPosition(fractionIndexInParent, horizontal);
    fractionParent.normalizeHorizontalMath();
    fractionParent.updateDOM();
    fractionParent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheLeftExponent() {
    if (this.type.type !== knownTypes.exponent.type) {
      return false;
    }
    let baseWithExponent = this.parent;
    let indexBaseWithExponent = baseWithExponent.indexInParent;
    let base = baseWithExponent.children[0];
    this.children[0].children[0].desiredCaretPosition = 0;
    base.appendChild(this.children[0]);
    base.normalizeHorizontalMath();
    let parent = baseWithExponent.parent;
    parent.replaceChildAtPosition(indexBaseWithExponent, base);
    writeDebugInfo();
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheLeftOfSqrtSign() {
    if (this.type.type !== knownTypes.radicalUnderBox.type) {
      return false;
    }
    let sqrt = this.parent;
    let indexSqrt = sqrt.indexInParent;
    let exponent = sqrt.children[0].children[0];
    this.children[0].children[0].desiredCaretPosition = 0;
    let parent = sqrt.parent;
    parent.replaceChildAtPositionWithChildren(indexSqrt, [exponent, sqrt.children[2].children[0]])
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction ocurred. */
  applyBackspaceToTheRightDelimiter() {
    if (
      this.type.type !== knownTypes.rightDelimiter.type &&
      this.type.type !== knownTypes.leftDelimiter.type
    ) {
      return false;
    }
    let matchingIndex = this.parent.findIndexMatchingDelimiter(this.indexInParent);
    if (matchingIndex === -1) {
      console.log('Unexpected failure to find matching left parenthesis.');
      return false;
    }
    if (!this.parent.children[matchingIndex].implied) {
      this.implied = true;
      this.parent.focusCancelOnce();
      this.focus(-1);
      this.updateDOM();
      return true;
    }
    let parent = this.parent;
    let startingIndexInParent = this.indexInParent;
    parent.removeChild(Math.max(startingIndexInParent, matchingIndex));
    parent.removeChild(Math.min(startingIndexInParent, matchingIndex));
    parent.normalizeHorizontalMath();
    this.parent.updateDOM();
    this.parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheLeftHorizontalMathParent() {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    if (this.indexInParent === 0) {
      return this.parent.applyBackspaceToTheLeft();
    }
    let indexPrevious = this.indexInParent - 1;
    let previous = parent.children[indexPrevious];
    return previous.applyBackspaceToTheRight();
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheLeft() {
    if (this.parent === null) {
      // We've reached the root node by pushing backspace in the first position.
      this.focusCancelOnce();
      return false;
    }
    if (this.applyBackspaceToTheLeftNumerator()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftDenominator()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftExponent()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftOfSqrtSign()) {
      return true;
    }
    if (this.indexInParent === 0) {
      return this.parent.applyBackspaceToTheLeft();
    }
    if (this.applyBackspaceToTheLeftHorizontalMathParent()) {
      return true;
    }
    this.parent.focusCancelOnce();
    return false;
  }

  /** @returns {boolean} whether backspae was applied */
  applyBackspaceToTheRightFraction() {
    if (this.type.type !== knownTypes.fraction.type) {
      return false;
    }
    this.parent.children[this.indexInParent + 1].focusCancelOnce();
    this.children[1].focus(1);
    return true;
  }

  /** @returns {boolean} whether backspae was applied */
  applyBackspaceToTheRightBaseWithExponent() {
    if (this.type.type !== knownTypes.baseWithExponent.type) {
      return false;
    }
    this.parent.children[this.indexInParent + 1].focusCancelOnce();
    this.children[1].focus(1);
    return true;
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheRightAtomImmutable() {
    if (this.type.type !== knownTypes.atomImmutable.type) {
      return false;
    }
    let parent = this.parent;
    parent.children[this.indexInParent + 1].desiredCaretPosition = 0;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheRight() {
    if (this.applyBackspaceToTheRightAtomImmutable()) {
      return true;
    }
    if (this.applyBackspaceToTheRightDelimiter()) {
      return true;
    }
    if (this.applyBackspaceToTheRightFraction()) {
      return true;
    }
    if (this.applyBackspaceToTheRightBaseWithExponent()) {
      return true;
    }
    return false;
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

  makeHorizontalOperatorCorrectInput(
    /** @type {string} */
    contentTransformedToMathSymbol,
  ) {
    if (contentTransformedToMathSymbol === "-") {
      this.makeHorizontalOperator("\u2212");
      return;
    }
    if (contentTransformedToMathSymbol === "*") {
      this.makeHorizontalOperator("\u00B7");
      return;
    }
    this.makeHorizontalOperator(contentTransformedToMathSymbol);
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
    this.makeParenthesesCommon(true);
  }

  makeParenthesesRight() {
    this.makeParenthesesCommon(false);
  }

  makeSqrt() {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let split = this.splitByCaret();
    let sqrt = mathNodeFactory.sqrt(this.equationEditor, split[1]);
    if (split[0] === null) {
      parent.replaceChildAtPosition(oldIndex, sqrt);
    } else {
      parent.replaceChildAtPosition(oldIndex, split[0]);
      parent.insertChildAtPosition(oldIndex + 1, sqrt);
    }
    parent.ensureEditableAtoms();
    parent.updateDOM();
    sqrt.children[2].focusStandard(0);
  }

  makeParenthesesCommon(
    /** @type{boolean} */
    isLeft,
  ) {
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
    parent.children[oldIndexInParent].doMakeParenthesesCommon(positionOperator, isLeft);
  }

  findIndexToInsertRightDelimiter(
    /**@type {number} */
    indexToInserLeftDelimiter,
  ) {
    if (indexToInserLeftDelimiter >= this.children.length) {
      return this.children.length;
    }
    let openDelimiters = 0;
    for (let i = indexToInserLeftDelimiter; i < this.children.length; i++) {
      let child = this.children[i];
      if (child.type.type === knownTypes.rightDelimiter.type) {
        openDelimiters--;
      } else if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters++;
      }
      if (openDelimiters < 0) {
        return i;
      }
    }
    return this.children.length;
  }

  findIndexToInsertLeftDelimiter(
    /**@type {number} */
    indexInsertedRightDelimiter,
  ) {
    let openDelimiters = 0;
    for (let i = indexInsertedRightDelimiter - 1; i >= 0; i--) {
      let child = this.children[i];
      if (child.type.type === knownTypes.rightDelimiter.type) {
        openDelimiters++;
      } else if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters--;
      }
      if (openDelimiters < 0) {
        return i + 1;
      }
    }
    return 0;
  }

  /** @returns {number}*/
  findIndexMatchingDelimiter(
    /**@type {number} */
    indexStartingDelimiter,
  ) {
    let startingDelimiter = knownTypes.leftDelimiter.type;
    let endingDelimiter = knownTypes.rightDelimiter.type;
    let direction = 1;
    if (this.children[indexStartingDelimiter].type.type === knownTypes.rightDelimiter.type) {
      endingDelimiter = knownTypes.leftDelimiter.type;
      startingDelimiter = knownTypes.rightDelimiter.type;
      direction = -1;
    }
    let openDelimiters = 1;
    for (
      let i = indexStartingDelimiter + direction;
      i < this.children.length && i >= 0;
      i += direction
    ) {
      let child = this.children[i];
      if (child.type.type === startingDelimiter) {
        openDelimiters++;
      } else if (child.type.type === endingDelimiter) {
        openDelimiters--;
      }
      if (openDelimiters == 0) {
        return i;
      }
    }
    return -1;
  }

  /** @returns {MathNode[]} */
  splitByCaret() {
    return this.splitByPosition(this.positionCaretBeforeKeyEvents)
  }

  /** @returns {MathNode[]} */
  splitByPosition(
    /** @type{number} */
    position,
  ) {
    if (this.type.type !== knownTypes.atom.type || position < 0) {
      return [null, this];
    }
    if (position === 0) {
      return [null, this];
    }
    if (position >= this.element.textContent.length) {
      return [this, null];
    }
    let leftContent = this.element.textContent.slice(0, position);
    let rightContent = this.element.textContent.slice(position, this.element.textContent.length);
    let leftNode = mathNodeFactory.atom(this.equationEditor, leftContent);
    let rightNode = mathNodeFactory.atom(this.equationEditor, rightContent);
    return [leftNode, rightNode];
  }

  doMakeParenthesesCommon(
    /**@type {number} */
    positionOperator,
    /**@type {boolean} */
    isLeft,
  ) {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      console.log('Warning: making parentheses in non-horizontal math.');
    }
    let oldIndexInParent = this.indexInParent;
    let leftParenthesis = mathNodeFactory.leftParenthesis(
      this.equationEditor,
      !isLeft,
    );
    let rightParenthesis = mathNodeFactory.rightParenthesis(
      this.equationEditor,
      isLeft,
    );
    if (positionOperator === 0) {
      let leftAndRight = this.splitByCaret();
      parent.replaceChildAtPositionWithChildren(
        oldIndexInParent, leftAndRight,
      );
    }
    let indexLeftDelimiter = - 1;
    let indexRightDelimiter = - 1;
    if (isLeft) {
      if (positionOperator === 1) {
        indexLeftDelimiter = oldIndexInParent + 1;
      } else if (positionOperator === 0) {
        indexLeftDelimiter = oldIndexInParent + 1;
      } else {
        indexLeftDelimiter = oldIndexInParent;
      }
      if (parent.replaceImpliedLeftDelimiter(indexLeftDelimiter)) {
        return;
      }
      indexRightDelimiter = parent.findIndexToInsertRightDelimiter(indexLeftDelimiter);
    } else {
      if (positionOperator === 1) {
        indexRightDelimiter = oldIndexInParent + 1;
      } else if (positionOperator === 0) {
        indexRightDelimiter = oldIndexInParent + 1;
      } else {
        indexRightDelimiter = oldIndexInParent;
      }
      if (parent.replaceImpliedRightDelimiter(indexRightDelimiter)) {
        return;
      }
      indexLeftDelimiter = parent.findIndexToInsertLeftDelimiter(indexRightDelimiter);
    }

    parent.insertChildAtPosition(indexRightDelimiter, rightParenthesis);
    parent.insertChildAtPosition(indexLeftDelimiter, leftParenthesis);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    if (isLeft) {
      leftParenthesis.focus(1);
    } else {
      rightParenthesis.focus(1);
    }
  }

  /** @returns{boolean} */
  replaceImpliedLeftDelimiter(
    /**@type {number}*/
    delimiterIndex,
  ) {
    let openDelimiters = 0;
    if (delimiterIndex >= this.children.length) {
      delimiterIndex = this.children[this.children.length - 1];
    }
    for (let i = delimiterIndex; i >= 0; i--) {
      let child = this.children[i];
      if (child.type.type === knownTypes.rightDelimiter.type) {
        openDelimiters++;
        continue
      }
      if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters--;
        if (openDelimiters >= 0) {
          continue;
        }
        if (child.implied) {
          return this.moveDelimiterMarkExplicit(delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @returns{boolean} */
  replaceImpliedRightDelimiter(
    /**@type {number}*/
    delimiterIndex,
  ) {
    let openDelimiters = 0;
    for (let i = delimiterIndex; i < this.children.length; i++) {
      let child = this.children[i];
      if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters++;
        continue
      }
      if (child.type.type === knownTypes.rightDelimiter.type) {
        openDelimiters--;
        if (openDelimiters >= 0) {
          continue;
        }
        if (child.implied) {
          return this.moveDelimiterMarkExplicit(delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @returns{boolean} */
  moveDelimiterMarkExplicit(
    /**@type {number}*/
    toIndex,
    /**@type {number}*/
    fromIndex,
  ) {
    let delimiterReplaced = this.children[fromIndex];
    delimiterReplaced.element = null;
    this.removeChild(fromIndex);
    if (toIndex > fromIndex) {
      toIndex--;
    }
    this.insertChildAtPosition(toIndex, delimiterReplaced);
    delimiterReplaced.implied = false;
    this.ensureEditableAtoms();
    this.updateDOM();
    delimiterReplaced.focus(1);
    return true;
  }

  ensureEditableAtoms() {
    if (this.type.type !== knownTypes.horizontalMath.type) {
      console.log("Warning: call ensureEditableAtomToTheRight on non-horizontal math. ");
      return;
    }
    let correctedChildren = [];
    if (this.children.length === 0) {
      correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
    } else if (!this.children[0].isAtomEditable()) {
      correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
    }
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let next = null;
      if (i + 1 < this.children.length) {
        next = this.children[i + 1];
      }
      correctedChildren.push(child);
      if (!child.isAtomEditable()) {
        if (next === null) {
          correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
        } else if (!next.isAtomEditable()) {
          correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
        }
      }
    }
    if (correctedChildren.length > this.children.length) {
      this.replaceChildRangeWithChildren(0, this.children.length, correctedChildren);
    }
  }

  makeFractionNumerator() {
    if (this.parent === null) {
      return;
    }
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection === null) {
      this.makeFractionNumeratorFromCaretPosition();
    } else {
      this.makeFractionNumeratorFromSelection(splitBySelection);
    }
  }

  makeFractionNumeratorFromCaretPosition() {
    const oldParent = this.parent;
    const oldIndexInParent = this.indexInParent;
    let fraction = null;
    let childIndexToFocus = 1;
    if (this.positionCaretBeforeKeyEvents === this.element.textContent.length) {
      fraction = mathNodeFactory.fraction(this.equationEditor, this, null);
    } else if (this.positionCaretBeforeKeyEvents === 0) {
      fraction = mathNodeFactory.fraction(this.equationEditor, null, this);
      childIndexToFocus = 0;
    } else {
      let split = this.splitByCaret();
      fraction = mathNodeFactory.fraction(this.equationEditor, split[0], split[1]);
    }
    oldParent.replaceChildAtPosition(oldIndexInParent, mathNodeFactory.atom(this.equationEditor, ""));
    oldParent.insertChildAtPosition(oldIndexInParent + 1, fraction);
    oldParent.insertChildAtPosition(oldIndexInParent + 2, mathNodeFactory.atom(this.equationEditor, ""));
    fraction.parent.updateDOM();
    fraction.children[childIndexToFocus].focus(- 1);
  }

  makeFractionNumeratorFromSelection(
    /**@type{SplitBySelectionResult} */
    split,
  ) {
    let ancestor = split.ancestor;
    ancestor.removeChildRange(0, ancestor.children.length - 1);
    for (let i = 0; i < split.beforeSplit.length; i++) {
      ancestor.appendChild(split.beforeSplit[i]);
    }
    let numerator = mathNodeFactory.horizontalMath(this.equationEditor, null);
    for (let i = 0; i < split.split.length; i++) {
      numerator.appendChild(split.split[i]);
    }
    numerator.normalizeHorizontalMath();
    let fraction = mathNodeFactory.fraction(this.equationEditor, numerator, null);
    ancestor.appendChild(fraction);
    for (let i = 0; i < split.afterSplit.length; i++) {
      ancestor.appendChild(split.afterSplit[i]);
    }
    ancestor.normalizeHorizontalMath();
    ancestor.ensureEditableAtoms();
    ancestor.updateDOM();
    fraction.children[1].focus(- 1);
  }

  isEmptyAtom() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    if (this.element === null) {
      return false;
    }
    return this.element.textContent === "";
  }

  previousHorizontalSibling() {
    if (this.parent === null) {
      return null;
    }
    if (this.parent.type.type !== knownTypes.horizontalMath.type) {
      return null;
    }
    if (this.indexInParent <= 0) {
      return null;
    }
    return this.parent.children[this.indexInParent - 1];
  }

  makeBaseWithExponent() {
    if (this.parent === null) {
      return;
    }
    if (this.isEmptyAtom()) {
      let previous = this.previousHorizontalSibling();
      if (previous !== null) {
        previous.makeBaseWithExponent();
        return;
      }
    }
    if (
      this.type.type === knownTypes.rightDelimiter.type
    ) {
      this.makeBaseWithExponentRightDelimiter();
      return;
    }
    this.makeBaseDefaultWithExponent();
  }

  makeBaseWithExponentRightDelimiter() {
    let rightIndex = this.indexInParent;
    let originalParent = this.parent;
    let leftIndex = originalParent.findIndexMatchingDelimiter(rightIndex);
    let base = mathNodeFactory.horizontalMath(this.equationEditor, null);
    for (let i = leftIndex; i < rightIndex + 1; i++) {
      base.children.push(originalParent.children[i]);
    }
    base.normalizeHorizontalMath();
    base.ensureEditableAtoms();
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, base);
    baseWithExponent.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildRangeWithChildren(leftIndex, rightIndex, [baseWithExponent]);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  makeBaseDefaultWithExponent() {
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, this);
    baseWithExponent.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildAtPosition(originalIndexInParent, baseWithExponent);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  focusAtom(endToFocus) {
    if (this.element === null) {
      this.desiredCaretPosition = - 1;
      return false;
    }
    // The call to focus(null) will wipe the desiredCaretPosition.
    let originalDesiredPosition = this.desiredCaretPosition;
    // this.element.focus(null);
    let position = 0;
    if (endToFocus === 0) {
      position = originalDesiredPosition;
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
    this.setCaretPosition(position);
    this.desiredCaretPosition = -1;
    return true;
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
      return this.focusAtom(endToFocus);
    }
    return this.focusStandard(endToFocus);
  }

  focusStandard(endToFocus) {
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
    if (endToFocus === 0) {
      return false;
    }
    let sibling = this.siblingAtom(endToFocus);
    if (sibling === null) {
      return false;
    }
    return sibling.focus(-endToFocus);
  }

  /** @returns {boolean} whether focus request was find. */
  focusCancelOnce() {
    if (this.desiredCaretPosition !== -1) {
      this.desiredCaretPosition = -1;
      return true;
    }
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].focusCancelOnce()) {
        return true;
      }
    }
    return false;
  }

  /** @returns {boolean} whether focused child was found. */
  focusRestore() {
    if (this.desiredCaretPosition !== -1) {
      this.focus(0);
      return true;
    }
    this.desiredCaretPosition = - 1;
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].focusRestore()) {
        return true;
      }
    }
    return false;
  }

  setRangeStart(
    /** @type{Range}  */
    range,
    /** @type {number} */
    position,
  ) {
    if (this.element.childNodes.length > 0) {
      range.setStart(this.element.childNodes[0], position);
    } else {
      range.setStart(this.element, 0);
    }
  }

  setRangeEnd(
    /** @type{Range}  */
    range,
    /** @type {number} */
    position,
  ) {
    if (this.element.childNodes.length > 0) {
      range.setEnd(this.element.childNodes[0], position);
    } else {
      range.setEnd(this.element, 0);
    }
  }

  setCaretPosition(/** @type {number}*/ position) {
    let range = document.createRange();
    let collapseToStart = true;
    if (position >= this.element.textContent.length) {
      collapseToStart = false;
      range.selectNodeContents(this.element);
    } else if (position > 0) {
      collapseToStart = false;
      this.setRangeStart(range, position);
    } else {
      collapseToStart = true;
      this.setRangeStart(range, 0)
    }
    console.log(`Position: ${position}, range ${range}, collapseToStart: ${collapseToStart} start offset: ${range.startOffset}, end offset: ${range.endOffset}, text len: ${this.element.textContent.length}`);
    range.collapse(collapseToStart);
    window.getSelection().removeAllRanges();
    window.getSelection().addRange(range);
    //    this.element.focus();
  }

  toString() {
    const result = [];
    if (this.isAtomic()) {
      result.push(this.element.textContent);
    }
    result.push(`[${this.type.type}]`)
    if (this.children.length > 0) {
      result.push("{");
      for (let i = 0; i < this.children.length; i++) {
        result.push(this.children[i].toString());
        if (i !== this.children.length - 1) {
          result.push(",");
        }
      }
      result.push("}");
    }
    return result.join("");
  }

  toHtml(indentationLevel) {
    if (indentationLevel === null || indentationLevel === undefined) {
      indentationLevel = 0;
    }
    const indentation = "-".repeat(indentationLevel);
    const result = [];
    let content = `${indentation}${this.type.type}`;
    if (this.element !== null) {
      content += `[${this.element.textContent}]`;
    }
    if (this.focused) {
      content += ', F';
    }
    if (this.desiredCaretPosition !== -1) {
      content += `, FD[${this.desiredCaretPosition}]`;
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
    }
    result.push(content);
    for (let i = 0; i < this.children.length; i++) {
      result.push(this.children[i].toHtml(indentationLevel + 1));
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
    if (this.element === null) {
      return "[null]";
    }
    return this.element.textContent;
  }

  toLatexBaseWithExponent() {
    return `{${this.children[0].toLatex()}}^{${this.children[1].toLatex()}}`;
  }

  toLatexLeftDelimiter() {
    if (this.element === null) {
      return "[null(]";
    }
    return `\\left${this.element.textContent}`;
  }

  toLatexRightDelimiter() {
    if (this.element === null) {
      return "[null)]";
    }
    return `\\right${this.element.textContent}`;
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
    if (this.type.type === knownTypes.baseWithExponent.type) {
      return this.toLatexBaseWithExponent();
    }
    if (this.type.type === knownTypes.leftDelimiter.type) {
      return this.toLatexLeftDelimiter();
    }
    if (this.type.type === knownTypes.rightDelimiter.type) {
      return this.toLatexRightDelimiter();
    }
    let toJoin = [];
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i] === null) {
        toJoin.push("[[null]]");
        continue;
      }
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
  document.getElementById("debug").innerHTML = lastCreatedEquationEditor.toHtml();
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