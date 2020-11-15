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
    this.minWidth = input["minWidth"];
    // Padding.
    this.paddingBottom = input["paddingBottom"];
    this.paddingTop = input["paddingTop"];
    this.paddingRight = input["paddingRight"];
    this.paddingLeft = input["paddingLeft"];
    this.padding = input["padding"];
    // Margins.
    this.margin = input["margin"];
    this.marginBottom = input["marginBottom"];
    this.marginRight = input["marginRight"];
    this.marginLeft = input["marginLeft"];
    // Borders
    this.borderStyle = input["borderStyle"];
    this.borderColor = input["borderColor"];
    this.borderBottom = input["borderBottom"];
    this.borderTop = input["borderTop"];
  }

  /**@return {MathNodeType} */
  clone() {
    return Object.assign(new MathNodeType({}), this);
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
  "boxSizing": "",
  "position": "absolute",
  // Colors
  "colorText": "",
  "colorImplied": "",
  // Padding
  "paddingBottom": "",
  "paddingTop": "",
  "paddingLeft": "",
  "paddingRight": "",
  "padding": "",
  // Margins
  "marginBottom": "",
  "marginLeft": "",
  "marginRight": "",
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

const defaultFractionScale = 0.9;
const atomMargin = `${0.02}em`;

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
    "margin": atomMargin,
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
  error: new MathNodeType({
    type: "error",
    colorText: "red",
    whiteSpace: "nowrap",
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
    "borderBottom": "1px solid black",
    "padding": "1px",
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
  matrix: new MathNodeType({
    "type": "matrix",
  }),
  matrixTable: new MathNodeType({
    "type": "matrixTable",
  }),
  matrixRow: new MathNodeType({
    "type": "matrixRow",
  }),
  matrixRowEntry: new MathNodeType({
    "type": "matrixRowEntry",
  }),
};

class MathNodeFactory {
  horizontalMath(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    const result = new MathNode(equationEditor, knownTypes.horizontalMath);
    if (content === null) {
      content = this.atom(equationEditor, "");
    }
    result.appendChild(content);
    return result;
  }

  /** @returns{MathNode} 
   * Returns a horizontal math with content given by the input array.
   * Normalizes the input but does not ensure editable atoms at the ends. 
   */
  horizontalMathFromArray(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null[]} */
    content,
  ) {
    let first = null;
    if (content.length > 0) {
      first = content[0];
    }
    let result = this.horizontalMath(equationEditor, first);
    for (let i = 1; i < content.length; i++) {
      if (content[i] === null) {
        continue;
      }
      result.appendChild(content[i]);
    }
    result.normalizeHorizontalMath();
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

  error(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    initialContent,
  ) {
    const result = new MathNode(equationEditor, knownTypes.error);
    result.initialContent = initialContent;
    return result;
  }

  atom(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    initialContent,
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
    /** @type{MathNode|null}*/
    exponent,
  ) {
    // Horizontal math wrapper for the exponent.
    const exponentWrapped = new MathNode(equationEditor, knownTypes.exponent);
    let exponentContainer = this.horizontalMath(equationEditor, exponent);
    exponentContainer.normalizeHorizontalMath();
    exponentContainer.ensureEditableAtoms();
    exponentWrapped.appendChild(exponentContainer);
    // Horizontal math wrapper for the base.
    const baseHorizontal = new MathNode(equationEditor, knownTypes.horizontalMath);
    baseHorizontal.appendChild(this.horizontalMath(equationEditor, base));
    baseHorizontal.normalizeHorizontalMath();
    // The base with the exponent.
    const baseWithExponent = new MathNode(equationEditor, knownTypes.baseWithExponent);
    baseWithExponent.appendChild(baseHorizontal);
    baseWithExponent.appendChild(exponentWrapped);
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

  /** @returns {MathNode} */
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

  /** @returns {MathNode} */
  matrix(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {number} */
    rows,
    /** @type {number} */
    columns,
  ) {
    const matrixTable = new MathNode(equationEditor, knownTypes.matrixTable);
    for (let i = 0; i < rows; i++) {
      matrixTable.appendChild(this.matrixRow(equationEditor, columns));
    }
    const parenthesesLayout = this.horizontalMath(equationEditor, this.leftParenthesis(equationEditor, false));
    parenthesesLayout.appendChild(matrixTable);
    parenthesesLayout.appendChild(this.rightParenthesis(equationEditor, false));
    const result = new MathNode(equationEditor, knownTypes.matrix);
    result.appendChild(parenthesesLayout);
    return result;
  }

  /** @returns {MathNode} */
  matrixRow(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {number} */
    columns,
  ) {
    let result = new MathNode(equationEditor, knownTypes.matrixRow);
    for (let i = 0; i < columns; i++) {
      result.appendChild(this.matrixRowEntry(equationEditor, null));
    }
    return result;
  }

  matrixRowEntry(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    let result = new MathNode(equationEditor, knownTypes.matrixRowEntry);
    result.appendChild(this.horizontalMath(equationEditor, content));
    return result;
  }
}

var mathNodeFactory = new MathNodeFactory();

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
  leftHorizontalNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, -1);
    }
    if (this.position > 0 && this.element.hasHorizintalMathParent()) {
      return new AtomWithPosition(this.element, this.position - 1);
    }
    let next = this.element.firstAtomUncle(-1);
    if (next === null) {
      return new AtomWithPosition(null, -1);
    }
    return new AtomWithPosition(next, next.lengthContentIfAtom());
  }

  /** @returns {AtomWithPosition} */
  rightHorizontalNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, -1);
    }
    if (this.position < this.element.lengthContentIfAtom() && this.element.hasHorizintalMathParent()) {
      return new AtomWithPosition(this.element, this.position + 1);
    }
    let next = this.element.firstAtomUncle(1);
    if (next === null) {
      return new AtomWithPosition(null, -1);
    }
    return new AtomWithPosition(next, 0);
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

/** @returns {EquationEditor} 
 * Converts the textContent of an html element to typeset math.
 */
function mathFromElement(
  /**@type{HTMLElement} */
  container,
  /**@type{boolean} */
  editable,
) {
  return mathFromLatex(container, container.textContent, editable);
}

/** @returns {EquationEditor} Returns typeset math.*/
function mathFromLatex(
  /**@type{HTMLElement} */
  container,
  /**@type{string} */
  latex,
  /**@type{boolean} */
  editable,
) {
  let result = new EquationEditor(container, new EquationEditorOptions(editable));
  result.writeLatex(latex);
  return result;
}

class SyntancticElement {
  constructor(
    /**@type{MathNode|null} */
    node,
    /**@type{string} */
    content,
    /**@type{string} */
    syntacticRole,
  ) {
    /**@type{MathNode|null} contains parsed math content. */
    this.node = node;
    /**@type{string} contains non-parsed atomic content. */
    this.content = content;
    /**@type{string} */
    this.syntacticRole = syntacticRole;
  }
  toString() {
    let result = "";
    if (this.node !== null) {
      let next = this.node.toLatex();
      if (next === "") {
        next = `[${this.node.type.type}]`;
      }
      result += next;
    }
    if (this.content !== "") {
      result += `<b style='color:orange'>${this.content}</b>`;
    }
    if (this.syntacticRole !== "") {
      result += `<b style='color:red'>${this.syntacticRole}</b>`;
    }
    return result;
  }
  isExpression() {
    return this.syntacticRole === "" && this.node !== null;
  }
}

class LaTeXConstants {
  constructor() {
    this.latinCharactersString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    this.latinCharacters = {};
    for (let i = 0; i < this.latinCharactersString.length; i++) {
      this.latinCharacters[this.latinCharactersString[i]] = true;
    }
    /**@type{Object.<string, string>} */
    this.operatorsNormalized = {
      // Full-widgth plus sign, wider and taller plus sign.
      "\uFF0B": "+",
      "+": "+",
      // Mathematical minus, a wider dash.
      "\u2212": "\u2212",
      "-": "\u2212",
      "*": "\u00B7",
      // A vertically centered small solid dot.
      "\u00B7": "\u00B7",
      "=": "=",
    };
    /**@type{Object.<string, string>} */
    this.latexSyntacticValues = {
      "{": "{",
      "}": "}",
      "^": "^",
      "(": "(",
      ")": ")",
      "\\": "\\",
      "&": "&",
    };
    /**@type{Object.<string, string>} */
    this.latexBackslashCommands = {
      "sqrt": "\\sqrt",
      "begin": "\\begin",
      "end": "\\end",
      "frac": "\\frac",
    };
    this.beginEndEnvironments = {
      "pmatrix": "pmatrix",
    };
    this.whiteSpaceCharacters = {
      " ": true,
      "\t": true,
      "\u00A0": true,
      "\n": true,
    };
  }
  isLatinCharacter(
    /**@type{string}*/
    input,
  ) {
    return input in this.latinCharacters;
  }
  isWhiteSpace(
    /**@type{string} */ input,
  ) {
    return input in this.whiteSpaceCharacters;
  }
  normalizeOperatorToUtf8(
    /**@type{string} */
    input,
  ) {
    if (input in this.operatorsNormalized) {
      return this.operatorsNormalized[input];
    }
    return input;
  }

}
const latexConstants = new LaTeXConstants();

class LaTeXParser {
  constructor(
    /**@type{EquationEditor} */
    equationEditor,
    /**@type{string} */
    latex,
  ) {
    /**@type{EquationEditor} */
    this.equationEditor = equationEditor;
    /**@type{string} */
    this.latex = latex;
    /**@type {MathNode} */
    this.result = new MathNode(equationEditor, knownTypes.horizontalMath);
    /**@type{string[]} */
    this.words = [];
    /**@type{SyntancticElement[]} */
    this.parsingStack = [];
    /**@type {number} 
     * Dummy elements are prepended to the parsing stack  
     * so parsing rules can safely assume a minimal number 
     * of syntanctic elements. 
     */
    this.dummyParsingElements = 6;
    /**@type{boolean} */
    this.debug = true;
    /**@type{string[]} */
    this.reductionLog = [];
    /**@type{string} */
    this.lastRuleName = "";
    /**@type{number} */
    this.startTime = 0;
  }

  intialize() {
    this.parsingStack = [];
    for (let i = 0; i < this.dummyParsingElements; i++) {
      this.parsingStack.push(new SyntancticElement(null, "", ""));
    }
  }

  parseWords() {
    let next = [];
    for (let i = 0; i < this.latex.length; i++) {
      let nextCharacter = this.latex[i];
      if (latexConstants.isLatinCharacter(nextCharacter)) {
        next.push(nextCharacter);
        continue;
      }
      if (next.length > 0) {
        this.words.push(next.join(""));
      }
      this.words.push(nextCharacter);
      next = [];
    }
    if (next.length > 0) {
      this.words.push(next.join(""));
    }
  }

  /**@returns{MathNode} */
  parse() {
    this.startTime = new Date().getTime();
    this.intialize();
    this.parseWords();
    this.result = new MathNode(this.equationEditor, knownTypes.horizontalMath);
    let startingNode = mathNodeFactory.horizontalMath(this.equationEditor, null);
    this.parsingStack.push(new SyntancticElement(startingNode, "", ""));
    for (let i = 0; i < this.words.length; i++) {
      if (!this.reduceStack(new SyntancticElement(null, this.words[i], ""))) {
        return this.setError();
      }
    }
    if (!this.reduceStack(new SyntancticElement(null, "", "parsingEnd"))) {
      return this.setError();
    }
    if (this.parsingStack.length !== this.dummyParsingElements + 2) {
      console.log(`Failed to parse ${this.latex}: not all syntactic elements were reduced.`);
      return this.setError();
    }
    let secondToLastElement = this.parsingStack[this.parsingStack.length - 2];
    if (secondToLastElement.node === null) {
      console.log(`Failed to parse ${this.latex}: final syntactic element is not a node.`);
      return this.setError();
    }
    if (this.equationEditor.options.editable) {
      secondToLastElement.node.ensureEditableAtomsRecursive();
    }
    let elapsedTime = new Date().getTime() - this.startTime;
    console.log(`Parsing of ${this.latex} took ${elapsedTime} ms.`);
    return secondToLastElement.node;
  }

  /**@returns{MathNode} */
  setError() {
    return mathNodeFactory.error(this.equationEditor, this.latex);
  }

  /** @returns{boolean} */
  reduceStack(
    /**@type {SyntancticElement} */
    syntacticElement,
  ) {
    let numberOfRuleApplications = 0;
    let startingLength = this.parsingStack.length;
    this.parsingStack.push(syntacticElement);
    if (this.debug) {
      this.reductionLog.push(this.toStringSyntacticStack() + "&nbsp;[append]");
      this.lastRuleName = "";
    }
    while (this.applyOneRule()) {
      if (this.debug) {
        this.reductionLog.push(this.toStringSyntacticStack() + `&nbsp;[${this.lastRuleName}]`);
        this.lastRuleName = "";
      }
      numberOfRuleApplications++;
      let stackReduction = startingLength - this.parsingStack.length;
      let maximumRuleApplications = Math.max(10, stackReduction * 10);
      if (numberOfRuleApplications > maximumRuleApplications) {
        // Too many rules to reduce an expression: no more than 10 rule applications per
        // unit of stack reduction.
        // Perhaps we have a substitution loop in our parsing rules?
        console.log("Too many parsing rule applications.");
        return false;
      }
    }
    return true;
  }

  /** @returns{boolean} */
  decreaseParsingStack(
    /**@type{number} */
    elementToRemove,
  ) {
    this.parsingStack.length = this.parsingStack.length - elementToRemove;
  }

  /** @returns{boolean} */
  replaceParsingStackTop(
    /**@type{MathNode|null} */
    node,
    /**@type{string} */
    syntancticValue,
    /**@type{number} */
    indexToRemove,
  ) {
    return this.replaceParsingStackRange(node, syntancticValue, indexToRemove, - 1);
  }

  /** @returns{boolean} */
  replaceParsingStackRange(
    /**@type{MathNode|null} */
    node,
    /**@type{string} */
    syntancticRole,
    /**@type{number} */
    firstIndexToRemove,
    /**@type{number} */
    lastIndexToRemove,
  ) {
    if (firstIndexToRemove < 0) {
      firstIndexToRemove += this.parsingStack.length;
    }
    if (lastIndexToRemove < 0) {
      lastIndexToRemove += this.parsingStack.length;
    }
    let incoming = new SyntancticElement(node, "", syntancticRole);
    this.parsingStack[firstIndexToRemove] = incoming;
    let nodesToRemove = lastIndexToRemove - firstIndexToRemove;
    for (let i = lastIndexToRemove + 1; i < this.parsingStack.length; i++) {
      this.parsingStack[i - nodesToRemove] = this.parsingStack[i];
    }
    this.parsingStack.length -= nodesToRemove;
    return true;
  }

  /**@returns{string} */
  toStringSyntacticStack() {
    let totalStack = [];
    for (let i = this.dummyParsingElements; i < this.parsingStack.length; i++) {
      totalStack.push(this.parsingStack[i].toString());
    }
    return totalStack.join("&nbsp;&nbsp;");
  }

  /**@returns{boolean} Applies the first possible rule to the top of the parsing stack. */
  applyOneRule() {
    let last = this.parsingStack[this.parsingStack.length - 1];
    if (latexConstants.isWhiteSpace(last.content)) {
      this.parsingStack.length = this.parsingStack.length - 1;
      return true;
    }
    if (last.content in latexConstants.latexSyntacticValues) {
      this.lastRuleName = "built-in syntactic element";
      return this.replaceParsingStackTop(null, latexConstants.latexSyntacticValues[last.content], - 1);
    }
    let secondToLast = this.parsingStack[this.parsingStack.length - 2];
    if (last.syntacticRole === "\\" && secondToLast.syntacticRole === "\\") {
      return this.replaceParsingStackTop(null, "\\\\", - 2);
    }
    let thirdToLast = this.parsingStack[this.parsingStack.length - 3];
    let fourthToLast = this.parsingStack[this.parsingStack.length - 4];
    if (secondToLast.syntacticRole === "{" && last.isExpression()) {
      if (last.node.type.type !== knownTypes.horizontalMath.type) {
        let node = mathNodeFactory.horizontalMath(this.equationEditor, last.node);
        return this.replaceParsingStackTop(node, "", - 1);
      }
    }
    if (thirdToLast.syntacticRole === "{" && secondToLast.isExpression() && last.syntacticRole === "}") {
      let node = secondToLast.node;
      if (node.type.type !== knownTypes.horizontalMath.type) {
        node = mathNodeFactory.horizontalMath(this.equationEditor, node);
      }
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.latexBackslashCommands) {
      return this.replaceParsingStackTop(null, latexConstants.latexBackslashCommands[last.content], - 2);
    }
    if (
      (thirdToLast.syntacticRole === "\\begin" || thirdToLast.syntacticRole === "\\end") &&
      secondToLast.syntacticRole === "{" &&
      last.content in latexConstants.beginEndEnvironments
    ) {
      this.lastRuleName = "begin or end environment";
      return this.replaceParsingStackTop(null, latexConstants.beginEndEnvironments[last.content], - 1);
    }
    if (
      fourthToLast.syntacticRole === "\\begin" &&
      thirdToLast.syntacticRole === "{" &&
      secondToLast.syntacticRole in latexConstants.beginEndEnvironments &&
      last.syntacticRole === "}"
    ) {
      let environment = latexConstants.beginEndEnvironments[secondToLast.syntacticRole];
      return this.replaceParsingStackTop(null, `\\begin{${environment}}`, - 4);
    }
    if (
      fourthToLast.syntacticRole === "\\end" &&
      thirdToLast.syntacticRole === "{" &&
      secondToLast.syntacticRole in latexConstants.beginEndEnvironments &&
      last.syntacticRole === "}"
    ) {
      let environment = latexConstants.beginEndEnvironments[secondToLast.syntacticRole];
      return this.replaceParsingStackTop(null, `\\end{${environment}}`, - 4);
    }
    if (last.syntacticRole === "\\begin{pmatrix}") {
      let matrix = mathNodeFactory.matrix(this.equationEditor, 1, 0);
      return this.replaceParsingStackTop(matrix, "matrixBuilder", - 1);
    }
    if (last.syntacticRole === "&" && secondToLast.isExpression() && thirdToLast.syntacticRole === "matrixBuilder") {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic comoplexity.
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      thirdToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(2);
    }
    if (last.syntacticRole === "\\\\" && secondToLast.isExpression() && thirdToLast.syntacticRole === "matrixBuilder") {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic comoplexity.
      let lastRow = thirdToLast.node.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
      return this.decreaseParsingStack(2);
    }
    if (thirdToLast.syntacticRole === "matrixBuilder" && secondToLast.isExpression() && last.syntacticRole === "\\end{pmatrix}") {
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      thirdToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      // Normalize the matrix: ensure all rows have same number of columns, no last empty row, etc.
      thirdToLast.node.normalizeMatrix();
      // Mark the matrix as a regular expression.
      thirdToLast.syntacticRole = "";
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === "matrixBuilder" && last.syntacticRole === "\\end{pmatrix}") {
      // Normalize the matrix: ensure all rows have same number of columns, no last empty row, etc.
      secondToLast.node.normalizeMatrix();
      // Mark the matrix as a regular expression.
      secondToLast.syntacticRole = "";
      return this.decreaseParsingStack(1);
    }
    if (last.content === "left" && secondToLast.syntacticRole === "\\") {
      return this.decreaseParsingStack(2);
    }
    if (last.content === "right" && secondToLast.syntacticRole === "\\") {
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === "\\sqrt" && last.syntacticRole === "") {
      let node = mathNodeFactory.sqrt(this.equationEditor, last.node);
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (secondToLast.syntacticRole === "{" && last.syntacticRole === "}") {
      this.lastRuleName = "{} to empty atom";
      let node = mathNodeFactory.atom(this.equationEditor, "");
      return this.replaceParsingStackTop(node, "", -2);
    }
    if (thirdToLast.syntacticRole === "(" && secondToLast.isExpression() && last.syntacticRole === ")") {
      this.lastRuleName = "parenthetic expression to expression";
      let leftParentheses = mathNodeFactory.leftParenthesis(this.equationEditor, false);
      let rightParentheses = mathNodeFactory.rightParenthesis(this.equationEditor, false);
      let horizontal = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [leftParentheses, secondToLast.node, rightParentheses]);
      return this.replaceParsingStackTop(horizontal, "", - 3);
    }
    if (thirdToLast.isExpression() && secondToLast.syntacticRole === "^" && last.isExpression()) {
      let node = mathNodeFactory.baseWithExponent(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (thirdToLast.syntacticRole === "\\frac" && secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.fraction(this.equationEditor, secondToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (last.content in latexConstants.operatorsNormalized) {
      this.lastRuleName = "atom immutable";
      let node = mathNodeFactory.atomImmutable(
        this.equationEditor,
        latexConstants.operatorsNormalized[last.content],
      );
      return this.replaceParsingStackTop(node, "", - 1);
    }
    if (last.content !== "" && last.syntacticRole === "" && last.node === null) {
      this.lastRuleName = "construct atom";
      let node = mathNodeFactory.atom(this.equationEditor, last.content);
      return this.replaceParsingStackTop(node, "", - 1);
    }
    if (thirdToLast.isExpression() && secondToLast.isExpression() && last.syntacticRole !== "^") {
      // Absorb atom / immutable atom to preceding horizontal math.
      if (thirdToLast.node.type.type === knownTypes.horizontalMath.type) {
        this.lastRuleName = "merge node into horizontal math";
        thirdToLast.node.appendChild(secondToLast.node);
        return this.replaceParsingStackRange(thirdToLast.node, "", - 3, - 2);
      }
      if (thirdToLast.node.isAtomic() && secondToLast.node.isAtomic()) {
        this.lastRuleName = "merge atomic nodes into horizontal math";
        let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [thirdToLast.node, secondToLast.node]);
        return this.replaceParsingStackRange(node, "", - 3, - 2);
      }
    }
    return false;
  }
}

class EquationEditorOptions {
  constructor(
    /** @type{boolean} */
    editable,
    /**@type{HTMLElement|null} */
    debugLogContainer,
  ) {
    /** @type{boolean} */
    this.editable = editable;
    /**@type{HTMLElement|null} */
    this.debugLogContainer = null;
    if (debugLogContainer instanceof HTMLElement) {
      this.debugLogContainer = debugLogContainer;
    }
    /** @type{boolean} */
    this.showLatexOnDoubleClick = !this.editable;
  }
}

class EquationEditor {
  constructor(
    /** @type{HTMLElement} */
    containerGiven,
    /** @type{EquationEditorOptions|null} */
    options,
  ) {
    this.containerGiven = containerGiven;
    this.containerGiven.innerHTML = "";
    /** @type{HTMLElement} */
    this.container = containerGiven;
    if (this.container.tagName !== "DIV") {
      this.container = document.createElement("DIV");
      this.containerGiven.appendChild(this.container);
    }
    this.container.style.display = "inline-block";
    this.container.style.position = "relative";
    /** @type{EquationEditorOptions} */
    this.options = new EquationEditorOptions(true);
    if (options !== null && options !== undefined) {
      this.options = options;
    }
    this.rootNode = mathNodeFactory.rootMath(this);
    if (!this.options.editable) {
      this.rootNode.type.borderStyle = "";
    }
    this.rootNode.externalDOM = this.container;
    /** @type{AtomWithPosition} */
    this.selectionStart = new AtomWithPosition(null, -1);
    /** @type{AtomWithPosition} */
    this.selectionStartExpanded = new AtomWithPosition(null, -1);
    /** @type{AtomWithPosition} */
    this.selectionEnd = new AtomWithPosition(null, -1);
    /** @type{string} */
    this.latexLastWritten = "";
  }

  updateDOM() {
    this.rootNode.updateDOM();
  }

  writeLatex(
    /**@type {string} */
    latex,
  ) {
    this.latexLastWritten = latex;
    this.rootNode.removeAllChildren();
    let parser = new LaTeXParser(this, latex);
    let newContent = parser.parse();
    if (this.options.debugLogContainer !== null) {
      this.options.debugLogContainer.innerHTML = parser.reductionLog.join("<hr>") + "<hr><hr>" + parser.toStringSyntacticStack();

    }
    if (newContent === null) {
      console.log(`Failed to construct node from your input ${latex}.`);
      return;
    }
    this.rootNode.appendChild(newContent);
    this.updateDOM();
    this.updateAlignment();
  }

  toHtml() {
    let result = `Latex: ${this.rootNode.toLatex()}`;
    result += `<br>Drawing: ${this.rootNode.toString()}`;
    result += `<br>Structure: ${this.rootNode.toHtml(0)}`;
    result += `<br>Selection: ${this.toStringSelection()}`;
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
      focused.element.textContent += key;
      focused.focus(1);
      return;
    }
    if (key === "ArrowLeft" && focused.positionCaretBeforeKeyEvents > 0) {
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

  /**@returns {string} */
  toStringSelection() {
    if (this.selectionStart.element === null) {
      return "";
    }
    return `Selection: from: ${this.selectionStart.toString()} to: ${this.selectionEnd.toString()}`;
  }

  /** @returns {SplitBySelectionResult|null} */
  splitOneAtomBySelection() {
    if (this.selectionStartExpanded.element === null) {
      return null;
    }
    if (
      this.selectionEnd.element !== this.selectionStartExpanded.element ||
      !this.selectionEnd.element.isAtomEditable()
    ) {
      return null;
    }
    let atom = this.selectionEnd.element;
    if (!atom.hasHorizintalMathParent()) {
      console.log("Atom has non-horizontal math parent.");
      return null;
    }
    let parent = atom.parent;
    let result = new SplitBySelectionResult(parent);
    for (let i = 0; i < atom.indexInParent; i++) {
      result.beforeSplit.push(parent.children[i]);
    }
    let left = null;
    let middle = null;
    let right = null;
    let leftPosition = this.selectionStartExpanded.position;
    let rightPosition = this.selectionEnd.position;
    if (rightPosition < leftPosition) {
      let temporary = rightPosition;
      rightPosition = leftPosition;
      leftPosition = temporary;
    }
    if (leftPosition < 0) {
      console.log("Unexpected negative left position.");
      return null;
    }
    let rightPositionShifted = rightPosition - leftPosition;
    if (rightPositionShifted < 0) {
      console.log("Unexpected negative shifted right position.");
      return null;
    }

    // Suppose our atom has content: "leftmiddleright". 
    // Suppose the middle is selected, i.e., the selection borders are as indicated: "left|middle|right".
    let leftSplit = atom.splitByPosition(leftPosition);
    if (leftSplit[0] !== null) {
      // The left section is non-empty.
      left = leftSplit[0];
    }
    if (leftSplit[1] === null) {
      // The middle section is empty - we have a zero-length selection.
      // We choose not to split.
      return null;
    }
    let rightSplit = leftSplit[1].splitByPosition(rightPositionShifted);
    middle = rightSplit[0];
    right = rightSplit[1];
    if (left !== null) {
      result.beforeSplit.push(left);
    }
    result.split.push(middle);
    if (right !== null) {
      result.afterSplit.push(right);
    }
    for (let i = atom.indexInParent + 1; i < parent.children.length; i++) {
      result.afterSplit.push(parent.children[i]);
    }
    return result;
  }

  /** @returns {SplitBySelectionResult|null} */
  splitAtomsBySelection() {
    if (this.selectionStartExpanded.element === null || this.selectionEnd.element === null) {
      return null;
    }
    let start = this.selectionStartExpanded;
    let end = this.selectionEnd;
    if (end.element.isToTheLeftOf(start.element)) {
      // Swap end and start
      let temporary = start;
      start = end;
      end = temporary;
    }
    if (
      this.selectionStartExpanded.element === this.selectionEnd.element &&
      this.selectionEnd.element.isAtomEditable()
    ) {
      return this.splitOneAtomBySelection();
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
      // Start and end are the same node, which is the ancestor.
      return null;
    }
    while (start.element.parent !== ancestor && start.element.parent !== null) {
      start.element = start.element.parent;
      start.position = - 1;
    }
    while (end.element.parent !== ancestor && end.element.parent !== null) {
      end.element = end.element.parent;
      end.position = 1;
    }
    if (end.position === - 1) {
      // The end is not atomic. Set its position to 1 so selections include it.
      end.position = 1;
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

  /** @returns{MathNode} */
  elementFromPath(path) {
    let current = this.rootNode;
    for (let i = 0; i < path.length; i++) {
      current = current.children[path[i]];
    }
    return current;
  }

  handleDoubleClick(
    /** @type{MouseEvent} */
    e,
  ) {
    if (!this.options.showLatexOnDoubleClick) {
      return;
    }
    if (this.container.children.length >= 2) {
      this.container.removeChild(this.container.children[1]);
      return;
    }
    let staticContainer = document.createElement("SPAN");
    let latex = "";
    if (!this.options.editable && this.latexLastWritten !== "") {
      latex = this.latexLastWritten;
    } else {
      latex = this.rootNode.toLatex();
    }
    staticContainer.textContent = latex;
    staticContainer.style.position = "absolute";
    staticContainer.style.left = this.container.children[0].style.width;
    staticContainer.style.whiteSpace = "nowrap";
    this.container.appendChild(staticContainer);
    let range = window.getSelection().getRangeAt(0);
    let rangeClone = range.cloneRange();
    rangeClone.selectNode(staticContainer);
    window.getSelection().removeAllRanges();
    window.getSelection().addRange(rangeClone);
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
    this.type = type.clone();
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
    if (this.type.type === knownTypes.atom.type && this.equationEditor.options.editable) {
      this.element.contentEditable = true;
    }
    // Padding.
    if (this.type.padding !== "") {
      this.element.style.padding = this.type.padding;
    }
    if (this.type.paddingBottom !== "") {
      this.element.style.paddingBottom = this.type.paddingBottom;
    }
    if (this.type.paddingTop !== "") {
      this.element.style.paddingTop = this.type.paddingTop;
    }
    if (this.type.paddingLeft !== "") {
      this.element.style.paddingLeft = this.type.paddingLeft;
    }
    if (this.type.paddingRight !== "") {
      this.element.style.paddingRight = this.type.paddingRight;
    }
    // Margins
    if (this.type.margin !== "") {
      this.element.style.margin = this.type.margin;
    }
    if (this.type.marginBottom !== "") {
      this.element.style.marginBottom = this.type.marginBottom;
    }
    if (this.type.marginLeft !== "") {
      this.element.style.marginLeft = this.type.marginLeft;
    }
    if (this.type.marginRight !== "") {
      this.element.style.marginRight = this.type.marginRight;
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
      this.element.style.minHeight = this.type.minHeightScale * fontSize / 1.6;
    }
    if (this.type.minWidth !== 0) {
      this.element.style.minWidth = this.type.minWidth;
    }
    this.element.style.verticalAlign = this.type.verticalAlign;
    this.element.style.outline = this.type.outline;
    // this.element.style.overflow = "hidden";
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
    if (this.type.type === knownTypes.atom.type) {
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
    if (this.type.type === knownTypes.root.type) {
      this.element.addEventListener("dblclick", (e) => {
        this.equationEditor.handleDoubleClick(e);
      });
    }
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

  /** @returns {number} 
   * Returns the length of the content of an atom.
   * Returns -1 if the element is not an atom. */
  lengthContentIfAtom() {
    if (!this.isAtomEditable()) {
      return - 1;
    }
    if (this.element === null) {
      return this.initialContent.length;
    }
    return this.element.textContent.length;
  }

  contentIfAtom() {
    if (!this.isAtomEditable()) {
      return "";
    }
    return this.textContentOrInitialContent();
  }

  textContentOrInitialContent() {
    if (this.element === null) {
      return this.initialContent;
    }
    return this.element.textContent;
  }

  contentIfAtomic() {
    if (!this.isAtomic()) {
      return "";
    }
    return this.textContentOrInitialContent();
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

  /**@returns {boolean} */
  hasHorizintalMathParent() {
    if (this.parent === null) {
      return false;

    }
    return this.parent.type.type === knownTypes.horizontalMath.type;
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
    let boundingRecangleDOM = this.element.getBoundingClientRect();
    this.boundingBox.width = boundingRecangleDOM.width;
    this.boundingBox.height = boundingRecangleDOM.height;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  computeDimensionsFraction() {
    let numerator = this.children[0];
    let denominator = this.children[1];
    this.boundingBox.fractionLineHeight = numerator.boundingBox.height + 2;
    this.boundingBox.height = numerator.boundingBox.height + denominator.boundingBox.height;
    this.boundingBox.width = Math.max(numerator.boundingBox.width, denominator.boundingBox.width);
    numerator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.top = numerator.boundingBox.height + 2;
    numerator.computeBoundingBoxLeftSingleChild();
    denominator.computeBoundingBoxLeftSingleChild();
    this.boundingBox.width += 4;
    numerator.boundingBox.left += 2;
    denominator.boundingBox.left += 2;
  }

  computeDimensionsBaseWithExponent() {
    let base = this.children[0];
    let exponent = this.children[1];
    let overlapRatio = 0.35;
    if (base.children[0].type.type === knownTypes.matrix.type) {
      overlapRatio = 0.1;
    }
    let overlap = base.boundingBox.height * overlapRatio;
    this.boundingBox.height = exponent.boundingBox.height + base.boundingBox.height - overlap;
    if (exponent.boundingBox.height > this.boundingBox.height) {
      this.boundingBox.height = exponent.boundingBox.height;
    }
    base.boundingBox.top = exponent.boundingBox.height - overlap;
    if (base.boundingBox.top < 0) {
      // The exponent is so small relative to the base that it 
      // fits entirely in the overlap between the base and the exponent.
      exponent.boundingBox.top = - base.boundingBox.top;
      base.boundingBox.top = 0;
    }
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
    this.boundingBox.height = this.children[0].boundingBox.height;
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

  computeDimensionsMatrixTable() {
    this.boundingBox = new BoundingBox();
    let numberOfColumns = this.children[0].children.length;
    let numberOfRows = this.children.length;
    let left = 0;
    let betweenColumns = 10;
    for (let i = 0; i < numberOfColumns; i++) {
      let width = 0;
      for (let j = 0; j < numberOfRows; j++) {
        width = Math.max(width, this.children[j].children[i].boundingBox.width);
      }
      for (let j = 0; j < numberOfRows; j++) {
        let child = this.children[j].children[i];
        child.boundingBox.left = left + (width - child.boundingBox.width) / 2;
        child.computeBoundingBoxLeftSingleChild();
      }
      left += width + betweenColumns;
    }
    let rowWidth = left - betweenColumns;
    let top = 0;
    let betweenRows = 10;
    for (let i = 0; i < numberOfRows; i++) {
      let row = this.children[i];
      row.boundingBox = new BoundingBox();
      row.boundingBox.width = rowWidth;
      let height = 0;
      for (let j = 0; j < numberOfColumns; j++) {
        height = Math.max(height, row.children[j].boundingBox.height);
      }
      for (let j = 0; j < numberOfColumns; j++) {
        row.children[j].boundingBox.top = (height - row.children[j].boundingBox.height) / 2;
      }
      row.boundingBox.top = top;
      top += height + betweenRows;
    }
    this.boundingBox.height = top - betweenRows;
    this.boundingBox.width = rowWidth;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
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
    if (this.type.type === knownTypes.error.type) {
      this.computeDimensionsAtomic();
      return;
    }
    if (this.type.type === knownTypes.sqrt.type) {
      this.computeDimensionsSqrt();
      return;
    }
    if (this.type.type === knownTypes.matrixRow.type) {
      return;
    }
    if (this.type.type === knownTypes.matrixTable.type) {
      this.computeDimensionsMatrixTable();
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
    switch (key) {
      case "/":
        this.makeFractionNumerator();
        return true;
      case "*":
      case "+":
      case "-":
      case "=":
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
      case "m":
        this.makeMatrix(2, 2);
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

  /** @returns {number[]|null} */
  commonPathToRoot(
    /**@type{MathNode} */
    other,
  ) {
    if (this.equationEditor !== other.equationEditor) {
      return null;
    }
    let thisPath = this.getPathToRoot();
    let otherPath = other.getPathToRoot();
    let result = [];
    for (let i = 0; i < thisPath.length; i++) {
      if (i >= otherPath.length) {
        return result;
      }
      if (thisPath[i] !== otherPath[i]) {
        return result;
      }
      result.push(thisPath[i]);
    }
    return result;
  }

  /** @returns{MathNode|null} returns the common ancestor of two nodes. */
  commonAncestor(
    /**@type{MathNode} */
    other,
  ) {
    if (this === other || other === null) {
      return this;
    }
    if (this.equationEditor !== other.equationEditor) {
      return null;
    }
    let commonPath = this.commonPathToRoot(other);
    return this.equationEditor.elementFromPath(commonPath);
  }

  /** @returns{ParentWithIndex} 
   * returns the common ancestor of two nodes 
   * with the index of the ancestor's child 
   * that contains this node.  
   */
  commonParent(
    /**@type{MathNode} */
    other,
  ) {
    let commonPath = this.commonPathToRoot(other);
    let thisPath = this.getPathToRoot();
    if (thisPath.length === commonPath.length) {
      // The other element was contained in this element.
      return new ParentWithIndex(this.parent, this.indexInParent);
    }
    return new ParentWithIndex(
      this.equationEditor.elementFromPath(commonPath),
      thisPath[commonPath.length],
    );
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
    let shouldSwap = end2.element.isToTheLeftOf(end1.element);
    if (end1.element === end2.element) {
      shouldSwap = (end2.position < end1.position);
    }
    if (shouldSwap) {
      // Swap the two ends.
      let end2copy = end2;
      end2 = end1;
      end1 = end2copy;
    }
    let newRange = document.createRange();
    end1.element.setRangeStart(newRange, end1.position);
    end2.element.setRangeEnd(newRange, end2.position);
    document.getSelection().removeAllRanges();
    document.getSelection().addRange(newRange);
  }

  /** @returns{boolean} whether the default should be prevented. */
  setSelectionEnd(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    if (!shiftHeld) {
      return false;
    }
    let newSelection = null;
    if (key === "ArrowLeft" || key === "ArrowUp") {
      newSelection = this.equationEditor.selectionEnd.leftHorizontalNeighbor();
      //  this.equationEditor.selectionEnd.leftNeighbor();
    } else {
      newSelection = this.equationEditor.selectionEnd.rightHorizontalNeighbor();
      // newSelection = this.equationEditor.selectionEnd.rightNeighbor();
    }
    if (newSelection.element === null) {
      return false;
    }
    this.equationEditor.selectionEnd = newSelection;
    this.equationEditor.selectionStartExpanded = this.equationEditor.selectionStart;
    if (this.equationEditor.selectionStart.element.parent != this.equationEditor.selectionEnd.element.parent) {
      let parentOfSelectionStart = this.equationEditor.selectionStart.element.commonParent(
        this.equationEditor.selectionEnd.element
      );
      this.equationEditor.selectionStartExpanded = new AtomWithPosition(
        parentOfSelectionStart.parent.children[parentOfSelectionStart.indexInParent],
        - 1,
      );
    }
    this.selectBetween(
      this.equationEditor.selectionStartExpanded,
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
  firstAtom(
    /**@type{number} */
    direction,
  ) {
    if (direction > 0) {
      return this.firstAtomToTheRight();
    } else {
      return this.firstAtomToTheLeft();
    }
  }

  /** @returns{MathNode|null} 
   * Returns first atom uncle to the left or right that is a child of a horizontal ancestor. 
   * Negative direction indicates we are looking for uncles to the left, 
   * else we are looking for uncles to the right of the this element.
   * */
  firstAtomUncle(direction) {
    if (direction < 0) {
      direction = -1;
    } else {
      direction = 1;
    }
    let current = this;
    while (true) {
      let horizontalAncestor = current.findHorizontalMathParent();
      if (horizontalAncestor.parent === null) {
        return null;
      }
      for (
        let nextIndex = horizontalAncestor.indexInParent + direction;
        nextIndex >= 0 && nextIndex < horizontalAncestor.parent.children.length;
        nextIndex += direction
      ) {
        let candidate = horizontalAncestor.parent.children[nextIndex];
        if (candidate.isAtomEditable()) {
          return candidate;
        }
      }
      current = horizontalAncestor.parent;
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

  firstAtomToTheRightOf(
    /** @type{number}*/
    childIndex,
  ) {
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

  firstAtomToTheLeftOf(
    /** @type {number} */
    childIndex,
  ) {
    for (let i = childIndex - 1; i >= 0; i--) {
      let candidate = this.children[i].rightmostAtomChild(i);
      if (candidate !== null) {
        return candidate;
      }
    }
    return this.firstAtomToTheLeft();
  }

  /** @returns{MathNode|null} */
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

  appendChildren(
    /** @type{MathNode[]|null[]} */
    newChildren,
  ) {
    for (let i = 0; i < newChildren.length; i++) {
      if (newChildren[i] === null) {
        continue;
      }
      this.appendChild(newChildren[i]);
    }
  }

  ensureEditableAtomsRecursive() {
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].ensureEditableAtomsRecursive();
    }
    if (this.type.type !== knownTypes.horizontalMath.type) {
      return;
    }
    this.normalizeHorizontalMath();
    this.ensureEditableAtoms();
  }

  normalizeHorizontalMath() {
    if (this.type.type !== knownTypes.horizontalMath.type) {
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
    let sibling = this.firstAtom(1);
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
      console.log("Unexpected failure to find matching left parenthesis.");
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
    parent.updateDOM();
    parent.focusRestore();
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

  applyBackspaceToTheRightMatrix() {
    if (this.type.type !== knownTypes.matrix.type) {
      return false;
    }
    let parent = this.parent;
    parent.removeChildRange(this.indexInParent);
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
    if (this.applyBackspaceToTheRightMatrix()) {
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
  findHorizontalMathParent() {
    let result = new ParentWithIndex(this.parent, this.indexInParent);
    while (result.parent !== null) {
      if (result.parent.type.type === knownTypes.horizontalMath.type) {
        return result;
      }
      result.indexInParent = result.parent.indexInParent;
      result.parent = result.parent.parent;
    }
    return result;
  }

  makeHorizontalOperatorCorrectInput(
    /** @type {string} */
    contentTransformedToMathSymbol,
  ) {
    this.makeHorizontalOperator(latexConstants.normalizeOperatorToUtf8(contentTransformedToMathSymbol));
  }

  makeHorizontalOperator(
    /** @type {string} */
    key,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorizintalMathParent()) {
      console.log("Warning: horizontal operator made on element not contained in horizontal math.");
      return;
    }
    let parent = this.parent;
    let split = this.splitByCaretEmptyAtoms();
    split[1].desiredCaretPosition = 0;
    parent.replaceChildAtPositionWithChildren(
      this.indexInParent, [
      split[0],
      mathNodeFactory.atomImmutable(this.equationEditor, key),
      split[1],
    ]);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    parent.focusRestore();
  }

  makeParenthesesLeft() {
    this.makeParenthesesCommon(true);
  }

  makeParenthesesRight() {
    this.makeParenthesesCommon(false);
  }

  makeMatrix(
    /**@type{number} */
    rows,
    /**@type{number} */
    columns,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorizintalMathParent()) {
      console.log("Warning: horizontal operator made on element not contained in horizontal math.");
      return;
    }
    let parent = this.parent;
    let split = this.splitByCaretEmptyAtoms();
    let matrix = mathNodeFactory.matrix(this.equationEditor, rows, columns);
    parent.replaceChildAtPositionWithChildren(
      this.indexInParent, [
      split[0],
      matrix,
      split[1],
    ]);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    matrix.children[0].children[1].children[0].children[0].focus(0);
  }

  makeSqrt() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.makeSqrtFromSelection(splitBySelection);
    } else {
      this.makeSqrtFromCaret(splitBySelection);
    }
  }

  makeSqrtFromSelection(
    /** @type{SplitBySelectionResult} */
    splitBySelection,
  ) {
    let ancestor = splitBySelection.ancestor;
    ancestor.removeAllChildren();
    let underTheRadical = mathNodeFactory.horizontalMathFromArray(this.equationEditor, splitBySelection.split);
    underTheRadical.ensureEditableAtoms();
    let sqrt = mathNodeFactory.sqrt(this.equationEditor, underTheRadical);
    ancestor.appendChildren(splitBySelection.beforeSplit);
    ancestor.appendChild(sqrt);
    ancestor.appendChildren(splitBySelection.afterSplit);
    ancestor.ensureEditableAtoms();
    ancestor.updateDOM();
    sqrt.children[2].focusStandard(0);
  }

  makeSqrtFromCaret() {
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
    let parentAndIndex = this.findHorizontalMathParent();
    if (parentAndIndex.parent === null) {
      // No ancestor is of type horizontal math. 
      console.log("Warning: could not find ancestor of type horizontal math.");
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
  splitByCaretEmptyAtoms() {
    return this.splitByPositionEmptyAtoms(this.positionCaretBeforeKeyEvents)
  }

  /** @returns {MathNode[]} */
  splitByPositionEmptyAtoms(
    /** @type{number} */
    position,
  ) {
    let result = this.splitByPosition(position);
    if (result[0] === null) {
      result[0] = mathNodeFactory.atom(this.equationEditor, "");
    }
    if (result[1] === null) {
      result[1] = mathNodeFactory.atom(this.equationEditor, "");
    }
    return result;
  }

  /** @returns {MathNode[]} */
  splitByPosition(
    /** @type{number} */
    position,
  ) {
    if (!this.isAtomEditable()) {
      if (position <= 0) {
        return [null, this];
      } else {
        return [this, null];
      }
    }
    if (position === 0) {
      return [null, this];
    }
    let content = this.contentIfAtom();
    if (position >= content.length) {
      return [this, null];
    }
    let leftContent = content.slice(0, position);
    let rightContent = content.slice(position, content.length);
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
      console.log("Warning: making parentheses in non-horizontal math.");
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
    if (!correctedChildren[correctedChildren.length - 1].isAtomEditable()) {
      correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
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

  removeAllChildren() {
    this.removeChildRange(0, this.children.length - 1);
  }

  makeFractionNumeratorFromSelection(
    /**@type{SplitBySelectionResult} */
    split,
  ) {
    let ancestor = split.ancestor;
    ancestor.removeAllChildren();
    for (let i = 0; i < split.beforeSplit.length; i++) {
      ancestor.appendChild(split.beforeSplit[i]);
    }
    let numerator = mathNodeFactory.horizontalMathFromArray(this.equationEditor, split.split);
    numerator.ensureEditableAtoms();
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

  /** Returns an immediate horizontal math sibling, provided the node 
   * sits in horizontally laid out math and has a previous sibling.  
   * @returns{MathNode|null} 
   */
  previousHorizontalSibling() {
    return this.horizontalSibling(-1);
  }

  /** Returns an immediate horizontal math sibling, provided the node 
   * sits in horizontally laid out math and has a next sibling.  
   * @returns{MathNode|null} 
   */
  nextHorizontalSibling() {
    return this.horizontalSibling(1);
  }

  horizontalSibling(
    /**@type{number}*/
    direction
  ) {
    if (this.parent === null) {
      return null;
    }
    if (this.parent.type.type !== knownTypes.horizontalMath.type) {
      return null;
    }
    let index = this.indexInParent + direction;
    if (index < 0 || index >= this.parent.children.length) {
      return null;
    }
    return this.parent.children[index];
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
    let baseContent = originalParent.children.slice(leftIndex, rightIndex + 1);
    let base = mathNodeFactory.horizontalMathFromArray(
      this.equationEditor,
      baseContent,
    );
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, base, null);
    baseWithExponent.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildRangeWithChildren(leftIndex, rightIndex, [baseWithExponent]);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  makeBaseDefaultWithExponent() {
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, this, null);
    baseWithExponent.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildAtPosition(originalIndexInParent, baseWithExponent);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /** @returns{boolean} Focuses the cursor on an atom. */
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
    let sibling = this.firstAtom(endToFocus);
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
    if (!this.isAtomEditable()) {
      range.setStartBefore(this.element);
    } else if (this.element.childNodes.length > 0) {
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
    if (!this.isAtomEditable()) {
      range.setEndAfter(this.element);
    } else if (this.element.childNodes.length > 0) {
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
      result.push(this.contentIfAtom());
    }
    result.push(`[${this.type.type}]`);
    if (this.children.length > 0) {
      result.push("{");
      for (let i = 0; i < this.children.length; i++) {
        if (this.children[i] === null) {
          result.push("null");
          continue;
        }
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
      content += ", F";
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

  toLatexAtomic() {
    return this.contentIfAtomic();
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

  toLatexSqrt() {
    if (this.element === null) {
      return "[null(]";
    }
    let exponent = "";
    let underTheRadical = "";
    if (this.children.length > 0) {
      exponent = this.children[0].toLatex();
    }
    if (this.children.length > 2) {
      underTheRadical = this.children[2].toLatex();
    }
    if (exponent !== "") {
      return `\\sqrt[${exponent}]{${underTheRadical}}`;
    }
    return `\\sqrt{${underTheRadical}}`;
  }

  toLatexRightDelimiter() {
    if (this.element === null) {
      return "[null)]";
    }
    return `\\right${this.element.textContent}`;
  }

  toLatexMatrix() {
    if (this.element === null) {
      return "[null)]";
    }
    let matrixContent = this.children[0].children[1];
    let result = [];
    result.push("\\begin{pmatrix}");
    let rows = [];
    for (let i = 0; i < matrixContent.children.length; i++) {
      let matrixRow = matrixContent.children[i];
      let currentRowStrings = [];
      for (let j = 0; j < matrixRow.children.length; j++) {
        let entry = matrixRow.children[j];
        currentRowStrings.push(entry.toLatex());
      }
      rows.push(currentRowStrings.join("&"));
    }
    result.push(rows.join("\\\\"));
    result.push("\\end{pmatrix}");
    return result.join("");
  }

  toLatex() {
    if (this.type.type === knownTypes.fraction.type) {
      return this.toLatexFraction();
    }
    if (this.isAtomic()) {
      return this.toLatexAtomic();
    }
    if (this.type.type === knownTypes.baseWithExponent.type) {
      return this.toLatexBaseWithExponent();
    }
    if (this.type.type === knownTypes.matrix.type) {
      return this.toLatexMatrix();
    }
    if (this.type.type === knownTypes.sqrt.type) {
      return this.toLatexSqrt();
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

  /** If the element is a matrix, fetches its last row.
   * @returns {MathNode|null}
   */
  getLastMatrixRow() {
    if (this.type.type !== knownTypes.matrix.type) {
      return null;
    }
    let matrixTable = this.children[0].children[1];
    return matrixTable.children[matrixTable.children.length - 1];
  }

  /** Ensures that a matrix has rows with equal number of columns. */
  normalizeMatrix() {
    console.log("Not implemented yet");
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
  document.getElementById("debug").innerHTML = defaultEquationEditorLocal.toHtml();
  document.getElementById("latexInputTest").value = defaultEquationEditorLocal.rootNode.toLatex();
}

/** @type {EquationEditor|null} */
var defaultEquationEditorLocal = null;

function initialize() {
  MathQuill.getInterface(2).MathField(document.getElementById("mq-editor"));
}

function testEquationEditor() {
  let editorElement = document.getElementById("equation-editor");
  defaultEquationEditorLocal = new EquationEditor(editorElement);
  defaultEquationEditorLocal.options.debugLogContainer = document.getElementById("parsingLog");
  defaultEquationEditorLocal.updateDOM();
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

function testTypeset(
  /** @type{string} */
  inputId,
  /** @type{string} */
  outputId,
) {
  let input = document.getElementById(inputId);
  let inputContent = input.value;
  defaultEquationEditorLocal.writeLatex(inputContent);
}

module.exports = {
  defaultEquationEditorLocal,
  testEquationEditor,
  testTypeset,
  EquationEditor,
  initialize,
  mathFromLatex,
  mathFromElement,
};