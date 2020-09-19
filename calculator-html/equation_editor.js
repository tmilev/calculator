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
    this.borderStyle = input["borderStyle"];
    this.borderBottom = input["borderBottom"];
    this.minHeightScale = input["minHeightScale"];
    this.width = input["width"];
    this.height = input["height"];
    this.display = input["display"];
    this.padding = input["padding"];
    this.flexDirection = input["flexDirection"];
    this.justifyContent = input["justifyContent"];
    this.alignContent = input["alignContent"];
    this.alignItems = input["alignItems"];
    this.verticalAlign = input["verticalAlign"];
    this.margin = input["margin"];
    this.outline = input["outline"];
    this.fontSize = input["fontSize"];
    this.whiteSpace = input["whiteSpace"];
    this.textAlign = input["textAlign"];
    this.float = input["float"];
    this.boxSizing = input["boxSizing"];
    this.position = input["position"];
  }
}

const knownTypeDefaults = {
  "display": "inline-block",
  "minHeightScale": 0,
  "borderBottom": "",
  "borderStyle": "",
  "flexDirection": "",
  "justifyContent": "",
  "alignContent": "",
  "alignItems": "",
  "verticalAlign": "",
  "margin": "",
  "padding": "",
  "fontSize": "",
  "whiteSpace": "",
  "float": "",
  "textAlign": "",
  "boxSizing": "border-box",
  "position": "absolute",
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
    "minHeightScale": 1,
    "padding": atomPad,
    "outline": "0px solid transparent",
  }),
  // A non-editable math expression/operator such as "+" or "-".
  atomImmutable: new MathNodeType({
    "type": "atomImmutable",
    "minHeightScale": 1,
    "padding": `${0.02}em`,
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
    "display": "block",
    "borderBottom": "1px solid black",
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
    "display": "block",
    "fontSize": defaultFractionScale,
    "minHeightScale": defaultFractionScale,
    "arrows": {
      "ArrowUp": arrowMotion.firstAtomToTheLeft,
      "ArrowDown": arrowMotion.firstAtomToTheRight,
    },
    "textAlign": "center",
    // "whiteSpace": "nowrap",
  }),
  exponent: new MathNodeType({
    "type": "exponent",
    // "minHeightScale": defaultSubSuperScriptScale,
    // "fontSize": defaultSubSuperScriptScalePercent,
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
    operator
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
    numeratorContent
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
  exponent(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    const exponent = new MathNode(knownTypes.exponent);
    exponent.appendChild(this.horizontalMath(null));
    return exponent;
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
    let specialKeys = new Set(["+", "-", "*", "/", "ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown"]);
    let key = keys[0];
    if (!specialKeys.has(key)) {
      this.dispatchInputToFirstFocusedChild(this.rootNode, key);
    } else {
      const eventsToSend = ["keydown", "keyup", "keypress"];
      for (let i = 0; i < eventsToSend.length; i++) {
        let event = new KeyboardEvent(
          eventsToSend[i], {
          key: key,
        });
        this.dispatchEventToFirstFocusedChild(this.rootNode, event);
      }
    }
    setTimeout(() => {
      this.sendKeys(keys.slice(1));
    }, 400);
  }
  dispatchEventToFirstFocusedChild(
    /** @type {MathNode} */
    container,
    event,
  ) {
    if (container.focused) {
      container.element.dispatchEvent(event);
      return true;
    }
    for (let i = 0; i < container.children.length; i++) {
      if (this.dispatchEventToFirstFocusedChild(container.children[i], event)) {
        return true;
      }
    }
    return false;
  }

  dispatchInputToFirstFocusedChild(
    /** @type {MathNode} */
    container,
    content,
  ) {
    if (container.focused) {
      container.element.textContent = content;
      container.focus(1);
      return true;
    }
    for (let i = 0; i < container.children.length; i++) {
      if (this.dispatchInputToFirstFocusedChild(container.children[i], content)) {
        return true;
      }
    }
    return false;
  }

  updateAlignment() {
    this.rootNode.updateAlignment(new BoundingBox());
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
    this.heightFromChildren = 0;
    /** @type {number} */
    this.scale = 0;
    /** @type {HTMLElement} */
    this.externalDOM = null;
    /** @type {number} */
    this.positionCaretBeforeKeyEvents = - 1;
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
    if (this.type.borderStyle !== "") {
      this.element.style.border = this.type.borderStyle;
    }
    if (this.type.borderBottom !== "") {
      this.element.style.borderBottom = this.type.borderBottom;
    }
    if (this.type.padding !== "") {
      this.element.style.padding = this.type.padding;
    }
    if (this.type.margin !== "") {
      this.element.style.margin = this.type.margin;
    }
    this.element.style.width = this.type.width;
    this.element.style.height = this.type.height;
    this.element.style.display = this.type.display;
    // this.element.style.minHeight = this.type.minHeightScale * fontSize;
    this.element.style.minWidth = this.type.minHeightScale * fontSize / 1.6;
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
    this.heightFromChildren = 0;
    this.element.setAttribute("mathTagName", this.type.type);
    this.element.addEventListener("keyup", (e) => { this.handleKeyUp(e); });
    this.element.addEventListener("keydown", (e) => { this.handleKeyDown(e); });
    this.element.addEventListener("focus", (e) => { this.handleFocus(e); });
    this.element.addEventListener("blur", (e) => { this.handleBlur(e); });
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
  hasHorizontalLayout() {
    if (this.type.type === knownTypes.fraction) {
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

  updateAlignment(
    /** @type {BoundingBox} */
    boundingBoxSoFar
  ) {

    let parentLayoutHorizontal = true;
    if (this.parent !== null) {
      parentLayoutHorizontal = this.parent.hasHorizontalLayout();
    }
    if (parentLayoutHorizontal) {
      this.boundingBox.left = boundingBoxSoFar.left + boundingBoxSoFar.width;
      this.boundingBox.top = boundingBoxSoFar.top;
    } else {
      this.boundingBox.top = boundingBoxSoFar.top + boundingBoxSoFar.height;
      this.boundingBox.left = boundingBoxSoFar.left;
    }
    this.element.style.left = this.boundingBox.left;
    this.element.style.top = this.boundingBox.top;
    let boundingBoxChildren = new BoundingBox();
    boundingBoxChildren.left = this.boundingBox.left;
    boundingBoxChildren.top = this.boundingBox.top;
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.updateAlignment(boundingBoxChildren);
      boundingBoxChildren.width += child.boundingBox.width;
      if (parentLayoutHorizontal) {
        boundingBoxChildren.height = Math.max(boundingBoxChildren.height, child.boundingBox.height);
      } else {
        boundingBoxChildren.width = Math.max(boundingBoxChildren.width, child.boundingBox.width);
      }
    }
    this.boundingBox.width = boundingBoxChildren.width;
    this.boundingBox.height = boundingBoxChildren.height;
    if (this.isAtomic()) {
      this.boundingBox.width = this.element.getBoundingClientRect().width;
      this.boundingBox.height = this.element.getBoundingClientRect().height;
    }
    this.element.style.width = this.boundingBox.width;
    this.element.style.height = this.boundingBox.height;
    this.element.style.left = this.boundingBox.left;
    this.element.style.right = this.boundingBox.right;
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

  handleKeyUp(
    /** @type {KeyboardEvent} */
    event,
  ) {
    this.handleKeyUpCases(event);
    event.preventDefault();
    event.stopPropagation();
    writeDebugInfo();
  }

  handleKeyUpCases() {
    let key = event.key;
    switch (key) {
      case "/":
        this.makeFractionNumerator();
        return;
      case "*":
      case "+":
      case "-":
        this.makeHorizontalOperator(key);
        return;
      case "^":
        this.makeExponent();
        return;
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        this.arrow(key);
        return;
      case "Backspace":
        this.updateBackspace(event);
        return;
      default:
        return;
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

  arrow(
    /** @type {string} */
    key,
  ) {
    if (this.arrowAbsorbedByAtom(key)) {
      return;
    }
    /** @type {AtomWithPosition} */
    const toFocus = this.getAtomToFocus(key);
    if (toFocus.element !== null) {
      toFocus.element.focus(toFocus.position);
    }
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
      return this.positionCaretBeforeKeyEvents !== this.element.textContent.length
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

  handleKeyDown(
    /** @type {KeyboardEvent} */
    event,
  ) {
    this.storePositionCarretBeforeKeyEvents();
    this.handleKeyDownCases(event);
    event.stopPropagation();
    writeDebugInfo();
  }

  handleKeyDownCases(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let key = event.key;
    switch (key) {
      case "+":
      case "-":
      case "*":
      case "/":
      case "^":
      case "ArrowDown":
      case "ArrowUp":
        event.preventDefault();
        return;
    }
  }

  storePositionCarretBeforeKeyEvents() {
    if (this.type.type !== knownTypes.atom.type) {
      this.positionCaretBeforeKeyEvents = - 1;
      return;
    }
    this.positionCaretBeforeKeyEvents = window.getSelection().getRangeAt(0).startOffset;
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
    child.parent = this;
    child.indexInParent = this.children.length;
    this.children.push(child);
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

  replaceChildRangeWithChildren(
    /** @type {number} */
    fromIndex,
    /** @type {number} */
    toIndex,
    /** @type {Array<MathNode>} */
    inputChildren
  ) {
    // Please do not modify removed as removed can
    // be in use as a member of a sub-tree of the inputChildren.
    let toBeShiftedDown = [];
    for (let i = toIndex + 1; i < this.children.length; i++) {
      toBeShiftedDown.push(this.children[i]);
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

  updateBackspace(/** @type {KeyboardEvent} */ event) {
    if (this.parent === null) {
      return;
    }
    if (
      this.positionCaretBeforeKeyEvents !== 0 ||
      this.type.type !== knownTypes.atom.type
    ) {
      return;
    }
  }

  makeHorizontalOperator(
    /** @type {string} */
    key,
  ) {
    let parent = this.parent;
    // - 1 stands for start ("+11"), 0 for middle ("1+1"), 1 for end ("11+"). 
    let positionOperator = 1;
    if (this.positionCaretBeforeKeyEvents === 0 && this.element.textContent.length > 0) {
      positionOperator = - 1;
    } else if (this.positionCaretBeforeKeyEvents > 0 && this.positionCaretBeforeKeyEvents < this.element.textContent.length) {
      positionOperator = 0;
    }
    // Find closest ancestor node that's of type horizontal math.
    while (parent !== null) {
      if (parent.type.type === knownTypes.horizontalMath.type) {
        break;
      }
      parent = parent.parent;
    }
    if (parent === null) {
      // No ancestor is of type horizontal math. 
      console.log('Warning: could not find ancestor of type horizontal math.');
      return;
    }
    let oldIndexInParent = this.indexInParent;
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
  makeExponent() {
    if (this.parent === null) {
      return;
    }
    const exponent = mathNodeFactory.exponent(this.equationEditor);
    this.parent.insertChildAtPosition(this.indexInParent + 1, exponent);
    this.parent.updateDOM();
    exponent.children[0].focus(- 1);
  }

  /** Focuses the HTMLElement that belongs to the math node.
   * 
   * The endToFocus parameter denotes where the focus should occur.
   * At the moment, negative endToFocus indicates the caret should
   * be on the element's left, and positive endToFocus indicates the caret 
   * should go on the right.
   */
  focus(endToFocus) {
    if (this.type.type === knownTypes.atom.type) {
      if (this.element !== null) {
        this.element.focus(null);
        let position = 0;
        if (endToFocus > 0) {
          position = this.element.textContent.length;
        }
        this.setCursorPosition(position);
      }
      return;
    }
    if (this.children.length < 1) {
      return;
    }
    this.children[0].focus(endToFocus);
  }

  setCursorPosition(/** @type {number}*/ position) {
    var range = document.createRange();
    var selection = window.getSelection();
    range.setStart(this.element, position);
    range.collapse(true);
    selection.removeAllRanges();
    selection.addRange(range);
  }

  toString(indentationLevel) {
    const indentation = "--".repeat(indentationLevel);
    const result = [];
    let content = `${indentation}${this.type.type}`;
    if (this.type.type === knownTypes.atom.type) {
      if (this.element !== null) {
        content += `[${this.element.textContent}]`;
      }
    }
    result.push(content);
    for (let i = 0; i < this.children.length; i++) {
      result.push(this.children[i].toString(indentationLevel + 1));
    }
    return result.join('\n<br>\n');
  }
}

function writeDebugInfo() {
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
      "1", "+", "1",
      "1", "+", "1", "/", "1", "+", "1", "/",
      "1", "+", "1", "/", "1",
      "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp",
      "+", "1", "/", "1"
    ]);
  }, 300);
}

module.exports = {
  defaultEquationEditorLocal,
  testEquationEditor,
  EquationEditor,
  initialize
};