// Licensed to you under the Apache 2.0 license.
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
    this.width = input["width"];
    this.height = input["height"];
    this.display = input["display"];
    this.verticalAlign = input["verticalAlign"];
    this.outline = input["outline"];
    this.fontSizeRatio = input["fontSizeRatio"];
    this.fontWeight = input["fontWeight"];
    this.whiteSpace = input["whiteSpace"];
    this.textAlign = input["textAlign"];
    this.position = input["position"];
    this.minWidth = input["minWidth"];
    this.cursor = input["cursor"];
    this.overflow = input["overflow"];
    this.boxSizing = input["boxSizing"];
    this.colorImplied = input["colorImplied"];
    this.colorText = input["colorText"];
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
    this.borderTopLeftRadius = input["borderTopLeftRadius"];
    this.borderTopRightRadius = input["borderTopRightRadius"];
    this.borderBottomLeftRadius = input["borderBottomLeftRadius"];
    this.borderBottomRightRadius = input["borderBottomRightRadius"];
    this.borderStyle = input["borderStyle"];
    this.borderColor = input["borderColor"];
    this.borderBottom = input["borderBottom"];
    this.borderTop = input["borderTop"];
    this.borderLeft = input["borderLeft"];
    this.borderRight = input["borderRight"];
  }

  /**@return {MathNodeType} */
  clone() {
    return Object.assign(new MathNodeType({}), this);
  }
}

const knownTypeDefaults = {
  "width": "",
  "height": "",
  "minWidth": "",
  "display": "inline-block",
  "verticalAlign": "",
  "fontSizeRatio": 1,
  "fontWeight": "",
  "whiteSpace": "",
  "textAlign": "",
  "position": "absolute",
  "cursor": "",
  "outline": "",
  "overflow": "",
  "boxSizing": "",
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
  "borderBottomLeftRadius": "",
  "borderBottomRightRadius": "",
  "borderTopLeftRadius": "",
  "borderTopRightRadius": "",
  "borderStyle": "",
  "borderColor": "",
  "borderBottom": "",
  "borderTop": "",
  "borderLeft": "",
  "borderRight": "",
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

const knownTypes = {
  root: new MathNodeType({
    "type": "root",
    "padding": "2px",
    "margin": "2px",
    "cursor": "text",
    "minWidth": "30px",
  }),
  // A math expression with no children such as "x", "2".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    "type": "atom",
    "outline": "0px solid transparent",
    "width": "auto",
    "height": "auto",
    "minWidth": "4px",
    "verticalAlign": "text-bottom",
    "overflow": "hidden",
  }),
  // An input element: rather than being a contentEditable div,
  // this is a text-only input box.
  formInput: new MathNodeType({
    "type": "formInput",
  }),
  // A non-editable math expression/operator such as "+" or "-".
  atomImmutable: new MathNodeType({
    "type": "atomImmutable",
    "paddingLeft": `${0.1}em`,
    "paddingRight": `${0.1}em`,
    "outline": "0px solid transparent",
    "textAlign": "center",
    "width": "auto",
    "height": "auto",
  }),
  eventCatcher: new MathNodeType({
    "type": "eventCatcher",
    "width": "0px",
    "maxWidth": "0px",
    "height": "0px",
    "maxHeight": "0px",
    "display": "hidden",
    "overflow": "hidden",
  }),
  error: new MathNodeType({
    "type": "error",
    "colorText": "red",
    "whiteSpace": "nowrap",
  }),
  leftDelimiterMark: new MathNodeType({
    "type": "leftDelimiterMark",
    "borderLeft": "2px solid",
    "colorImplied": "silver",
    "boxSizing": "border-box",
  }),
  // Left delimiter (parentheses, bracket, ...)
  leftDelimiter: new MathNodeType({
    "type": "leftDelimiter",
    "colorImplied": "silver",
  }),
  // Right delimiter mark surrounded by two spacers.
  rightDelimiterMark: new MathNodeType({
    "type": "rightDelimiterMark",
    "borderRight": "2px solid",
    "colorImplied": "silver",
    "boxSizing": "border-box",
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
    "verticalAlign": "text-bottom",
    "textAlign": "center",
  }),
  verticalLineInTable: new MathNodeType({
    "type": "verticalLine",
    "borderLeft": "1px solid black",
  }),
  // Represents expressions such as "x/y" or "\frac{x}{y}".
  fraction: new MathNodeType({
    "type": "fraction",
  }),
  cancel: new MathNodeType({
    "type": "cancel",
  }),
  cancelSign: new MathNodeType({
    "type": "cancelSign",
    "borderLeft": "2px solid black",
  }),
  cancelUnderBox: new MathNodeType({
    "type": "cancelUnderBox",
  }),
  // Represents the numerator x of a fraction x/y.
  numerator: new MathNodeType({
    "type": "numerator",
    "borderBottom": "1px solid black",
    "fontSizeRatio": defaultFractionScale,
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
    "fontSizeRatio": defaultFractionScale,
    "arrows": {
      "ArrowUp": arrowMotion.firstAtomToTheLeft,
      "ArrowDown": arrowMotion.firstAtomToTheRight,
    },
    "textAlign": "center",
  }),
  overLinedBox: new MathNodeType({
    "type": "overLinedBox",
    "borderTop": "1px solid black",
    "boxSizing": "border-box",
  }),
  genericMathBox: new MathNodeType({
    "type": "genericMathBox",
  }),
  baseWithExponent: new MathNodeType({
    "type": "baseWithExponent",
  }),
  exponent: new MathNodeType({
    "type": "exponent",
    "fontSizeRatio": 0.75,
  }),
  baseWithSubscript: new MathNodeType({
    "type": "baseWithSubscript",
  }),
  underBrace: new MathNodeType({
    "type": "underBrace",
  }),
  overBrace: new MathNodeType({
    "type": "overBrace",
  }),
  horizontalBraceUp: new MathNodeType({
    "type": "horizontalBraceUp",
  }),
  topRightQuarterCircle: new MathNodeType({
    "type": "topRightQuarterCircle",
    "borderTopRightRadius": "4px",
  }),
  topLeftQuarterCircle: new MathNodeType({
    "type": "topLeftQuarterCircle",
    "borderTopLeftRadius": "4px",
  }),
  bottomRightQuarterCircle: new MathNodeType({
    "type": "bottomRightQuarterCircle",
    "borderBottomRightRadius": "4px",
  }),
  bottomLeftQuarterCircle: new MathNodeType({
    "type": "bottomLeftQuarterCircle",
    "borderBottomLeftRadius": "4px",
  }),
  horizontalLineBottomMargin: new MathNodeType({
    "type": "horizontalLineBottomMargin",
    "borderBottom": "2px solid black",
  }),
  verticalLineLeftMargin: new MathNodeType({
    "type": "verticalLineLeftMargin",
    "borderLeft": "solid",
  }),
  verticalLineRightMargin: new MathNodeType({
    "type": "verticalLineRightMargin",
    "borderRight": "solid",
  }),
  subscript: new MathNodeType({
    "type": "subscript",
    "fontSizeRatio": 0.75,
  }),
  sqrt: new MathNodeType({
    "type": "sqrt",
    "margin": "2px",
  }),
  sqrtSign: new MathNodeType({
    "type": "sqrtSign",
  }),
  sqrtSignDecoration: new MathNodeType({
    "type": "sqrtSignDecoration",
    "borderTop": "1px solid black",
  }),
  sqrtSignLeft: new MathNodeType({
    "type": "sqrtSignLeft",
    "borderLeft": "1px solid black",
  }),
  sqrtSignRight: new MathNodeType({
    "type": "sqrtSignRight",
    "borderLeft": "1px solid black",
  }),
  radicalExponentBox: new MathNodeType({
    "type": "radicalExponentBox",
    "fontSizeRatio": 0.75,
  }),
  radicalUnderBox: new MathNodeType({
    "type": "radicalUnderBox",
    "borderTop": "1px solid black",
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
  operatorWithSuperAndSubscript: new MathNodeType({
    "type": "operatorWithSuperAndSubscript",
  }),
  operatorWithSubscript: new MathNodeType({
    "type": "operatorWithSubscript",
  }),
  operatorStandalone: new MathNodeType({
    "type": "operatorStandalone",
    "fontSizeRatio": 1.8,
  }),
  operatorSubscript: new MathNodeType({
    "type": "operatorSubscript",
    "fontSizeRatio": 0.55,
  }),
  operatorSuperscript: new MathNodeType({
    "type": "operatorSuperscript",
    "fontSizeRatio": 0.55,
  }),
};

class MathNodeFactory {
  horizontalMath(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    const result = new MathNodeHorizontalMath(equationEditor);
    if (content === null) {
      content = this.atom(equationEditor, "");
    }
    result.appendChild(content);
    return result;
  }

  genericMathBox(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    const result = new MathNodeGenericBox(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, content));
    return result;
  }

  /** @returns{MathNode} */
  // Returns a horizontal math with content given by the input array.
  // Normalizes the input but does not ensure editable atoms at the ends. 
  horizontalMathFromArray(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {(MathNode|null)[]} */
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

  atom(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    initialContent,
  ) {
    const result = new MathNodeAtom(equationEditor);
    result.positionCaretBeforeKeyEvents = initialContent.length;
    result.initialContent = initialContent;
    return result;
  }

  atomImmutable(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    operator,
  ) {
    const result = new MathNodeAtomImmutable(equationEditor);
    if (operator in latexConstants.operatorsExtraPadding) {
      let extraPadding = latexConstants.operatorsExtraPadding[operator];
      result.type.paddingLeft = extraPadding;
      result.type.paddingRight = extraPadding;
    }
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
    const fraction = new MathNodeFraction(equationEditor);
    const numerator = new MathNodeNumerator(equationEditor);
    const denominator = new MathNodeDenominator(equationEditor);
    numerator.appendChild(this.horizontalMath(equationEditor, numeratorContent));
    denominator.appendChild(this.horizontalMath(equationEditor, denominatorContent));
    fraction.appendChild(numerator);
    fraction.appendChild(denominator);
    return fraction;
  }

  rootMath(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    const result = new MathNodeRoot(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, null));
    return result;
  }

  error(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    initialContent,
  ) {
    const result = new MathNodeError(equationEditor);
    result.initialContent = initialContent;
    return result;
  }

  /** @returns {MathNode} */
  cancel(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode}*/
    content,
  ) {
    const result = new MathNodeCancel(equationEditor);
    const cancelSign = new MathNode(equationEditor, knownTypes.cancelSign);
    const horizontal = this.horizontalMath(equationEditor, content);
    let underTheCancel = new MathNodeCancelUnderBox(equationEditor);
    underTheCancel.appendChild(horizontal);
    underTheCancel.normalizeHorizontalMath();
    underTheCancel.ensureEditableAtoms();
    result.appendChild(cancelSign);
    result.appendChild(underTheCancel);
    return result;
  }

  /** @returns {MathNode} */
  sqrtSign(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    let result = new MathNodeSqrtSign(equationEditor);
    result.appendChild(new MathNode(equationEditor, knownTypes.sqrtSignDecoration));
    result.appendChild(new MathNode(equationEditor, knownTypes.sqrtSignLeft));
    result.appendChild(new MathNode(equationEditor, knownTypes.sqrtSignRight));
    return result;
  }

  /** @returns {MathNode} */
  sqrt(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    underTheRadicalContent,
    /** @type {MathNode|null} */
    exponentContent,
  ) {
    const sqrt = new MathNodeSqrt(equationEditor);
    const radicalExponentBox = new MathNode(equationEditor, knownTypes.radicalExponentBox);
    radicalExponentBox.appendChild(this.horizontalMath(equationEditor, exponentContent));
    sqrt.appendChild(radicalExponentBox);
    sqrt.appendChild(mathNodeFactory.sqrtSign(equationEditor));
    const underTheRadical = new MathNodeRadicalUnderBox(equationEditor);
    underTheRadical.appendChild(mathNodeFactory.horizontalMath(equationEditor, underTheRadicalContent));
    sqrt.appendChild(underTheRadical);
    return sqrt;
  }

  formInput(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    content,
  ) {
    let nameAndContent = content.split(",");
    if (nameAndContent.length < 2) {
      nameAndContent.push("");
      nameAndContent.push("");
    }
    nameAndContent[0] = nameAndContent[0].trim();
    nameAndContent[1] = nameAndContent[1].trim();
    const formInput = new MathNodeFormInput(
      equationEditor, nameAndContent[0], nameAndContent[1],
    );
    return formInput;
  }

  /** @returns {MathNode} */
  overLine(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    const result = new MathNodeOverLine(equationEditor);
    const horizontalMath = this.horizontalMath(equationEditor, content);
    result.appendChild(horizontalMath);
    return result;
  }

  /** @returns {MathNode} */
  overBrace(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
    /** @type {MathNode|null} */
    overBraceContent,
  ) {
    const result = new MathNodeOverBrace(equationEditor);
    const base = mathNodeFactory.horizontalMath(equationEditor, content);
    // 6 components of overbrace line
    const horizontalBraceTopLeft = new MathNode(equationEditor, knownTypes.topLeftQuarterCircle);
    const horizontalBraceLeft = new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceBottomRight = new MathNode(equationEditor, knownTypes.bottomRightQuarterCircle);
    const horizontalBraceBottomLeft = new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle);
    const horizontalBraceRight = new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceTopRight = new MathNode(equationEditor, knownTypes.topRightQuarterCircle);

    const horizontalBrace = new MathNode(equationEditor, knownTypes.horizontalBraceUp);
    horizontalBrace.appendChildren([
      horizontalBraceTopLeft,
      horizontalBraceLeft,
      horizontalBraceBottomRight,
      horizontalBraceBottomLeft,
      horizontalBraceRight,
      horizontalBraceTopRight,
    ]);
    let superscript = mathNodeFactory.genericMathBox(equationEditor, overBraceContent);
    result.appendChild(base);
    result.appendChild(horizontalBrace);
    result.appendChild(superscript);
    return result;
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
    const baseWithExponent = new MathNodeBaseWithExponent(equationEditor);
    // Horizontal math wrapper for the exponent.
    const exponentWrapped = new MathNodeExponent(equationEditor);
    let exponentContainer = this.horizontalMath(equationEditor, exponent);
    exponentContainer.normalizeHorizontalMath();
    exponentContainer.ensureEditableAtoms();
    exponentWrapped.appendChild(exponentContainer);
    // Horizontal math wrapper for the base.
    const baseHorizontal = new MathNode(equationEditor, knownTypes.horizontalMath);
    baseHorizontal.appendChild(base);
    baseHorizontal.normalizeHorizontalMath();
    // The base with the exponent.
    baseWithExponent.appendChild(baseHorizontal);
    baseWithExponent.appendChild(exponentWrapped);
    return baseWithExponent;
  }

  /** @returns {MathNode} */
  baseWithSubscript(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type{MathNode}*/
    base,
    /** @type{MathNode|null}*/
    subscript,
  ) {
    // Horizontal math wrapper for the exponent.
    const subscriptWrapped = new MathNodeSubscript(equationEditor);
    let subscriptContainer = this.horizontalMath(equationEditor, subscript);
    subscriptContainer.normalizeHorizontalMath();
    subscriptContainer.ensureEditableAtoms();
    subscriptWrapped.appendChild(subscriptContainer);
    // Horizontal math wrapper for the base.
    const baseHorizontal = new MathNode(equationEditor, knownTypes.horizontalMath);
    baseHorizontal.appendChild(base);
    baseHorizontal.normalizeHorizontalMath();
    // The base with the subscript.
    const baseWithSubscript = new MathNodeBaseWithSubscript(equationEditor);
    baseWithSubscript.appendChild(baseHorizontal);
    baseWithSubscript.appendChild(subscriptWrapped);
    return baseWithSubscript;
  }

  /** @returns {MathNode} */
  leftParenthesis(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {boolean} */
    implied,
  ) {
    return this.leftDelimiter(equationEditor, "(", implied);
  }

  /** @returns {MathNode} */
  leftDelimiter(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    content,
    /** @type {boolean} */
    implied,
  ) {
    const leftDelimiter = new MathNodeLeftDelimiter(equationEditor);
    leftDelimiter.implied = implied;
    if (content === "") {
      return leftDelimiter;
    }
    leftDelimiter.appendChild(this.delimiterMark(equationEditor, content, true, implied));
    leftDelimiter.extraData = content;
    return leftDelimiter;
  }

  delimiterMark(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    content,
    /**@type{boolean} */
    left,
    /** @type {boolean} */
    implied,
  ) {
    let result = null;
    switch (content) {
      case "|":
        result = new MathNodeAbsoluteValue(equationEditor, left);
        break;
      case "\\rangle":
      case "\\langle":
      // utf16 version of \\rangle 
      case "\u27E9":
      // utf16 version of \\langle 
      case "\u27E8":
        result = new MathNodeAngleBrackets(equationEditor, left);
        if (left) {
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
        } else {
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
        }
        break;
      case "[":
      case "]":
        result = new MathNodeSquareBrackets(equationEditor, left);
        break;
      case "(":
      case ")":
        result = new MathNodeParenthesis(equationEditor, left);
        if (left) {
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
        } else {
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
        }
        break;
      case "{":
      case "}":
      default:
        result = new MathNodeCurlyBrace(equationEditor, left);
        if (left) {
          result.appendChild(new MathNode(equationEditor, knownTypes.topLeftQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.bottomRightQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle));
        } else {
          result.appendChild(new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(equationEditor, knownTypes.bottomRightQuarterCircle));
        }
        break;
    }
    result.implied = implied;
    return result;
  }

  /** @returns {MathNode} */
  rightParenthesis(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {boolean} */
    implied,
  ) {
    return this.rightDelimiter(equationEditor, ")", implied);
  }

  /** @returns {MathNode} */
  rightDelimiter(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    content,
    /** @type {boolean} */
    implied,
  ) {
    const rightDelimiter = new MathNodeRightDelimiter(equationEditor);
    rightDelimiter.implied = implied;
    if (content === "") {
      return rightDelimiter;
    }
    rightDelimiter.appendChild(this.delimiterMark(equationEditor, content, false, implied));
    rightDelimiter.extraData = content;
    return rightDelimiter;
  }

  /** @returns {MathNode} */
  matrix(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {number} */
    rows,
    /** @type {number} */
    columns,
    /** @type {string} */
    columnStyle,
  ) {
    const matrixTable = new MathNode(equationEditor, knownTypes.matrixTable);
    for (let i = 0; i < rows; i++) {
      matrixTable.appendChild(this.matrixRow(equationEditor, columns));
    }
    let leftDelimiter = null;
    let rightDelimiter = null;
    if (columnStyle === "") {
      leftDelimiter = this.leftParenthesis(equationEditor, false);
      rightDelimiter = this.rightParenthesis(equationEditor, false);
    } else {
      leftDelimiter = this.atom(equationEditor, "");
      rightDelimiter = this.atom(equationEditor, "");
    }
    let parenthesesLayout = this.horizontalMath(equationEditor, leftDelimiter);
    parenthesesLayout.appendChild(matrixTable);
    parenthesesLayout.appendChild(rightDelimiter);
    let result = new MathNodeMatrix(equationEditor);
    result.latexExtraStyle = columnStyle;
    result.appendChild(parenthesesLayout);
    result.appendChildren(
      new LatexColumnStyleIterator(columnStyle).verticalSeparators(equationEditor),
    );
    return result;
  }

  /** @returns {MathNode} */
  matrixRow(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {number} */
    columns,
  ) {
    let result = new MathNodeMatrixRow(equationEditor);
    for (let i = 0; i < columns; i++) {
      result.appendChild(this.matrixRowEntry(equationEditor, null));
    }
    return result;
  }

  /** @returns {MathNode} */
  matrixRowEntry(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {MathNode|null} */
    content,
  ) {
    let result = new MathNodeRowEntry(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, content));
    return result;
  }

  /** @returns {MathNode} */
  matrixVerticalLine(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {number} */
    columnIndex,
    /** @type {number} */
    numberOfStrips,
  ) {
    let result = new MathNodeVerticalLine(equationEditor);
    result.extraData = new VerticalBarData(columnIndex, numberOfStrips);
    return result;
  }

  /** @returns {MathNode} */
  operatorWithSuperAndSubscript(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    operator,
    /** @type {MathNode|null} */
    superscript,
    /** @type {MathNode|null} */
    subscript,
  ) {
    let result = new MathNodeOperatorWithSuperAndSubscript(equationEditor);
    let superscriptNode = new MathNodeOperatorSuperscript(equationEditor);
    let subscriptNode = new MathNodeOperatorSubscript(equationEditor);
    let operatorNode = new MathNodeOperatorStandalone(equationEditor);
    superscriptNode.appendChild(this.horizontalMath(equationEditor, superscript));
    subscriptNode.appendChild(this.horizontalMath(equationEditor, subscript));
    operatorNode.appendChild(this.atomImmutable(equationEditor, operator));
    result.appendChild(superscriptNode);
    result.appendChild(operatorNode);
    result.appendChild(subscriptNode);
    return result;
  }

  /** @returns {MathNode} */
  operatorWithSubscript(
    /** @type {EquationEditor} */
    equationEditor,
    /** @type {string} */
    operator,
    /** @type {MathNode|null} */
    subscript,
  ) {
    let result = new MathNodeOperatorWithSubscript(equationEditor);
    let subscriptNode = new MathNodeOperatorSubscript(equationEditor);
    let subscriptScale = 0.8;
    subscriptNode.type.fontSizeRatio = subscriptScale;
    let operatorNode = new MathNode(equationEditor, knownTypes.operatorStandalone);
    let operatorScale = 1;
    operatorNode.type.fontSizeRatio = operatorScale;
    subscriptNode.appendChild(this.horizontalMath(equationEditor, subscript));
    operatorNode.appendChild(this.atomImmutable(equationEditor, operator));
    result.appendChild(operatorNode);
    result.appendChild(subscriptNode);
    return result;
  }
}

var mathNodeFactory = new MathNodeFactory();

class MathNodeWithCaretPosition {
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
      this.position = - 1;
    }
  }

  assign(
    /**@type{MathNodeWithCaretPosition} */
    other,
  ) {
    this.element = other.element;
    this.position = other.position;
  }

  /**@returns{number} */
  getPositionWholeItemIfNegative(
    /**@type{number} If position is negative, 
     * determines which end to select: 
     * negative direction for left end, non-negative for right.*/
    direction,
  ) {
    if (this.position >= 0) {
      return this.position;
    }
    if (direction < 0) {
      return 0;
    }
    return this.element.textContentOrInitialContent().length;
  }

  /**@returns{number} */
  nextPositionInDirection(
    /**@type{number} */
    direction,
  ) {
    if (direction === 0) {
      direction = 1;
    }
    let content = this.element.textContentOrInitialContent();
    let result = this.position + direction;
    for (; result >= 0 && result < content.length; result += direction) {
      let code = content.charCodeAt(result);
      if (code < 0xD800 || code > 0xDFFF) {
        return result;
      }
    }
    if (result >= content.length) {
      return content.length;
    }
    if (result < 0) {
      return 0;
    }
    return - 1;
  }

  /** @returns {MathNodeWithCaretPosition} */
  leftHorizontalNeighborBalanced(
    /**@type{boolean} */
    shrinking,
  ) {
    if (this.element === null) {
      return new MathNodeWithCaretPosition(null, - 1);
    }
    if (this.position > 0 && this.element.hasHorozintalMathParent()) {
      return new MathNodeWithCaretPosition(this.element, this.nextPositionInDirection(- 1));
    }
    let next = null;
    if (shrinking && this.element.type.type === knownTypes.rightDelimiter.type) {
      let matching = this.element.findMatchingDelimiter();
      if (matching === null) {
        return new MathNodeWithCaretPosition(null, - 1);
      }
      next = matching.firstAtomSiblingOrUncle(- 1);
    } else {
      next = this.element.firstAtomSiblingOrUncle(- 1);
    }
    if (next === null) {
      return new MathNodeWithCaretPosition(null, - 1);
    }
    return new MathNodeWithCaretPosition(next, - 1);
  }

  /** @returns {MathNodeWithCaretPosition} */
  rightHorizontalNeighborBalanced(
    /**@type{boolean} */
    shrinking,
  ) {
    if (this.element === null) {
      return new MathNodeWithCaretPosition(null, -1);
    }
    if (
      this.position >= 0 &&
      this.position < this.element.lengthContentIfAtom() &&
      this.element.hasHorozintalMathParent()
    ) {
      return new MathNodeWithCaretPosition(this.element, this.nextPositionInDirection(1));
    }
    let next = null;
    if (shrinking && this.element.type.type === knownTypes.leftDelimiter.type) {
      let matching = this.element.findMatchingDelimiter();
      if (matching === null) {
        return new MathNodeWithCaretPosition(null, - 1);
      }
      next = matching.firstAtomSiblingOrUncle(1);
    } else {
      next = this.element.firstAtomSiblingOrUncle(1);
    }
    if (next === null) {
      return new MathNodeWithCaretPosition(null, -1);
    }
    return new MathNodeWithCaretPosition(next, - 1);
  }

  /** @returns {MathNodeWithCaretPosition} */
  leftNeighbor() {
    if (this.element === null) {
      return new MathNodeWithCaretPosition(null, -1);
    }
    if (this.position > 0) {
      return new MathNodeWithCaretPosition(this.element, this.nextPositionInDirection(- 1));
    }
    let resultElement = this.element.firstAtomToTheLeft();
    if (resultElement === null) {
      return new MathNodeWithCaretPosition(null, - 1);
    }
    return new MathNodeWithCaretPosition(resultElement, resultElement.element.textContent.length);
  }

  /** @returns {MathNodeWithCaretPosition} */
  rightNeighbor() {
    if (this.element === null) {
      return new MathNodeWithCaretPosition(null, -1);

    }
    if (this.position < this.element.lengthContentIfAtom()) {
      return new MathNodeWithCaretPosition(this.element, this.nextPositionInDirection(1));
    }
    return new MathNodeWithCaretPosition(this.element.firstAtomToTheRight(), 0);
  }

  /** @returns {string} */
  toString() {
    if (this.element === null) {
      return `[null, ${this.position}]`;
    }
    let detached = "";
    if (this.element.isDetached()) {
      detached = ",<b style='color:red'> detached</b>";
    }
    return `[${this.element.toString()}, ${this.position}${detached}]`;
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
  /**@type{boolean} */
  sanitizeLatexSource,
  /**@type{boolean} whether to remove \\displaystyle from latex source.*/
  removeDisplayStyle,
  /**@type{Function|null} callback after the element has been typeset.*/
  callback,
) {
  let content = container.textContent;
  if (content === null) {
    content = "";
  }
  return mathFromLatex(
    container,
    content,
    editable,
    sanitizeLatexSource,
    removeDisplayStyle,
    callback,
  );
}

/** @returns {EquationEditor} Returns typeset math.*/
function mathFromLatex(
  /**@type{HTMLElement} */
  container,
  /**@type{string} */
  latex,
  /**@type{boolean} */
  editable,
  /**@type{boolean} */
  sanitizeLatexSource,
  /**@type{boolean} whether to remove \\displaystyle from latex source.*/
  removeDisplayStyle,
  /**@type{Function|null} callback after the element has been typeset*/
  callback,
) {
  let lineBreakWidthString = container.getAttribute("lineBreakWidth");
  let lineBreakWidth = 0;
  if (lineBreakWidthString !== "") {
    lineBreakWidth = parseInt(lineBreakWidthString);
  }
  let result = new EquationEditor(container, new EquationEditorOptions({
    editable: editable,
    sanitizeLatexSource: sanitizeLatexSource,
    removeDisplayStyle: removeDisplayStyle,
    logTiming: true,
    lineBreakWidth: lineBreakWidth,
  }));
  result.writeLatex(latex);
  if (callback !== undefined && callback !== null) {
    callback(result);
  }
  return result;
}

/**@returns{HTMLElement}*/
function boldElement(
  /**@type{string} */
  text,
  /**@type{string} */
  color,
) {
  let result = document.createElement("b");
  result.textContent = text;
  result.style.color = color;
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

  /**@returns{HTMLElement[]} */
  toHtmlDebugData() {
    let result = [];
    if (this.node !== null) {
      let next = this.node.toLatex();
      if (next === "") {
        next = `[${this.node.type.type}]`;
      }
      result.push(document.createTextNode(next));
    }
    if (this.content !== "") {
      result.push(boldElement(this.content, "orange"));
    }
    if (this.syntacticRole !== "") {
      result.push(boldElement(this.syntacticRole, "red"));
    }
    return result;
  }

  isExpression() {
    return this.syntacticRole === "" && this.node !== null;
  }

  isMatrixEnder() {
    return this.syntacticRole in latexConstants.matrixEnder;
  }
}

class ToLatexOptions {
  constructor(useCaretCommand) {
    this.useCaretCommand = useCaretCommand;
  }
}

let latexOptionsWithCaret = new ToLatexOptions(true);

class LaTeXConstants {
  constructor() {
    this.latinCharactersString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    this.latinCharacters = {};
    for (let i = 0; i < this.latinCharactersString.length; i++) {
      this.latinCharacters[this.latinCharactersString[i]] = true;
    }
    this.digitString = "0123456789";
    this.digits = {};
    for (let i = 0; i < this.digitString.length; i++) {
      this.digits[this.digitString[i]] = true;
    }
    this.characterReplacingSelectionString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 +-*";
    this.characterReplacingSelection = {};
    for (let i = 0; i < this.characterReplacingSelectionString.length; i++) {
      this.characterReplacingSelection[this.characterReplacingSelectionString[i]] = true;
    }
    this.operatorsExtraPadding = {
      "=": "0.3em",
      // leq
      "\u2264": "0.3em",
      // geq
      "\u2265": "0.3em",
      // Latex to (right arrow).
      "\u2192": "0.3em",
      ">": "0.3em",
      "<": "0.3em",
      // neq
      "\u2260": "0.3em",
      // ellipsis dots
      "\u2026": "0.3em",
    };
    /**@type{Object.<string, string>} */
    this.operatorsNormalized = {
      // Full-width plus sign, wider and taller plus sign.
      "\uFF0B": "+",
      "+": "+",
      // \pm
      "\u00B1": "\u00B1",
      // Mathematical minus, a wider dash.
      "\u2212": "\u2212",
      "-": "\u2212",
      "*": "\u00B7",
      // A vertically centered small solid dot.
      "\u00B7": "\u00B7",
      "=": "=",
      ">": ">",
      "<": "<",
    };
    /**@type{Object.<string, string>} */
    this.operatorsFromUft8 = {
      "\u2212": "-",
    };
    /**@type{Object.<string, string>} */
    this.latexSyntacticValues = {
      "{": "{",
      "}": "}",
      "^": "^",
      "_": "_",
      "(": "(",
      ")": ")",
      "\\": "\\",
      "&": "&",
      "[": "[",
      "]": "]",
      "|": "|",
    };
    /**@type{Object.<string, string>} */
    this.latexBackslashCommands = {
      "left": "\\left",
      "right": "\\right",
      "cancel": "\\cancel",
      "sqrt": "\\sqrt",
      "begin": "\\begin",
      // Special command reserved for indicating the 
      // caret position when the latex is used in an editable box.
      "caret": "\\caret",
      // Special command reserved for generation of input box.
      "formInput": "\\formInput",
      "end": "\\end",
      "frac": "\\frac",
      "mathcal": "\\mathcal",
      "mathfrak": "\\mathfrak",
      "mathbb": "\\mathbb",
      "langle": "\\langle",
      "rangle": "\\rangle",
      "{": "\\{",
      "binom": "\\binom",
      "stackrel": "\\stackrel",
      "overbrace": "\\overbrace",
      "overline": "\\overline",
      "color": "\\color",
      "mathbf": "\\mathbf",
    };
    /**@type{Object.<string, string>} */
    this.latexBackslashOperators = {
      "bullet": "\u2022",
      "otimes": "\u2297",
      "oplus": "\u2295",
      "times": "\u00D7",
      "circ": "\u2218",
      "cdot": "\u00B7",
      "leq": "\u2264",
      "neq": "\u2260",
      "dots": "\u2026",
      "vdots": "\u22EE",
      "approx": "~",
      "subset": "\u2282",
      "supset": "\u2283",
      "setminus": "\\",
      "lt": "<",
      "gt": ">",
      "sqcup": "\u2294",
      "to": "\u2192",
      "nwarrow": "\u2196",
      "nearrow": "\u2197",
      "searrow": "\u2198",
      "swarrow": "\u2199",
      "perp": "\u27C2",
      "pm": "\u00B1",
      "det": "det",
      "geq": "\u2265",
      "sin": "sin",
      "cos": "cos",
      "tan": "tan",
      "cot": "cot",
      "sec": "sec",
      "csc": "csc",
      "ln": "ln",
      "log": "log",
      "arctan": "arctan",
      "arccos": "arccos",
      "arcsin": "arcsin",
      "in": "\u2208",
      "cap": "\u2229",
      "cup": "\u222A",
    };
    /**@type{Object.<string, string>} */
    this.latexBackslashOperatorsBackslashed = {};
    for (let operator in this.latexBackslashOperators) {
      this.latexBackslashOperatorsBackslashed["\\" + operator] = this.latexBackslashOperators[operator];
    }

    /**@type{Object.<string, string>} */
    this.operatorWithSuperAndSubscript = {
      "sum": "\u03A3",
      "int": "\u222B",
    };
    /**@type{Object.<string, string>} */
    this.operatorWithSuperAndSubscriptBackslashed = {};
    for (let operator in this.operatorWithSuperAndSubscript) {
      this.operatorWithSuperAndSubscriptBackslashed["\\" + operator] = this.operatorWithSuperAndSubscript[operator];
    }

    /**@type{Object.<string, string>} */
    this.operatorsWithSubscript = {
      "lim": "lim",
    };
    /**@type{Object.<string, string>} */
    this.operatorsWithSubscriptBackslashed = {};
    for (let operator in this.operatorsWithSubscript) {
      this.operatorsWithSubscriptBackslashed["\\" + operator] = this.operatorsWithSubscript[operator];
    }

    /**@type{Object.<string, string>} */
    this.latexBackslashAtomsEditable = {
      "alpha": "\u03B1",
      "beta": "\u03B2",
      "gamma": "\u03B3",
      "delta": "\u03B4",
      "epsilon": "\u03B5",
      "varepsilon": "\u03B5",
      "zeta": "\u03B6",
      "eta": "\u03B7",
      "theta": "\u03B8",
      "vartheta": "",
      "iota": "\u03B9",
      "emptyset": "\u2205",
      "kappa": "\u03BA",
      "lambda": "\u03BB",
      "mu": "\u03BC",
      "nu": "\u03BD",
      "xi": "\u03BE",
      "omicron": "\u03BF",
      "pi": "\u03C0",
      "rho": "\u03C1",
      "sigma": "\u03C3",
      "tau": "\u03C4",
      "upsilon": "\u03C5",
      "phi": "\u03C6",
      "varphi": "\u03D5",
      "chi": "\u03C7",
      "psi": "\u03C8",
      "omega": "\u03C9",
      "Alpha": "\u0391",
      "Beta": "\u0392",
      "Gamma": "\u0393",
      "Delta": "\u0394",
      "Epsilon": "\u0395",
      "Zeta": "\u0396",
      "Eta": "\u0397",
      "Theta": "\u0398",
      "Iota": "\u0399",
      "Kappa": "\u039A",
      "Lambda": "\u039B",
      "Mu": "\u039C",
      "Nu": "\u039D",
      "Xi": "\u039E",
      "Omicron": "\u039F",
      "Pi": "\u03A0",
      "Rho": "\u03A1",
      "Sigma": "\u03A3",
      "Tau": "\u03A4",
      "Upsilon": "\u03A5",
      "Phi": "\u03A6",
      "Chi": "\u03A7",
      "Psi": "\u03A8",
      "Omega": "\u03A9",
      "infty": "\u221E",
      "S": "\u00A7",
      "partial": "\u2202",
    };
    /**@type{Object.<string, string>} */
    this.latexBackslashAtomsEditableBackslashed = {};
    for (let operator in this.latexBackslashAtomsEditable) {
      this.latexBackslashAtomsEditableBackslashed["\\" + operator] = this.latexBackslashAtomsEditable[operator];
    }

    /**@type{Object.<string, string>} */
    this.mathcalEquivalents = {
      "A": "\uD835\uDC9C",
      "C": "\uD835\uDC9E",
      "D": "\uD835\uDC9F",
      "G": "\uD835\uDCA2",
      "J": "\uD835\uDCA5",
      "K": "\uD835\uDCA6",
      "L": "\u2112",
      "N": "\uD835\uDCA9",
      "O": "\uD835\uDCAA",
      "P": "\uD835\uDCAB",
      "Q": "\uD835\uDCAC",
      "S": "\uD835\uDCAE",
      "T": "\uD835\uDCAF",
      "U": "\uD835\uDCB0",
      "V": "\uD835\uDCB1",
      "W": "\uD835\uDCB2",
      "X": "\uD835\uDCB3",
      "Y": "\uD835\uDCB4",
      "Z": "\uD835\uDCB5",
    };
    /**@type{Object.<string, string>} */
    this.mathfrakEquivalents = {
      "a": "\uD835\uDD1E",
      "b": "\uD835\uDD1F",
      "c": "\uD835\uDD20",
      "d": "\uD835\uDD21",
      "e": "\uD835\uDD22",
      "f": "\uD835\uDD23",
      "g": "\uD835\uDD24",
      "h": "\uD835\uDD25",
      "i": "\uD835\uDD26",
      "j": "\uD835\uDD27",
      "k": "\uD835\uDD28",
      "l": "\uD835\uDD29",
      "m": "\uD835\uDD2A",
      "n": "\uD835\uDD2B",
      "o": "\uD835\uDD2C",
      "p": "\uD835\uDD2D",
      "q": "\uD835\uDD2E",
      "r": "\uD835\uDD2F",
      "s": "\uD835\uDD30",
      "t": "\uD835\uDD31",
      "u": "\uD835\uDD32",
      "v": "\uD835\uDD33",
      "w": "\uD835\uDD34",
      "x": "\uD835\uDD35",
      "y": "\uD835\uDD36",
      "z": "\uD835\uDD37",
    };
    /**@type{Object.<string, string>} */
    this.mathbbEquivalents = {
      "A": "\uD835\uDD38",
      "C": "\u2102",
      "H": "\u210D",
      "N": "\u2115",
      "O": "\uD835\uDD46",
      "P": "\u2119",
      "Q": "\u211A",
      "R": "\u211D",
      "Z": "\u2124",
    };
    /**@type{Object.<string, string>} */
    this.leftDelimiters = {
      "\\left.": "",
      "\\langle": "\u27E8",
      "(": "(",
      "[": "[",
      "|": "|",
      "\\{": "{",
    };
    /**@type{Object.<string, string>} */
    this.delimitersAmbiguous = {
      "|": "|",
    };
    /**@type{Object.<string, string>} */
    this.rightDelimiters = {
      "\\right.": "",
      "\\rangle": "\u27E9",
      ")": ")",
      "|": "|",
      "]": "]",
      "\\}": "}",
    };
    /**@type{Object.<string, string>} */
    this.leftRightDelimiterPair = {
      "\\langle": "\\rangle",
      "\u27E8": "\u27E9", // langle, rangle
      "[": "]",
      "(": ")",
      "{": "}",
      "|": "|",
    };
    /**@type{Object.<string, string>} */
    this.rightLeftDelimiterPair = {
      "\\rangle": "\\langle",
      "\u27E9": "\u27E8", // rangle, langle
      "]": "[",
      ")": "(",
      "}": "{",
    };
    this.latexCommandsIgnored = {
      "displaystyle": true,
      "text": true,
      "mathrm": true,
      "phantom": true,
      "limits": true,
    };
    this.beginEndEnvironments = {
      "pmatrix": "pmatrix",
      "array": "array",
    };
    this.matrixEnder = {
      "\\end{pmatrix}": true,
      "\\end{array}": true,
    };
    /** @type{Object.<string, boolean>} */
    // The boolean indicates that the white space should be ignored.
    this.whiteSpaceCharactersIgnored = {
      "~": false,
      " ": true,
      "\t": true,
      "\u00A0": true,
      "\n": true,
    };
    this.whiteSpaceUtf16 = {
      "\u00A0": " ",
      "\u200A": " ",
      "\u200B": " ",
      "\u2009": " ",
    };
    /** @type{Object.<string, string>} */
    this.utf16ToLatexMap = null;

    /** @type{Object.<string, boolean>} */
    this.recognizedCommandsKeyInput = {
      "\\langle": true,
      "\\rangle": true,
      "\\matrix": true,
      "\\pmatrix": true,
      "\\cancel": true,
      "\\sqrt": true,
    };
    for (let operator in this.operatorWithSuperAndSubscriptBackslashed) {
      this.recognizedCommandsKeyInput[operator] = true;
    }
    for (let operator in this.operatorsWithSubscriptBackslashed) {
      this.recognizedCommandsKeyInput[operator] = true;
    }
    for (let operator in this.latexBackslashOperatorsBackslashed) {
      this.recognizedCommandsKeyInput[operator] = true;
    }
    for (let operator in this.latexBackslashAtomsEditableBackslashed) {
      this.recognizedCommandsKeyInput[operator] = true;
    }
  }

  computeUtf16ToLatexMap() {
    if (this.utf16ToLatexMap !== null) {
      return;
    }
    this.utf16ToLatexMap = {};
    for (let i = 32; i < 125; i++) {
      let character = String.fromCharCode(i);
      this.utf16ToLatexMap[character] = character;
    }
    // Non-breakable space.
    for (let key in this.whiteSpaceUtf16) {
      this.utf16ToLatexMap[key] = " ";
    }
    for (let key in this.latexBackslashAtomsEditable) {
      let current = this.latexBackslashAtomsEditable[key];
      this.utf16ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.latexBackslashOperators) {
      let current = this.latexBackslashOperators[key];
      this.utf16ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.operatorWithSuperAndSubscript) {
      let current = this.operatorWithSuperAndSubscript[key];
      this.utf16ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.operatorsWithSubscript) {
      let current = this.operatorsWithSubscript[key];
      this.utf16ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.mathcalEquivalents) {
      let current = this.mathcalEquivalents[key];
      this.utf16ToLatexMap[current] = `\\mathcal{${key}} `;
    }
    for (let key in this.mathbbEquivalents) {
      let current = this.mathbbEquivalents[key];
      this.utf16ToLatexMap[current] = `\\mathbb{${key}} `;
    }
    for (let key in this.operatorsFromUft8) {
      this.utf16ToLatexMap[key] = this.operatorsFromUft8[key];
    }
  }

  /** @returns {LatexWithAnnotation} */
  convertUtf16ToLatex(
    /** @type{string} */
    input,
    /**@type{number} */
    positionCaret,
  ) {
    this.computeUtf16ToLatexMap();
    let result = [];
    if (positionCaret === 0) {
      result.push("\\caret ");
    }
    for (let i = 0; i < input.length; i++) {
      let current = "";
      for (let j = 0; j < 4 && i + j < input.length; j++) {
        current += input[i + j];
        if (current in this.utf16ToLatexMap) {
          current = this.utf16ToLatexMap[current];
          i += j;
          result.push(current);
          current = "";
          break;
        }
      }
      if (current !== "") {
        result.push(current);
      }
      if (i + 1 === positionCaret) {
        result.push("\\caret ");
      }
    }
    return new LatexWithAnnotation(result.join(""));
  }

  isCharacterReplacingSelection(
    /**@type{string}*/
    input,
  ) {
    return input in this.characterReplacingSelection;
  }

  isLatinCharacter(
    /**@type{string}*/
    input,
  ) {
    return input in this.latinCharacters;
  }

  isDigit(
    /**@type{string}*/
    input,
  ) {
    return input in this.digits;
  }

  isWhiteSpace(
    /**@type{string} */ input,
  ) {
    return (input in this.whiteSpaceCharactersIgnored);
  }

  isWhiteSpaceIgnored(
    /**@type{string} */ input,
  ) {
    if (!(input in this.whiteSpaceCharactersIgnored)) {
      return false;
    }
    return this.whiteSpaceCharactersIgnored[input];
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
    /** @type{EquationEditor} */
    equationEditor,
    /** @type{string} */
    latex,
  ) {
    /** @type{EquationEditor} */
    this.equationEditor = equationEditor;
    /** @type{string} */
    this.latex = latex;
    /** @type {MathNode} */
    this.result = new MathNode(equationEditor, knownTypes.horizontalMath);
    /** @type{string[]} */
    this.words = [];
    /** @type{SyntancticElement[]} */
    this.parsingStack = [];
    /** @type {number} */
    // Dummy elements are prepended to the parsing stack  
    // so parsing rules can safely assume a minimal number 
    // of syntanctic elements. 
    this.dummyParsingElements = 6;
    /** @type{boolean} */
    this.debug = false;
    /** @type{HTMLElement[][]} */
    this.reductionLog = [];
    /** @type{string} */
    this.lastRuleName = "";
    /** @type{number} */
    this.startTime = 0;
    /** @type{boolean} */
    // Whether the special command \caret is found. 
    // Used to indicate the position of the 
    // caret when the latex is used in an editable box.
    this.caretFound = false;
  }

  initialize() {
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

  /** @returns{MathNode} */
  parse() {
    this.startTime = new Date().getTime();
    this.initialize();
    this.parseWords();
    this.result = new MathNode(this.equationEditor, knownTypes.horizontalMath);
    let startingNode = mathNodeFactory.horizontalMath(this.equationEditor, null);
    this.parsingStack.push(new SyntancticElement(startingNode, "", ""));
    for (let i = 0; i < this.words.length; i++) {
      if (!this.reduceStack(new SyntancticElement(null, this.words[i], ""))) {
        return this.constructError();
      }
    }
    if (!this.reduceStack(new SyntancticElement(null, "", "parsingEnd"))) {
      return this.constructError();
    }
    if (this.parsingStack.length !== this.dummyParsingElements + 2) {
      if (this.equationEditor.options.debugLogContainer !== null) {
        console.log(`Failed to parse ${this.latex}: not all syntactic elements were reduced.`);
      }
      return this.constructError();
    }
    let secondToLastElement = this.parsingStack[this.parsingStack.length - 2];
    if (secondToLastElement.node === null) {
      if (this.equationEditor.options.debugLogContainer !== null) {
        console.log(`Failed to parse ${this.latex}: final syntactic element is not a node.`);
      }
      return this.constructError();
    }
    let elapsedTime = new Date().getTime() - this.startTime;
    if (elapsedTime > 200 && this.equationEditor.options.logTiming) {
      console.log(`Parsing took too long: ${elapsedTime}ms`);
    }
    secondToLastElement.node.normalizeHorizontalMathRecursive();
    if (this.equationEditor.options.editable) {
      secondToLastElement.node.ensureEditableAtomsRecursive();
    }
    if (elapsedTime > 200 && this.equationEditor.options.logTiming) {
      console.log(`Normalization+parsing took too long: ${elapsedTime}ms`);
    }
    // let elapsedTime = new Date().getTime() - this.startTime;
    return secondToLastElement.node;
  }

  /** @returns{MathNode} */
  constructError() {
    let error = mathNodeFactory.error(this.equationEditor, this.latex);
    let left = mathNodeFactory.atom(this.equationEditor, "");
    let right = mathNodeFactory.atom(this.equationEditor, "");
    return mathNodeFactory.horizontalMathFromArray(this.equationEditor, [left, error, right]);
  }

  /** @returns{boolean} */
  reduceStack(
    /** @type {SyntancticElement} */
    syntacticElement,
  ) {
    let numberOfRuleApplications = 0;
    let startingLength = this.parsingStack.length;
    this.parsingStack.push(syntacticElement);
    if (this.debug) {
      this.reductionLog.push(this.toHtmlStackDebug("append"));
      this.lastRuleName = "";
    }
    while (this.applyOneRule()) {
      if (this.debug) {
        this.reductionLog.push(this.toHtmlStackDebug(this.lastRuleName));
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
    /** @type{number} */
    elementsToRemove,
  ) {
    this.parsingStack.length = this.parsingStack.length - elementsToRemove;
    return true;
  }

  /** @returns{boolean} */
  replaceParsingStackTop(
    /** @type{MathNode|null} */
    node,
    /** @type{string} */
    syntancticValue,
    /** @type{number} */
    indexToRemove,
  ) {
    return this.replaceParsingStackRange(node, syntancticValue, indexToRemove, - 1);
  }

  /** @returns{boolean} */
  replaceParsingStackRange(
    /** @type{MathNode|null} */
    node,
    /** @type{string} */
    syntancticRole,
    /** @type{number} */
    firstIndexToRemove,
    /** @type{number} */
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

  /** @returns{HtmlElement[]} */
  toHtmlStackDebug(
    /** @type{string} */
    ruleName,
  ) {
    let result = [];
    for (let i = this.dummyParsingElements; i < this.parsingStack.length; i++) {
      let currentElements = this.parsingStack[i].toHtmlDebugData();
      for (let j = 0; j < currentElements.length; j++) {
        result.push(currentElements[j]);
      }
    }
    result.push(document.createTextNode(` [${ruleName}]`));
    return result;
  }

  /** @returns{HtmlElement[]} */
  toHtmlDebug() {
    let result = [];
    for (let i = 0; i < this.reductionLog.length; i++) {
      for (let j = 0; j < this.reductionLog[i].length; j++) {
        result.push(this.reductionLog[i][j]);
      }
      result.push(document.createElement("hr"));
    }
    return result;
  }

  specialFont(
    /**@type{Object.<string, string>} */
    specialFontEquivalents,
  ) {
    let last = this.parsingStack[this.parsingStack.length - 1];

    if (latexConstants.isLatinCharacter(last.content)) {
      if (!(last.content in specialFontEquivalents)) {
        return false;
      }
      this.lastRuleName = "special font";
      let node = mathNodeFactory.atom(this.equationEditor, specialFontEquivalents[last.content]);
      return this.replaceParsingStackTop(node, "", -2);
    }
    if (!last.isExpression() || last.node.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    if (last.node.children.length !== 1) {
      return false;
    }
    let contentIfAtom = last.node.children[0].contentIfAtom();
    if (!(contentIfAtom in specialFontEquivalents)) {
      return false;
    }
    this.lastRuleName = "special font horizontal math";
    let node = mathNodeFactory.atom(this.equationEditor, specialFontEquivalents[contentIfAtom]);
    return this.replaceParsingStackTop(node, "", - 2);
  }

  /** @returns{boolean} */
  // Applies the first possible rule to the top of the parsing stack.
  applyOneRule() {
    let last = this.parsingStack[this.parsingStack.length - 1];
    if (latexConstants.isWhiteSpace(last.content)) {
      if (latexConstants.isWhiteSpaceIgnored(last.content)) {
        this.lastRuleName = "clean whitespace";
        this.parsingStack.length = this.parsingStack.length - 1;
        return true;
      } else {
        this.lastRuleName = "create whitespace";
        let node = mathNodeFactory.atom(this.equationEditor, " ");
        return this.replaceParsingStackTop(node, "", - 1);
      }
    }
    if (last.content in latexConstants.latexSyntacticValues) {
      this.lastRuleName = "built-in syntactic element";
      return this.replaceParsingStackTop(null, latexConstants.latexSyntacticValues[last.content], - 1);
    }

    let secondToLast = this.parsingStack[this.parsingStack.length - 2];
    if (secondToLast.syntacticRole === "\\") {
      if (last.syntacticRole === "\\") {
        this.lastRuleName = "double backslash";
        return this.replaceParsingStackTop(null, "\\\\", - 2);
      }
      if (last.syntacticRole === "{") {
        this.lastRuleName = "{ delimiter";
        return this.replaceParsingStackTop(null, "\\{", - 2);
      }
      if (last.syntacticRole === "}") {
        this.lastRuleName = "} delimiter";
        return this.replaceParsingStackTop(null, "\\}", - 2);
      }
    }
    let thirdToLast = this.parsingStack[this.parsingStack.length - 3];
    let fourthToLast = this.parsingStack[this.parsingStack.length - 4];
    if (secondToLast.syntacticRole === "{" && last.isExpression()) {
      if (last.node.type.type !== knownTypes.horizontalMath.type) {
        this.lastRuleName = "horizontal math after curly brace";
        let node = mathNodeFactory.horizontalMath(this.equationEditor, last.node);
        return this.replaceParsingStackTop(node, "", - 1);
      }
    }
    if (thirdToLast.syntacticRole === "{" && secondToLast.isExpression() && last.syntacticRole === "}") {
      this.lastRuleName = "remove curly braces";
      let node = secondToLast.node;
      if (node.type.type !== knownTypes.horizontalMath.type) {
        node = mathNodeFactory.horizontalMath(this.equationEditor, node);
      }
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.latexBackslashCommands) {
      this.lastRuleName = "latex command";
      return this.replaceParsingStackTop(null, latexConstants.latexBackslashCommands[last.content], - 2);
    }
    if (secondToLast.syntacticRole === "\\left" && last.syntacticRole in latexConstants.leftDelimiters) {
      this.lastRuleName = "\\left combined with left delimiter";
      this.parsingStack[this.parsingStack.length - 2] = last;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === "\\color" && last.isExpression()) {
      if (secondToLast.node === null) {
        this.lastRuleName = "set color";
        // We have a situation along the lines of \color{red} with last.node = red.
        // Write the last node as the node that determines the color.
        secondToLast.node = last.node;
        return this.decreaseParsingStack(1);
      } else {
        this.lastRuleName = "apply color";
        // We already have the color set along the lines of \\color{red}{last}
        // with secondToLast = \color{red} and secondToLast.node = red.
        let node = mathNodeFactory.genericMathBox(this.equationEditor, last.node);
        node.type.colorText = secondToLast.node.toLatex();
        return this.replaceParsingStackTop(node, "", - 2);
      }
    }
    if (secondToLast.syntacticRole === "\\mathbf" && last.isExpression()) {
      this.lastRuleName = "apply bold";
      // We already have the color set along the lines of \\color{red}{last}
      // with secondToLast = \color{red} and secondToLast.node = red.
      let node = mathNodeFactory.genericMathBox(this.equationEditor, last.node);
      node.type.fontWeight = "bold";
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (secondToLast.syntacticRole === "\\right" && last.syntacticRole in latexConstants.rightDelimiters) {
      this.lastRuleName = "\\right combined with right delimiter";
      this.parsingStack[this.parsingStack.length - 2] = last;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === "\\left" && last.content === ".") {
      return this.replaceParsingStackTop(null, "\\left.", - 2);
    }
    if (secondToLast.syntacticRole === "\\right" && last.content === ".") {
      return this.replaceParsingStackTop(null, "\\right.", - 2);
    }
    if (secondToLast.syntacticRole === "\\mathcal") {
      if (this.specialFont(latexConstants.mathcalEquivalents)) {
        return true;
      }
    }
    if (secondToLast.syntacticRole === "\\mathfrak") {
      if (this.specialFont(latexConstants.mathfrakEquivalents)) {
        return true;
      }
    }
    if (secondToLast.syntacticRole === "\\mathbb") {
      if (this.specialFont(latexConstants.mathbbEquivalents)) {
        return true;
      }
    }
    if (last.content in latexConstants.latexCommandsIgnored && secondToLast.syntacticRole === "\\") {
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.operatorWithSuperAndSubscript) {
      this.lastRuleName = "operator with superscript and subscript";
      let operatorSymbol = latexConstants.operatorWithSuperAndSubscript[last.content];
      let syntacticElement = new SyntancticElement(null, operatorSymbol, "operatorWithSuperAndSubscript");
      this.parsingStack[this.parsingStack.length - 2] = syntacticElement;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.operatorsWithSubscript) {
      this.lastRuleName = "operator with subscript";
      let operatorSymbol = latexConstants.operatorsWithSubscript[last.content];
      let syntacticElement = new SyntancticElement(null, operatorSymbol, "operatorWithSubscript");
      this.parsingStack[this.parsingStack.length - 2] = syntacticElement;
      return this.decreaseParsingStack(1);
    }
    if (
      secondToLast.syntacticRole === "\\overline" &&
      last.isExpression()
    ) {
      this.lastRuleName = "over-line";
      let node = mathNodeFactory.overLine(this.equationEditor, last.node);
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (
      fourthToLast.syntacticRole === "\\overbrace" &&
      thirdToLast.isExpression() &&
      secondToLast.syntacticRole === "^" &&
      last.isExpression()
    ) {
      this.lastRuleName = "over-brace";
      let node = mathNodeFactory.overBrace(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 4);
    }
    let fifthToLast = this.parsingStack[this.parsingStack.length - 5];
    if (
      fifthToLast.syntacticRole === "operatorWithSuperAndSubscript" &&
      fourthToLast.syntacticRole === "^" &&
      thirdToLast.isExpression() &&
      secondToLast.syntacticRole === "_" &&
      last.isExpression()
    ) {
      this.lastRuleName = "operatorWithSuperAndSubscript combines with super and subscript.";
      let node = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, fifthToLast.content, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 5);
    }
    if (
      fifthToLast.syntacticRole === "operatorWithSuperAndSubscript" &&
      fourthToLast.syntacticRole === "_" &&
      thirdToLast.isExpression() &&
      secondToLast.syntacticRole === "^" &&
      last.isExpression()
    ) {
      this.lastRuleName = "operatorWithSuperAndSubscript combines with sub and superscript.";
      let node = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, fifthToLast.content, last.node, thirdToLast.node);
      return this.replaceParsingStackTop(node, "", - 5);
    }
    if (
      secondToLast.syntacticRole === "operatorWithSuperAndSubscript" &&
      last.syntacticRole !== "^" &&
      last.syntacticRole !== "_" &&
      last.syntacticRole !== "\\"
    ) {
      this.lastRuleName = "operator standalone";
      let node = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, secondToLast.content, null, null);
      return this.replaceParsingStackRange(node, "", - 2, - 2);
    }
    if (
      fourthToLast.syntacticRole === "operatorWithSuperAndSubscript" &&
      thirdToLast.syntacticRole === "^" &&
      secondToLast.isExpression() &&
      last.syntacticRole !== "_"
    ) {
      this.lastRuleName = "operator with superscript only";
      let node = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, fourthToLast.content, secondToLast.node, null);
      return this.replaceParsingStackRange(node, "", - 4, - 2);
    }
    if (
      fourthToLast.syntacticRole === "operatorWithSuperAndSubscript" &&
      thirdToLast.syntacticRole === "_" &&
      secondToLast.isExpression() &&
      last.syntacticRole !== "^"
    ) {
      this.lastRuleName = "operator with subscript only (allows superscript)";
      let node = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, fourthToLast.content, null, secondToLast.node);
      return this.replaceParsingStackRange(node, "", - 4, - 2);
    }
    if (
      thirdToLast.syntacticRole === "operatorWithSubscript" &&
      secondToLast.syntacticRole === "_" &&
      last.isExpression()
    ) {
      this.lastRuleName = "operator with subscript only";
      let node = mathNodeFactory.operatorWithSubscript(this.equationEditor, thirdToLast.content, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.latexBackslashOperators) {
      this.lastRuleName = "atom immutable from backslash";
      let node = mathNodeFactory.atomImmutable(
        this.equationEditor,
        latexConstants.latexBackslashOperators[last.content],
      );
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (secondToLast.syntacticRole === "\\" && last.content in latexConstants.latexBackslashAtomsEditable) {
      this.lastRuleName = "latex symbol";
      let node = mathNodeFactory.atom(
        this.equationEditor,
        latexConstants.latexBackslashAtomsEditable[last.content],
      );
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (last.syntacticRole === "\\caret") {
      this.lastRuleName = "caret location";
      let node = mathNodeFactory.atom(this.equationEditor, "");
      node.desiredCaretPosition = 0;
      return this.replaceParsingStackTop(node, "", - 1);
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
      this.lastRuleName = "begin pmatrix to matrix builder";
      let matrix = mathNodeFactory.matrix(this.equationEditor, 1, 0, "");
      return this.replaceParsingStackTop(matrix, "matrixBuilder", - 1);
    }
    if (
      fourthToLast.syntacticRole === "\\begin{array}" &&
      thirdToLast.syntacticRole === "{" &&
      secondToLast.isExpression() &&
      last.syntacticRole === "|"
    ) {
      if (secondToLast.node.type.type === knownTypes.horizontalMath.type) {
        secondToLast.node.appendChild(mathNodeFactory.atom(this.equationEditor, last.syntacticRole));
        return this.decreaseParsingStack(1);
      }
    }
    if (secondToLast.syntacticRole === "\\begin{array}" && last.isExpression()) {
      this.lastRuleName = "begin array to matrix builder";
      let matrix = mathNodeFactory.matrix(this.equationEditor, 1, 0, last.node.toLatex());
      return this.replaceParsingStackTop(matrix, "matrixBuilder", - 2);
    }
    if (thirdToLast.syntacticRole === "matrixBuilder" && secondToLast.isExpression() && last.syntacticRole === "&") {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      thirdToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === "matrixBuilder" && last.syntacticRole === "&") {
      // Modify secondToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      this.lastRuleName = "matrix builder ampersand";
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, mathNodeFactory.atom(this.equationEditor, ""));
      secondToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(1);
    }
    if (last.syntacticRole === "\\\\" && secondToLast.isExpression() && thirdToLast.syntacticRole === "matrixBuilder") {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      this.lastRuleName = "matrix builder expression double backslash";
      let lastRow = thirdToLast.node.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
      return this.decreaseParsingStack(2);
    }
    if (last.syntacticRole === "\\\\" && secondToLast.syntacticRole === "matrixBuilder") {
      // Modify secondToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic comoplexity.
      this.lastRuleName = "matrix builder double backslash";
      let lastRow = secondToLast.node.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, mathNodeFactory.atom(this.equationEditor, ""));
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
      return this.decreaseParsingStack(1);
    }
    if (last.syntacticRole === "\\\\") {
      // Double backslah indicates a line break, which we choose to ignore.
      this.lastRuleName = "remove double backslash";
      return this.decreaseParsingStack(1);
    }
    if (thirdToLast.syntacticRole === "matrixBuilder" && secondToLast.isExpression() && last.isMatrixEnder()) {
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, secondToLast.node);
      thirdToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      // Normalize the matrix: ensure all rows have same number of columns, no last empty row, etc.
      thirdToLast.node.normalizeMatrix();
      (new LatexColumnStyleIterator(thirdToLast.node.latexExtraStyle)).applyStyleToMatrix(thirdToLast.node);
      // Mark the matrix as a regular expression.
      thirdToLast.syntacticRole = "";
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === "matrixBuilder" && last.isMatrixEnder()) {
      this.lastRuleName = "finish matrix";
      // Normalize the matrix: ensure all rows have same number of columns, no last empty row, etc.
      secondToLast.node.normalizeMatrix();
      (new LatexColumnStyleIterator(secondToLast.node.latexExtraStyle)).applyStyleToMatrix(secondToLast.node);
      // Mark the matrix as a regular expression.
      secondToLast.syntacticRole = "";
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === "\\sqrt" && last.isExpression()) {
      this.lastRuleName = "Square root";
      let node = mathNodeFactory.sqrt(this.equationEditor, last.node, secondToLast.node);
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (secondToLast.syntacticRole === "\\cancel" && last.isExpression()) {
      this.lastRuleName = "cancel expression";
      let node = mathNodeFactory.cancel(this.equationEditor, last.node);
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (
      secondToLast.syntacticRole === "\\formInput" &&
      last.isExpression() &&
      !this.equationEditor.options.editable
    ) {
      this.lastRuleName = "form input";
      let node = mathNodeFactory.formInput(this.equationEditor, last.node.toLatex());
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (secondToLast.syntacticRole === "{" && last.syntacticRole === "}") {
      this.lastRuleName = "{} to empty atom";
      let node = mathNodeFactory.atom(this.equationEditor, "");
      return this.replaceParsingStackTop(node, "", -2);
    }
    if (
      fourthToLast.syntacticRole === "\\sqrt" &&
      fourthToLast.node === null &&
      thirdToLast.syntacticRole === "[" &&
      secondToLast.isExpression() &&
      last.syntacticRole === "]"
    ) {
      this.lastRuleName = "nth radical";
      fourthToLast.node = secondToLast.node;
      return this.decreaseParsingStack(3);
    }
    if (
      secondToLast.isExpression()
      &&
      this.areMatchingDelimiters(thirdToLast, last)
    ) {
      this.lastRuleName = "parenthetic expression to expression";
      let leftDelimiter = latexConstants.leftDelimiters[thirdToLast.syntacticRole];
      let rightDelimiter = latexConstants.rightDelimiters[last.syntacticRole];
      let left = mathNodeFactory.leftDelimiter(this.equationEditor, leftDelimiter, false);
      let right = mathNodeFactory.rightDelimiter(this.equationEditor, rightDelimiter, false);
      let horizontal = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [left, secondToLast.node, right]);
      return this.replaceParsingStackTop(horizontal, "", - 3);
    }
    if (
      secondToLast.syntacticRole in latexConstants.leftDelimiters &&
      last.syntacticRole in latexConstants.rightDelimiters
    ) {
      this.lastRuleName = "parenthetic expression to expression";
      let leftDelimiter = latexConstants.leftDelimiters[secondToLast.syntacticRole];
      let rightDelimiter = latexConstants.rightDelimiters[last.syntacticRole];
      let left = mathNodeFactory.leftDelimiter(this.equationEditor, leftDelimiter, false);
      let right = mathNodeFactory.rightDelimiter(this.equationEditor, rightDelimiter, false);
      let atom = mathNodeFactory.atom(this.equationEditor, "\u200B");
      let horizontal = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [left, atom, right]);
      return this.replaceParsingStackTop(horizontal, "", - 2);
    }
    if (last.isExpression() && secondToLast.syntacticRole === "_" && thirdToLast.isExpression()) {
      this.lastRuleName = "make subscript";
      let node = mathNodeFactory.baseWithSubscript(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }

    if (thirdToLast.syntacticRole === "\\frac" && secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.fraction(this.equationEditor, secondToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (
      last.isExpression() &&
      secondToLast.isExpression() &&
      latexConstants.isDigit(last.node.contentIfAtom())
    ) {
      if (secondToLast.node.type.type === knownTypes.horizontalMath.type && secondToLast.node.children.length > 0) {
        if (latexConstants.isDigit(secondToLast.node.children[secondToLast.node.children.length - 1].contentIfAtom())) {
          this.lastRuleName = "merge digits into horizontal math";
          secondToLast.node.appendChild(last.node);
          return this.decreaseParsingStack(1);
        }
      }
    }
    if (
      last.isExpression() &&
      secondToLast.isExpression() &&
      latexConstants.isDigit(last.node.contentIfAtom()) &&
      latexConstants.isDigit(secondToLast.node.contentIfAtom())
    ) {
      this.lastRuleName = "merge two digits";
      let node = mathNodeFactory.horizontalMath(this.equationEditor, secondToLast.node);
      node.appendChild(last.node);
      return this.replaceParsingStackTop(node, "", - 2);
    }
    if (thirdToLast.isExpression() && secondToLast.syntacticRole === "^" && last.isExpression()) {
      this.lastRuleName = "make exponent";
      let node = mathNodeFactory.baseWithExponent(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (thirdToLast.syntacticRole === "\\binom" && secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.matrix(this.equationEditor, 2, 1, "");
      node.getMatrixCell(0, 0).children[0].appendChild(secondToLast.node);
      node.getMatrixCell(1, 0).children[0].appendChild(last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (thirdToLast.syntacticRole === "\\stackrel" && secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.matrix(this.equationEditor, 3, 1, "");
      node.getMatrixCell(0, 0).children[0].appendChild(secondToLast.node);
      node.getMatrixCell(1, 0).children[0].appendChild(last.node);
      node.getMatrixCell(2, 0).children[0].appendChild(mathNodeFactory.atom(this.equationEditor, "\u00A0"));
      node.children[0].replaceChildAtPosition(0, mathNodeFactory.leftDelimiter(this.equationEditor, "", false));
      node.children[0].replaceChildAtPosition(2, mathNodeFactory.rightDelimiter(this.equationEditor, "", false));
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
    if (thirdToLast.isExpression() && secondToLast.isExpression() && last.syntacticRole !== "^" && last.syntacticRole !== "_") {
      // Absorb atom / immutable atom to preceding horizontal math.
      if (thirdToLast.node.type.type === knownTypes.horizontalMath.type) {
        this.lastRuleName = "merge node into horizontal math";
        thirdToLast.node.appendChild(secondToLast.node);
        return this.replaceParsingStackRange(thirdToLast.node, "", - 3, - 2);
      } else {
        this.lastRuleName = "merge expressions nodes into horizontal math";
        let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [thirdToLast.node, secondToLast.node]);
        return this.replaceParsingStackRange(node, "", - 3, - 2);
      }
    }
    return false;
  }

  /**@returns{boolean} */
  areMatchingDelimiters(
    /**@type{SyntancticElement} */
    left,
    /**@type{SyntancticElement} */
    right,
  ) {
    if (
      !(left.syntacticRole in latexConstants.leftDelimiters) ||
      !(right.syntacticRole in latexConstants.rightDelimiters)
    ) {
      return false;
    }
    if (left.syntancticRole === "|" || right.syntacticRole === "|") {
      return left.syntacticRole === right.syntacticRole;
    }
    return true;
  }
}

class EquationEditorOptions {
  constructor(
    /** 
     * @type{{
     * editable?:boolean,
     * removeDisplayStyle?:boolean,
     * sanitizeLatexSource?:boolean,
     * debugLogContainer?:HTMLElement|null,
     * latexInput?:HTMLElement|null,
     * editHandler?:Function|null,
     * lineBreakWidth?: boolean,
     * }} */
    options,
  ) {
    /** @type{boolean} */
    this.editable = options.editable;
    /** @type{boolean} */
    this.removeDisplayStyle = options.removeDisplayStyle;
    /** @type{boolean} */
    this.sanitizeLatexSource = options.sanitizeLatexSource;
    /**@type{HTMLElement|null} */
    this.debugLogContainer = options.debugLogContainer;
    /**@type{HTMLElement|null} */
    this.svgContainer = options.svgContainer;
    /**@type{HTMLElement|null} */
    this.latexInput = options.latexInput;
    /** @type{boolean} */
    this.logTiming = options.logTiming;
    /**@type{number} */
    this.lineBreakWidth = options.lineBreakWidth;
    if (this.lineBreakWidth === null || this.lineBreakWidth === undefined) {
      this.lineBreakWidth = 0;
    }
    /**
     * @type{Function|null} 
     * Called on modification of the editor. 
     * Will provide two arguments: 
     * the editor and the MathNode being edited.
    */
    this.editHandler = options.editHandler;
    if (this.svgContainer === undefined) {
      this.svgContainer = null;
    }
    if (this.editable === undefined) {
      this.editable = true;
    }
    if (this.removeDisplayStyle === undefined) {
      this.removeDisplayStyle = false;
    }
    if (this.sanitizeLatexSource === undefined) {
      this.sanitizeLatexSource = false;
    }
    if (this.debugLogContainer === undefined) {
      this.debugLogContainer = null;
    }
    if (this.latexInput === undefined) {
      this.latexInput = null;
    }
    if (this.editHandler === undefined) {
      this.editHandler = null;
    }
    if (this.logTiming === undefined) {
      this.logTiming = false;
    }
    /** @type{boolean} */
    this.showLatexOnDoubleClick = !this.editable;
  }
}

class KeyHandlerResult {
  constructor(
    /**@type{boolean} */
    preventDefault,
    /**@type{boolean} */
    updateAlignment,
  ) {
    this.preventDefault = preventDefault;
    this.updateAlignment = updateAlignment;
  }
}

class CircularBuffer {
  constructor(
    /**@type{number} */
    capacity,
  ) {
    /**@type{string[]} */
    this.elements = [];
    /**@type{number} */
    this.lowestIndex = 0;
    /**@type{number} */
    this.highestIndex = - 1;
    /**@type{number} */
    this.capacity = capacity;
  }

  push(
    /**@type{string} */
    input,
  ) {
    this.highestIndex++;
    if (this.totalElements() > this.capacity) {
      this.lowestIndex++;
    }
    this.setElement(this.highestIndex, input);
  }

  pop() {
    if (this.totalElements() <= 0) {
      return null;
    }
    let circularIndex = this.indexToCircular(this.highestIndex);
    let result = this.elements[circularIndex];
    this.highestIndex--;
    return result;
  }

  setElement(
    /**@type{number} */
    index,
    /**@type{string} */
    input,
  ) {
    let circularIndex = this.indexToCircular(index);
    for (let i = this.elements.length; i <= circularIndex; i++) {
      this.elements.push("");
    }
    this.elements[circularIndex] = input;
  }

  indexToCircular(
    /**@type{number} */
    index,
  ) {
    return index % this.capacity;
  }

  /**@returns{string|null} */
  get(
    /**@type{number} */
    index,
  ) {
    if (index > this.highestIndex || index < this.lowestIndex) {
      return null;
    }
    let circularIndex = this.indexToCircular(index);
    if (circularIndex >= this.elements.length) {
      return null;
    }
    return this.elements[circularIndex];
  }

  /**@returns{number} */
  totalElements() {
    return this.highestIndex - this.lowestIndex + 1;
  }

  /**@returns{string} */
  toString() {
    let elementsToShowAtEachEnd = 2;
    if (this.totalElements() <= elementsToShowAtEachEnd * 2) {
      let result = [];
      for (let i = this.lowestIndex; i <= this.highestIndex; i++) {
        result.push(this.get(i));
      }
      return result.join(", ");
    }
    let resultLeft = [];
    for (let i = this.lowestIndex; i < this.lowestIndex + elementsToShowAtEachEnd; i++) {
      resultLeft.push(this.get(i));
    }
    let resultRight = [];
    for (let i = this.highestIndex - elementsToShowAtEachEnd + 1; i <= this.highestIndex; i++) {
      resultRight.push(this.get(i));
    }
    let left = resultLeft.join(", ");
    let right = resultRight.join(", ");
    let omitted = this.totalElements() - 2 * elementsToShowAtEachEnd;
    return `${left} ...(${omitted} omitted)...${right}`;
  }
}

class FocusInformation {
  constructor() {
    /** @type {MathNode|null} Used to write to the last focused node.*/
    this.lastFocused = null;
    /** @type{MathNodeWithCaretPosition} */
    this.lastSelectionStart = new MathNodeWithCaretPosition(null, -1);
    /** @type{MathNodeWithCaretPosition} */
    this.lastSelectionStartExpanded = new MathNodeWithCaretPosition(null, - 1);
    /** @type{MathNodeWithCaretPosition} */
    this.lastSelectionEnd = new MathNodeWithCaretPosition(null, -1);
    /** @type{MathNodeWithCaretPosition} */
    this.lastSelectionEndExpanded = new MathNodeWithCaretPosition(null, -1);
  }

  setLastFocused(
    /**@type{MathNode|null} */
    lastFocused,
  ) {
    this.lastFocused = lastFocused;
  }

  setLastSelection(
    /**@type{EquationEditor} */
    editor,
  ) {
    this.lastSelectionStart.assign(editor.selectionStart);
    this.lastSelectionStartExpanded.assign(editor.selectionStartExpanded);
    this.lastSelectionEnd.assign(editor.selectionEnd);
    this.lastSelectionEndExpanded.assign(editor.selectionEndExpanded);
  }

  restoreSelection(
    /**@type{EquationEditor} */
    editor,
  ) {
    editor.selectionStart.assign(this.lastSelectionStart);
    editor.selectionStartExpanded.assign(this.lastSelectionStartExpanded);
    editor.selectionEnd.assign(this.lastSelectionEnd);
    editor.selectionEndExpanded.assign(this.lastSelectionEndExpanded);
  }

  /**@returns {HTMLElement[]} */
  toHtmlDebugData() {
    let result = [];
    if (this.lastFocused === null) {
      return result;
    }
    result.push(document.createElement("br"));
    result.push(document.createTextNode(
      `Last focused: ${this.lastFocused.toString()}, position: ${this.lastFocused.positionCaretBeforeKeyEvents}`
    ));
    if (this.lastFocused.isDetached()) {
      result.push(document.createElement("br"));
      result.push(boldElement("Detached last focused.", "red"));
    }
    return result;
  }
}

class EquationEditor {
  constructor(
    /** @type{HTMLElement} */
    containerGiven,
    /** @type{EquationEditorOptions|null} */
    options,
  ) {
    /** @type{HTMLElement} */
    this.container = containerGiven;
    /** @type{HTMLElement|null} */
    this.latexContainer = null;
    this.container.style.display = "inline-block";
    this.container.style.position = "relative";
    this.container.textContent = "";
    /** @type{EquationEditorOptions} */
    this.options = new EquationEditorOptions({});
    if (options !== null && options !== undefined) {
      this.options = options;
    }
    this.rootNode = mathNodeFactory.rootMath(this);
    if (!this.options.editable) {
      this.rootNode.type.borderStyle = "";
      this.rootNode.type.padding = "";
      this.rootNode.type.margin = "";
      this.rootNode.type.cursor = "";
      this.rootNode.type.minWidth = "";
    } else {
      this.container.style.margin = "2px";
      this.container.style.padding = "2px";
    }
    /**@type{boolean} whether there's a pending writeLatex event to be trigerred when the event is visible. */
    this.pendingWriteLatex = false;
    /**@type{IntersectionObserver|null} */
    this.observer = null;
    /**@type{boolean} */
    this.preventEditorBlur = false;
    /**@type{boolean} */
    this.mouseSelectionStarted = false;
    /**@type{boolean} */
    this.selectionNoMoreDefault = false;
    /**@type{boolean} */
    this.mouseIgnoreNextClick = false;
    /** @type {boolean} */
    this.mouseSelectionVisible = false;
    /** @type{MathNodeWithCaretPosition} */
    this.selectionStart = new MathNodeWithCaretPosition(null, -1);
    /** @type{MathNodeWithCaretPosition} */
    this.selectionStartExpanded = new MathNodeWithCaretPosition(null, - 1);
    /** @type{MathNodeWithCaretPosition} */
    this.selectionEnd = new MathNodeWithCaretPosition(null, -1);
    /** @type{MathNodeWithCaretPosition} */
    this.selectionEndExpanded = new MathNodeWithCaretPosition(null, -1);
    /** @type{string} */
    this.latexLastWritten = "";
    /** @type{string} */
    this.latexLastEdit = "";
    /** @type{string} */
    this.latexLastEditWithCaret = "";
    /** @type{CircularBuffer} */
    this.history = new CircularBuffer(5000);
    /** @type{string[]} */
    this.redoBuffer = [];
    /** @type {boolean}*/
    this.backslashSequenceStarted = false;
    /** @type {string}*/
    this.backslashSequence = "";
    /** @type {number} */
    this.backslashPosition = - 1;
    /**@type {string} */
    this.positionDebugString = "";
    /** @type {string} */
    this.fontFamily = "";
    /** @type {boolean} */
    this.hasFocusDOM = false;
    /**@type{boolean} If changing this.container's font size, set this to true before the change, and back to false after.*/
    this.resizingEditorFont = false;
    if (this.options.latexInput !== null) {
      this.options.latexInput.addEventListener('keyup', (e) => {
        this.writeLatexFromInput();
      });
    }
    this.focusInformation = new FocusInformation();
    /** @type{string} */
    this.lastSelectionAction = "";
    /** @type{number} */
    this.standardAtomHeight = 0;
    /** @type{string} */
    this.lastCopied = "";
    /** @type{MathNode|null} */
    this.eventCatcher = null;
    this.prepareEventCatcher();
  }

  getFontFamily() {
    if (this.fontFamily === "") {
      this.fontFamily = this.container.style.fontFamily;
    }
    return this.fontFamily;
  }

  focusEventCatcher() {
    if (this.eventCatcher.element === null) {
      return false;
    }
    this.eventCatcher.element.focus();
    return true;
  }

  focusRestore() {
    if (!this.rootNode.focusRestore()) {
      this.resetSelectionFocusBestChoice();
    }
  }
  undo() {
    if (this.history.totalElements() <= 0) {
      return;
    }
    this.redoBuffer.push(this.latexLastEditWithCaret);
    let element = this.history.pop();
    this.writeLatex(element);
    this.latexLastEditWithCaret = element;
    this.latexLastEdit = this.rootNode.toLatexWithAnnotation(null).latex;
    this.focusRestore();
  }

  redo() {
    if (this.redoBuffer.length === 0) {
      return;
    }
    let element = this.redoBuffer.pop();
    this.writeLatex(element);
    this.writeLatexToInput(false);
    this.focusRestore();
  }

  /**@returns{KeyHandlerResult} */
  pasteFromClipboard() {
    if (!this.hasSelection() || !this.selectionEscapedOriginalAtom()) {
      return new KeyHandlerResult(false, false);
    }
    navigator.clipboard.readText().then((text) => {
      this.deleteSelection(text);
    });
    return new KeyHandlerResult(true, true);
  }

  copyToClipboard() {
    let toBeCopied = "";
    if (this.selectionEndExpanded.element === null || this.selectionStartExpanded.element === null) {
      toBeCopied = this.rootNode.toLatexWithAnnotation(null).latex;
    } else {
      let left = this.selectionStartExpanded.element;
      let right = this.selectionEndExpanded.element;
      if (right.isToTheLeftOf(left)) {
        let copy = right;
        right = left;
        left = copy;
      }
      left = left.beefUpToHorizontalParent();
      right = right.beefUpToHorizontalParent();
      /**@type {string[]} */
      let latexContent = [];
      /**@type {MathNode} */
      let current = left;
      for (; ;) {
        latexContent.push(current.toLatexWithAnnotation(null).latex);
        if (current === right) {
          break;
        }
        current = current.horizontalSibling(1);
        if (current === null) {
          break;
        }
      }
      toBeCopied = latexContent.join("");
    }
    this.lastCopied = toBeCopied;
    navigator.clipboard.writeText(toBeCopied);
    this.writeDebugInfo(null);
  }

  computeStandardAtomHeight() {
    // In firefox, empty space can be interpretted to have zero height; 
    // not so for non-breaking space.
    this.eventCatcher.initialContent = "\u200A";
    this.eventCatcher.element = document.createElement("div");
    this.eventCatcher.element.textContent = this.eventCatcher.initialContent;
    this.container.appendChild(this.eventCatcher.element);
    let boundingBox = this.eventCatcher.element.getBoundingClientRect();
    this.standardAtomHeight = boundingBox.height;
    this.container.removeChild(this.eventCatcher.element);
    this.eventCatcher.element = null;
  }

  prepareEventCatcher() {
    if (!this.options.editable) {
      return;
    }
    this.eventCatcher = new MathNode(this, knownTypes.eventCatcher);
    this.computeStandardAtomHeight();
    this.eventCatcher.createDOMElementIfMissing();
    this.container.appendChild(this.eventCatcher.element);
    this.eventCatcher.element.style.position = "absolute";
    this.eventCatcher.element.style.left = 0;
    this.eventCatcher.element.style.top = 0;
    this.eventCatcher.element.addEventListener("copy", (_) => {
      this.copyToClipboard();
    });
    this.eventCatcher.element.addEventListener("paste", (e) => {
      this.pasteFromClipboard(e);
    });
    this.eventCatcher.element.addEventListener("keydown", (e) => {
      this.handleKeyDownCatchAll(e);
    });
    this.container.addEventListener("keydown", (e) => {
      this.handleKeyDownCatchAll(e);
    });
    this.eventCatcher.element.addEventListener("blur", (e) => {
      this.blur();
    });
  }

  /** Removes all ranges from the window selection. */
  resetSelectionDOM() {
    try {
      window.getSelection().removeAllRanges();
    } catch (e) {
      console.log(`Failed to remove all ranges ${e}`);
    }
  }

  resetSelectionFullSelectEventCatcher() {
    this.resetSelectionLeaveRangesIntact();
    this.resetSelectionDOMSelectEventCatcher();
  }

  /** Removes all ranges from the window selection. */
  resetSelectionDOMSelectEventCatcher() {
    this.resetSelectionDOM();
    let range = document.createRange();
    range.setStart(this.eventCatcher.element, 0);
    range.setEnd(this.eventCatcher.element, 0);
    window.getSelection().addRange(range);
    this.eventCatcher.focus(1);
  }

  storeSelection() {
    this.focusInformation.setLastSelection(this);
  }

  updateDOM() {
    this.rootNode.updateDOM();
  }

  writeSVG() {
    if (this.options.svgContainer === null) {
      return;
    }
    this.options.svgContainer.textContent = "";
    let graphicsWriter = new ScalableVectorGraphicsWriter();
    this.options.svgContainer.appendChild(graphicsWriter.typeset(this.rootNode));
  }

  writeDebugInfo(
    /** @type{LaTeXParser|null} */ parser,
  ) {
    if (this.options.debugLogContainer === null) {
      return;
    }
    let result = [];
    if (parser !== null) {
      let syntacticStack = parser.toHtmlDebug();
      for (let i = 0; i < syntacticStack.length; i++) {
        result.push(syntacticStack[i]);
      }
      result.push(document.createElement("hr"));
    }
    let debugPart = this.toHtmlDebugData();
    for (let i = 0; i < debugPart.length; i++) {
      result.push(debugPart[i]);
    }
    this.options.debugLogContainer.textContent = "";
    for (let i = 0; i < result.length; i++) {
      this.options.debugLogContainer.appendChild(result[i]);
    }
  }

  writeLatexToInput(
    /**@type{boolean} */
    wipeOffRedoBuffer,
  ) {
    if (this.backslashSequenceStarted) {
      return;
    }
    let latexWithAnnotationNoCaret = this.rootNode.toLatexWithAnnotation(null);
    if (this.latexLastEdit === latexWithAnnotationNoCaret.latex) {
      return;
    }
    if (wipeOffRedoBuffer) {
      this.redoBuffer = [];
    }
    this.history.push(this.latexLastEditWithCaret);
    this.latexLastEditWithCaret = this.rootNode.toLatexWithAnnotation(latexOptionsWithCaret).latex;
    this.latexLastEdit = latexWithAnnotationNoCaret.latex;
    if (this.options.latexInput !== null) {
      this.options.latexInput.value = latexWithAnnotationNoCaret.latex;
    }
  }

  writeLatexFromInput() {
    if (this.options.latexInput === null) {
      return;
    }
    let inputContent = this.options.latexInput.value;
    this.writeLatex(inputContent);
  }

  writeLatex(
    /**@type {string} */
    latex,
  ) {
    if (this.container === null) {
      throw "Attempt to write to non-initialized equation editor.";
    }
    if (this.observer === null) {
      // Please note: Intersection observer is a recent addition to web browsers.
      // It "fires events" when the visibility of an element changes. 
      // The purpose of this snippet is the following. 
      // Suppose we want to write latex to an element that is nested in another element
      // with visibility:none. Then the getBoundingClientRect() bounding rectangles 
      // of all atoms in our editor will have zero dimensions, thus throwing off the 
      // typesetting computation. 
      // So, we need to postpone the writeLatex() function to the time when 
      // the element comes visible, hence the observer below.
      // Please note: common sense suggests that this.container removed from the DOM tree, 
      // both this.container and the observer below will be garbage-collected.
      // However, this has not been 100% tested to be the case. 
      // Since in most pages this.container will not be removed from the DOM tree, 
      // it seems that the question of garbage collection correctness 
      // can be ignored.
      this.observer = new IntersectionObserver((unused1, unused2) => {
        this.writeLatexPartTwo(latex);
      }, {
        root: document.documentElement,
      });
      this.observer.observe(this.container);
    }
    this.pendingWriteLatex = true;
    if (this.container.style.minWidth === "") {
      this.container.style.minWidth = "1px";
    }
    this.writeLatexPartTwo(latex);
  }

  /**@returns{boolean} */
  writeLatexPartTwo(
    /**@type {string} */
    latex,
  ) {
    if (this.pendingWriteLatex !== true) {
      return;
    }
    let boundingRectangle = this.container.getBoundingClientRect();
    if (boundingRectangle.width === 0) {
      return false;
    }
    this.pendingWriteLatex = false;
    this.latexLastWritten = latex;
    this.rootNode.removeAllChildren();
    if (this.rootNode.element !== null) {
      this.rootNode.element.textContent = "";
    }
    let parser = new LaTeXParser(this, latex);
    if (this.options.debugLogContainer !== null) {
      parser.debug = true;
    }
    let newContent = parser.parse();
    if (newContent === null) {
      this.writeDebugInfo(parser);
      console.log(`Failed to construct node from your input ${latex}.`);
      return;
    }
    this.rootNode.appendChild(newContent);
    this.updateDOM();
    this.updateAlignment();
    this.writeDebugInfo(parser);
    this.container.setAttribute("latexsource", latex);
    this.setLastFocused(this.rootNode.rightmostAtomChild());
    return true;
  }

  writeLatexLastFocused(
    /**@type {string} */
    latex,
  ) {
    if (this.focusInformation.lastFocused === null) {
      let toWriteTo = this.rootNode.rightmostAtomChild();
      if (toWriteTo === null) {
        console.log("Unexpected failure to find atom child.");
        return;
      }
      toWriteTo.positionCaretBeforeKeyEvents = toWriteTo.textContentOrInitialContent().length;
      toWriteTo.writeLatex(latex);
      return;
    }
    let lastFocusedReference = this.focusInformation.lastFocused;
    // Ensure reference to last modified is wiped early.
    this.focusInformation.lastFocused = null;
    lastFocusedReference.writeLatex(latex);
  }

  accountFrameTime(
    /**@type{number} */
    frameStart,
  ) {
    let now = new Date().getTime();
    let elapsedTime = now - frameStart;
    if (elapsedTime > 100) {
      console.log(`Warning: last equation editor frame took a full ${elapsedTime}ms.`);
    }
  }

  setLastFocused(
    /**@type{MathNode|null} */
    lastFocused,
  ) {
    this.focusInformation.setLastFocused(lastFocused);
  }

  /**@returns {HTMLElement[]} */
  toHtmlDebugData() {
    let latexWithAnnotation = this.rootNode.toLatexWithAnnotation(null);
    let result = [];
    result.push(document.createTextNode(`Latex: ${latexWithAnnotation.latex}`));
    result.push(document.createElement("br"));
    result.push(document.createTextNode(`URL-encoded: ${encodeURIComponent(latexWithAnnotation.latex)}`));
    if (this.lastCopied !== "") {
      result.push(document.createElement("br"));
      result.push(document.createTextNode(`Last copied: ${this.lastCopied}`))
    }
    let focusInformation = this.focusInformation.toHtmlDebugData();
    for (let i = 0; i < focusInformation.length; i++) {
      result.push(focusInformation[i]);
    }
    result.push(document.createElement("br"));
    result.push(document.createTextNode(`History: ${this.toStringHistory()}`));
    if (this.lastSelectionAction !== "") {
      result.push(document.createElement("br"));
      result.push(document.createTextNode(`Last selection action: ${this.lastSelectionAction}`));
    }
    result.push(document.createElement("br"));
    result.push(document.createTextNode(this.toStringSelection()));
    result.push(document.createElement("br"));
    result.push(document.createTextNode(`Structure: `));
    let elements = this.rootNode.toHtmlDebugData(0);
    for (let i = 0; i < elements.length; i++) {
      result.push(document.createElement("br"));
      result.push(elements[i]);
    }
    result.push(document.createElement("br"));
    result.push(document.createTextNode(
      `Backslash position: ${this.backslashPosition}; started: ${this.backslashSequenceStarted}, sequence: ${this.backslashSequence}.`
    ));
    result.push(document.createElement("br"));
    result.push(document.createTextNode(
      `Position computation string: ${this.positionDebugString}.`
    ));
    return result;
  }

  toStringHistory() {
    let result = this.history.toString();
    if (this.redoBuffer.length > 0) {
      result += `<br>Redo buffer: ${this.redoBuffer.join(", ")}`;
    }
    return result;
  }

  /** Resets the selection and dispatches a key string to the last focused / best element.
   * 
   * @param {string} key 
   */
  dispatchKey(
    /**@type{string} */
    key,
  ) {
    let focused = this.restoreSelectionOrFocusBestChoice();
    this.dispatchKeyToNode(focused, key);
  }

  dispatchKeyToNode(
    /**@type{MathNode|null}*/
    focused,
    /**@type{string} */
    key,
  ) {
    if (focused === null) {
      return;
    }
    if (key === "ArrowLeft" && focused.positionCaretBeforeKeyEvents > 0) {
      focused.focus(0);
      return;
    }
    let event = new KeyboardEvent(
      "keydown", {
      key: key,
    });
    if (focused === this.eventCatcher) {
      this.handleKeyDownCatchAll(event);
      return;
    }
    focused.handleKeyDownDontComputeCaretPosition(event);
  }

  /**@returns{MathNode} */
  getLastFocused() {
    if (this.focusInformation.lastFocused === null) {
      return this.rootNode.rightmostAtomChild();
    }
    if (this.focusInformation.lastFocused.isDetached()) {
      return this.rootNode.rightmostAtomChild();
    }
    return this.focusInformation.lastFocused;
  }

  /**@returns{KeyHandlerResult} */
  arrowShiftHeld(
    /**@type{MathNode} */
    target,
    /**@type{string} */
    arrowKey,
  ) {
    this.initializeSelectionStart(target);
    let preventDefault = this.computeSelectionEndFromShiftKey(arrowKey);
    return new KeyHandlerResult(preventDefault, preventDefault);
  }

  /**@returns{MathNode} The node that was focused or the event catcher. */
  restoreSelectionOrFocusBestChoice() {
    if (this.hasSelection()) {
      this.eventCatcher.focus(0);
      return this.eventCatcher;
    }
    return this.resetSelectionFocusBestChoice();
  }

  /**@returns{MathNode} The node that was focused. */
  resetSelectionFocusBestChoice() {
    if (this.hasSelection()) {
      let lastSelected = this.selectionEndExpanded.element;
      let direction = - 1;
      if (this.selectionStartToTheLeftOfSelectionEnd()) {
        direction = 1;
      }
      this.resetSelectionFullSelectEventCatcher();
      lastSelected.focus(direction);
      return lastSelected;
    }
    let result = this.getLastFocused();
    this.resetSelectionFullSelectEventCatcher();
    let position = - 1;
    if (result.isAtomEditable()) {
      position = result.positionCaretBeforeKeyEvents;
    }
    result.focus(1);
    if (position !== -1) {
      result.positionCaretBeforeKeyEvents = position;
    }
    return result;
  }

  makeSqrtFromSelection(
    /** @type{SplitBySelectionResult} */
    splitBySelection,
  ) {
    let ancestor = splitBySelection.ancestor;
    ancestor.removeAllChildren();
    let underTheRadical = mathNodeFactory.horizontalMathFromArray(this, splitBySelection.split);
    underTheRadical.ensureEditableAtoms();
    let sqrt = mathNodeFactory.sqrt(this, underTheRadical, null);
    ancestor.appendChildren(splitBySelection.beforeSplit);
    ancestor.appendChild(sqrt);
    ancestor.appendChildren(splitBySelection.afterSplit);
    ancestor.ensureEditableAtoms();
    ancestor.updateDOM();
    sqrt.children[2].focusStandard(0);
  }

  makeFractionNumeratorFromSelection(
    /**@type{SplitBySelectionResult|split} */
    split,
  ) {
    if (split === null) {
      return;
    }
    let ancestor = split.ancestor;
    ancestor.removeAllChildren();
    for (let i = 0; i < split.beforeSplit.length; i++) {
      ancestor.appendChild(split.beforeSplit[i]);
    }
    let numerator = mathNodeFactory.horizontalMathFromArray(this, split.split);
    numerator.ensureEditableAtoms();
    let fraction = mathNodeFactory.fraction(this, numerator, null);
    ancestor.appendChild(fraction);
    for (let i = 0; i < split.afterSplit.length; i++) {
      ancestor.appendChild(split.afterSplit[i]);
    }
    ancestor.normalizeHorizontalMath();
    ancestor.ensureEditableAtoms();
    ancestor.updateDOM();
    fraction.children[1].focus(- 1);
  }

  /** Determines whether to delete a selection with non-default editable action when
   * a key is pressed.
   * 
   * @param {string} key
   * 
   */
  shouldDeleteSelection(
    key,
  ) {
    return this.hasSelection() &&
      this.selectionEscapedOriginalAtom() &&
      latexConstants.isCharacterReplacingSelection(key);
  }

  /** @returns{KeyHandlerResult} */
  handleControlKeys(
    /**@type{KeyboardEvent} */
    e,
  ) {
    if (!e.ctrlKey) {
      return new KeyHandlerResult(false, false);
    }
    switch (e.key) {
      case "Z":
        this.redo();
        return new KeyHandlerResult(true, true);
      case "z":
        this.undo();
        return new KeyHandlerResult(true, true);
      case "c":
        this.copyToClipboard();
        return new KeyHandlerResult(true, false);
      case "x":
        this.copyToClipboard();
        return this.deleteSelection(null);
      case "v":
        this.pasteFromClipboard();
        return new KeyHandlerResult()
      default:
        return new KeyHandlerResult(false, false);
    }
  }

  handleKeyDownCatchAll(
    /**@type{KeyboardEvent} */
    e,
  ) {
    e.stopPropagation();
    e.preventDefault();
    let key = e.key;
    if (e.ctrlKey) {
      this.handleControlKeys(e);
      return;
    }
    if (this.shouldDeleteSelection(key)) {
      this.deleteSelection(key);
      return;
    }
    switch (key) {
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        if (e.shiftKey) {
          this.arrowShiftHeld(this.getLastFocused(), key);
          return;
        }
        this.resetSelectionFocusBestChoice();
        return;
      case "/":
        this.makeFractionNumeratorFromSelection(this.splitAtomsBySelection());
        return;
      case "\\sqrt":
        if (!this.hasSelection()) {
          this.resetSelectionFocusBestChoice().makeSqrt();
          return;
        }
        this.makeSqrtFromSelection(this.splitAtomsBySelection());
        return;
      case "Delete":
      case "Backspace":
        if (this.hasSelection()) {
          this.deleteSelection(null);
          return;
        }
        this.resetSelectionFocusBestChoice();
        return;
      default:
        return;
    }
  }

  /**@returns{boolean} */
  hasSelection() {
    return this.selectionStart.element !== null;
  }

  /**@returns{boolean} */
  selectionEscapedOriginalAtom() {
    return this.selectionStartExpanded.position === - 1;
  }

  /** @returns{KeyHandlerResult} whether the default should be prevented. 
   * 
   * @param {string|null} keyOrLatex A Latex snippet, latin character, whitespace or digit to replace the selection.
  */
  deleteSelection(
    /**@type{string|null} */
    keyOrLatex,
  ) {
    if (!this.hasSelection()) {
      return new KeyHandlerResult(false, false);
    }
    if (!this.selectionEscapedOriginalAtom()) {
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let expanded = this.selectionStartExpanded.element.beefUpToHorizontalParent();
    if (expanded.type.type === knownTypes.root.type) {
      // Delete the root node.
      this.rootNode.removeAllChildren();
      this.rootNode.appendChild(mathNodeFactory.horizontalMath(this, null));
      this.resetSelectionLeaveRangesIntact();
      this.updateDOM();
      this.rootNode.focus(0);
      this.writeLatexToInput(true);
      this.writeDebugInfo(null);
      return new KeyHandlerResult(true, true);
    }
    let horizontalOwner = expanded.parent;
    let parent = horizontalOwner.parent;
    if (parent === null) {
      console.log("Unexpected horizontal math element without parent.");
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let splitBySelection = this.splitAtomsBySelection();
    if (splitBySelection === null) {
      console.log("Unexpected failure to split by selection");
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let horizontalReplacement = mathNodeFactory.horizontalMathFromArray(this, splitBySelection.beforeSplit);
    /**@type{MathNode} */
    let replacing = null;
    if (keyOrLatex !== null) {
      replacing = new LaTeXParser(this, keyOrLatex).parse();
    } else {
      replacing = mathNodeFactory.atom(this, "");
    }
    if (replacing.type.type === knownTypes.atom.type) {
      replacing.desiredCaretPosition = replacing.textContentOrInitialContent().length;
    } else {
      replacing.desiredCaretPosition = 1;
    }
    horizontalReplacement.appendChild(replacing);
    horizontalReplacement.appendChildren(splitBySelection.afterSplit);
    horizontalReplacement.normalizeHorizontalMath();
    horizontalReplacement.ensureEditableAtoms();
    parent.replaceChildAtPosition(horizontalOwner.indexInParent, horizontalReplacement);
    this.resetSelectionLeaveRangesIntact();
    parent.updateDOM();
    parent.focusRestore();
    this.writeLatexToInput(true);
    this.writeDebugInfo(null);
    return new KeyHandlerResult(true, true);
  }

  updateAlignment() {
    this.rootNode.computeBoundingBox();
    this.rootNode.doAlign();
    let boundingRectangle = this.rootNode.element.getBoundingClientRect();
    // Bounding box may not be computed correctly at this point in Firefox.
    let desiredHeight = Math.max(boundingRectangle.height, this.rootNode.boundingBox.height, this.standardAtomHeight);
    // Bounding box may not be computed correctly at this point in Firefox.
    let desiredWidth = Math.max(boundingRectangle.width, this.rootNode.boundingBox.width);
    if (this.options.editable) {
      desiredHeight += 2;
    }
    this.container.style.height = desiredHeight;
    this.container.style.width = desiredWidth;
    if (this.rootNode.boundingBox.needsMiddleAlignment && !this.options.editable) {
      this.container.style.verticalAlign = "middle";
    } else {
      this.container.style.verticalAlign = "text-bottom";
    }
  }

  /**@returns {string} */
  toStringSelection() {
    if (this.selectionStart.element === null) {
      return "";
    }
    let startAnnotationString = "[expanded selection null]";
    if (this.selectionStartExpanded.element !== null) {
      startAnnotationString = this.selectionStartExpanded.element.toLatexWithAnnotation(null).latex;
    }
    let endAnnotationString = this.selectionEnd.element.toLatexWithAnnotation(null).latex;
    let result = `Selection: from: ${this.selectionStartExpanded.toString()} to: ${this.selectionEndExpanded.toString()}.`;
    result += `Actually selected: ${this.selectionStart.toString()} to: ${this.selectionEnd.toString()}.`;
    result += `Latex from: ${startAnnotationString} to ${endAnnotationString}`;
    return result;
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
    if (!atom.hasHorozintalMathParent()) {
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
    if (this.selectionStartExpanded.element === null || this.selectionEndExpanded.element === null) {
      return null;
    }
    let start = this.selectionStartExpanded;
    let end = this.selectionEndExpanded;

    if (end.element.isToTheLeftOf(start.element)) {
      // Swap end and start
      let temporary = start;
      start = end;
      end = temporary;
    }
    if (
      this.selectionStartExpanded.element === this.selectionEndExpanded.element &&
      this.selectionStartExpanded.element.isAtomEditable() &&
      !this.selectionEscapedOriginalAtom()
    ) {
      // Selection is contained within a single atom.
      return this.splitOneAtomBySelection();
    }
    let ancestor = start.element.commonAncestor(end.element);
    if (ancestor === null) {
      console.log(`Not expected: no common ancestor element between ${start} and ${end}. `);
      return null;
    }
    if (ancestor.type.type !== knownTypes.horizontalMath.type) {
      ancestor = ancestor.beefUpToHorizontalParent().parent;
      if (ancestor === null) {
        // The common ancestor is the root node. 
        let result = new SplitBySelectionResult(this.rootNode.children[0]);
        for (let i = 0; i < result.ancestor.children.length; i++) {
          result.split.push(result.ancestor.children[i]);
        }
        return result;
      }
    }
    if (start.element === ancestor || end.element === ancestor) {
      // Start and end are the same node, which is the ancestor.
      return null;
    }
    while (start.element.parent !== ancestor && start.element.parent !== null) {
      start.element = start.element.parent;
    }
    while (end.element.parent !== ancestor && end.element.parent !== null) {
      end.element = end.element.parent;
    }
    let result = new SplitBySelectionResult(ancestor);
    for (let i = 0; i < start.element.indexInParent; i++) {
      result.beforeSplit.push(ancestor.children[i]);
    }
    for (let i = start.element.indexInParent; i <= end.element.indexInParent; i++) {
      result.split.push(ancestor.children[i]);
    }
    for (let i = end.element.indexInParent + 1; i < ancestor.children.length; i++) {
      result.afterSplit.push(ancestor.children[i]);
    }
    return result;
  }

  /** @returns{MathNode} */
  elementFromPath(
    /** @type{number[]} */
    path,
  ) {
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
    e.preventDefault();
    e.stopPropagation();
    if (this.latexContainer !== null) {
      this.container.removeChild(this.latexContainer);
      this.latexContainer = null;
      this.container.style.width = this.rootNode.element.style.width;
      return;
    }
    this.latexContainer = document.createElement("SPAN");
    let latex = "";
    let writeOriginal = false;
    let removeDisplayStyle = false;
    if (this.rootNode.children[0].type.type === knownTypes.error.type) {
      writeOriginal = true;
    } else if (!this.options.editable && this.latexLastWritten !== "" && !this.options.sanitizeLatexSource) {
      writeOriginal = true;
      removeDisplayStyle = true;
    }
    if (writeOriginal) {
      if (removeDisplayStyle) {
        latex = this.latexLastWritten.split("\\displaystyle").join("");
      } else {
        latex = this.latexLastWritten;
      }
    } else {
      latex = this.rootNode.toLatex();
    }
    this.latexContainer.textContent = latex;
    this.latexContainer.style.position = "absolute";
    this.latexContainer.style.left = this.rootNode.element.style.width;
    this.latexContainer.style.whiteSpace = "nowrap";
    this.container.appendChild(this.latexContainer);
    let newWidth = this.rootNode.element.getBoundingClientRect().width + this.latexContainer.getBoundingClientRect().width;
    this.container.style.width = newWidth;
    let range = window.getSelection().getRangeAt(0);
    let rangeClone = range.cloneRange();
    rangeClone.selectNode(this.latexContainer);
    this.resetSelectionDOM();
    window.getSelection().addRange(rangeClone);
  }

  resetSelectionLeaveRangesIntact() {
    if (this.selectionStart.element !== null) {
      this.rootNode.unSelectMouseRecursive();
    }
    this.selectionStart.element = null;
    this.selectionStart.position = - 1;
    this.selectionEnd.element = null;
    this.selectionEnd.position = - 1;
    this.selectionStartExpanded.element = null;
    this.selectionStartExpanded.position = - 1;

    this.mouseSelectionStarted = false;
    this.mouseIgnoreNextClick = false;
    this.selectionNoMoreDefault = false;
    this.mouseSelectionVisible = false;
  }

  initializeSelectionStart(
    /** @type {MathNode} */
    start,
  ) {
    if (this.selectionStart.element !== null) {
      return;
    }
    this.resetSelectionLeaveRangesIntact();
    this.selectionStart = new MathNodeWithCaretPosition(start, start.positionCaretBeforeKeyEvents);
    this.selectionEnd = new MathNodeWithCaretPosition(start, start.positionCaretBeforeKeyEvents);
  }

  /**@returns{MathNodeWithCaretPosition|null} */
  computeNewSelectionFromShiftKey(
    /** @type {string} */
    key,
  ) {
    if (key === "ArrowLeft" || key === "ArrowUp") {
      let shrinking = this.selectionStartToTheLeftOfSelectionEnd();
      return this.selectionEnd.leftHorizontalNeighborBalanced(shrinking);
    } else {
      let shrinking = !this.selectionStartToTheLeftOfSelectionEnd();
      return this.selectionEnd.rightHorizontalNeighborBalanced(shrinking);
    }
  }

  /** @returns{boolean} whether the default should be prevented. */
  computeSelectionEndFromShiftKey(
    /** @type {string} */
    key,
  ) {
    let newSelection = this.computeNewSelectionFromShiftKey(key);
    if (newSelection === null) {
      return false;
    }
    if (newSelection.element === null) {
      return false;
    }
    this.selectionEnd = newSelection;
    setTimeout(() => {
      this.selectFromElement(newSelection.element);
    }, 0);
    if (this.selectionNoMoreDefault) {
      return true;
    }
    if (newSelection.element !== this.selectionStart.element) {
      return true;
    }
    return false;
  }

  /** @returns{boolean} whether the default browser selection action should be prevented. */
  selectFromElement(
    /** @type {MathNode} */
    element,
  ) {
    this.mouseIgnoreNextClick = true;
    if (element === this.selectionStart.element && !this.selectionNoMoreDefault) {
      this.selectionStartExpanded.assign(this.selectionStart);
      this.selectionEnd.element.storeCaretPosition("", false);
      this.selectionEnd.position = this.selectionEnd.element.positionCaretBeforeKeyEvents;
      if (
        this.selectionEnd.position === this.selectionStart.position &&
        this.selectionEnd.element.selectionLength !== 0
      ) {
        this.selectionEnd.position -= this.selectionEnd.element.selectionLength;
      }
      this.selectionEndExpanded.assign(this.selectionEnd);
      this.storeSelection();
      this.writeDebugInfo(null);
      return false;
    }
    this.preventEditorBlur = true;
    this.selectionNoMoreDefault = true;
    this.resetSelectionDOMSelectEventCatcher();
    this.eventCatcher.focus();
    // The selection has escaped the original element.
    // Once we are out of the original element, we 
    // can only select an entire atom.
    this.selectionStart.position = - 1;
    this.selectionEnd.element = element;
    this.selectionEnd.position = - 1;
    this.computeSelectionExpanded();
    this.highlightSelectionMouse();
    if (this.options.debugLogContainer !== null) {
      this.lastSelectionAction = `Mouse move selection above ${element.toString()}.`;
    }
    this.storeSelection();
    this.writeDebugInfo(null);
    this.preventEditorBlur = false;
    return true;
  }

  /**@returns{MathNodeWithCaretPosition} */
  expandElementForSelection(
    /**@type{MathNode} */
    toBeExpanded,
    /**@type{MathNode} */
    peer,
  ) {
    let parent = toBeExpanded.commonParent(peer);
    if (parent.parent === null) {
      return new MathNodeWithCaretPosition(this.rootNode, -1);
    }
    let container = parent.parent.children[parent.indexInParent];
    return new MathNodeWithCaretPosition(
      container.beefUpToHorizontalParent(),
      - 1,
    );
  }

  computeSelectionExpanded() {
    if (this.selectionStart.element === null || this.selectionEnd.element === null) {
      return;
    }
    this.selectionStartExpanded = this.expandElementForSelection(this.selectionStart.element, this.selectionEnd.element);
    this.selectionEndExpanded = this.expandElementForSelection(this.selectionEnd.element, this.selectionStartExpanded.element);
    this.balanceSelection();
  }

  balanceSelection() {
    let startElement = this.selectionStartExpanded.element;
    let endElement = this.selectionEndExpanded.element;
    let parent = startElement.parent;
    if (
      parent === null ||
      parent !== endElement.parent ||
      parent.type.type !== knownTypes.horizontalMath.type
    ) {
      console.log("Unexpected form of the selection (null, non-horizontal parent or different parent).");
      return;
    }
    let left = startElement.indexInParent;
    let right = endElement.indexInParent;
    let swapped = (right < left);
    if (swapped) {
      let swapper = right;
      right = left;
      left = swapper;
    }
    let leftOpenCount = 0;
    let rightOpenCount = 0;
    function accountChildToTheLeft(
      /**@type{MathNode} */
      child,
    ) {
      if (child.type.type === knownTypes.leftDelimiter.type) {
        if (rightOpenCount > 0) {
          rightOpenCount--;
        } else {
          leftOpenCount++;
        }
      } else if (child.type.type === knownTypes.rightDelimiter.type) {
        rightOpenCount++;
      }
    }
    function accountChildToTheRight(
      /**@type{MathNode} */
      child,
    ) {
      if (child.type.type === knownTypes.rightDelimiter.type) {
        if (leftOpenCount > 0) {
          leftOpenCount--;
        } else {
          rightOpenCount++;
        }
      } else if (child.type.type === knownTypes.leftDelimiter.type) {
        leftOpenCount++;
      }
    }
    for (let i = left; i <= right; i++) {
      accountChildToTheRight(parent.children[i]);
    }
    while (leftOpenCount > 0 || rightOpenCount > 0) {
      if (leftOpenCount > 0) {
        right++;
        if (right >= parent.children.length) {
          console.log("Missing right deliimiter");
          return;
        }
        accountChildToTheRight(parent.children[right]);
      }
      if (rightOpenCount > 0) {
        left--;
        if (left < 0) {
          console.log("Missing left delimiter");
          return;
        }
        accountChildToTheLeft(parent.children[left]);
      }
    }
    if (swapped) {
      this.selectionStartExpanded.element = parent.children[right];
      this.selectionEndExpanded.element = parent.children[left];
      this.selectionEnd.element = parent.children[left];
    } else {
      this.selectionStartExpanded.element = parent.children[left];
      this.selectionEndExpanded.element = parent.children[right];
      this.selectionEnd.element = parent.children[right];
    }
  }

  /**@return{boolean} */
  selectionStartToTheLeftOfSelectionEnd() {
    if (this.selectionStartExpanded.element === null || this.selectionEnd.element === null) {
      return true;
    }
    return this.selectionStartExpanded.element.isToTheLeftOf(this.selectionEnd.element);
  }

  highlightSelectionMouse() {
    if (this.selectionStartExpanded.element === null || this.selectionEnd.element === null) {
      return;
    }
    let left = this.selectionStartExpanded.element;
    let right = this.selectionEndExpanded.element;
    if (right.isToTheLeftOf(left)) {
      let copy = right;
      right = left;
      left = copy;
    }
    left = left.beefUpToHorizontalParent();
    right = right.beefUpToHorizontalParent();
    /**@type {MathNode} */
    let current = left;
    this.rootNode.unSelectMouseRecursive();
    this.rootNode.blurRecursive();
    for (; ;) {
      current.selectElementByMouse();
      if (current === right) {
        break;
      }
      current = current.horizontalSibling(1);
      if (current === null) {
        break;
      }
    }
  }

  handleMouseMoveSelection(
    /**@type{MouseEvent} */
    e,
    /**@type{MathNode} */
    element,
  ) {
    if (!this.mouseSelectionStarted) {
      return;
    }
    e.stopPropagation();
    if (this.selectFromElement(element)) {
      e.preventDefault();
    }
  }

  handleMouseSelectionStart(
    /**@type{MouseEvent} */
    e,
    /**@type{MathNode} */
    element,
  ) {
    this.preventEditorBlur = true;
    if (!this.hasFocusDOM) {
      this.eventCatcher.focus(1);
      this.hasFocusDOM = true;
    }
    e.stopPropagation();
    // e.preventDefault();
    element.storeCaretPosition("", false);
    // Discard previous selection data.
    this.resetSelectionLeaveRangesIntact();
    this.initializeSelectionStart(element);
    this.resetSelectionDOMSelectEventCatcher();
    this.mouseSelectionStarted = true;
    this.mouseSelectionVisible = true;
    if (this.options.debugLogContainer !== null) {
      this.lastSelectionAction = `Mouse selection start over ${element.toString()}, position:${element.positionCaretBeforeKeyEvents}.`;
    }
    // The mouse down event may fire before the built-in element ranges are updated.
    // So, release the event handling to allow built-in processing to take place, and 
    // schedule an element update immediately after.
    setTimeout(() => {
      element.storeCaretPosition("", false);
      this.selectionStart.position = element.positionCaretBeforeKeyEvents;
      this.writeDebugInfo(null);
    }, 0);
    this.writeDebugInfo(null);
    return;
  }

  handleMouseSelectionEnd(
    /**@type{MouseEvent} */
    e,
  ) {
    if (this.options.debugLogContainer !== null) {
      this.lastSelectionAction = `Mouse selection end.`;
    }
    this.mouseSelectionStarted = false;
    this.preventEditorBlur = false;
    this.writeDebugInfo(null);
    e.preventDefault();
    e.stopPropagation();
  }

  blur() {
    if (this.preventEditorBlur) {
      return;
    }
    this.hasFocusDOM = false;
    this.backslashSequenceStarted = false;
    this.resetSelectionLeaveRangesIntact();
    this.rootNode.blurRecursive();
  }
}

class SplitBySelectionResult {
  constructor(
    /** @type{MathNode} */
    ancestor,
  ) {
    /** @type{MathNode[]} */
    this.beforeSplit = [];
    /** @type{MathNode[]} */
    this.afterSplit = [];
    /** @type{MathNode[]} */
    this.split = [];
    /** @type{MathNode} */
    this.ancestor = ancestor;
  }
}

class BoundingBox {
  constructor() {
    /** @type{number}*/
    this.left = 0;
    /** @type{number}*/
    this.top = 0;
    /** @type{number}*/
    this.width = 0;
    /** @type{number}*/
    this.height = 0;
    /** @type{number} 
     * Used with broken lines to 
     * indicate the height of the broken line.
     */
    this.lineHeight = - 1;
    /** @type{number}*/
    this.fractionLineHeight = 0;

    /**@type {boolean} */
    this.needsMiddleAlignment = false;
    /** @type{number[]}*/
    // Used for math nodes that have columns (matrices, tables (\begin{array}...)).
    this.columnOffsets = [];
    /** @type{number}*/
    // In a^b the following measures the width of b. 
    this.superScriptWidth = 0;
    /** @type{number}*/
    // In a_b the following measures the width of b. 
    this.subScriptWidth = 0;
    /** @type{string}*/
    this.transform = "";
    /** @type{string}*/
    this.transformOrigin = "";
    /** @type{number}*/
    this.heightBeforeTransform = - 1;
    /** @type{number}*/
    this.widthBeforeTransform = - 1;
    /** @type{number}*/
    this.fontSizeInPixels = 0;
  }

  /** @returns {number} */
  getColumnOffset(
    /**@type{number} */
    columnIndex,
  ) {
    if (columnIndex < 0) {
      return 0;
    }
    if (columnIndex >= this.columnOffsets.length) {
      return this.width;
    }
    return this.columnOffsets[columnIndex];
  }

  toString() {
    return `w: ${this.width.toFixed(2)}, h: ${this.height.toFixed(2)}, fl:${this.fractionLineHeight.toFixed(2)}`;
  }
}

/** Contains latex of a math node with metadata.  
 * 
 * Currently, this class contains LaTeX data only. 
 */
// This class used to contain more information 
// than the latex string but
// that turned out to be bug-prone so 
// the metadata was removed. We keep the class however, 
// in case we need to store metada in the future again.
class LatexWithAnnotation {
  constructor(
    /**@type{string} */
    latex,
  ) {
    /** @type {string} */
    this.latex = latex;
  }
}

class BackslashResult {
  constructor(
    /**@type{boolean} */
    keyAccountedCarryOutDefaultEvent,
    /**@type{string} */
    command,
  ) {
    this.keyAccountedCarryOutDefaultEvent = keyAccountedCarryOutDefaultEvent;
    this.command = command;
  }
}

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
    this.positionCaretBeforeKeyEvents = - 1;
    /** @type {number} */
    this.selectionLength = - 1;
    /** @type {number} */
    this.desiredCaretPosition = - 1;
    /** @type {string} */
    this.initialContent = "";
    /** @type {boolean} */
    this.focused = false;
    /** @type {boolean} 
     * Indicates whether the node was explicitly entered by the user 
     * or has been implied by a user action, such as parentheses auto-balancing.
     */
    this.implied = false;
    /** @type {boolean} */
    this.selectedByMouse = false;
    /** @type {EquationEditor} */
    this.equationEditor = equationEditor;
    this.boundingBox = new BoundingBox();
    /**@type {string} Used for matrix column formatting; additionally reserved for unforeseen use cases.*/
    this.latexExtraStyle = "";
    /** @type{Object} Reserved for data specific to particular MathNode types.*/
    this.extraData = null;
  }

  createDOMElementIfMissing() {
    if (this.element !== null) {
      // Element already created;
      return;
    }
    if (this.type.type === knownTypes.formInput.type) {
      this.element = document.createElement("input");
    } else {
      this.element = document.createElement("div");
    }
    if (
      (this.type.type === knownTypes.eventCatcher.type || this.type.type === knownTypes.atom.type) &&
      this.equationEditor.options.editable
    ) {
      this.element.contentEditable = true;
    }
    if (this.type.type === knownTypes.root.type && this.equationEditor.options.editable) {
      // equationeditoreditable one word for maximum portability.
      this.element.classList.add("equationeditoreditable");
      if (getComputedStyle(this.element).borderWidth === "") {
        this.element.style.borderStyle = "solid";
      }
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
    if (this.type.colorText !== "") {
      this.element.style.color = this.type.colorText;
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
    if (this.type.boxSizing !== "") {
      this.element.style.boxSizing = this.type.boxSizing;
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
    if (this.type.borderTopLeftRadius !== "") {
      this.element.style.borderTopLeftRadius = this.type.borderTopLeftRadius;
    }
    if (this.type.borderTopRightRadius !== "") {
      this.element.style.borderTopRightRadius = this.type.borderTopRightRadius;
    }
    if (this.type.borderBottomLeftRadius !== "") {
      this.element.style.borderBottomLeftRadius = this.type.borderBottomLeftRadius;
    }
    if (this.type.borderBottomRightRadius !== "") {
      this.element.style.borderBottomRightRadius = this.type.borderBottomRightRadius;
    }
    if (this.type.borderTop !== "") {
      this.element.style.borderTop = this.type.borderTop;
    }
    if (this.type.borderLeft !== "") {
      this.element.style.borderLeft = this.type.borderLeft;
    }
    if (this.type.borderRight !== "") {
      this.element.style.borderRight = this.type.borderRight;
    }
    this.element.style.width = this.type.width;
    this.element.style.height = this.type.height;
    this.element.style.display = this.type.display;
    if (this.type.minWidth !== "") {
      this.element.style.minWidth = this.type.minWidth;
    }
    if (this.type.overflow !== "") {
      this.element.style.overflow = this.type.overflow;
    }
    this.element.style.verticalAlign = this.type.verticalAlign;
    this.element.style.outline = this.type.outline;
    if (this.type.cursor !== "") {
      this.element.style.cursor = this.type.cursor;
    }
    if (this.type.textAlign !== "") {
      this.element.style.textAlign = this.type.textAlign;
    }
    if (this.type.whiteSpace !== "") {
      this.element.style.whiteSpace = this.type.whiteSpace;
    }
    // Fonts. 
    if (this.type.fontWeight !== "") {
      this.element.style.fontWeight = this.type.fontWeight;
    }
    if (this.type.fontSizeRatio !== 1) {
      this.element.style.fontSize = `${this.type.fontSizeRatio * 100}%`;
    }
    if (this.initialContent !== "") {
      if (this.type.type === knownTypes.formInput.type) {
        this.element.value = this.initialContent;
      } else {
        this.element.textContent = this.initialContent;
      }
    }
    if (this.type.position !== "") {
      this.element.style.position = this.type.position;
    }
    this.element.setAttribute("mathTagName", this.type.type);
    if (this.equationEditor.options.editable) {
      this.element.addEventListener("copy", (e) => {
        this.equationEditor.copyToClipboard();
      });
      if (this.type.type === knownTypes.atom.type) {
        this.element.addEventListener("paste", (e) => {
          this.pasteFromClipboard(e);
        });
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
      if (this.type.type !== knownTypes.eventCatcher.type) {
        this.element.addEventListener("click", (e) => {
          this.handleSingleClick(e);
        });
        this.element.addEventListener("mousedown", (e) => {
          this.equationEditor.handleMouseSelectionStart(e, this);
        });
        this.element.addEventListener("mouseup", (e) => {
          this.equationEditor.handleMouseSelectionEnd(e);
        });
        this.element.addEventListener("mousemove", (e) => {
          this.equationEditor.handleMouseMoveSelection(e, this);
        });
      }
      if (this.type.type === knownTypes.root.type) {
        this.element.addEventListener("focusin", () => {
          // equationeditoreditablehighlighted one word for portability.
          this.element.classList.add("equationeditoreditablehighlighted")
        });
        this.element.addEventListener("focusout", () => {
          this.element.classList.remove("equationeditoreditablehighlighted")
        });
      }
    }
    if (this.type.type === knownTypes.root.type) {
      this.element.addEventListener("dblclick", (e) => {
        this.equationEditor.handleDoubleClick(e);
      });
    }
  }

  handleSingleClick(
    /** @type{MouseEvent} */
    e,
  ) {
    e.stopPropagation();
    e.preventDefault();
    if (this.equationEditor.mouseIgnoreNextClick) {
      this.equationEditor.mouseIgnoreNextClick = false;
      if (this.equationEditor.selectionEscapedOriginalAtom()) {
        this.equationEditor.resetSelectionDOMSelectEventCatcher();
        if (this.equationEditor.options.debugLogContainer !== null) {
          this.equationEditor.lastSelectionAction = "Set mouse ignore next to true.";
        }
      }
      this.equationEditor.writeDebugInfo(null);
      return;
    }
    if (this.equationEditor.options.debugLogContainer !== null) {
      this.equationEditor.lastSelectionAction = "Reset selection on single click.";
    }
    this.equationEditor.resetSelectionLeaveRangesIntact();
    this.storeCaretPosition("", false);
    if (this.type.type === knownTypes.atom.type) {
      return;
    }
    this.focus(1);
  }

  /** @returns {MathNode|null} */
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

  selectElementByMouse() {
    if (!this.selectedByMouse && this.element !== null) {
      this.element.style.backgroundColor = "lightskyblue";
    }
    this.selectedByMouse = true;
  }

  unSelectElementByMouse() {
    if (this.selectedByMouse && this.element !== null) {
      this.element.style.backgroundColor = "";
    }
    this.selectedByMouse = false;
  }

  unSelectMouseRecursive() {
    this.unSelectElementByMouse();
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i] === null) {
        continue;
      }
      this.children[i].unSelectMouseRecursive();
    }
  }

  focusElement() {
    this.element.style.background = "#f0f0f0";
    this.focused = true;
    this.equationEditor.hasFocusDOM = true;
  }

  /**@returns {MathNode} */
  beefUpToHorizontalParent() {
    let parentWithIndex = this.findHorizontalMathParent();
    if (parentWithIndex.parent === null) {
      return this;
    }
    return parentWithIndex.parent.children[parentWithIndex.indexInParent];
  }

  blurElement() {
    this.focused = false;
    if (this.element === null) {
      return;
    }
    this.element.style.background = "";
  }

  blurRecursive() {
    this.blurElement();
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].blurRecursive();
    }
  }

  updateDOM() {
    this.updateDOMRecursive(0);
    this.equationEditor.updateAlignment();
    this.equationEditor.writeSVG();
  }

  /** @returns {number} 
   * Returns the length of the content of an atom.
   * Returns -1 if the element is not an atom. 
   */
  lengthContentIfAtom() {
    if (!this.isAtomEditable()) {
      return - 1;
    }
    if (this.element === null) {
      return this.initialContent.length;
    }
    if (this.element.textContent === null) {
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

  /** @returns {string} */
  textContentOrInitialContent() {
    if (this.element === null) {
      if (this.initialContent === null) {
        console.log("Unexpected: initial content is null.");
        return "[null]";
      }
      return this.initialContent;
    }
    let result = this.element.textContent;
    if (result === null) {
      return "";
    }
    return result;
  }

  /** @returns {string} */
  contentIfAtomic() {
    if (!this.isAtomic()) {
      return "";
    }
    return this.textContentOrInitialContent();
  }

  /** @returns {boolean} */
  isAtomicNonEmptyOrHorizontalMathWithNonEmptyAtomic() {
    if (this.isAtomic()) {
      return this.contentIfAtomic() !== "";
    }
    if (this.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    let numberOfAtomicNonEmpty = 0;
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      if (!child.isAtomic()) {
        return false;
      }
      if (child.contentIfAtomic() !== "") {
        numberOfAtomicNonEmpty++;
        if (numberOfAtomicNonEmpty > 1) {
          return false;
        }
      }
    }
    return numberOfAtomicNonEmpty === 1;
  }

  /** @returns {boolean} */
  isAtomic() {
    return (
      this.type.type === knownTypes.atom.type ||
      this.type.type === knownTypes.atomImmutable.type ||
      this.type.type === knownTypes.sqrtSign.type
    );
  }

  /** @returns {boolean} */
  isDelimiter() {
    return (
      this.type.type === knownTypes.leftDelimiter.type ||
      this.type.type === knownTypes.rightDelimiter.type
    );
  }

  /** @returns {boolean} */
  isAtomEditable() {
    return this.type.type === knownTypes.atom.type;
  }

  /**@returns {boolean} */
  hasHorozintalMathParent() {
    if (this.parent === null) {
      return false;
    }
    return this.parent.type.type === knownTypes.horizontalMath.type;
  }

  updateDOMRecursive(
    /** @type {number} */
    depth,
  ) {
    if (
      this.type.type === knownTypes.atom.type ||
      this.type.type === knownTypes.atomImmutable.type ||
      this.type.type === knownTypes.formInput
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
      this.element.style.borderColor = this.type.colorImplied;
    } else {
      this.element.style.color = this.type.colorText;
      this.element.style.borderColor = this.type.colorText;
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

  computeDimensionsDelimiter() {
    let boundingRecangleDOM = this.element.getBoundingClientRect();
    this.boundingBox.width = boundingRecangleDOM.width;
  }

  computeDimensionsAtomicNoTransform() {
    if (this.boundingBox.widthBeforeTransform >= 0) {
      return;
    }
    this.element.style.transformOrigin = "top left";
    this.element.style.transform = "";
    this.element.style.width = "";
    this.element.style.height = "";
    this.element.style.top = "";
    this.element.style.left = "";
    this.computeDimensionsAtomic();
    this.boundingBox.heightBeforeTransform = this.boundingBox.height;
    this.boundingBox.widthBeforeTransform = this.boundingBox.width;
  }

  computeDimensionsAtomic() {
    let boundingRecangleDOM = this.element.getBoundingClientRect();
    this.boundingBox.width = boundingRecangleDOM.width;
    this.boundingBox.height = boundingRecangleDOM.height;
    if (this.boundingBox.height === 0) {
      this.boundingBox.height = this.equationEditor.standardAtomHeight;
    }
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  computeDimensionsHorizontalBrace(
    /**@type{number} */
    desiredWidth,
  ) {
    this.boundingBox.width = desiredWidth;
    const topLeft = this.children[0];
    const leftStraight = this.children[1];
    const bottomRight = this.children[2];
    const bottomLeft = this.children[3];
    const rightStraight = this.children[4];
    const topRight = this.children[5];
    const desiredRadius = Math.max(4, Math.floor(desiredWidth * 0.02));
    const desiredHeight = desiredRadius;

    const radiusString = `${desiredRadius}px`;
    const borderString = `2px solid`;

    topLeft.element.style.borderTopLeftRadius = radiusString;
    topLeft.element.style.borderTop = borderString;
    topRight.element.style.borderTopRightRadius = radiusString;
    topRight.element.style.borderTop = borderString;
    bottomLeft.element.style.borderBottomLeftRadius = radiusString;
    bottomLeft.element.style.borderBottom = borderString;
    bottomRight.element.style.borderBottomRightRadius = radiusString;
    bottomRight.element.style.borderBottom = borderString;

    const halfWidth = desiredWidth / 2;

    topLeft.boundingBox.width = desiredRadius;
    topRight.boundingBox.width = desiredRadius;
    bottomRight.boundingBox.width = desiredRadius;
    bottomLeft.boundingBox.width = desiredRadius;

    topLeft.boundingBox.height = desiredHeight;
    topRight.boundingBox.height = desiredHeight;
    bottomRight.boundingBox.height = desiredHeight;
    bottomLeft.boundingBox.height = desiredHeight;

    topLeft.boundingBox.top = desiredHeight;
    topRight.boundingBox.top = desiredHeight;

    bottomRight.boundingBox.left = halfWidth - desiredRadius;
    bottomLeft.boundingBox.left = halfWidth;
    topRight.boundingBox.left = desiredWidth - desiredRadius;

    leftStraight.boundingBox.width = halfWidth - 2 * desiredRadius;
    leftStraight.boundingBox.left = desiredRadius;
    rightStraight.boundingBox.width = halfWidth - 2 * desiredRadius;
    rightStraight.boundingBox.left = halfWidth + desiredRadius;

    leftStraight.boundingBox.height = desiredHeight;
    rightStraight.boundingBox.height = desiredHeight;
    this.boundingBox.height = desiredHeight + 2;
  }

  computeDimensionsOverLine() {
    this.computeDimensionsStandard();
    // The border add 1 extra pixel of height.
    this.boundingBox.fractionLineHeight = this.children[0].boundingBox.fractionLineHeight + 1;
  }

  computeDimensionsOverBrace() {
    let base = this.children[0];
    let brace = this.children[1];
    let superscript = this.children[2];
    brace.computeDimensionsHorizontalBrace(base.boundingBox.width);
    superscript.boundingBox.width = base.boundingBox.width;
    superscript.computeBoundingBoxLeftSingleChild();
    this.boundingBox.width = base.boundingBox.width;
    this.boundingBox.height = base.boundingBox.height + brace.boundingBox.height + superscript.boundingBox.height;
    this.boundingBox.fractionLineHeight = brace.boundingBox.height + superscript.boundingBox.height + base.boundingBox.fractionLineHeight;
    brace.boundingBox.top = superscript.boundingBox.height;
    base.boundingBox.top = superscript.boundingBox.height + brace.boundingBox.height;
  }

  computeDimensionsOperatorStandalone() {
    let child = this.children[0];
    // see latexConstants.operatorWithSuperAndSubscript
    let operatorShifts = {
      // sum
      "\u03A3": 0.22,
      // integral
      "\u222B": 0.03,
    };
    let operatorScales = {
      // sum
      "\u03A3": 2,
      // integral
      "\u222B": 2,
    };
    let proportionShiftUp = 0.1;
    let operatorName = child.initialContent;
    if (operatorName in operatorShifts) {
      proportionShiftUp = operatorShifts[operatorName];
    }
    let operatorScale = 1;
    if (operatorName in operatorScales) {
      operatorScale = operatorScales[operatorName];
    }
    child.boundingBox.top = - child.boundingBox.height * proportionShiftUp;
    this.boundingBox.height = child.boundingBox.height * (1 - proportionShiftUp * operatorScale);
    this.boundingBox.width = child.boundingBox.width;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  containsFractionLineRecursive() {
    if (this.type.type === knownTypes.fraction.type) {
      return true;
    }
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].containsFractionLineRecursive()) {
        return true;
      }
    }
    return false;
  }

  requiresTallExponent(
    /**@type{MathNode} */
    base,
  ) {
    if (base.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    for (let i = base.children.length - 1; i >= 0; i--) {
      let child = base.children[i];
      if (child.type.type === knownTypes.matrix.type) {
        return true;
      }
      if (child.type.type === knownTypes.rightDelimiter.type) {
        return base.containsFractionLineRecursive();
      }
      if (child.type.type === knownTypes.atom.type) {
        if (child.textContentOrInitialContent() === "") {
          continue;
        }
      }
      return false;
    }
    return false;
  }

  computeDimensionsBaseWithSubscript() {
    let base = this.children[0];
    let subscript = this.children[1];
    let overlapRatio = 0.35;
    if (this.requiresTallExponent(base)) {
      overlapRatio = 0.1;
    }
    let overlap = base.boundingBox.height * overlapRatio;
    this.boundingBox.height = subscript.boundingBox.height + base.boundingBox.height - overlap;
    if (subscript.boundingBox.height > this.boundingBox.height) {
      this.boundingBox.height = subscript.boundingBox.height;
    }
    let baseWithExponent = null;
    if (base.type.type === knownTypes.horizontalMath.type) {
      if (base.children.length === 1) {
        if (base.children[0].type.type === knownTypes.baseWithExponent.type) {
          baseWithExponent = base.children[0];
        }
      }
    }
    base.boundingBox.left = 0;
    this.boundingBox.width = base.boundingBox.width + subscript.boundingBox.width;
    subscript.boundingBox.top = base.boundingBox.height * (1 - overlapRatio);
    if (baseWithExponent === null) {
      subscript.boundingBox.left = base.boundingBox.width;
      this.boundingBox.subScriptWidth = subscript.boundingBox.width;
    } else {
      subscript.boundingBox.left = base.boundingBox.width - baseWithExponent.boundingBox.superScriptWidth;
      this.boundingBox.superScriptWidth = 0;
      this.boundingBox.subScriptWidth = 0;
      this.boundingBox.width = Math.max(
        baseWithExponent.boundingBox.width +
        subscript.boundingBox.width -
        baseWithExponent.boundingBox.superScriptWidth,
        baseWithExponent.boundingBox.width,
      );
    }
    this.boundingBox.fractionLineHeight = base.boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
  }

  computeBoundingBoxLeftSingleChild() {
    let child = this.children[0];
    child.boundingBox.left = (this.boundingBox.width - child.boundingBox.width) / 2;
  }

  verticallyStretchParenthesis(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    if (this.children.length === 0) {
      // Unbalanced parenthesis: the "missing delimiter" is there but has
      // no delimiter mark.
      return;
    }
    let child = this.children[0];
    child.verticallyStretch(heightToEnclose, fractionLineHeightEnclosed);

    this.boundingBox.top = 0;
    this.boundingBox.height = child.boundingBox.height;
    this.boundingBox.fractionLineHeight = child.boundingBox.fractionLineHeight;
    this.boundingBox.width = child.boundingBox.width;
  }

  computeDimensionsNumerator() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height;
  }

  computeDimensionsMatrix() {
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = this.children[0].boundingBox.height;
    this.boundingBox.width = this.children[0].boundingBox.width;
    this.boundingBox.fractionLineHeight = this.children[0].boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
    let table = this.children[0].children[1];
    for (let i = 1; i < this.children.length; i++) {
      let verticalBar = this.children[i];
      verticalBar.boundingBox.height = this.boundingBox.height;
      verticalBar.boundingBox.width = 1;
      let columnIndex = verticalBar.extraData.columnIndex;
      let offset = table.boundingBox.getColumnOffset(columnIndex);
      verticalBar.boundingBox.left = offset;
    }
  }

  computeDimensionsMatrixTable() {
    this.boundingBox = new BoundingBox();
    let numberOfColumns = this.children[0].children.length;
    let numberOfRows = this.children.length;
    let left = 0;
    let betweenColumns = 10;
    for (let i = 0; i < numberOfColumns; i++) {
      this.boundingBox.columnOffsets.push(left);
      let width = 0;
      for (let j = 0; j < numberOfRows; j++) {
        width = Math.max(width, this.children[j].children[i].boundingBox.width);
      }
      for (let j = 0; j < numberOfRows; j++) {
        let child = this.children[j].children[i];
        let extraShift = (width - child.boundingBox.width) / 2;
        if (child.latexExtraStyle === "l") {
          extraShift = 0;
        } else if (child.latexExtraStyle === "r") {
          extraShift = width - child.boundingBox.width;
        }
        child.boundingBox.left = left + extraShift;
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
      row.mergeVerticalComponentsBoundingBoxesHorizontallyAlignedElements();
      row.boundingBox.top = top;
      top += row.boundingBox.height + betweenRows;
    }
    this.boundingBox.height = top - betweenRows;
    this.boundingBox.width = rowWidth;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  computeDimensionsRadicalUnderBox() {
    this.computeDimensionsStandard();
    let child = this.children[0];
    this.boundingBox.height = child.boundingBox.height + 1;
    this.boundingBox.fractionLineHeight = child.boundingBox.fractionLineHeight + 1;
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
      throw "This case should be handled by MathNodeSqrt.computeDimensions()";
      return;
    }
    if (this.type.type === knownTypes.matrixRow.type) {
      return;
    }
    if (this.type.type === knownTypes.matrixTable.type) {
      this.computeDimensionsMatrixTable();
      return;
    }
    if (this.type.type === knownTypes.matrix.type) {
      this.computeDimensionsMatrix();
      return;
    }
    if (this.type.type === knownTypes.radicalUnderBox.type) {
      this.computeDimensionsRadicalUnderBox();
      return;
    }
    if (this.type.type === knownTypes.operatorStandalone.type) {
      this.computeDimensionsOperatorStandalone();
      return;
    }
    if (this.type.type === knownTypes.numerator.type) {
      this.computeDimensionsNumerator();
    }
    if (this.type.type === knownTypes.baseWithSubscript.type) {
      this.computeDimensionsBaseWithSubscript();
      return;
    }
    if (this.type.type === knownTypes.overBrace.type) {
      this.computeDimensionsOverBrace();
      return;
    }
    if (this.type.type === knownTypes.overLinedBox.type) {
      this.computeDimensionsOverLine();
      return;
    }
    this.computeDimensionsStandard();
  }

  computeDimensionsStandard() {
    this.boundingBox = new BoundingBox();
    this.computeDimensionsParenthesesHeight();
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.left = this.boundingBox.width;
      this.boundingBox.width += child.boundingBox.width;
    }
    this.mergeVerticalComponentsBoundingBoxesHorizontallyAlignedElements();
    this.computeMiddleAlignment();
  }

  computeDimensionsParenthesesHeight() {
    // Compute parentheses height
    /** @type {number[]} */
    let enclosedHeights = [];
    let enclosedFractionLineHeights = [];
    /** @type {number[]} */
    let indicesOpenedParentheses = [];
    for (let i = 0; i < this.children.length; i++) {
      this.computeDimensionsParenthesesAccountChild(
        i, enclosedHeights, enclosedFractionLineHeights, indicesOpenedParentheses,
      );
    }
  }

  computeDimensionsParenthesesAccountChild(
    /** @type {number} */
    index,
    /** @type {number[]} */
    enclosedHeights,
    /** @type {number[]} */
    enclosedFractionLineHeights,
    /** @type {number[]} */
    indicesOpenedParentheses,
  ) {
    let child = this.children[index];
    if (child.type.type === knownTypes.leftDelimiter.type) {
      enclosedHeights.push(0);
      enclosedFractionLineHeights.push(0);
      indicesOpenedParentheses.push(index);
      return;
    }
    if (child.type.type === knownTypes.rightDelimiter.type) {
      if (enclosedHeights.length <= 0) {
        console.log("Warning: unbalanced right parenthesis.");
        return;
      }
      let currentHeight = enclosedHeights.pop();
      let currentFractionLineHeight = enclosedFractionLineHeights.pop();
      child.verticallyStretchParenthesis(currentHeight, currentFractionLineHeight);
      let leftCounterpart = this.children[indicesOpenedParentheses.pop()];
      leftCounterpart.verticallyStretchParenthesis(currentHeight, currentFractionLineHeight);
    }
    if (enclosedHeights.length > 0) {
      enclosedHeights[enclosedHeights.length - 1] = Math.max(
        child.boundingBox.height, enclosedHeights[enclosedHeights.length - 1]
      );
      enclosedFractionLineHeights[enclosedFractionLineHeights.length - 1] = Math.max(
        child.boundingBox.fractionLineHeight,
        enclosedFractionLineHeights[enclosedFractionLineHeights.length - 1]
      );
    }
  }

  mergeVerticalComponentsBoundingBoxesHorizontallyAlignedElements() {
    // Compute fraction line height.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      this.boundingBox.fractionLineHeight = Math.max(
        this.boundingBox.fractionLineHeight,
        child.boundingBox.fractionLineHeight,
      );
    }
    // Compute top offsets of children.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.top = this.boundingBox.fractionLineHeight - child.boundingBox.fractionLineHeight;
    }
    // Compute present element height.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let heightFromChild = child.boundingBox.height -
        child.boundingBox.fractionLineHeight +
        this.boundingBox.fractionLineHeight;
      this.boundingBox.height = Math.max(this.boundingBox.height, heightFromChild);
    }
  }


  computeMiddleAlignment() {
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      if (child.boundingBox.needsMiddleAlignment) {
        this.boundingBox.needsMiddleAlignment = true;
        return;
      }
    }
  }

  doAlign() {
    if (this.boundingBox.transform !== "") {
      this.element.style.transformOrigin = this.boundingBox.transformOrigin;
      this.element.style.transform = this.boundingBox.transform;
    } //else {
    if (this.element.style.width !== "auto") {
      this.element.style.width = this.boundingBox.width;
    }
    if (this.element.style.height !== "auto") {
      this.element.style.height = this.boundingBox.height;
    }
    //    }
    this.element.style.left = this.boundingBox.left;
    this.element.style.top = this.boundingBox.top;
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].doAlign();
    }
  }

  updateParentDOM(
    /** @type {HTMLElement|null} */
    oldElement,
  ) {
    let parentElement = null;
    if (this.type.type === knownTypes.root.type) {
      parentElement = this.equationEditor.container;
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

  handleFocus(_) {
    this.focusElement();
    this.storeCaretPosition("", false);
  }

  handleBlur(_) {
    this.blurElement();
    this.equationEditor.blur();
  }

  handleKeyDown(
    /** @type {KeyboardEvent} */
    event,
  ) {
    if (event.key === "Shift" || event.key === "Ctrl") {
      event.stopPropagation();
      return;
    }
    this.storeCaretPosition(event.key, event.shiftKey);
    this.handleKeyDownDontComputeCaretPosition(event);
  }

  handleKeyDownDontComputeCaretPosition(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let frameStart = new Date().getTime();
    event.stopPropagation();
    let handlerResult = this.handleKeyDownCases(event);
    if (handlerResult.preventDefault) {
      event.preventDefault();
      this.equationEditor.accountFrameTime(frameStart);
    }
    if (
      handlerResult.updateAlignment &&
      this.type.type === knownTypes.atom.type &&
      this.element !== null &&
      !this.equationEditor.resizingEditorFont
    ) {
      // The content is not yet written to our editable field: 
      // that will happen once the event default is carried out.
      // This means that we cannot compute the width of our atom accurately now,
      // but will be able to do in the body of the setTimeout'ed function below.
      // This is a problem as the screen may be redrawn between the 
      // time the event default is called and the time we call 
      // equationEditor.updateAlignment in the timeout below, in which case the extra 
      // content will overflow its container box.
    }
    // While we no longer propagate the event, we need to 
    // release the thread so the browser can finish computations
    // with the released element. 
    setTimeout(() => {
      this.equationEditor.storeSelection();
      if (handlerResult.updateAlignment) {
        this.storeCaretPosition(event.key, event.shiftKey);
        this.element.style.maxWidth = "";
        this.element.style.maxHeight = "";
        this.equationEditor.updateAlignment();
        this.equationEditor.accountFrameTime(frameStart);
      }
      this.equationEditor.writeLatexToInput(true);
      this.equationEditor.writeDebugInfo(null);
      this.equationEditor.writeSVG();
      if (this.equationEditor.options.editHandler !== null) {
        this.equationEditor.options.editHandler(this.equationEditor, this);
      }
    }, 0);
  }

  handleKeyUp(
    /** @type {KeyboardEvent} */
    event,
  ) {
    if (!event.shiftKey && event.key !== "Shift") {
      this.equationEditor.selectionStart = new MathNodeWithCaretPosition(null, - 1);
      this.equationEditor.selectionEnd = new MathNodeWithCaretPosition(null, - 1);
    }
  }

  /** @returns {BackslashResult} */
  processBackslash(
    /** @type {string} */
    key,
    /** @type{boolean} */
    shiftHeld,
  ) {
    let result = new BackslashResult(false, key);
    if (key === "Shift") {
      return result;
    }
    if (this.type.type !== knownTypes.atom.type) {
      return result;
    }
    if (key === "\\" && !shiftHeld) {
      this.equationEditor.backslashSequenceStarted = true;
      this.equationEditor.backslashSequence = "\\";
      this.equationEditor.backslashPosition = this.positionCaretBeforeKeyEvents;
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    if (!this.equationEditor.backslashSequenceStarted) {
      return result;
    }
    if (key === " " && this.equationEditor.backslashSequenceStarted) {
      this.equationEditor.backslashSequenceStarted = false;
      if (this.equationEditor.backslashSequence in latexConstants.recognizedCommandsKeyInput) {
        return this.removeBackslashSequence();
      }
      return result;
    }
    if (latexConstants.isLatinCharacter(key)) {
      this.equationEditor.backslashSequence += key;
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    if (key === "Backspace") {
      if (this.equationEditor.backslashSequence === "\\") {
        this.equationEditor.backslashSequenceStarted = false;
      } else {
        this.equationEditor.backslashSequence = this.equationEditor.backslashSequence.slice(
          0, this.equationEditor.backslashSequence.length - 1,
        );
      }
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    this.equationEditor.backslashSequenceStarted = false;
    return result;
  }

  pasteFromClipboard(event) {
    event.preventDefault();
    let data = event.clipboardData.getData('text');
    event.preventDefault();
    this.storeCaretPosition("", false);
    this.writeLatex(data);
  }

  writeLatex(
    /** @type {string} */
    data,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      return;
    }
    let parser = new LaTeXParser(this.equationEditor, data);
    let newContent = parser.parse();
    if (newContent === null) {
      let content = this.textContentOrInitialContent();
      let leftSlice = content.slice(0, this.positionCaretBeforeKeyEvents);
      let rightSlice = content.slice(this.positionCaretBeforeKeyEvents);
      let endContent = leftSlice + data + rightSlice;
      this.element.textContent = endContent;
      this.desiredCaretPosition = this.positionCaretBeforeKeyEvents + data.length;
      this.updateDOM();
      this.equationEditor.updateAlignment();
      this.focusRestore();
      return;
    }
    let split = this.splitByCaretEmptyAtoms();
    /**@type {MathNode} */
    let newRightMostAtom = newContent.rightmostAtomChild();
    if (newRightMostAtom === null) {
      split[1].desiredCaretPosition = 0;
    } else {
      newRightMostAtom.desiredCaretPosition = newRightMostAtom.textContentOrInitialContent().length;
    }
    let horizontalContent = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], newContent, split[1]]);
    horizontalContent.normalizeHorizontalMath();
    let parent = this.parent;
    parent.replaceChildAtPosition(this.indexInParent, horizontalContent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    this.equationEditor.updateAlignment();
    parent.focusRestore();
  }

  insertString(
    /**@type{string} */
    input,
  ) {
    let split = this.splitByCaret();
    let atomNode = mathNodeFactory.atom(this.equationEditor, input);
    atomNode.desiredCaretPosition = input.length;
    let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], atomNode, split[1]]);
    node.normalizeHorizontalMath();
    let parent = this.parent;
    parent.replaceChildAtPosition(this.indexInParent, node);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {KeyHandlerResult} whether default should be prevented. */
  handleKeyDownCases(
    /** @type {KeyboardEvent} */
    e,
  ) {
    let key = e.key;
    if (e.ctrlKey) {
      return this.equationEditor.handleControlKeys(e);
    }
    let shiftHeld = e.shiftKey;
    let backslashPreprocessing = this.processBackslash(key, shiftHeld);
    if (backslashPreprocessing.keyAccountedCarryOutDefaultEvent) {
      return new KeyHandlerResult(false, true);
    }
    let command = backslashPreprocessing.command;

    if (command in latexConstants.operatorWithSuperAndSubscriptBackslashed) {
      let operator = latexConstants.operatorWithSuperAndSubscriptBackslashed[command];
      this.makeOperatorWithSuperscriptAndSubscript(operator);
      return new KeyHandlerResult(true, false);
    }
    if (command in latexConstants.operatorsWithSubscriptBackslashed) {
      let operator = latexConstants.operatorsWithSubscriptBackslashed[command];
      this.makeOperatorWithSubscript(operator);
      return new KeyHandlerResult(true, false);
    }
    if (command in latexConstants.latexBackslashOperatorsBackslashed) {
      let operator = latexConstants.latexBackslashOperatorsBackslashed[command];
      this.makeHorizontalOperator(operator);
      return new KeyHandlerResult(true, false);
    }
    if (
      command in latexConstants.leftDelimiters &&
      !(command in latexConstants.delimitersAmbiguous)
    ) {
      this.makeDelimiterLeft(command);
      return new KeyHandlerResult(true, false);
    }
    if (
      command in latexConstants.rightDelimiters &&
      !(command in latexConstants.delimitersAmbiguous)
    ) {
      this.makeDelimiterRight(command);
      return new KeyHandlerResult(true, false);
    }
    if (command in latexConstants.latexBackslashAtomsEditableBackslashed) {
      let content = latexConstants.latexBackslashAtomsEditableBackslashed[command];
      this.insertString(content);
      return new KeyHandlerResult(true, false);
    }
    switch (backslashPreprocessing.command) {
      case "/":
        this.makeFractionNumerator();
        return new KeyHandlerResult(true, false);
      case "*":
      case "+":
      case "-":
      case "=":
      case ">":
      case "<":
        this.makeHorizontalOperatorCorrectInput(key);
        return new KeyHandlerResult(true, false);
      case "^":
        this.makeBaseWithExponent();
        return new KeyHandlerResult(true, false);
      case "_":
        this.makeBaseWithSubscript();
        return new KeyHandlerResult(true, false);
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        return this.arrow(key, shiftHeld);
      case "|":
        this.makeDelimiterAmbiguous(key);
        return new KeyHandlerResult(true, false);
      case "Enter":
        return new KeyHandlerResult(true, false);
      case "Delete":
        return this.deleteButton();
      case "Backspace":
        return this.backspace();
      case "{":
        this.makeDelimiterLeft("{");
        return new KeyHandlerResult(true, false);
      case "}":
        this.makeDelimiterRight("}");
        return new KeyHandlerResult(true, false);
      case "\\sqrt":
        this.makeSqrt();
        return new KeyHandlerResult(true, false);
      case "\\cancel":
        this.makeCancel();
        return new KeyHandlerResult(true, false);
      case "\\pmatrix":
      case "\\matrix":
        this.makeMatrix(2, 2);
        return new KeyHandlerResult(true, false);
      default:
        return new KeyHandlerResult(false, true);
    }
  }

  arrowRight(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      this.arrow(key, shiftHeld);
      return;
    }
    if (this.isAtomWithRightmostCursor()) {
      this.arrow(key, shiftHeld);
      return;
    }
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

  /** @returns{boolean} Given two atoms, elements whether this is to the left of the other atom. */
  isToTheLeftOf(
    /**@type{MathNode} */ other,
  ) {
    let thisPath = this.getPathToRoot();
    let otherPath = other.getPathToRoot();
    for (let i = 0; i < thisPath.length; i++) {
      if (i >= otherPath.length) {
        // The this element is contained in the other element.
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

  /** @returns{KeyHandlerResult} whether the default should be prevented. */
  arrow(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    if (shiftHeld) {
      return this.equationEditor.arrowShiftHeld(this, key);
    }
    this.equationEditor.resetSelectionLeaveRangesIntact();
    if (this.arrowAbsorbedByAtom(key)) {
      if (key === "ArrowLeft") {
        this.positionCaretBeforeKeyEvents--;
      } else {
        this.positionCaretBeforeKeyEvents++;
      }
      return new KeyHandlerResult(false, false);
    }
    /** @type {MathNodeWithCaretPosition} */
    const toFocus = this.getAtomToFocus(key);
    if (toFocus.element !== null) {
      toFocus.element.positionCaretBeforeKeyEvents = toFocus.element.element.textContent.length + 1;
      toFocus.element.focus(toFocus.position);
    }
    return new KeyHandlerResult(true, true);
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

  /** @returns {MathNodeWithCaretPosition} */
  getAtomToFocus(/** @type {string} */ key) {
    return this.getAtomToFocusFromAction(key, this.type.arrows[key]);
  }

  /** @returns {MathNodeWithCaretPosition} */
  getAtomToFocusFromAction(
    /** @type {string} */ key,
    /** @type {string} */ arrowType,
  ) {
    if (arrowType === arrowMotion.parentForward) {
      if (this.parent === null) {
        return new MathNodeWithCaretPosition(null, - 1);
      }
      return this.parent.getAtomToFocus(key);
    }
    if (arrowType === arrowMotion.firstAtomToTheLeft) {
      return (new MathNodeWithCaretPosition(this, 0)).leftNeighbor();
    }
    if (arrowType === arrowMotion.firstAtomToTheRight) {
      return (new MathNodeWithCaretPosition(this, this.textContentOrInitialContent().length)).rightNeighbor();
    }
    return new MathNodeWithCaretPosition(null, - 1);
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
  firstAtomSiblingOrUncle(
    /**@type{number} */
    direction,
  ) {
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
        if (candidate.isAtomic()) {
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

  /** @returns{MathNode|null} */
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

  /** @returns{MathNode|null} */
  firstAtomToTheLeft() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.firstAtomToTheLeftOf(this.indexInParent);
  }

  /** @returns{MathNode|null} */
  firstAtomToTheLeftOf(
    /** @type {number} */
    childIndex,
  ) {
    for (let i = childIndex - 1; i >= 0; i--) {
      let candidate = this.children[i].rightmostAtomChild();
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

  /**@returns{MathNode|null} */
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

  storeCaretPositionPreExisingRange(
    /** @type{string} */
    key,
    /** @type{boolean} */
    shiftHeld,
  ) {
    let previousPosition = this.positionCaretBeforeKeyEvents;
    let selection = window.getSelection();
    let range = null;
    let rangeClone = null;
    if (selection.rangeCount > 0) {
      range = selection.getRangeAt(0);
    } else {
      range = document.createRange();
    }
    rangeClone = range.cloneRange();
    rangeClone.selectNodeContents(this.element);
    rangeClone.setEnd(range.endContainer, range.endOffset);
    this.positionCaretBeforeKeyEvents = rangeClone.toString().length;// range.endOffset;
    this.selectionLength = range.toString().length;
    this.equationEditor.setLastFocused(this);
    this.equationEditor.positionDebugString = `Computed position: ${this.positionCaretBeforeKeyEvents}.`
    this.equationEditor.positionDebugString += `Range: [${range}], clone: [${rangeClone}], previous position: ${previousPosition}.`;
    this.equationEditor.positionDebugString += `end offset: ${range.endOffset}, start offset: ${range.startOffset}`;
    this.equationEditor.writeDebugInfo(null);
  }

  storeCaretPosition(
    /** @type{string} */
    key,
    /** @type{boolean} */
    shiftHeld,
  ) {
    if (this.type.type === knownTypes.eventCatcher.type) {
      return;
    }
    if (this.type.type !== knownTypes.atom.type) {
      this.positionCaretBeforeKeyEvents = - 1;
      this.selectionLength = 0;
      this.equationEditor.setLastFocused(null);
      return;
    }
    try {
      this.storeCaretPositionPreExisingRange(key, shiftHeld);
    } catch (e) {
      console.log(`Failed to store caret position ${e}.`);
    }
  }

  appendChild(
    /** @type{MathNode} */
    child,
  ) {
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
    if (this.parent !== null) {
      if (this.parent.type.type === knownTypes.matrix.type) {
        return;
      }
    }
    this.ensureEditableAtoms();
  }

  normalizeHorizontalMathRecursive() {
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].normalizeHorizontalMathRecursive();
    }
    if (this.type.type !== knownTypes.horizontalMath.type) {
      return;
    }
    if (this.parent !== null) {
      if (this.parent.type.type === knownTypes.matrix.type) {
        return;
      }
    }
    this.normalizeHorizontalMath();
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

  /**@returns{boolean} Whether normalization of horizontal math is needed. */
  normalizeHorizontalMathInHorizontalMathOnceAccountOneChild(
    /**@type{MathNode[]} */
    normalizedChildren,
    /**@type{MathNode} */
    current,
  ) {
    if (current.type.type !== knownTypes.horizontalMath.type) {
      normalizedChildren.push(current);
      return false;
    }
    for (let j = 0; j < current.children.length; j++) {
      if (current.desiredCaretPosition === 0 && j === 0) {
        current.children[j].desiredCaretPosition = 0;
      } else if (current.desiredCaretPosition > 0 && j === current.children.length - 1) {
        current.children[j].desiredCaretPosition = 1;
      }
      normalizedChildren.push(current.children[j]);
    }
    return true;

  }

  normalizeHorizontalMathInHorizontalMathOnce() {
    let normalizedChildren = [];
    let found = false;
    for (let i = 0; i < this.children.length; i++) {
      if (this.normalizeHorizontalMathInHorizontalMathOnceAccountOneChild(
        normalizedChildren, this.children[i],
      )) {
        found = true;
      }
    }
    if (!found) {
      return false;
    }
    this.replaceChildRangeWithChildren(0, this.children.length - 1, normalizedChildren);
    return true;
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
    if (right.desiredCaretPosition !== - 1) {
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
        next.type.type !== knownTypes.atom.type
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
  removeChildReplaceWithNull(
    /** @type{number} */
    childIndex,
  ) {
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
      if (inputChildren[i] === null) {
        continue;
      }
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

  /** @returns{KeyHandlerResult} whether the default should be prevented. */
  deleteButton() {
    if (this.equationEditor.hasSelection()) {
      return this.equationEditor.deleteSelection(null);
    }
    if (
      this.positionCaretBeforeKeyEvents !== this.element.textContent.length ||
      this.type.type !== knownTypes.atom.type
    ) {
      return new KeyHandlerResult(false, true);
    }
    let cousinAtom = this.firstAtom(1);
    if (cousinAtom === null) {
      return new KeyHandlerResult(false, false);
    }
    let cousinLeftSibling = cousinAtom.previousHorizontalSibling();
    if (cousinLeftSibling !== null) {
      if (cousinLeftSibling.implied) {
        cousinAtom.desiredCaretPosition = 0;
        this.parent.focusRestore();
        return new KeyHandlerResult(true, false);
      }
    }
    let rightSibling = this.nextHorizontalSibling();
    if (rightSibling !== null && rightSibling.type.type === knownTypes.baseWithExponent.type) {
      rightSibling.focus(- 1);
      return new KeyHandlerResult(true, false);
    }
    this.positionCaretBeforeKeyEvents = - 1;
    cousinAtom.positionCaretBeforeKeyEvents = 0;
    return cousinAtom.backspace();
  }

  /** @returns{KeyHandlerResult} whether the default should be prevented. */
  backspace() {
    if (this.equationEditor.hasSelection()) {
      return this.equationEditor.deleteSelection(null);
    }
    if (
      this.positionCaretBeforeKeyEvents !== 0 ||
      this.type.type !== knownTypes.atom.type
    ) {
      return new KeyHandlerResult(false, true);
    }
    this.desiredCaretPosition = 0;
    let result = this.applyBackspaceToTheLeft();
    return new KeyHandlerResult(result, !result);
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftChildWithSiblingWrapper() {
    let parent = this.parent;
    let parentIndexInParent = parent.indexInParent;
    let base = parent.children[0];
    this.children[0].children[0].desiredCaretPosition = 0;
    base.appendChild(this.children[0]);
    base.normalizeHorizontalMath();
    let parentParent = parent.parent;
    parentParent.replaceChildAtPosition(parentIndexInParent, base);
    parentParent.normalizeHorizontalMath();
    parentParent.updateDOM();
    parentParent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftWrapperChildWithSiblingWrapper() {
    let parent = this.parent;
    let parentIndexInParent = parent.indexInParent;
    let horizontal = parent.children[0].children[0];
    horizontal.appendChild(parent.children[1].children[0]);
    horizontal.normalizeHorizontalMath();
    let parentParent = parent.parent;
    parentParent.replaceChildAtPosition(parentIndexInParent, horizontal);
    parentParent.normalizeHorizontalMath();
    parentParent.updateDOM();
    parentParent.focusRestore();
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
    if (this.indexInParent === 0) {
      return this.parent.applyBackspaceToTheLeft();
    }
    if (this.applyBackspaceToTheLeftHorizontalMathParent()) {
      return true;
    }
    this.parent.focusCancelOnce();
    return false;
  }

  applyBackspaceToTheRightAsLeftArrow() {
    let sibling = this.nextHorizontalSibling();
    if (sibling !== null) {
      sibling.focusCancelOnce();
    }
    this.rightmostAtomChild().focus(1);
    return true;
  }

  applyBackspaceToTheLeftAsLeftArrow() {
    let sibling = this.previousHorizontalSibling();
    if (sibling === null) {
      return false;
    }
    sibling.rightmostAtomChild().focus(1);
    return true;
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheRight() {
    return false;
  }

  applyBackspaceToTheRightDelimiter() {
    if (
      this.type.type !== knownTypes.rightDelimiter.type &&
      this.type.type !== knownTypes.leftDelimiter.type
    ) {
      return false;
    }
    let matchingIndex = this.parent.findIndexMatchingDelimiter(this.indexInParent);
    if (matchingIndex === - 1) {
      console.log("Unexpected failure to find matching left parenthesis.");
      return false;
    }
    if (!this.parent.children[matchingIndex].implied) {
      this.implied = true;
      this.children[0].implied = true;
      this.parent.focusCancelOnce();
      this.focus(- 1);
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
    let split = this.splitByCaretEmptyAtoms();
    this.makeHorizontalOperatorFromSplit(key, split);
  }

  makeHorizontalOperatorFromSplit(
    /** @type {string} */
    key,
    /** @type {MathNode[]} */
    split,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorozintalMathParent()) {
      console.log("Warning: horizontal operator made on element not contained in horizontal math.");
      return;
    }
    let parent = this.parent;
    let operator = mathNodeFactory.atomImmutable(this.equationEditor, key);
    parent.replaceChildAtPositionWithChildren(
      this.indexInParent, [
      split[0],
      operator,
      split[1],
    ]);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    operator.focus(1);
  }

  makeDelimiterAmbiguous(
    /** @type{string} */
    ambiguousDelimiter,
  ) {
    this.makeDelimiterCommon(
      ambiguousDelimiter,
      latexConstants.leftRightDelimiterPair[ambiguousDelimiter],
      true,
      true,
    );
  }

  makeDelimiterLeft(
    /** @type{string} */
    leftDelimiter,
  ) {
    this.makeDelimiterCommon(leftDelimiter, latexConstants.leftRightDelimiterPair[leftDelimiter], true, false);
  }

  makeDelimiterRight(
    /** @type{string} */
    rightDelimiter,
  ) {
    this.makeDelimiterCommon(latexConstants.rightLeftDelimiterPair[rightDelimiter], rightDelimiter, false, false);
  }

  makeMatrix(
    /**@type{number} */
    rows,
    /**@type{number} */
    columns,
  ) {
    let split = this.splitByCaretEmptyAtoms();
    this.makeMatrixFromSplit(rows, columns, split);
  }

  makeMatrixFromSplit(
    /**@type{number} */
    rows,
    /**@type{number} */
    columns,
    /** @type {MathNode[]} */
    split,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorozintalMathParent()) {
      console.log("Warning: horizontal operator made on element not contained in horizontal math.");
      return;
    }
    let parent = this.parent;
    let matrix = mathNodeFactory.matrix(this.equationEditor, rows, columns, "");
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

  makeCancel() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.makeCancelFromSplit(splitBySelection);
      return;
    }
    this.makeCancelFromSplit(this.splitByCaret());
  }

  makeSqrt() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.equationEditor.makeSqrtFromSelection(splitBySelection);
    } else {
      this.makeSqrtFromCaret(splitBySelection);
    }
  }

  makeSum() {
    this.makeOperatorWithSuperscriptAndSubscript(latexConstants.latexBackslashAtomsEditable["Sigma"]);
  }

  makeOperatorWithSuperscriptAndSubscript(
    /** @type{string} */
    operator,
  ) {
    let split = this.splitByCaret();
    return this.makeOperatorWithSuperscriptAndSubscriptFromSplit(operator, split)
  }

  makeOperatorWithSuperscriptAndSubscriptFromSplit(
    /** @type {string} */
    operator,
    /** @type {MathNode[] */
    split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let operatorNode = mathNodeFactory.operatorWithSuperAndSubscript(this.equationEditor, operator, null, null);
    if (split[0] === null) {
      parent.replaceChildAtPosition(oldIndex, operatorNode);
    } else {
      parent.replaceChildAtPosition(oldIndex, split[0]);
      oldIndex++;
      parent.insertChildAtPosition(oldIndex, operatorNode);
    }
    if (split[1] !== null) {
      parent.insertChildAtPosition(oldIndex + 1, split[1]);
    }
    parent.ensureEditableAtoms();
    parent.updateDOM();
    operatorNode.focus(1);
  }

  makeOperatorWithSubscript(
    /** @type {string} */
    operator,
  ) {
    let split = this.splitByCaret();
    return this.makeOperatorWithSubscriptFromSplit(operator, split)
  }

  makeOperatorWithSubscriptFromSplit(
    /** @type {string} */
    operator,
    /** @type {MathNode[] */
    split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let operatorNode = mathNodeFactory.operatorWithSubscript(this.equationEditor, operator, null);
    if (split[0] === null) {
      parent.replaceChildAtPosition(oldIndex, operatorNode);
    } else {
      parent.replaceChildAtPosition(oldIndex, split[0]);
      oldIndex++;
      parent.insertChildAtPosition(oldIndex, operatorNode);
    }
    if (split[1] !== null) {
      parent.insertChildAtPosition(oldIndex + 1, split[1]);
    }
    parent.ensureEditableAtoms();
    parent.updateDOM();
    operatorNode.focus(1);
  }

  makeCancelFromSplit(
    /** @type {MathNode[]} */
    split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let cancel = mathNodeFactory.cancel(this.equationEditor, split[1]);
    if (split[0] === null) {
      parent.replaceChildAtPosition(oldIndex, cancel);
    } else {
      parent.replaceChildAtPosition(oldIndex, split[0]);
      parent.insertChildAtPosition(oldIndex + 1, cancel);
    }
    parent.ensureEditableAtoms();
    parent.updateDOM();
    cancel.children[1].focusStandard(- 1);
  }

  makeSqrtFromCaret() {
    let split = this.splitByCaret();
    this.makseSqrtFromSplit(split);
  }

  makseSqrtFromSplit(
    /** @type {MathNode[]} */
    split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let sqrt = mathNodeFactory.sqrt(this.equationEditor, split[1], null);
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

  makeDelimiterCommon(
    /** @type{string} */
    leftDelimiter,
    /** @type{string} */
    rightDelimiter,
    /** @type{boolean} */
    isLeft,
    /** @type{boolean} */
    isAmbiguous,
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
    parent.children[oldIndexInParent].doMakeDelimiterCommon(leftDelimiter, rightDelimiter, positionOperator, isLeft, isAmbiguous);
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

  /**@returns{MathNode|null} */
  findMatchingDelimiter() {
    if (
      this.type.type !== knownTypes.rightDelimiter.type &&
      this.type.type !== knownTypes.leftDelimiter.type
    ) {
      return null;
    }
    if (!this.hasHorozintalMathParent()) {
      return null;
    }
    let parent = this.parent;
    let index = parent.findIndexMatchingDelimiter(this.indexInParent);
    if (index === - 1) {
      return null;
    }
    return parent.children[index];
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
      direction = - 1;
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
      if (openDelimiters === 0) {
        return i;
      }
    }
    return - 1;
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
    return this.splitByPositionChopOffCharacters(position, 0);
  }

  /** @returns {string[]} */
  splitByPositionIntoStringsChopOffCharacters(
    /** @type{number} */
    position,
    /** @type{number} */
    charactersToRemove,
  ) {
    if (!this.isAtomEditable() || position < 0) {
      if (position <= 0) {
        return ["", ""];
      } else {
        return ["", ""];
      }
    }
    let content = this.contentIfAtom();
    let leftContent = content.slice(0, position);
    let rightContent = content.slice(position + charactersToRemove, content.length);
    return [leftContent, rightContent];
  }

  /** @returns {MathNode[]} */
  splitByPositionChopOffCharacters(
    /** @type{number} */
    position,
    /** @type{number} */
    charactersToRemove,
  ) {
    if (!this.isAtomEditable() || position < 0) {
      if (position <= 0) {
        return [null, this];
      } else {
        return [this, null];
      }
    }
    let splitStrings = this.splitByPositionIntoStringsChopOffCharacters(position, charactersToRemove);
    let leftContent = splitStrings[0];
    let rightContent = splitStrings[1]
    let leftNode = mathNodeFactory.atom(this.equationEditor, leftContent);
    let rightNode = mathNodeFactory.atom(this.equationEditor, rightContent);
    if (leftContent === "") {
      return [null, rightNode];
    }
    if (rightContent === "") {
      return [leftNode, null];
    }
    return [leftNode, rightNode];
  }

  doMakeDelimiterCommon(
    /** @type{string} */
    leftDelimiterString,
    /** @type{string} */
    rightDelimiterString,
    /**@type {number} */
    positionOperator,
    /**@type {boolean} */
    isLeft,
    /** @type{boolean} */
    isAmbiguous,
  ) {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      console.log("Warning: making parentheses in non-horizontal math.");
    }
    let oldIndexInParent = this.indexInParent;
    let leftDelimiter = mathNodeFactory.leftDelimiter(
      this.equationEditor,
      leftDelimiterString,
      !isLeft,
    );
    let rightDelimiter = mathNodeFactory.rightDelimiter(
      this.equationEditor,
      rightDelimiterString,
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
    if (isAmbiguous) {
      for (let i = oldIndexInParent; i >= 0; i--) {
        let child = parent.children[i];
        if (
          child.type.type === knownTypes.leftDelimiter.type &&
          child.initialContent === "|" &&
          !child.implied
        ) {
          isLeft = true;
        }
      }
      if (isLeft) {
        if (oldIndexInParent + 1 < parent.children.length) {
          let child = parent.children[oldIndexInParent + 1];
          if (child.type.type === knownTypes.rightDelimiter.type && child.implied === true) {
            isLeft = false;
          }
        }
      }
    }
    if (isLeft) {
      if (positionOperator === 1) {
        indexLeftDelimiter = oldIndexInParent + 1;
      } else if (positionOperator === 0) {
        indexLeftDelimiter = oldIndexInParent + 1;
      } else {
        indexLeftDelimiter = oldIndexInParent;
      }
      if (parent.replaceImpliedLeftDelimiter(leftDelimiterString, indexLeftDelimiter)) {
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
      if (parent.replaceImpliedRightDelimiter(rightDelimiterString, indexRightDelimiter)) {
        return;
      }
      indexLeftDelimiter = parent.findIndexToInsertLeftDelimiter(indexRightDelimiter);
    }
    parent.insertChildAtPosition(indexRightDelimiter, rightDelimiter);
    parent.insertChildAtPosition(indexLeftDelimiter, leftDelimiter);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    if (isLeft) {
      leftDelimiter.focus(1);
    } else {
      rightDelimiter.focus(1);
    }
  }

  /** @returns{boolean} */
  replaceImpliedLeftDelimiter(
    /** @type {string} */
    delimiterString,
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
        continue;
      }
      if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters--;
        if (openDelimiters >= 0) {
          continue;
        }
        if (child.implied) {
          return this.moveDelimiterMarkExplicit(delimiterString, delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @returns{boolean} */
  replaceImpliedRightDelimiter(
    /** @type {string} */
    delimiterString,
    /** @type {number} */
    delimiterIndex,
  ) {
    let openDelimiters = 0;
    for (let i = delimiterIndex; i < this.children.length; i++) {
      let child = this.children[i];
      if (child.type.type === knownTypes.leftDelimiter.type) {
        openDelimiters++;
        continue;
      }
      if (child.type.type === knownTypes.rightDelimiter.type) {
        openDelimiters--;
        if (openDelimiters >= 0) {
          continue;
        }
        if (child.implied) {
          return this.moveDelimiterMarkExplicit(delimiterString, delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @returns{boolean} */
  moveDelimiterMarkExplicit(
    /** @type {string} */
    delimiterString,
    /** @type {number}*/
    toIndex,
    /** @type {number}*/
    fromIndex,
  ) {
    let isLeft = this.children[fromIndex].type.type === knownTypes.leftDelimiter.type;
    this.removeChild(fromIndex);
    if (toIndex > fromIndex) {
      toIndex--;
    }
    let incomingDelimiter = null;
    if (isLeft) {
      incomingDelimiter = mathNodeFactory.leftDelimiter(
        this.equationEditor, delimiterString, false
      );
    } else {
      incomingDelimiter = mathNodeFactory.rightDelimiter(
        this.equationEditor, delimiterString, false
      );
    }
    this.insertChildAtPosition(toIndex, incomingDelimiter);
    this.ensureEditableAtoms();
    this.updateDOM();
    incomingDelimiter.focus(1);
    return true;
  }

  /** @returns{boolean} */
  hasExponentOrSubscriptParent() {
    if (this.parent === null) {
      return false;
    }
    if (this.indexInParent !== 0) {
      return false;
    }
    let type = this.parent.type.type;
    return type === knownTypes.baseWithExponent.type || type === knownTypes.baseWithSubscript.type;
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
      if (!this.hasExponentOrSubscriptParent()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
      }
    }
    for (let i = 0; i < this.children.length - 1; i++) {
      let child = this.children[i];
      let next = this.children[i + 1];
      correctedChildren.push(child);
      if (!child.isAtomEditable() && !next.isAtomEditable()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
      }
    }
    correctedChildren.push(this.children[this.children.length - 1]);
    // Editable atom not inserted in the end if the parent is exponent or subscript.
    if (!correctedChildren[correctedChildren.length - 1].isAtomEditable()) {
      if (!this.hasExponentOrSubscriptParent()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ""));
      }
    }
    if (correctedChildren.length > this.children.length) {
      this.replaceChildRangeWithChildren(0, this.children.length, correctedChildren);
    }
  }

  setTextContent(
    /**@type{string} */
    input,
  ) {
    if (this.element === null) {
      this.initialContent = input;
      return;
    }
    this.element.textContent = input;
  }

  /** @returns {BackslashResult} */
  removeBackslashSequence() {
    let backslashSequence = this.equationEditor.backslashSequence;
    let split = this.splitByPositionIntoStringsChopOffCharacters(
      this.equationEditor.backslashPosition,
      backslashSequence.length,
    );
    this.positionCaretBeforeKeyEvents = this.equationEditor.backslashPosition;
    this.setTextContent(split[0] + split[1]);
    return new BackslashResult(false, backslashSequence);
  }

  makeFractionNumerator() {
    if (this.parent === null) {
      return;
    }
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection === null) {
      this.makeFractionNumeratorFromCaretPosition();
    } else {
      this.equationEditor.makeFractionNumeratorFromSelection(splitBySelection);
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
    return this.horizontalSibling(- 1);
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
    direction,
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
    let previous = this.previousHorizontalSibling();
    if (previous !== null) {
      if (this.isEmptyAtom() || previous.type.type == knownTypes.rightDelimiter.type) {
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
    this.makeBaseDefaultWithExponentNoDelimiter();
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

  makeBaseDefaultWithExponentNoDelimiter() {
    if (!this.isAtomEditable()) {
      this.makeBaseDefaultWithExponent();
      return;
    }
    let content = this.textContentOrInitialContent();
    let numberOfCharactersToSlice = 0;
    for (let i = content.length - 1; i >= 0; i--) {
      let current = content[i];
      if (!latexConstants.isDigit(current)) {
        break;
      }
      numberOfCharactersToSlice++;
    }
    if (numberOfCharactersToSlice === 0) {
      numberOfCharactersToSlice = 1;
    }
    if (numberOfCharactersToSlice >= content.length) {
      this.makeBaseDefaultWithExponent();
      return;
    }
    let leftContent = content.slice(0, content.length - numberOfCharactersToSlice);
    let rightContent = content.slice(content.length - numberOfCharactersToSlice);
    let left = mathNodeFactory.atom(this.equationEditor, leftContent);
    let right = mathNodeFactory.atom(this.equationEditor, rightContent);
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithExponent = mathNodeFactory.baseWithExponent(this.equationEditor, right, null);
    baseWithExponent.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildAtPositionWithChildren(originalIndexInParent, [left, baseWithExponent]);
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

  makeBaseWithSubscript() {
    if (this.parent === null) {
      return;
    }
    if (this.isEmptyAtom()) {
      let previous = this.previousHorizontalSibling();
      if (previous !== null) {
        previous.makeBaseWithSubscript();
        return;
      }
    }
    if (
      this.type.type === knownTypes.rightDelimiter.type
    ) {
      this.makeBaseWithSubscriptRightDelimiter();
      return;
    }
    this.makeBaseDefaultWithSubscript();
  }

  makeBaseWithSubscriptRightDelimiter() {
    let rightIndex = this.indexInParent;
    let originalParent = this.parent;
    let leftIndex = originalParent.findIndexMatchingDelimiter(rightIndex);
    let baseContent = originalParent.children.slice(leftIndex, rightIndex + 1);
    let base = mathNodeFactory.horizontalMathFromArray(
      this.equationEditor,
      baseContent,
    );
    const baseWithSubscript = mathNodeFactory.baseWithSubscript(this.equationEditor, base, null);
    baseWithSubscript.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildRangeWithChildren(leftIndex, rightIndex, [baseWithSubscript]);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  makeBaseDefaultWithSubscript() {
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithSubscript = mathNodeFactory.baseWithSubscript(this.equationEditor, this, null);
    baseWithSubscript.children[1].children[0].children[0].desiredCaretPosition = 0;
    originalParent.replaceChildAtPosition(originalIndexInParent, baseWithSubscript);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /** @returns{boolean} Focuses the cursor on an atom. */
  focusAtom(
    /** @type{number} */
    endToFocus,
  ) {
    if (this.element === null) {
      this.desiredCaretPosition = - 1;
      return false;
    }
    // The call to focus(null) will wipe the desiredCaretPosition.
    let originalDesiredPosition = this.desiredCaretPosition;
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
    this.equationEditor.setLastFocused(this);
    this.desiredCaretPosition = - 1;
    return true;
  }

  /** Focuses the HTMLElement that belongs to the math node.
   * 
   * The endToFocus parameter denotes where the focus should occur.
   * At the moment, negative endToFocus indicates the caret should
   * be on the element's left, and positive endToFocus indicates the caret 
   * should go on the right. Zero indicates to focus at this.positionCaretBeforeKeyEvents.
   * 
   * @returns{boolean} Whether the focus was successful.
   */
  focus(
    /** @type{number} */
    endToFocus,
  ) {
    if (this.type.type === knownTypes.atom.type) {
      return this.focusAtom(endToFocus);
    }
    if (this.type.type === knownTypes.eventCatcher.type) {
      if (this.element === null) {
        return false;
      }
      this.element.focus();
      return true;
    }

    return this.focusStandard(endToFocus);
  }

  focusStandard(
    /** @type{number} */
    endToFocus,
  ) {
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
    return sibling.focus(- endToFocus);
  }

  /** @returns {boolean} whether focus request was found. */
  focusCancelOnce() {
    if (this.desiredCaretPosition !== - 1) {
      this.desiredCaretPosition = - 1;
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
    if (this.desiredCaretPosition !== - 1) {
      this.focus(0);
      return true;
    }
    this.desiredCaretPosition = - 1;
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].focusRestore()) {
        return true;
      }
    }
    let sibling = null;
    if (this.desiredCaretPosition > 0) {
      sibling = this.firstAtomToTheRight();
    } else if (this.desiredCaretPosition === 0) {
      sibling = this.firstAtomToTheLeft();
    }
    if (sibling !== null) {
      sibling.focus(0);
      return true;
    }
    return false;
  }

  setRangeStartEntireElement(
    /** @type{Range}  */
    range,
  ) {
    if (this.element.childNodes.length > 0) {
      range.setStart(this.element.childNodes[0], 0);
      return;
    }
    if (this.element.textContent !== null) {
      range.setStart(this.element, 0);
      return;
    }
    range.setStart(this.element, 0);
  }

  setRangeEndEntireElement(
    /** @type{Range}  */
    range,
  ) {
    if (this.element.childNodes.length > 0) {
      range.setEndAfter(this.element.lastChild);
      return;
    }
    if (this.element.textContent !== null) {
      range.setEnd(this.element, this.element.textContent.length);
      return;
    }
    range.setEnd(this.element, 0);
  }

  setRangeStart(
    /** @type{Range}  */
    range,
    /** @type {number} */
    position,
  ) {
    if (!this.isAtomEditable() || position < 0) {
      this.setRangeStartEntireElement(range);
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
    if (!this.isAtomEditable() || position < 0) {
      this.setRangeEndEntireElement(range);
    } else if (this.element.childNodes.length > 0) {
      range.setEnd(this.element.childNodes[0], position);
    } else {
      range.setEnd(this.element, 0);
    }
  }

  setCaretPosition(
    /** @type {number}*/
    position,
  ) {
    let selection = window.getSelection();
    if (this.element !== null) {
      if (selection.rangeCount <= 0) {
        this.element.focus();
      }
    }
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
      this.setRangeStart(range, 0);
    }
    // console.log(`Position: ${position}, range ${range}, collapseToStart: ${collapseToStart} start offset: ${range.startOffset}, end offset: ${range.endOffset}, text len: ${this.element.textContent.length}`);
    range.collapse(collapseToStart);
    this.equationEditor.resetSelectionDOM();
    selection.addRange(range);
    this.positionCaretBeforeKeyEvents = position;
    //    this.element.focus();
  }

  isDetached() {
    let ancestor = this;
    while (ancestor.parent !== null) {
      ancestor = ancestor.parent;
    }
    return ancestor.type.type !== knownTypes.root.type;
  }

  boundingBoxForChildren(
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    let result = new BoundingBox();
    result.left = this.boundingBox.left + boundingBoxFromParent.left;
    result.top = this.boundingBox.top + boundingBoxFromParent.top;
    result.width = this.boundingBox.width;
    result.height = this.boundingBox.height;
    result.fontSizeInPixels = this.type.fontSizeRatio * boundingBoxFromParent.fontSizeInPixels;
    return result;
  }

  toScalableVectorGraphicsAtomic(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    let graphics = document.createElementNS("http://www.w3.org/2000/svg", "text");
    graphics.setAttributeNS(null, "x", boundingBoxFromParent.left + this.boundingBox.left);
    let height = boundingBoxFromParent.top + this.boundingBox.top + this.boundingBox.height;
    graphics.setAttributeNS(null, "y", height);
    graphics.setAttributeNS(null, "alignment-baseline", "text-after-edge");
    graphics.setAttributeNS(null, "font-family", this.equationEditor.getFontFamily());
    let fontSize = this.type.fontSizeRatio * boundingBoxFromParent.fontSizeInPixels;
    if (fontSize != 0) {
      graphics.setAttributeNS(null, "font-size", `${fontSize}px`);
    }
    if (this.type.colorText !== "") {
      graphics.setAttributeNS(null, "font-color", this.type.colorText);
    }
    graphics.appendChild(document.createTextNode(this.textContentOrInitialContent()));
    container.appendChild(graphics);
  }

  toScalableVectorGraphicsBase(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    let shiftedBoundingBox = this.boundingBoxForChildren(boundingBoxFromParent);
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].toScalableVectorGraphics(container, shiftedBoundingBox);
    }
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
  }

  toString() {
    const result = [];
    if (this.isAtomic()) {
      result.push(this.textContentOrInitialContent());
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

  /**@returns{HTMLElement[]} */
  toHtmlDebugData(
    /** @type{number} */
    indentationLevel,
  ) {
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
    if (this.desiredCaretPosition !== - 1) {
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
    result.push(document.createTextNode(content));
    for (let i = 0; i < this.children.length; i++) {
      let childrenElements = this.children[i].toHtmlDebugData(indentationLevel + 1);
      for (let j = 0; j < childrenElements.length; j++) {
        result.push(childrenElements[j]);
      }
    }
    return result;
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotationAtomic(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let positionCaret = - 1;
    if (options !== null && options.useCaretCommand) {
      positionCaret = this.desiredCaretPosition;
    }
    let result = latexConstants.convertUtf16ToLatex(
      this.contentIfAtomic(),
      positionCaret,
    );
    return result;
  }

  /** @returns{string} */
  toLatex() {
    return this.toLatexWithAnnotation(null).latex;
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let toJoin = [];
    let result = new LatexWithAnnotation("");
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      if (child === null) {
        toJoin.push("[[null]]");
        continue;
      }
      let childLatex = child.toLatexWithAnnotation(options);
      toJoin.push(childLatex.latex);
    }
    result.latex = toJoin.join("");
    return result;
  }

  /** If the element is a matrix, fetches its last row.
   * @returns {MathNode|null}
   */
  getLastMatrixRow() {
    if (this.type.type !== knownTypes.matrix.type) {
      return null;
    }
    let matrixTable = this.children[0].children[1];
    if (matrixTable.children.length === 0) {
      return null;
    }
    return matrixTable.children[matrixTable.children.length - 1];
  }

  getMatrixCell(
    /**@type {number} */
    rowIndex,
    /**@type {number} */
    columnIndex,
  ) {
    if (this.type.type !== knownTypes.matrix.type) {
      return null;
    }
    let matrixTable = this.children[0].children[1];
    if (rowIndex >= matrixTable.children.length) {
      return null;
    }
    let row = matrixTable.children[rowIndex];
    if (columnIndex >= row.children.length) {
      return null;
    }
    return row.children[columnIndex];
  }

  matrixColumnCount() {
    if (this.type.type !== knownTypes.matrix.type) {
      return - 1;
    }
    let matrixTable = this.children[0].children[1];
    let columnCount = 0;
    let numberOfRows = matrixTable.children.length;
    for (let i = 0; i < numberOfRows; i++) {
      columnCount = Math.max(columnCount, matrixTable.children[i].children.length);
    }
    return columnCount;
  }

  /** Ensures that a matrix has rows with equal number of columns. */
  normalizeMatrix() {
    let matrixTable = this.children[0].children[1];
    let columnCount = this.matrixColumnCount();
    let numberOfRows = matrixTable.children.length;
    // Last empty row is ignored. Previous empty rows are preserved.
    if (matrixTable.children[numberOfRows - 1].children.length === 0) {
      numberOfRows--;
      matrixTable.removeChild(numberOfRows);
    }
    // Expand rows to the colum count.
    for (let i = 0; i < matrixTable.children.length; i++) {
      let child = matrixTable.children[i];
      for (let j = child.children.length; j < columnCount; j++) {
        child.appendChild(mathNodeFactory.matrixRowEntry(this.equationEditor, null));
      }
    }
  }
}

class ScalableVectorGraphicsWriter {
  constructor() {
    /**@type{HTMLElement|null} */
    this.underConstruction = null;
  }

  /**@returns{HTMLElement} */
  typeset(
    /**@type{MathNode} */
    input,
  ) {
    this.underConstruction = document.createElementNS("http://www.w3.org/2000/svg", "svg");
    input.toScalableVectorGraphics(this.underConstruction, new BoundingBox());
    this.underConstruction.setAttributeNS(null, "width", input.boundingBox.width);
    this.underConstruction.setAttributeNS(null, "height", input.boundingBox.height);
    return this.underConstruction;
  }
}

class MathNodeAtom extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.atom);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    return this.toLatexWithAnnotationAtomic(options);
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsAtomic(container, boundingBoxFromParent);
  }
}

class MathNodeAtomImmutable extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.atomImmutable);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    return this.toLatexWithAnnotationAtomic(options);
  }

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.children[this.indexInParent + 1].desiredCaretPosition = 0;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsAtomic(container, boundingBoxFromParent);
  }
}

class MathNodeFraction extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.fraction);
    this.extraSpaceBetweenNumeratorAndDenominator = 3;
    this.extraWidth = 4;
  }

  computeDimensions() {
    let numerator = this.children[0];
    let denominator = this.children[1];
    this.boundingBox.fractionLineHeight = numerator.boundingBox.height + 1;
    this.boundingBox.height = numerator.boundingBox.height + denominator.boundingBox.height + this.extraSpaceBetweenNumeratorAndDenominator;
    this.boundingBox.width = Math.max(numerator.boundingBox.width, denominator.boundingBox.width) + this.extraWidth;
    this.boundingBox.needsMiddleAlignment = true;
    numerator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.top = numerator.boundingBox.height + this.extraSpaceBetweenNumeratorAndDenominator;
    numerator.computeBoundingBoxLeftSingleChild();
    denominator.computeBoundingBoxLeftSingleChild();
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = document.createElementNS("http://www.w3.org/2000/svg", "line");
    result.setAttributeNS(null, "x1", boundingBoxFromParent.left + this.boundingBox.left);
    result.setAttributeNS(null, "x2", boundingBoxFromParent.left + this.boundingBox.left + this.boundingBox.width + this.extraWidth / 2);
    result.setAttributeNS(null, "y1", boundingBoxFromParent.top + this.boundingBox.top + this.boundingBox.fractionLineHeight);
    result.setAttributeNS(null, "y2", boundingBoxFromParent.top + this.boundingBox.top + this.boundingBox.fractionLineHeight);
    let color = this.type.colorText;
    if (color === "") {
      color = "black";
    }
    result.setAttributeNS(null, "stroke", color);
    container.appendChild(result);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let numerator = this.children[0].toLatexWithAnnotation(options);
    let denominator = this.children[1].toLatexWithAnnotation(options);
    let result = new LatexWithAnnotation(`\\frac{${numerator.latex}}{${denominator.latex}}`);
    if (this.children.length <= 2) {
      return result;
    }
    // This is not expected to happen: a fraction should have exactly two children.
    // console.log("Warning: corrupt fraction.");
    result.latex += "[";
    for (let i = 2; i < this.children.length; i++) {
      result.latex += this.children[i].toLatex();
    }
    result.latex += "]";
    return result;
  }

  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }
}

class MathNodeNumerator extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.numerator);
  }
  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftWrapperChildWithSiblingWrapper();
  }
}

class MathNodeDenominator extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.denominator);
  }
  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftWrapperChildWithSiblingWrapper();
  }
}

class MathNodeBaseWithExponent extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithExponent);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let base = this.children[0];
    let baseLatex = base.toLatexWithAnnotation(options);
    let exponentLatex = this.children[1].toLatexWithAnnotation(options);
    let useBracesInBase = false;
    if (base.type.type === knownTypes.fraction.type) {
      useBracesInBase = true;
    }
    if (useBracesInBase) {
      return new LatexWithAnnotation(`{${baseLatex.latex}}^{${exponentLatex.latex}}`);
    } else {
      return new LatexWithAnnotation(`${baseLatex.latex}^{${exponentLatex.latex}}`);
    }
  }

  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftAsLeftArrow();
  }

  computeDimensions() {
    let base = this.children[0];
    let exponent = this.children[1];
    let overlapRatio = 0.35;
    if (this.requiresTallExponent(base)) {
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
    let baseWithSubscript = null;
    if (base.type.type === knownTypes.horizontalMath.type) {
      if (base.children.length === 1) {
        if (base.children[0].type.type === knownTypes.baseWithSubscript.type) {
          baseWithSubscript = base.children[0];
        }
      }
    }
    if (baseWithSubscript === null) {
      this.boundingBox.width = base.boundingBox.width + exponent.boundingBox.width;
      this.boundingBox.superScriptWidth = exponent.boundingBox.width;
      exponent.boundingBox.left = base.boundingBox.width;
    } else {
      this.boundingBox.width = Math.max(
        baseWithSubscript.boundingBox.width +
        exponent.boundingBox.width -
        baseWithSubscript.boundingBox.subScriptWidth,
        baseWithSubscript.boundingBox.width,
      );
      this.boundingBox.superScriptWidth = 0;
      this.boundingBox.subScriptWidth = 0;
      exponent.boundingBox.left = base.boundingBox.width - baseWithSubscript.boundingBox.subScriptWidth;

    }
    this.boundingBox.fractionLineHeight = base.boundingBox.top + base.boundingBox.fractionLineHeight;
    this.computeMiddleAlignment();
  }
}

class MathNodeHorizontalMath extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.horizontalMath);
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    let boundingBox = this.boundingBoxForChildren(boundingBoxFromParent);
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.toScalableVectorGraphics(container, boundingBox);
    }
  }

  computeDimensions() {
    this.computeDimensionsHorizontalMath();
  }

  // Same as computeDimensions but easier to code search.
  computeDimensionsHorizontalMath() {
    this.boundingBox = new BoundingBox();
    this.computeDimensionsParenthesesHeight();
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.left = this.boundingBox.width;
      this.boundingBox.width += child.boundingBox.width;
    }
    this.mergeVerticalComponentsBoundingBoxesHorizontallyAlignedElements();
    this.computeMiddleAlignment();
    this.computeLineBreaks();
  }

  computeLineBreaks() {
    if (this.parent === null) {
      return;
    }
    if (this.parent.type.type !== knownTypes.root.type) {
      return;
    }
    if (this.equationEditor.options.lineBreakWidth <= 0) {
      return;
    }
    this.computeLineBreaksPartTwo(this.equationEditor.options.lineBreakWidth);
  }

  computeLineBreaksPartTwo(
    /**@type{number} */
    width,
  ) {
    // this.equationEditor.rootNode.boundingBox.needsMiddleAlignment = false;
    let widthBroken = 0;
    let row = 0;
    this.boundingBox.lineHeight = this.boundingBox.height;
    let finalWidth = 0;
    let hasMoreThanOneLine = false;
    let lastLineOverFlown = false;
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let widthSoFar = child.boundingBox.left + child.boundingBox.width;
      let currentLineWidth = widthSoFar - widthBroken;
      if (lastLineOverFlown && currentLineWidth > 0) {
        hasMoreThanOneLine = true;
        row++;
        lastLineOverFlown = false;
      }
      finalWidth = Math.max(currentLineWidth, finalWidth);
      child.boundingBox.top += this.boundingBox.lineHeight * row;
      child.boundingBox.left -= widthBroken;
      if (currentLineWidth > width) {
        widthBroken = widthSoFar;
        lastLineOverFlown = true;
      }
    }
    this.boundingBox.height = this.boundingBox.lineHeight * (row + 1);
    this.boundingBox.width = finalWidth;
    if (!hasMoreThanOneLine) {
      this.boundingBox.lineHeight = - 1;
    }
  }

}

class MathNodeGenericBox extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.genericMathBox);
  }

  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let childLatex = this.children[0].toLatexWithAnnotation(options);
    let color = this.type.colorText;
    if (color === "") {
      return new LatexWithAnnotation(`{${childLatex.latex}}`)
    }
    return new LatexWithAnnotation(`{\\color{${color}}{${childLatex.latex}}}`);
  }
}

class MathNodeRoot extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.root);
  }

  computeDimensions() {
    this.computeDimensionsStandard();
    this.boundingBox.lineHeight = this.children[0].boundingBox.lineHeight;
    if (!this.boundingBox.needsMiddleAlignment || this.equationEditor.options.editable) {
      return;
    }
    let lineHeight = this.boundingBox.height;
    if (this.boundingBox.lineHeight > 0) {
      // We have a broken-line expression.
      lineHeight = this.boundingBox.lineHeight;
    }
    let bottomDistance = lineHeight - this.boundingBox.fractionLineHeight;
    if (bottomDistance > this.boundingBox.fractionLineHeight) {
      this.boundingBox.height += bottomDistance * 2 - lineHeight;
      this.boundingBox.top += bottomDistance - this.boundingBox.fractionLineHeight;
      this.boundingBox.fractionLineHeight = bottomDistance;
    } else {
      this.boundingBox.height += this.boundingBox.fractionLineHeight * 2 - lineHeight;
    }
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    let boundingBox = new BoundingBox();
    boundingBox.top = boundingBoxFromParent.top;
    boundingBox.left = boundingBoxFromParent.left;
    boundingBox.fontSizeInPixels = parseInt(window.getComputedStyle(this.element, null).getPropertyValue("font-size"));
    this.children[0].toScalableVectorGraphics(container, boundingBox);
  }
}

class MathNodeError extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.error);
  }
  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.children[this.indexInParent + 1].desiredCaretPosition = 0;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeCancel extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.cancel);
  }

  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let childLatex = this.children[1].toLatexWithAnnotation(options);
    return new LatexWithAnnotation(`\\cancel{${childLatex.latex}}`);
  }

  applyBackspaceToTheRight() {
    this.applyBackspaceToTheRightAsLeftArrow();
  }

  computeDimensions() {
    let content = this.children[1];
    content.boundingBox.left = 0;
    this.boundingBox.height = content.boundingBox.height;
    this.boundingBox.width = content.boundingBox.width;
    this.boundingBox.fractionLineHeight = content.boundingBox.fractionLineHeight;
    let cancelSign = this.children[0];
    cancelSign.boundingBox.width = 4;
    cancelSign.boundingBox.height = content.boundingBox.height;
    cancelSign.boundingBox.left = 0;
    if (cancelSign.element !== null) {
      cancelSign.element.style.width = cancelSign.boundingBox.width;
      cancelSign.element.style.height = cancelSign.boundingBox.height;
    }
    let stretch = content.boundingBox.width / cancelSign.boundingBox.height;
    cancelSign.boundingBox.transformOrigin = "top left";
    cancelSign.boundingBox.transform = `matrix(1,0,${-stretch},1,${content.boundingBox.width},0)`;
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let leftX = boundingBoxFromParent.left + this.boundingBox.left;
    let leftY = boundingBoxFromParent.top + this.boundingBox.top + this.boundingBox.height;
    let rightX = boundingBoxFromParent.left + this.boundingBox.left + this.boundingBox.width;
    let rightY = boundingBoxFromParent.top + this.boundingBox.top;

    let result = document.createElementNS("http://www.w3.org/2000/svg", "line");
    result.setAttributeNS(null, "x1", leftX);
    result.setAttributeNS(null, "y1", leftY);
    result.setAttributeNS(null, "x2", rightX);
    result.setAttributeNS(null, "y2", rightY);
    let color = this.type.colorText;
    if (color === "") {
      color = "black";
    }
    result.setAttributeNS(null, "stroke", color);
    container.appendChild(result);
  }
}

class MathNodeCancelUnderBox extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.cancelUnderBox);
  }
  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    let cancel = this.parent;
    let indexCancel = cancel.indexInParent;
    let content = this.children[0];
    content.children[0].desiredCaretPosition = 0;
    let parent = cancel.parent;
    parent.replaceChildAtPosition(indexCancel, content.children[0]);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeSqrtSign extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrtSign);
  }

  verticallyStretchSqrt(
    /** @type {number}*/
    heightToEnclose,
  ) {
    let decoration = this.children[0];
    let left = this.children[1];
    let right = this.children[2];
    let heightRatioRight = 0.5;
    let heightToWidthRatio = 0.1;
    let maxWidthHalfCheck = 10;
    let preferredWidth = heightToEnclose * heightToWidthRatio;

    decoration.boundingBox.width = 3;
    decoration.boundingBox.height = 1;
    decoration.boundingBox.top = heightToEnclose * (1 - heightRatioRight);
    if (preferredWidth > maxWidthHalfCheck) {
      preferredWidth = maxWidthHalfCheck;
    }

    left.boundingBox.height = heightToEnclose * heightRatioRight;
    left.boundingBox.width = preferredWidth;
    left.boundingBox.top = heightToEnclose * (1 - heightRatioRight);
    left.boundingBox.left = decoration.boundingBox.width - 1;

    right.boundingBox.height = heightToEnclose;
    right.boundingBox.width = 1;
    right.boundingBox.left = left.boundingBox.left + preferredWidth;
    right.boundingBox.top = 0;

    left.boundingBox.transformOrigin = "top left";
    right.boundingBox.transformOrigin = "top left";
    let leftSkew = left.boundingBox.width / left.boundingBox.height;
    left.boundingBox.transform = `matrix(1,0,${leftSkew},1,0,0)`;
    let rightSkew = preferredWidth / right.boundingBox.height;
    right.boundingBox.transform = `matrix(1,0,${-rightSkew},1,${preferredWidth},0)`;
    this.boundingBox.height = heightToEnclose;
    this.boundingBox.width = left.boundingBox.width + preferredWidth + decoration.boundingBox.width;
  }
}

class MathNodeSqrt extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrt);
    this.sqrtProportionOverlapped = 0.7;
    this.widthSqrtSign = 0.99;
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    if (this.element === null) {
      return new LatexWithAnnotation("[null(]");
    }
    let result = new LatexWithAnnotation("");
    let exponent = null;
    let underTheRadical = null;
    if (this.children.length > 0) {
      exponent = this.children[0].toLatexWithAnnotation(options);
    }
    if (this.children.length > 2) {
      underTheRadical = this.children[2].toLatexWithAnnotation(options);
    }
    if (exponent !== null) {
      if (exponent.latex !== "") {
        result.latex = `\\sqrt[${exponent.latex}]{${underTheRadical.latex}}`;
        return result;
      }
    }
    result.latex = `\\sqrt{${underTheRadical.latex}}`;
    return result;
  }

  sqrtSignLeftShift() {
    let radicalExponentBox = this.children[0];
    let sqrtSign = this.children[1];
    return Math.max(0, radicalExponentBox.boundingBox.width - sqrtSign.boundingBox.width * this.sqrtProportionOverlapped);
  }

  computeDimensions() {
    let radicalExponentBox = this.children[0];
    let sqrtSign = this.children[1];
    let underTheRadical = this.children[2];
    // The proportion of the width of the sqrt sign that overlaps with the radical exponent.
    // The top of the sqrt sign may not connect perfectly with the overline of the under-the-radical content.
    // The following variable compensates for that.
    sqrtSign.verticallyStretchSqrt(underTheRadical.boundingBox.height);
    radicalExponentBox.boundingBox.top = - 0.15 * radicalExponentBox.boundingBox.height;
    sqrtSign.boundingBox.left = this.sqrtSignLeftShift();
    underTheRadical.boundingBox.left = sqrtSign.boundingBox.left + sqrtSign.boundingBox.width * this.widthSqrtSign;
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = underTheRadical.boundingBox.height * 1.15;
    this.boundingBox.fractionLineHeight = underTheRadical.boundingBox.fractionLineHeight + 2.2;
    let extraSpaceAfterRadical = 4;
    this.boundingBox.width = underTheRadical.boundingBox.left + underTheRadical.boundingBox.width + extraSpaceAfterRadical;
    this.boundingBox.needsMiddleAlignment = underTheRadical.boundingBox.needsMiddleAlignment;
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = document.createElementNS("http://www.w3.org/2000/svg", "path");
    let sqrtSign = this.children[1];
    let decoration = sqrtSign.children[0];
    let leftStroke = sqrtSign.children[1];
    let rightStroke = sqrtSign.children[2];
    let overline = this.children[2];
    let sqrtTop = boundingBoxFromParent.top + this.boundingBox.top + sqrtSign.boundingBox.top;
    let sqrtLeft = boundingBoxFromParent.left + this.boundingBox.left + sqrtSign.boundingBox.left;
    let bottom = sqrtTop + leftStroke.boundingBox.top + leftStroke.boundingBox.height;
    let midX = sqrtLeft + rightStroke.boundingBox.left;
    let decorationY = sqrtTop + decoration.boundingBox.top;
    let decorationLeft = sqrtLeft + decoration.boundingBox.left;
    let decorationRight = decorationLeft + decoration.boundingBox.width;
    let topY = sqrtTop + rightStroke.boundingBox.top;
    let rightX = boundingBoxFromParent.left + this.boundingBox.left + overline.boundingBox.left;
    let overlineRight = boundingBoxFromParent.left + this.boundingBox.left + overline.boundingBox.left + overline.boundingBox.width;
    let path = `M ${decorationLeft} ${decorationY} L ${decorationRight} ${decorationY} `;
    path += `L ${decorationRight} ${decorationY} `;
    path += `L ${midX} ${bottom} `;
    path += `L ${rightX} ${topY} `;
    path += `L ${overlineRight} ${topY} `;
    result.setAttributeNS(null, "d", path);
    let color = this.type.colorText;
    if (color === "") {
      color = "black";
    }
    result.setAttributeNS(null, "stroke", color);
    result.setAttributeNS(null, "fill", "none");
    container.appendChild(result);
  }


  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }
}

class MathNodeOverLine extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.overLinedBox);
  }
}

class MathNodeFormInput extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type{string} */
    value,
    /**@type{string} */
    name,
  ) {
    super(equationEditor, knownTypes.formInput);
    this.value = value;
    this.name = name;
    this.initialContent = this.value;
  }

  computeDimensions() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.equationEditor.standardAtomHeight;
    if (this.boundingBox.height === 0) {
      this.boundingBox.height = 20;
    }
    this.boundingBox.width = this.value.length * this.boundingBox.height;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }
}

class MathNodeOverBrace extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.overBrace);
  }
}

class MathNodeExponent extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.exponent);
  }

  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftChildWithSiblingWrapper();
  }
}

class MathNodeSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.subscript);
  }

  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftChildWithSiblingWrapper();
  }
}

class MathNodeBaseWithSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithSubscript);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let result = null;
    let numerator = this.children[0].toLatexWithAnnotation(options);
    let denominator = this.children[1].toLatexWithAnnotation(options);
    if (this.children[0].isAtomicNonEmptyOrHorizontalMathWithNonEmptyAtomic()) {
      result = new LatexWithAnnotation(`${numerator.latex}_{${denominator.latex}}`);
    } else {
      result = new LatexWithAnnotation(`{${numerator.latex}}_{${denominator.latex}}`);
    }
    return result;
  }

  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }
}

class MathNodeLeftDelimiter extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.leftDelimiter);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    /**@type{LatexWithAnnotation} */
    let result = null;
    if (this.children.length === 0) {
      result = new LatexWithAnnotation("\\left.");
    } else if (this.extraData === "{") {
      result = new LatexWithAnnotation("\\{");
    } else {
      result = new LatexWithAnnotation(this.extraData);
    }
    return result;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightDelimiter();
  }

  computeDimensions() {
    this.computeDimensionsDelimiter();
  }
}

class MathNodeRightDelimiter extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.rightDelimiter);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    /**@type{LatexWithAnnotation} */
    let result = null;
    if (this.children.length === 0) {
      result = new LatexWithAnnotation("\\right.");
    } else if (this.extraData === "{") {
      result = new LatexWithAnnotation("\\}");
    } else {
      result = new LatexWithAnnotation(this.extraData);
    }
    return result;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightDelimiter();
  }

  computeDimensions() {
    this.computeDimensionsDelimiter();
  }
}

class MathNodeDelimiterMark extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left ? knownTypes.leftDelimiterMark : knownTypes.rightDelimiterMark);
    this.parenthesisThickness = 0;
  }

  verticallyStretchCommon(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    let scaleHeight = 1.1;
    this.parenthesisThickness = heightToEnclose / 24;
    this.parenthesisThickness = Math.min(3, this.parenthesisThickness);
    this.parenthesisThickness = Math.max(1.5, this.parenthesisThickness);
    heightToEnclose = Math.max(2 * fractionLineHeightEnclosed, 2 * (heightToEnclose - fractionLineHeightEnclosed));
    this.boundingBox.top = 0;
    this.boundingBox.height = heightToEnclose * scaleHeight;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

}

class MathNodeAbsoluteValue extends MathNodeDelimiterMark {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    this.boundingBox.width = Math.max(heightToEnclose / 6, 3);
    this.element.style.borderWidth = `${this.parenthesisThickness}px`;
  }
}

class MathNodeSquareBrackets extends MathNodeDelimiterMark {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    this.parenthesisThickness = heightToEnclose / 30;
    this.parenthesisThickness = Math.max(0.5, this.parenthesisThickness);
    this.parenthesisThickness = Math.min(3, this.parenthesisThickness);
    this.boundingBox.width = heightToEnclose / 5;
    this.boundingBox.width = Math.min(this.boundingBox.width, 10);
    this.boundingBox.width = Math.max(this.boundingBox.width, 2);
    let shift = 1;
    if (this.left) {
      this.boundingBox.left = shift;
    } else {
      this.boundingBox.left = - shift;
    }
    if (this.element !== null) {
      this.element.style.borderTop = `solid`;
      this.element.style.borderBottom = `solid`;
      this.element.style.borderWidth = `${this.parenthesisThickness}px`;
    }
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = document.createElementNS("http://www.w3.org/2000/svg", "path");
    let xStart = boundingBoxFromParent.left + this.boundingBox.left;
    let xMiddle = boundingBoxFromParent.left + this.boundingBox.left;
    if (this.left) {
      xStart += this.boundingBox.width;
    } else {
      xMiddle += this.boundingBox.width;
    }
    let yHigh = boundingBoxFromParent.top + this.boundingBox.top;
    let yLow = yHigh + this.boundingBox.height - this.parenthesisThickness;
    yHigh += this.parenthesisThickness;

    let command = `M ${xStart} ${yLow} L ${xMiddle} ${yLow} L ${xMiddle} ${yHigh} L ${xStart} ${yHigh}`; // move to point.
    result.setAttributeNS(null, "d", command);
    let color = this.type.colorText;
    if (color === "") {
      color = "black";
    }
    result.setAttributeNS(null, "stroke", color);
    result.setAttributeNS(null, "fill", "none");
    container.appendChild(result);
  }
}

class MathNodeAngleBrackets extends MathNodeDelimiterMark {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    let topBar = this.children[0];
    let bottomBar = this.children[1];
    let halfHeight = this.boundingBox.height / 2;
    let width = heightToEnclose / 6;
    topBar.boundingBox.height = halfHeight;
    bottomBar.boundingBox.height = halfHeight;
    topBar.boundingBox.width = this.parenthesisThickness;
    bottomBar.boundingBox.width = this.parenthesisThickness;
    bottomBar.boundingBox.top = halfHeight;

    let margin = 2;
    topBar.boundingBox.left = margin;
    bottomBar.boundingBox.left = margin;

    topBar.boundingBox.transformOrigin = "top left";
    bottomBar.boundingBox.transformOrigin = "top left";
    let scale = width / halfHeight;

    if (this.left) {
      topBar.boundingBox.transform = `matrix(1,0,${-scale},1,${width},0)`;
      bottomBar.boundingBox.transform = `matrix(1,0,${scale},1,0,0)`;
    } else {
      topBar.boundingBox.transform = `matrix(1,0,${scale},1,0,0)`;
      bottomBar.boundingBox.transform = `matrix(1,0,${-scale},1,${width},0)`;
    }
    this.element.style.borderLeft = "";
    this.element.style.borderRight = "";
    this.boundingBox.width = Math.max(width + 2 * margin + 2 * this.parenthesisThickness, 3);
    this.element.style.borderWidth = `${this.parenthesisThickness}px`;
  }
}

class MathNodeParenthesis extends MathNodeDelimiterMark {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    this.boundingBox.width = Math.min(this.boundingBox.height / 4, 20);
    this.element.style.borderLeft = "";
    this.element.style.borderRight = "";
    let scale = this.scale();

    let radius = this.boundingBox.height / 2;
    let child = this.children[0];
    child.element.style.borderRadius = `${radius}px`;
    child.element.style.borderWidth = `${this.borderWidth()}px`;
    child.boundingBox.width = this.boundingBox.height;
    child.boundingBox.height = this.boundingBox.height;
    child.boundingBox.transformOrigin = "top left";
    child.boundingBox.left += this.horizontalShift();
    child.boundingBox.transform = `matrix(${scale},0,0,1,0,0)`;
  }

  scale() {
    return this.boundingBox.width / this.boundingBox.height;
  }

  borderWidth() {
    return this.parenthesisThickness / this.scale();
  }

  horizontalShift() {
    let shift = this.boundingBox.width / 3.5;
    if (this.left) {
      return shift;
    } else {
      return - shift;
    }
  }

  toScalableVectorGraphics(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsParenthesis(container, boundingBoxFromParent);
  }

  // Same as toScalableVectorGraphics but easier to code search.
  toScalableVectorGraphicsParenthesis(
    /**@type{SVGElement}*/
    container,
    /**@type{BoundingBox} */
    boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = document.createElementNS("http://www.w3.org/2000/svg", "path");
    let x = boundingBoxFromParent.left + this.boundingBox.left + this.horizontalShift();
    if (!this.left) {
      x += this.boundingBox.width;
    }
    let startY = boundingBoxFromParent.top + this.boundingBox.top;
    let endY = boundingBoxFromParent.top + this.boundingBox.height;
    if (this.left) {
      let swap = startY;
      startY = endY;
      endY = swap;
    }
    let rx = this.boundingBox.width / 2;
    let rxInner = rx - this.parenthesisThickness;
    if (rxInner < 0) {
      rxInner = rx / 2;
    }
    let ry = this.boundingBox.height / 2;
    let degrees = 180;

    let moveBottom = `M ${x} ${startY}`; // move to point.
    let arcOuter = `A ${rx} ${ry} ${degrees} 1 1 ${x} ${endY}`;
    let arcInner = `A ${rxInner} ${ry} ${degrees} 0 0 ${x} ${startY}`;
    let command = `${moveBottom} ${arcOuter} ${arcInner}`;
    result.setAttributeNS(null, "d", command);
    let color = this.type.colorText;
    if (color === "") {
      color = "black";
    }
    result.setAttributeNS(null, "stroke", "none");
    result.setAttributeNS(null, "fill", color);
    container.appendChild(result);
  }
}

class MathNodeCurlyBrace extends MathNodeDelimiterMark {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
    /**@type {boolean} */
    left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
    /** @type {number}*/
    heightToEnclose,
    /** @type {number}*/
    fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    let topBar = this.children[1];
    let bottomBar = this.children[4];
    let radius = Math.floor(this.boundingBox.height / 12);
    if (radius < 2) {
      radius = 2;
    }
    let heightBar = (this.boundingBox.height - 4 * radius) / 2;
    this.boundingBox.width = radius * 2 + 1;
    this.boundingBox.height += radius;
    let borderWidthString = `${this.parenthesisThickness}px`;

    topBar.boundingBox.width = this.parenthesisThickness;
    topBar.boundingBox.top = radius;
    topBar.boundingBox.height = heightBar;
    topBar.boundingBox.left = radius;
    topBar.element.style.borderWidth = borderWidthString;

    bottomBar.boundingBox.width = this.parenthesisThickness;
    bottomBar.boundingBox.height = heightBar;
    bottomBar.boundingBox.top = heightBar + radius * 3;
    bottomBar.boundingBox.left = radius;
    bottomBar.element.style.borderWidth = borderWidthString;
    let topQuarterCircle = this.children[0];
    let topMiddleQuarterCircle = this.children[2];
    let bottomMiddleQuarterCircle = this.children[3];
    let bottomQuarterCircle = this.children[5];
    let allQuarterCircles = [topQuarterCircle, topMiddleQuarterCircle, bottomMiddleQuarterCircle, bottomQuarterCircle];
    for (let i = 0; i < allQuarterCircles.length; i++) {
      let quarterCircle = allQuarterCircles[i];
      quarterCircle.boundingBox.width = radius;
      quarterCircle.boundingBox.height = radius;
      quarterCircle.element.style.borderWidth = borderWidthString;
    }
    let radiusString = `${radius}px`;
    topQuarterCircle.boundingBox.top = 0;
    topQuarterCircle.boundingBox.left = this.left ? radius : 0;

    topMiddleQuarterCircle.boundingBox.top = heightBar + radius;
    topMiddleQuarterCircle.boundingBox.left = this.left ? 0 : radius;

    bottomMiddleQuarterCircle.boundingBox.top = heightBar + 2 * radius;
    bottomMiddleQuarterCircle.boundingBox.left = this.left ? 0 : radiusString;

    bottomQuarterCircle.boundingBox.top = 2 * heightBar + 3 * radius;
    bottomQuarterCircle.boundingBox.left = this.left ? radius : 0;
    if (this.element !== null) {
      this.element.style.borderLeft = "";
      this.element.style.borderRight = "";
    }
    let borderStyleString = `solid ${borderWidthString}`;
    if (this.left) {
      topQuarterCircle.element.style.borderTopLeftRadius = radiusString;
      topQuarterCircle.element.style.borderLeft = borderStyleString;

      topMiddleQuarterCircle.element.style.borderBottomRightRadius = radiusString;
      topMiddleQuarterCircle.element.style.borderRight = borderStyleString;

      bottomMiddleQuarterCircle.element.style.borderTopRightRadius = radiusString;
      bottomMiddleQuarterCircle.element.style.borderRight = borderStyleString;

      bottomQuarterCircle.element.style.borderBottomLeftRadius = radiusString;
      bottomQuarterCircle.element.style.borderLeft = borderStyleString;
    } else {
      topQuarterCircle.element.style.borderTopRightRadius = radiusString;
      topQuarterCircle.element.style.borderRight = borderStyleString;

      topMiddleQuarterCircle.element.style.borderBottomLeftRadius = radiusString;
      topMiddleQuarterCircle.element.style.borderLeft = borderStyleString;

      bottomMiddleQuarterCircle.element.style.borderTopLeftRadius = radiusString;
      bottomMiddleQuarterCircle.element.style.borderLeft = borderStyleString;

      bottomQuarterCircle.element.style.borderBottomRightRadius = radiusString;
      bottomQuarterCircle.element.style.borderRight = borderStyleString;
    }
  }
}

class MathNodeMatrix extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.matrix);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    if (this.element === null) {
      return new LatexWithAnnotation("[null)]");
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
        currentRowStrings.push(entry.toLatexWithAnnotation(options).latex);
      }
      rows.push(currentRowStrings.join("&"));
    }
    result.push(rows.join("\\\\"));
    result.push("\\end{pmatrix}");
    return new LatexWithAnnotation(result.join(""));
  }

  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeRadicalUnderBox extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.radicalUnderBox);
  }
  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    let sqrt = this.parent;
    let indexSqrt = sqrt.indexInParent;
    let exponent = sqrt.children[0].children[0];
    this.children[0].children[0].desiredCaretPosition = 0;
    let parent = sqrt.parent;
    parent.replaceChildAtPositionWithChildren(indexSqrt, [exponent, sqrt.children[2].children[0]]);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeMatrixRow extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.matrixRow);
  }
}

class MathNodeRowEntry extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.matrixRowEntry);
  }
}

class MathNodeVerticalLine extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.verticalLineInTable);
  }
}

class MathNodeOperatorWithSuperAndSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorWithSuperAndSubscript);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    let top = this.children[0].toLatexWithAnnotation(options).latex;
    let result = this.children[1].toLatexWithAnnotation(options).latex;
    let bottom = this.children[2].toLatexWithAnnotation(options).latex;
    if (bottom !== "") {
      result += `_{${bottom}}`;
    }
    if (top !== "") {
      result += `^{${top}}`;
    }
    return new LatexWithAnnotation(result);
  }

  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftEndOfOperatorSubscript() {
    let superscript = this.children[0];
    let subscript = this.children[2];
    let horizontal = mathNodeFactory.horizontalMathFromArray(
      this.equationEditor,
      [superscript.children[0], subscript.children[0]],
    );
    horizontal.normalizeHorizontalMath();
    let parent = this.parent;
    let indexOperator = this.indexInParent;
    parent.replaceChildAtPosition(indexOperator, horizontal);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** Computes the dimensions of the DOM.
   * @override 
   */
  computeDimensions() {
    let superscript = this.children[0];
    let operator = this.children[1];
    let subscript = this.children[2];
    let extraSpaceBelowSuperscriptPercent = 0.1;
    let extraSpaceBelowOperatorPercent = 0.1;
    let operatorContent = operator.children[0].initialContent;
    let extraSpaceBelowOperatorCustom = {
      // integral
      "\u222B": - 0.1,
    };
    if (operatorContent in extraSpaceBelowOperatorCustom) {
      extraSpaceBelowOperatorPercent = extraSpaceBelowOperatorCustom[operatorContent];
    }
    let extraSpaceBelowSuperscript = operator.boundingBox.height * extraSpaceBelowSuperscriptPercent;
    let extraSpaceBelowOperator = operator.boundingBox.height * extraSpaceBelowOperatorPercent;
    superscript.boundingBox.top = 0;
    operator.boundingBox.top = superscript.boundingBox.height + extraSpaceBelowSuperscript;
    subscript.boundingBox.top = superscript.boundingBox.height + extraSpaceBelowSuperscript + operator.boundingBox.height + extraSpaceBelowOperator;
    this.boundingBox.height =
      superscript.boundingBox.height + operator.boundingBox.height + subscript.boundingBox.height +
      extraSpaceBelowOperator + extraSpaceBelowSuperscript;
    this.boundingBox.width = Math.max(superscript.boundingBox.width, operator.boundingBox.width, subscript.boundingBox.width);
    this.boundingBox.fractionLineHeight = superscript.boundingBox.height + extraSpaceBelowSuperscript + operator.boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
    superscript.boundingBox.width = this.boundingBox.width;
    subscript.boundingBox.width = this.boundingBox.width;
    operator.boundingBox.width = this.boundingBox.width;
    superscript.computeBoundingBoxLeftSingleChild();
    operator.computeBoundingBoxLeftSingleChild();
    subscript.computeBoundingBoxLeftSingleChild();
  }
}

class MathNodeOperatorStandalone extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorStandalone);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation(
    /**@type{ToLatexOptions|null} */
    options,
  ) {
    latexConstants.computeUtf16ToLatexMap();
    const content = this.textContentOrInitialContent();
    if (content in latexConstants.utf16ToLatexMap) {
      return new LatexWithAnnotation(latexConstants.utf16ToLatexMap[content]);
    }
    return new LatexWithAnnotation(`${content}`);
  }
}

class MathNodeOperatorWithSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorWithSubscript);
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftEndOfOperatorSubscript() {
    let subscript = this.children[1];
    let horizontal = mathNodeFactory.horizontalMathFromArray(
      this.equationEditor,
      [subscript.children[0]],
    );
    horizontal.normalizeHorizontalMath();
    let parent = this.parent;
    let indexOperator = this.indexInParent;
    parent.replaceChildAtPosition(indexOperator, horizontal);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /**
   * @override
   */
  computeDimensions() {
    let operator = this.children[0];
    let subscript = this.children[1];
    let extraSpaceBelowOperatorPercent = 0.1;
    let extraSpaceBelowOperator = operator.boundingBox.height * extraSpaceBelowOperatorPercent;
    operator.boundingBox.top = 0;
    subscript.boundingBox.top = operator.boundingBox.height + extraSpaceBelowOperator;
    this.boundingBox.height = operator.boundingBox.height + extraSpaceBelowOperator + subscript.boundingBox.height;
    this.boundingBox.width = Math.max(operator.boundingBox.width, subscript.boundingBox.width);
    this.boundingBox.fractionLineHeight = operator.boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
    subscript.boundingBox.width = this.boundingBox.width;
    operator.boundingBox.width = this.boundingBox.width;
    operator.computeBoundingBoxLeftSingleChild();
    subscript.computeBoundingBoxLeftSingleChild();
  }
}

class MathNodeOperatorSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorSubscript);
  }

  applyBackspaceToTheLeft() {
    return this.parent.applyBackspaceToTheLeftEndOfOperatorSubscript();
  }
}

class MathNodeOperatorSuperscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorSuperscript);
  }

  applyBackspaceToTheLeft() {
    return this.parent.applyBackspaceToTheLeftEndOfOperatorSubscript();
  }
}

class VerticalBarData {
  constructor(
    /**@type {number} */
    columnIndex,
    /**@type {number} */
    stripIndex,
  ) {
    this.columnIndex = columnIndex;
    this.stripIndex = stripIndex;
  }
}

class LatexColumnStyleIterator {
  constructor(/**@type{string} */ style) {
    this.style = style;
    this.nextStyleStartCharacter = 0;
    this.currentColumnAlignment = "";
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    this.exhausted = false;
  }

  reset() {
    this.nextStyleStartCharacter = 0;
    this.currentColumnAlignment = "";
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    this.exhausted = false;
  }

  next() {
    this.currentColumnAlignment = "";
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    if (this.nextStyleStartCharacter >= this.style.length) {
      this.exhausted = true;
      return;
    }
    for (let i = this.nextStyleStartCharacter; i < this.style.length; i++) {
      let next = this.style[i];
      if (next === "c" || next === "r" || next === "l") {
        if (this.currentColumnAlignment !== "") {
          return;
        }
        this.currentColumnAlignment = next;
      }
      this.nextStyleStartCharacter = i + 1;
      if (next === "|") {
        if (this.currentColumnAlignment === "") {
          this.verticalBarLeftCount++;
        } else {
          this.verticalBarRightCount++;
        }
      }
    }
  }

  applyStyleToMatrix(
    /** @type{MathNode} */
    matrix,
  ) {
    if (matrix.type.type !== knownTypes.matrix.type) {
      return;
    }
    let matrixTable = matrix.children[0].children[1];
    // Expand rows to the colum count.
    for (let i = 0; i < matrixTable.children.length; i++) {
      let row = matrixTable.children[i];
      this.applyStyleToRow(row);
    }
  }

  applyStyleToRow(
    /** @type{MathNode} */
    row,
  ) {
    this.reset();
    this.next();
    for (let j = 0; j < row.children.length && !this.isExhausted(); j++, this.next()) {
      row.children[j].latexExtraStyle = this.currentColumnAlignment;
    }
  }

  /** @returns{MathNode[]} Given matrix column style such as {c|c}, generates the vertical bars.
   * Example: \begin{array}{c|c}1&2\end{array}.
   */
  verticalSeparators(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    this.reset();
    this.next();
    let result = [];
    let columnCount = 0;
    for (; !this.isExhausted(); this.next()) {
      for (let i = 0; i < this.verticalBarLeftCount; i++) {
        result.push(mathNodeFactory.matrixVerticalLine(
          equationEditor, columnCount, i,
        ));
      }
      columnCount++;
      for (let i = 0; i < this.verticalBarRightCount; i++) {
        result.push(mathNodeFactory.matrixVerticalLine(
          equationEditor, columnCount, i,
        ));
      }
    }
    return result;
  }

  /** @returns {string} */
  borderFromVerticalBarCount(
    /**@type {number} */
    verticalBars,
  ) {
    if (verticalBars <= 0) {
      return "";
    }
    if (verticalBars === 1) {
      return "1px solid black";
    }
    return "1px double black";
  }

  /**@returns {boolean} */
  isExhausted() {
    return this.exhausted;
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

class MathTagData {
  constructor(
    /**@type {number} */
    startIndex,
    /**@type {number} */
    startIndexData,
    /**@type {number} */
    closeIndexData,
    /**@type {number} */
    closeIndex,
    /**@type{string} */
    ambientContent,
  ) {
    this.startIndex = startIndex;
    this.startIndexData = startIndexData;
    this.closeIndexData = closeIndexData;
    this.closeIndex = closeIndex;
    this.content = ambientContent.slice(this.startIndexData, closeIndexData + 1);
  }
}

class MathTagCoverter {
  constructor(
    /**@type{string} */
    style,
    /**@type{boolean} */
    sanitizeLatexSource,
    /**@type{boolean} whether to remove \\displaystyle from latex source.*/
    removeDisplayStyle,
    /**@type{boolean} whether to log timing information. */
    logTiming,
    /**@type{Object<string,string>} */
    extraAttributes,
  ) {
    /**@type{HTMLElement|null} */
    this.elementProcessed = null;
    /**@type{number} */
    this.startTime = 0;
    /**@type{number} */
    this.lastTimeSample = 0;
    /**@type{number} */
    this.typesetTimeout = 50;
    /**@type{number} */
    this.elementsToTypeset = - 1;
    /**@type{number} */
    this.typesetTotal = 0;
    let styleComputer = document.createElement("DIV");
    styleComputer.style = style;
    /**@type{boolean} */
    this.sanitizeLatexSource = sanitizeLatexSource;
    /**@type{boolean} */
    this.removeDisplayStyle = removeDisplayStyle;
    /**@type{boolean} */
    this.logTiming = logTiming;
    this.style = {
      fontFamily: styleComputer.style.fontFamily,
      display: styleComputer.style.display,
      fontSize: styleComputer.style.fontSize,
      verticalAlign: styleComputer.style.verticalAlign,
      marginBottom: styleComputer.style.marginBottom,
    };
    this.extraAttributes = extraAttributes;
  }

  convertTags(
    /**@type{HTMLElement} */
    toBeModified,
    /**@type{MathTagData[]} */
    toBeConverted,
  ) {
    if (toBeConverted.length === 0) {
      return;
    }
    let content = toBeModified.textContent;
    let newChildren = [];
    for (let i = 0; i < toBeConverted.length; i++) {
      let previousIndex = 0;
      if (i > 0) {
        previousIndex = toBeConverted[i - 1].closeIndex + 1;
      }
      let intermediateContent = content.slice(previousIndex, toBeConverted[i].startIndex);
      if (intermediateContent.length > 0) {
        newChildren.push(document.createTextNode(intermediateContent));
      }
      let mathTag = document.createElement("DIV");
      mathTag.className = "mathcalculator";
      mathTag.textContent = toBeConverted[i].content;
      mathTag.style.fontFamily = this.style.fontFamily;
      mathTag.style.display = this.style.display;
      mathTag.style.fontSize = this.style.fontSize;
      mathTag.style.verticalAlign = this.style.verticalAlign;
      mathTag.style.marginBottom = this.style.marginBottom;
      mathTag.style.maxWidth = this.style.maxWidth;
      newChildren.push(mathTag);
      for (let label in this.extraAttributes) {
        mathTag.setAttribute(label, this.extraAttributes[label]);
      }
    }
    let previousIndex = toBeConverted[toBeConverted.length - 1].closeIndex + 1;
    let remainingContent = content.slice(previousIndex);
    if (remainingContent !== "" && remainingContent !== null) {
      newChildren.push(document.createTextNode(remainingContent));
    }
    toBeModified.textContent = "";
    let replacing = document.createElement("span");
    for (let i = 0; i < newChildren.length; i++) {
      replacing.appendChild(newChildren[i]);
    }
    toBeModified.parentElement.replaceChild(replacing, toBeModified);
  }

  processTextContent(
    /**@type{HTMLElement} */
    toBeModified,
  ) {
    if (toBeModified.textContent === "" || toBeModified.textContent === null) {
      return;
    }
    let content = toBeModified.textContent;
    let openIndex = - 1;
    /**@type{MathTagData[]} */
    let toBeConverted = [];
    for (let i = 1; i < content.length; i++) {
      let previous = content[i - 1];
      let current = content[i];
      if (previous === "\\") {
        if (openIndex === - 1) {
          if (current === "(") {
            openIndex = i;
            continue;
          }
        } else {
          if (current === ")") {
            toBeConverted.push(new MathTagData(openIndex - 1, openIndex + 1, i - 2, i, content));
            openIndex = - 1;
          }
        }
      }
    }
    this.convertTags(toBeModified, toBeConverted);
  }

  convertTagsRecursive(
    /**@type{HTMLElement} */
    toBeModified,
    /**@type{number} */
    recursionDepth,
  ) {
    if (recursionDepth > 40) {
      console.log("While converting mathtags, reached recursion depth limits");
      return;
    }
    if (toBeModified.childNodes.length === 0) {
      this.processTextContent(toBeModified);
      return;
    }
    for (let i = 0; i < toBeModified.childNodes.length; i++) {
      this.convertTagsRecursive(toBeModified.childNodes[i], recursionDepth + 1);
    }
  }

  typeset(
    /**@type{HTMLElement|null} */
    toBeModified,
    /**@type{Function|null} callback after the element has been typeset */
    callback,
  ) {
    this.elementProcessed = toBeModified;
    this.startTime = (new Date()).getTime();
    this.lastTimeSample = this.startTime;
    if (this.elementProcessed !== null) {
      this.convertTagsRecursive(this.elementProcessed, 0);
    }
    this.typesetMathTags(callback);
  }

  /**@returns{boolean} */
  postponeTypeset(
    /**@type{Function|null} callback after the element has been typeset*/
    callback,
  ) {
    let currentTime = (new Date()).getTime();
    let elapsedTime = currentTime - this.lastTimeSample;
    if (
      elapsedTime < this.typesetTimeout ||
      this.typesetTotal >= this.elementsToTypeset
    ) {
      return false;
    }
    this.lastTimeSample = currentTime;
    setTimeout(() => {
      this.typesetMathTags(callback);
    }, 10);
    if (this.logTiming) {
      console.log(`Typeset ${this.typesetTotal} out of ${this.elementsToTypeset} elements.`);
    }
    return true;
  }

  typesetMathTags(
    /**@type{Function|null} callback after the element has been typeset*/
    callback,
  ) {
    let mathElements = document.getElementsByClassName("mathcalculator");
    if (this.elementsToTypeset < 0) {
      this.elementsToTypeset = mathElements.length;
      this.typesetTotal = 0;
    }
    for (; this.typesetTotal < mathElements.length; this.typesetTotal++) {
      if (this.postponeTypeset(callback)) {
        return;
      }
      /** @type {HTMLElement} */
      let element = mathElements[this.typesetTotal];
      if (element["typeset"] === "true") {
        continue;
      }
      element["typeset"] = "true";
      let startTime = (new Date()).getTime();
      mathFromElement(
        element,
        false,
        this.sanitizeLatexSource,
        this.removeDisplayStyle,
        callback,
      );
      let typeSetTime = (new Date()).getTime() - startTime;
      if (this.logTiming) {
        console.log(`Typeset of element ${this.typesetTotal + 1} out of ${this.elementsToTypeset} took ${typeSetTime} ms.`);
      }
    }
  }
}

function typeset(
  /**@type{HTMLElement} */
  toBeModified,
  /**@type{Function|null} */
  callbackEquationCreation,
  /** @type {{style: string, sanitizeLatexSource: boolean, removeDisplayStyle: boolean, boolean,extraAttributes: Object<string, string>}} */
  // sanitizeLatexSource: whether to convert the original latex to parsed one.
  // removeDisplayStyle: whether to remove \\displaystyle from latex source.
  // logTiming: whether to log in the console timing statistics.
  // extraAttributes: an string-key string-value object.
  options,
) {
  if (callbackEquationCreation === undefined) {
    callbackEquationCreation = null;
  }
  if (options === null || options === undefined) {
    options = {};
  }
  let style = options.style;
  if (style === "") {
    style = "font-family:'Times New Roman'; display:inline-block;";
  }
  if (style === undefined || style === null) {
    style = "";
  }
  let sanitizeLatexSource = options.sanitizeLatexSource;
  let removeDisplayStyle = options.removeDisplayStyle;
  let logTiming = options.logTiming;
  let extraAttributes = options.extraAttributes;
  if (logTiming) {
    console.log("Logging parsing speed times; to turn off, set logTiming=false.")
  }
  if (sanitizeLatexSource === undefined) {
    sanitizeLatexSource = false;
  }
  if (removeDisplayStyle === undefined) {
    removeDisplayStyle = false;
  }
  new MathTagCoverter(
    style,
    sanitizeLatexSource,
    removeDisplayStyle,
    logTiming,
    extraAttributes,
  ).typeset(
    toBeModified,
    callbackEquationCreation,
  );
}

class EquationEditorAction {
  constructor(
    /**@type{string} */
    command,
    /**@type{boolean} */
    isKeyPress,
  ) {
    this.command = command;
    this.isKeyPress = isKeyPress;
  }

  execute(
    /**@type{EquationEditor} */
    editor,
  ) {
    if (!this.isKeyPress) {
      editor.writeLatexLastFocused(this.command);
    } else {
      editor.focusInformation.restoreSelection(editor);
      editor.dispatchKey(this.command);
    }
  }
}

class EquationEditorButtonFactory {
  constructor(
    /**@type{string} */
    command,
    /**@type{boolean} */
    isKeyPress,
    /**@type{string} */
    label,
    /**@type{string} */
    additionalStyle,
  ) {
    /**@type{EquationEditorAction[]} */
    this.commands = [];
    if (command !== "" && command !== null) {
      this.commands.push(new EquationEditorAction(command, isKeyPress));
    }
    this.label = label;
    /**@type {string} */
    this.additionalStyle = additionalStyle;
    if (this.additionalStyle === null || this.additionalStyle === undefined) {
      this.additionalStyle = "";
    }
  }

  addEditorAction(
    /**@type{EquationEditorAction}*/
    command,
  ) {
    this.commands.push(command);
  }

  /**@return {HTMLButtonElement} */
  getButton(
    /**@type{EquationEditor} */
    editor,
  ) {
    let result = document.createElement("button");
    if (this.additionalStyle !== undefined && this.additionalStyle !== null && this.additionalStyle !== "") {
      result.style = this.additionalStyle;
    }
    result.textContent = this.label;
    return this.attachToClick(result, editor);
  }

  /**@return {HTMLButtonElement} */
  attachToClick(
    /**@type{HTMLElement} */
    element,
    /**@type{EquationEditor} */
    editor,
  ) {
    element.addEventListener(
      'click', () => {
        this.clickFunction(editor);
      }
    );
    return element;
  }

  /**@return {HTMLButtonElement|null} */
  attachToClickById(
    /**@type{string} */
    buttonId,
    /**@type{EquationEditor} */
    editor,
  ) {
    let button = document.getElementById(buttonId);
    if (button === null) {
      return null;
    }
    return this.attachToClick(button, editor);
  }

  clickFunction(
    /**@type{EquationEditor} */
    editor,
  ) {
    for (let i = 0; i < this.commands.length; i++) {
      this.commands[i].execute(editor);
    }
  }
}

let buttonFactories = {
  "sqrt": new EquationEditorButtonFactory("\\sqrt", true, "\u221A", ""),
  "fraction": new EquationEditorButtonFactory("\\frac{\\caret}{}", false, "/", ""),
  "divide": new EquationEditorButtonFactory("/", true, "(\u2022)/(\u2022)", ""),
  "exponent": new EquationEditorButtonFactory("^", true, "^", ""),
  "matrix2x2": new EquationEditorButtonFactory("\\begin{pmatrix}\\caret&\\\\&\\end{pmatrix}", false, "2x2", ""),
};

module.exports = {
  typeset,
  EquationEditor,
  EquationEditorButtonFactory,
  EquationEditorAction,
  buttonFactories,
  EquationEditorOptions,
  mathFromLatex,
  mathFromElement,
  latexConstants,
  MathNode,
  knownTypes,
};