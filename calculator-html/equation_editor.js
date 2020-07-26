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
    this.scale = input["scale"];
    this.padding = input["padding"];
    this.flexDirection = input["flexDirection"];
    this.justifyContent = input["justifyContent"];
    this.alignContent = input["alignContent"];
    this.alignItems = input["alignItems"];
    this.verticalAlign = input["verticalAlign"];
    this.margin = input["margin"];
  }
}

const knownTypeDefaults = {
  "display": "inline-block",
  "minHeightScale": 0,
  "borderBottom": "",
  "borderStyle": "",
  "scale": 1,
  "flexDirection": "",
  "justifyContent": "",
  "alignContent": "",
  "alignItems": "",
  "verticalAlign": "",
  "margin": "",
  "padding": "",
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
  "ArrowUp": [arrowMotion.parentForward],
  "ArrowDown": [arrowMotion.parentForward],
  "ArrowLeft": [arrowMotion.firstAtomToTheLeft],
  "ArrowRight": [arrowMotion.firstAtomToTheRight],
};

const knownTypes = {
  root: new MathNodeType({
    "type": "root",
    "borderStyle": "1px solid black",
    "padding": "2px",
  }),
  // A math expression with no children such as "x", "2".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    "type": "atom",
    "minHeightScale": 1,
    "padding": "2px",
  }),
  // A math expression with no children that is non-editable. 
  // Includes most math operators such as "+". 
  atomImmutable: new MathNodeType({
    "type": "atomImmutable",
    "minHeightScale": 1,
    "padding": "2px",
  }),
  verticalMath: new MathNodeType({
    "type": "verticalMath",
    // "display": "flex",
    // "flexDirection": "column",
    // "alignContent": "center",
    // "alignItems": "center",
    // "justifyContent": "center",
  }),
  // Horizontally laid out math such as "x+2".
  // ["x", "+" 2].
  // Not allowed to contain other horizontally laid out math elements.
  horizontalMath: new MathNodeType({
    "type": "horizontalMath",
    // "display": "flex",
    // "flexDirection": "row",
    //    "justifyContent": "center",
    //    "alignContent": "center",
    //    "alignItems": "center",
  }),
  // Represents expressions such as "x/y" or "\frac{x}{y}".
  fraction: new MathNodeType({
    "type": "fraction",
    // "display": "flex",
    // "flexDirection": "column",
    // "justifyContent": "center",
    // "alignContent": "center",
    // "alignItems": "center",
    "verticalAlign": "-1em",
  }),
  // Represents the numerator x of a fraction x/y.
  numerator: new MathNodeType({
    "type": "numerator",
    "display": "block",
    "borderBottom": "1px solid black",
    "scale": 0.6,
    "arrows": {
      "ArrowUp": [arrowMotion.firstAtomToTheLeft],
      "ArrowDown": [arrowMotion.firstAtomToTheRight],
    },
  }),
  // Represents the denominator y of a fraction x/y.
  denominator: new MathNodeType({
    "type": "denominator",
    "display": "block",
    "scale": 0.6,
    "arrows": {
      "ArrowUp": [arrowMotion.firstAtomToTheLeft],
      "ArrowDown": [arrowMotion.firstAtomToTheRight],
    },
  }),
};

class MathNodeFactory {
  horizontalMath(
    /** @type {MathNode|null} */
    content
  ) {
    const result = new MathNode(knownTypes.horizontalMath);
    // const element = this.verticalMath(content);
    // result.appendChild(element);
    if (content === null) {
      content = this.atom();
    }
    result.appendChild(content);
    return result;
  }
  verticalMath(
    /** @type {MathNode|null} */
    content
  ) {
    const result = new MathNode(knownTypes.verticalMath);
    if (content === null) {
      content = this.atom();
    }
    result.appendChild(content);
    return result;
  }
  rootMath() {
    const result = new MathNode(knownTypes.root);
    result.appendChild(this.horizontalMath(null));
    return result;
  }
  verticalMathWithAtomImmutable(key) {
    return this.verticalMath(this.atomImmutable(key));
  }

  atomImmutable(
    /** @type {string} */
    content
  ) {
    const result = new MathNode(knownTypes.atomImmutable);
    result.operator = content;
    return result;
  }
  atom() {
    return new MathNode(knownTypes.atom);
  }
  fractionEmptyDenominator(/** @type{MathNode}*/ numeratorContent) {
    const fraction = new MathNode(knownTypes.fraction);
    const numerator = new MathNode(knownTypes.numerator);
    const denominator = new MathNode(knownTypes.denominator);
    numerator.appendChild(this.horizontalMath(numeratorContent));
    denominator.appendChild(this.horizontalMath(null));
    fraction.appendChild(numerator);
    fraction.appendChild(denominator);
    return fraction;
  }
}

var mathNodeFactory = new MathNodeFactory();
/** @type {EquationEditor} */
var lastCreatedEquationEditor = null;

class EquationEditor {
  constructor(
    /** @type{HTMLElement} */
    container,
  ) {
    this.container = container;
    this.container.innerHTML = "";
    this.container.style.display = "inline-block";
    this.rootNode = mathNodeFactory.rootMath();
    this.rootNode.externalDOM = this.container;
    this.rootNode.updateDOMRecursive(0);
    lastCreatedEquationEditor = this;
  }
  toString() {
    return this.rootNode.toString(0);
  }
}

class MathNode {
  constructor(
    /** @type {MathNodeType} */
    type
  ) {
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
    /** @type {HTMLElement} */
    this.externalDOM = null;
    /** @type {number} */
    this.positionCaretBeforeKeyEvents = - 1;
    /** @type {string} */
    this.operator = "";
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
    this.element.style.minHeight = this.type.minHeightScale * fontSize;
    this.element.style.minWidth = this.type.minHeightScale * fontSize / 1.6;
    this.element.style.verticalAlign = this.type.verticalAlign;
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
    if (this.type.scale !== 1) {
      this.element.style.transform = `scale(${this.type.scale},${this.type.scale})`;
    }
    this.element.innerHTML = "";
    if (this.operator !== "") {
      this.element.textContent = this.operator;
    }
    this.element.setAttribute("mathTagName", this.type.type);
    this.element.addEventListener("keyup", (e) => { this.handleKeyUp(e); });
    this.element.addEventListener("keydown", (e) => { this.handleKeyDown(e); });
    this.element.addEventListener("focus", (e) => { this.handleFocus(e); });
    this.element.addEventListener("blur", (e) => { this.handleBlur(e); });
  }

  updateDOM() {
    this.updateDOMRecursive(0);
  }

  updateDOMRecursive(/** @type {number} */ depth) {
    if (
      this.type.type === knownTypes.atom.type
    ) {
      this.createDOMElementIfMissing();
      return;
    }
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].updateDOMRecursive(depth + 1);
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
    this.element.style.background = "lightblue";
  }

  handleBlur(e) {
    this.element.style.background = "";
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
    const childToFocus = this.getChildToFocusFromKey(key);
    if (childToFocus !== null) {
      childToFocus.focus(null);
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

  getChildToFocusFromKey(
    /** @type {string} */
    key,
  ) {
    /** @type {string[]} */
    const arrowArray = this.type.arrows[key];
    let childToFocus = this;
    for (let i = 0; i < arrowArray.length; i++) {
      childToFocus = this.getChildToFocusFromAction(key, childToFocus, arrowArray[i]);
    }
    return childToFocus;
  }

  getChildToFocusFromAction(
    /** @type {string} */ key,
    /** @type {MathNode}*/ childToFocus,
    /** @type {string} */ arrowType,
  ) {
    if (arrowType === arrowMotion.parentForward) {
      if (childToFocus.parent === null) {
        return childToFocus;
      }
      return childToFocus.parent.getChildToFocusFromKey(key);
    }
    if (arrowType === arrowMotion.firstAtomToTheLeft) {
      return childToFocus.firstAtomToTheLeft();
    }
    if (arrowType === arrowMotion.firstAtomToTheRight) {
      return childToFocus.firstAtomToTheRight();
    }
    return null;
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
    if (child.type.type === knownTypes.horizontalMath.type &&
      this.type.type === knownTypes.horizontalMath.type) {
      needsNormalization = true;
    }
    if (child.type.type === knownTypes.verticalMath.type && this.type.type == knownTypes.verticalMath.type) {
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
    if (this.positionCaretBeforeKeyEvents !== 0 || this.type.type !== knownTypes.atom.type) {
      return;
    }
  }

  makeHorizontalOperator(
    /** @type {string} */
    key,
  ) {
    let parent = this.parent;
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
    parent.insertChildAtPosition(this.indexInParent + 1, mathNodeFactory.verticalMathWithAtomImmutable(key));
    parent.insertChildAtPosition(this.indexInParent + 2, mathNodeFactory.verticalMath(null));
    parent.updateDOM();
    parent.children[this.indexInParent + 2].focus(null);
  }

  makeFractionNumerator() {
    if (this.parent === null) {
      return;
    }
    const oldParent = this.parent;
    const oldIndexInParent = this.indexInParent;
    const fraction = mathNodeFactory.fractionEmptyDenominator(this);
    oldParent.replaceChildAtPosition(oldIndexInParent, fraction);
    fraction.parent.updateDOMRecursive(0);
    fraction.children[1].focus(null);
  }

  focus(desiredPositionUnused) {
    if (this.type.type === knownTypes.atom.type) {
      if (this.element !== null) {
        this.element.focus(desiredPositionUnused);
      }
      return;
    }
    if (this.children.length < 1) {
      return;
    }
    this.children[0].focus(desiredPositionUnused);
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

function initialize() {
  new EquationEditor(document.getElementById('equation-editor'));
  MathQuill.getInterface(2).MathField(document.getElementById('mq-editor'));
}

module.exports = {
  EquationEditor,
};