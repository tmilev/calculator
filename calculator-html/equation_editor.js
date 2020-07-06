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
      if (!label in input["arrows"]) {
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
  }
}

const knownTypeDefaults = {
  "display": "inline-block",
  "minHeightScale": 0,
  "borderBottom": "",
  "borderStyle": "",
  "scale": 1,
};

class ArrowMotionTypes {
  constructor() {
    this.parentForward = "parentForward";
    this.parent = "parent";
    this.leftmostChild = "leftmostChild";
    this.leftSibling = "leftSibling";
    this.rightSibling = "rightSibling";
  }
}

let arrowMotion = new ArrowMotionTypes();

const knownTypeDefaultsArrows = {
  "arrowUp": [arrowMotion.parent],
  "arrowDown": [arrowMotion.leftmostChild],
  "arrowLeft": [arrowMotion.leftSibling],
  "arrowRight": [arrowMotion.rightSibling],
};

const knownTypes = {
  root: new MathNodeType({
    "type": "root",
    "borderStyle": "1px solid black",
    "padding": "2px",
  }),
  // A math expression with no children such as "x", "2", "+".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    "type": "atom",
    "minHeightScale": 1,
  }),
  // Horizontally laid out math such as "x+2".
  // ["x", "+" 2].
  // Not allowed to contain other horizontally laid out math elements.
  horizontalMath: new MathNodeType({
    "type": "horizontalMath",
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
    "scale": 0.93,
  }),
  // Represents the denominator y of a fraction x/y.
  denominator: new MathNodeType({
    "type": "denominator",
    "display": "block",
    "scale": 0.93,
    "arrows": {
      "arrowUp": [arrowMotion.parent, arrowMotion]
    },
  }),
};

class MathNodeFactory {
  horizontalMath(
    /** @type {MathNode|null} */
    content
  ) {
    if (content === null) {
      content = this.atom();
    }
    const result = new MathNode(knownTypes.horizontalMath);
    result.appendChild(content);
    return result;
  }
  rootMath() {
    const result = new MathNode(knownTypes.root);
    result.appendChild(this.horizontalMath(null));
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
    console.log("DEBUG: Hello world!!!");
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
    /** @type{Array<MathNode>} */
    this.children = [];
    /** @type {MathNodeType} */
    this.type = type;
    /** @type{HTMLElement} */
    this.element = null;
    /** @type {MathNode} */
    this.parent = null;
    /** @type {number} */
    this.indexInParent = - 1;
    /** @type{HTMLElement} */
    this.externalDOM = null;
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
    this.element.style.width = this.type.width;
    this.element.style.height = this.type.height;
    this.element.style.display = this.type.display;
    this.element.style.minHeight = this.type.minHeightScale * fontSize;
    this.element.style.minWidth = this.type.minHeightScale * fontSize / 1.6;
    if (this.type.scale !== 1) {
      this.element.style.transform = `scale(${this.type.scale},${this.type.scale})`;
    }
    this.element.innerHTML = "";
    this.element.setAttribute("mathTagName", this.type.type);
    this.element.addEventListener("keyup", (e) => { this.handleKeyUp(e); });
    this.element.addEventListener("keydown", (e) => { this.handleKeyDown(e); });
    this.element.addEventListener("focus", (e) => { this.handleFocus(e); });
    this.element.addEventListener("blur", (e) => { this.handleBlur(e); });
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
    if (parentElement.contains(this.element)) {
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
        this.makeFractionNumerator(event);
        return;
      case "ArrowUp":
      case "ArrowDown":
      case "ArrowLeft":
      case "ArrowRight":
        this.arrow(event, key);
        return;
      case "Backspace":
        this.updateBackspace(event);
        return;
      default:
        return;
    }
  }

  arrow(
    /** @type {string} */
    key,
  ) {
    /** @type {string[]} */
    const arrowArray = this.type.arrows[key];
    let childToFocus = this;
    for (let i = 0; i < arrowArray.length; i++) {
      childToFocus = this.processOneArrow(childToFocus, arrowArray[i]);
    }
    childToFocus.focus();
  }

  processOneArrow(
    /** @type {string}*/ key,
    /** @type {MathNode}*/ childToFocus,
    /** @type {string} */ type,
  ) {
    if (type === arrowMotion.parent) {
      if (childToFocus.parent === null) {
        return childToFocus;
      }
      return childToFocus.parent;
    }
    if (type === arrowMotion.parentForward) {
      if (childToFocus.parent === null) {
        return childToFocus;
      }
      return childToFocus.parent(key);
    }
    if (type === arrowMotion.rightSibling) {
      return childToFocus.toTheRightOrSelf();
    }
  }

  // Returns first MathNode atom that lies to the right of 
  // the present element or null if there is no such element. 
  atomToTheRight() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.to
  }

  toTheRightOf(/** @type{number}*/ childIndex) {
    childIndex++;
    if (childIndex < this.children.length) {
      return this.children[childIndex];
    }
  }

  handleKeyDown(
    /** @type {KeyboardEvent} */
    event,
  ) {
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
      case "/":
        event.preventDefault();
        return;
    }
  }

  appendChild(/** @type{MathNode} */ child) {
    if (
      child.type.type === knownTypes.horizontalMath.type &&
      this.type.type === knownTypes.horizontalMath.type
    ) {
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
    this.replaceChildAtPositionWithChildren(index, [child]);
  }

  replaceChildAtPositionWithChildren(
    /** @type {number} */
    index,
    /** @type {Array<MathNode>} */
    inputChildren
  ) {
    // Please do not modify removed as removed can
    // be in use as a member of a sub-tree of the inputChildren.
    const removed = this.children[index];
    let toBeShiftedDown = [];
    for (let i = index + 1; i < this.children.length; i++) {
      toBeShiftedDown.push(this.children[i]);
    }
    this.children.length = index;
    for (let i = 0; i < inputChildren.length; i++) {
      this.appendChild(inputChildren[i]);
    }
    for (let i = 0; i < toBeShiftedDown.length; i++) {
      this.appendChild(toBeShiftedDown[i]);
    }
    return removed;
  }

  updateBackspace(/** @type {KeyboardEvent} */ event) {
    if (this.parent === null) {
      return;
    }
    if (this.parent.type.type === knownTypes.denominator.type || this.parent.type.type === knownTypes.numerator.type) {
      let offset = window.getSelection().getRangeAt(0).startOffset;
      if (offset === 0) {
        this.parent.parent.dissoveFraction(event);
      }
    }
  }

  dissoveFraction() {
    let numeratorContent = this.children[0].children[0];
    let denominatorContent = this.children[1].children[0];
    this.parent.replaceChildAtPositionWithChildren(this.indexInParent, [numeratorContent, denominatorContent]);
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
    fraction.children[1].focus();
  }

  focus() {
    if (this.type.type === knownTypes.atom.type) {
      if (this.element !== null) {
        this.element.focus();
      }
      return;
    }
    if (this.children.length < 1) {
      return;
    }
    this.children[0].focus();
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

module.exports = {
  EquationEditor,
};