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
    this.minHeightScale = input["minHeightScale"];
    this.width = input["width"];
    this.height = input["height"];
    this.display = input["display"];
    this.verticalAlign = input["verticalAlign"];
    this.outline = input["outline"];
    this.fontSizeRatio = input["fontSizeRatio"];
    this.fontWeight = input["fontWeight"];
    this.whiteSpace = input["whiteSpace"];
    this.textAlign = input["textAlign"];
    this.colorImplied = input["colorImplied"];
    this.colorText = input["colorText"];
    this.position = input["position"];
    this.minWidth = input["minWidth"];
    this.cursor = input["cursor"];
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
  "minHeightScale": 0,
  "verticalAlign": "",
  "fontSizeRatio": 1,
  "fontWeight": "",
  "whiteSpace": "",
  "textAlign": "",
  "position": "absolute",
  "cursor": "",
  "outline": "",
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
    "borderStyle": "1px solid black",
    "padding": "2px",
    "margin": "2px",
    "cursor": "text",
  }),
  // A math expression with no children such as "x", "2".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    "type": "atom",
    // "margin": atomMargin,
    "outline": "0px solid transparent",
    "width": "auto",
    "height": "auto",
    "minWidth": "4px",
    "verticalAlign": "text-bottom",
  }),
  // A non-editable math expression/operator such as "+" or "-".
  atomImmutable: new MathNodeType({
    "type": "atomImmutable",
    "paddingLeft": `${0.1}em`,
    "paddingRight": `${0.1}em`,
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
  }),
  cancelUnderBox: new MathNodeType({
    "type": "cancelUnderBox",
  }),
  // Represents the numerator x of a fraction x/y.
  numerator: new MathNodeType({
    "type": "numerator",
    "borderBottom": "1px solid black",
    "padding": "1px",
    "fontSizeRatio": defaultFractionScale,
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
    "fontSizeRatio": defaultFractionScale,
    "minHeightScale": defaultFractionScale,
    "arrows": {
      "ArrowUp": arrowMotion.firstAtomToTheLeft,
      "ArrowDown": arrowMotion.firstAtomToTheRight,
    },
    "textAlign": "center",
  }),
  overLinedBox: new MathNodeType({
    "type": "overLinedBox",
    "borderTop": "1px solid black",
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
    "minHeightScale": 0.75,
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
  halfHorizontalBraceTopRight: new MathNodeType({
    "type": "halfHorizontalBraceTopRight",
    "borderTop": "2px solid black",
    "borderTopRightRadius": "4px",
  }),
  halfHorizontalBraceTopLeft: new MathNodeType({
    "type": "halfHorizontalBraceTopLeft",
    "borderTop": "2px solid black",
    "borderTopLeftRadius": "4px",
  }),
  halfHorizontalBraceBottomRight: new MathNodeType({
    "type": "halfHorizontalBraceBottomRight",
    "borderBottom": "2px solid black",
    "borderBottomRightRadius": "4px",
  }),
  halfHorizontalBraceBottomLeft: new MathNodeType({
    "type": "halfHorizontalBraceBottomLeft",
    "borderBottom": "2px solid black",
    "borderBottomLeftRadius": "4px",
  }),
  horizontalLineBottomMargin: new MathNodeType({
    "type": "horizontalLineBottomMargin",
    "borderBottom": "2px solid black",
  }),
  subscript: new MathNodeType({
    "type": "subscript",
    "fontSizeRatio": 0.75,
    "minHeightScale": 0.75,
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
    "minHeightScale": 0.75,
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
    "minHeightScale": 1.8,
  }),
  operatorSubscript: new MathNodeType({
    "type": "operatorSubscript",
    "fontSizeRatio": 0.55,
    "minHeightScale": 0.55,
  }),
  operatorSuperscript: new MathNodeType({
    "type": "operatorSuperscript",
    "fontSizeRatio": 0.55,
    "minHeightScale": 0.55,
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
    const numerator = new MathNode(equationEditor, knownTypes.numerator);
    const denominator = new MathNode(equationEditor, knownTypes.denominator);
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
    cancelSign.initialContent = "\u2571"; // top-left to bottom-right: "\u2572"; x-cross: "\u2573"
    const horizontal = this.horizontalMath(equationEditor, content);
    let underTheCancel = new MathNode(equationEditor, knownTypes.cancelUnderBox);
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
    const underTheRadical = new MathNode(equationEditor, knownTypes.radicalUnderBox);
    underTheRadical.appendChild(mathNodeFactory.horizontalMath(equationEditor, underTheRadicalContent));
    sqrt.appendChild(underTheRadical);
    return sqrt;
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
    const horizontalBraceTopLeft = new MathNode(equationEditor, knownTypes.halfHorizontalBraceTopLeft);
    const horizontalBraceLeft = new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceBottomRight = new MathNode(equationEditor, knownTypes.halfHorizontalBraceBottomRight);
    const horizontalBraceBottomLeft = new MathNode(equationEditor, knownTypes.halfHorizontalBraceBottomLeft);
    const horizontalBraceRight = new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceTopRight = new MathNode(equationEditor, knownTypes.halfHorizontalBraceTopRight);

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
    const subscriptWrapped = new MathNode(equationEditor, knownTypes.subscript);
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
    const leftParentheses = new MathNodeLeftDelimiter(equationEditor);
    leftParentheses.initialContent = content;
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
    const rightParentheses = new MathNodeBaseRightDelimiter(equationEditor);
    rightParentheses.initialContent = content;
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
    let superscriptNode = new MathNode(equationEditor, knownTypes.operatorSuperscript);
    let subscriptNode = new MathNode(equationEditor, knownTypes.operatorSubscript);
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
    let subscriptNode = new MathNode(equationEditor, knownTypes.operatorSubscript);
    let subscriptScale = 0.8;
    subscriptNode.type.fontSizeRatio = subscriptScale;
    subscriptNode.type.minHeightScale = subscriptScale;
    let operatorNode = new MathNode(equationEditor, knownTypes.operatorStandalone);
    let operatorScale = 1;
    operatorNode.type.fontSizeRatio = operatorScale;
    operatorNode.type.minHeightScale = operatorScale;
    subscriptNode.appendChild(this.horizontalMath(equationEditor, subscript));
    operatorNode.appendChild(this.atomImmutable(equationEditor, operator));
    result.appendChild(operatorNode);
    result.appendChild(subscriptNode);
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
      this.position = - 1;
    }
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

  /** @returns {AtomWithPosition} */
  leftHorizontalNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, - 1);
    }
    if (this.position > 0 && this.element.hasHorizintalMathParent()) {
      return new AtomWithPosition(this.element, this.nextPositionInDirection(- 1));
    }
    let next = this.element.firstAtomUncle(- 1);
    if (next === null) {
      return new AtomWithPosition(null, - 1);
    }
    return new AtomWithPosition(next, next.lengthContentIfAtom());
  }

  /** @returns {AtomWithPosition} */
  rightHorizontalNeighbor() {
    if (this.element === null) {
      return new AtomWithPosition(null, -1);
    }
    if (this.position < this.element.lengthContentIfAtom() && this.element.hasHorizintalMathParent()) {
      return new AtomWithPosition(this.element, this.nextPositionInDirection(1));
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
      return new AtomWithPosition(this.element, this.nextPositionInDirection(- 1));
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
    if (this.position < this.element.lengthContentIfAtom()) {
      return new AtomWithPosition(this.element, this.nextPositionInDirection(1));
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
  /**@type{boolean} */
  sanitizeLatexSource,
  /**@type{boolean} whether to remove \\displaystyle from latex source.*/
  removeDisplayStyle,
) {
  let content = container.textContent;
  if (content === null) {
    content = "";
  }
  return mathFromLatex(container, content, editable, sanitizeLatexSource, removeDisplayStyle);
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
) {
  let result = new EquationEditor(container, new EquationEditorOptions({
    editable: editable,
    sanitizeLatexSource: sanitizeLatexSource,
    removeDisplayStyle: removeDisplayStyle,
  }));
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

  isMatrixEnder() {
    return this.syntacticRole in latexConstants.matrixEnder;
  }
}

class LaTeXConstants {
  constructor() {
    this.latinCharactersString = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    this.latinCharacters = {};
    for (let i = 0; i < this.latinCharactersString.length; i++) {
      this.latinCharacters[this.latinCharactersString[i]] = true;
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
      // Full-widgth plus sign, wider and taller plus sign.
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
      "end": "\\end",
      "frac": "\\frac",
      "mathcal": "\\mathcal",
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
      "circ": "\u25CB",
      "cdot": "\u00B7",
      "leq": "\u2264",
      "neq": "\u2260",
      "dots": "\u2026",
      "vdots": "\u22EE",
      "approx": "~",
      "lt": "<",
      "gt": ">",
      "to": "\u2192",
      "nwarrow": "\u2196",
      "nearrow": "\u2197",
      "searrow": "\u2198",
      "swarrow": "\u2199",
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
      "emptyset": "\u2205",
      "in": "\u2208",
      "cap": "\u2229",
      "cup": "\u222A",
    };
    /**@type{Object.<string, string>} */
    this.operatorWithSuperAndSubscript = {
      "sum": "\u03A3",
      "int": "\u222B",
    };
    /**@type{Object.<string, string>} */
    this.operatorsWithSubscript = {
      "lim": "lim",
    };
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
    };
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
      // \langle, \rangle
      "\u27E8": "\u27E9",
      "[": "]",
      "(": ")",
      "{": "}",
      "|": "|",
    };
    /**@type{Object.<string, string>} */
    this.rightLeftDelimiterPair = {
      // \rangle, \langle
      "\u27E9": "\u27E8",
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
    /** @type{Object.<string, string>} */
    this.utf8ToLatexMap = null;
  }

  computeUtf8ToLatexMap() {
    if (this.utf8ToLatexMap !== null) {
      return;
    }
    this.utf8ToLatexMap = {};
    for (let i = 32; i < 125; i++) {
      let character = String.fromCharCode(i);
      this.utf8ToLatexMap[character] = character;
    }
    for (let key in this.latexBackslashAtomsEditable) {
      let current = this.latexBackslashAtomsEditable[key];
      this.utf8ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.latexBackslashOperators) {
      let current = this.latexBackslashOperators[key];
      this.utf8ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.operatorWithSuperAndSubscript) {
      let current = this.operatorWithSuperAndSubscript[key];
      this.utf8ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.operatorsWithSubscript) {
      let current = this.operatorsWithSubscript[key];
      this.utf8ToLatexMap[current] = `\\${key} `;
    }
    for (let key in this.mathcalEquivalents) {
      let current = this.mathcalEquivalents[key];
      this.utf8ToLatexMap[current] = `\\mathcal{${key}} `;
    }
    for (let key in this.mathbbEquivalents) {
      let current = this.mathbbEquivalents[key];
      this.utf8ToLatexMap[current] = `\\mathbb{${key}} `;
    }
    for (let key in this.operatorsFromUft8) {
      this.utf8ToLatexMap[key] = this.operatorsFromUft8[key];
    }
  }

  /** @returns {LatexWithAnnotation} */
  convertUtf8ToLatex(
    /** @type{string} */
    input,
    /** @type{number} */
    selectionStart,
    /** @type{number} */
    selectionEnd,
  ) {
    this.computeUtf8ToLatexMap();
    let result = [];
    for (let i = 0; i < input.length; i++) {
      let current = "";
      for (let j = 0; j < 4 && i + j < input.length; j++) {
        current += input[i + j];
        if (current in this.utf8ToLatexMap) {
          current = this.utf8ToLatexMap[current];
          i += j;
          break;
        }
      }
      result.push(current);
    }
    return new LatexWithAnnotation(result.join(""), selectionStart, selectionEnd);
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

  /**@returns{MathNode} */
  parse() {
    this.startTime = new Date().getTime();
    this.initialize();
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
    secondToLastElement.node.normalizeHorizontalMathRecursive();
    if (this.equationEditor.options.editable) {
      secondToLastElement.node.ensureEditableAtomsRecursive();
    }
    // let elapsedTime = new Date().getTime() - this.startTime;
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
    elementsToRemove,
  ) {
    this.parsingStack.length = this.parsingStack.length - elementsToRemove;
    return true;
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

  /**@returns{boolean} Applies the first possible rule to the top of the parsing stack. */
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
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, mathNodeFactory.atom(this.equationEditor, ""));
      secondToLast.node.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(1);
    }
    if (last.syntacticRole === "\\\\" && secondToLast.isExpression() && thirdToLast.syntacticRole === "matrixBuilder") {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
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
      let lastRow = secondToLast.node.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(this.equationEditor, mathNodeFactory.atom(this.equationEditor, ""));
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
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
      thirdToLast.syntacticRole in latexConstants.leftDelimiters &&
      secondToLast.isExpression() &&
      last.syntacticRole in latexConstants.rightDelimiters
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
      let atom = mathNodeFactory.atom(this.equationEditor, "");
      let horizontal = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [left, atom, right]);
      return this.replaceParsingStackTop(horizontal, "", - 2);
    }
    if (last.isExpression() && secondToLast.syntacticRole === "_" && thirdToLast.isExpression()) {
      this.lastRuleName = "make subscript";
      let node = mathNodeFactory.baseWithSubscript(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (thirdToLast.isExpression() && secondToLast.syntacticRole === "^" && last.isExpression()) {
      this.lastRuleName = "make exponent";
      let node = mathNodeFactory.baseWithExponent(this.equationEditor, thirdToLast.node, last.node);
      return this.replaceParsingStackTop(node, "", - 3);
    }
    if (thirdToLast.syntacticRole === "\\frac" && secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.fraction(this.equationEditor, secondToLast.node, last.node);
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
      node.children[0].children[0].initialContent = "";
      node.children[0].children[2].initialContent = "";
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
}

class EquationEditorOptions {
  constructor(
    /** @type{{editable:boolean,removeDisplayStyle:boolean,sanitizeLatexSource:boolean,debugLogContainer:HTMLElement|null,latexInput:HTMLElement|null}} */
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
    this.latexInput = options.latexInput;
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
    /** @type{HTMLElement} */
    this.container = containerGiven;
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
    }
    /** @type{AtomWithPosition} */
    this.selectionStart = new AtomWithPosition(null, -1);
    /** @type{AtomWithPosition} */
    this.selectionStartExpanded = new AtomWithPosition(null, - 1);
    /** @type{AtomWithPosition} */
    this.selectionEnd = new AtomWithPosition(null, -1);
    /** @type{string} */
    this.latexLastWritten = "";
    /** @type {boolean}*/
    this.backslashSequenceStarted = false;
    /** @type {string}*/
    this.backslashSequence = "";
    /** @type {number} */
    this.backslashPosition = - 1;
    /**@type {string} */
    this.positionDebugString = ""
    if (this.options.latexInput !== null) {
      this.options.latexInput.addEventListener('keyup', (e) => {
        this.writeLatexFromInput();
      });
    }
  }

  updateDOM() {
    this.rootNode.updateDOM();
  }

  writeDebugInfo(
    /** @type{LaTeXParser|null} */ parser,
  ) {
    if (this.options.debugLogContainer === null) {
      return;
    }
    let debugHTML = "";
    if (parser !== null) {
      debugHTML += parser.reductionLog.join("<hr>");
      debugHTML += `<hr><hr>${parser.toStringSyntacticStack()}<hr><hr>`;
    }
    debugHTML += this.toHtml();
    this.options.debugLogContainer.innerHTML = debugHTML;
  }

  writeLatexToInput() {
    if (
      this.options.latexInput === null
    ) {
      return;
    }
    let latexWithAnnotation = this.rootNode.toLatexWithAnnotation();
    if (this.options.latexInput !== null) {
      this.options.latexInput.value = latexWithAnnotation.latex;
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
    this.latexLastWritten = latex;
    this.rootNode.removeAllChildren();
    this.container.textContent = "";
    let parser = new LaTeXParser(this, latex);
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
  }

  toHtml() {
    let latexWithAnnotation = this.rootNode.toLatexWithAnnotation();
    let result = `Latex: ${latexWithAnnotation.latex}`
    result += `<br>Latex selection: ${latexWithAnnotation.selectionStart}, ${latexWithAnnotation.selectionEnd}`;
    result += `<br>Drawing: ${this.rootNode.toString()}`;
    result += `<br>Structure: ${this.rootNode.toHtml(0)}`;
    result += `<br>${this.toStringSelection()}`;
    result += `<br>Backslash position: ${this.backslashPosition}; started: ${this.backslashSequenceStarted}, sequence: ${this.backslashSequence}.`;
    result += `<br>Position computation string: ${this.positionDebugString}.`;
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
    /**@type{MathNode|null}*/ focused,
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
    console.log("DEBUG: updateAlignment...");
    this.rootNode.computeBoundingBox();
    this.rootNode.doAlign();
    let boundingRectangle = this.rootNode.element.getBoundingClientRect();
    let desiredHeight = boundingRectangle.height;
    if (this.options.editable) {
      desiredHeight += 2;
    }
    this.container.style.height = desiredHeight;
    this.container.style.width = boundingRectangle.width;
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
    let startAnnotation = this.selectionStartExpanded.element.toLatexWithAnnotation();
    let endAnnotation = this.selectionEnd.element.toLatexWithAnnotation();
    let result = `Selection: from: ${this.selectionStartExpanded.toString()} to: ${this.selectionEnd.toString()}.`;
    result += ` Latex from: ${startAnnotation.toString()} to ${endAnnotation.toString()}`;
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
      console.log(`Not expected: no common ancestor element between ${start} and ${end}. `);
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
    if (this.container.children.length >= 2) {
      this.container.removeChild(this.container.children[1]);
      this.container.style.width = this.rootNode.element.style.width;
      return;
    }
    let staticContainer = document.createElement("SPAN");
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
    staticContainer.textContent = latex;
    staticContainer.style.position = "absolute";
    staticContainer.style.left = this.rootNode.element.style.width;
    staticContainer.style.whiteSpace = "nowrap";
    this.container.appendChild(staticContainer);
    let newWidth = this.rootNode.element.getBoundingClientRect().width + staticContainer.getBoundingClientRect().width;
    this.container.style.width = newWidth;
    let range = window.getSelection().getRangeAt(0);
    let rangeClone = range.cloneRange();
    rangeClone.selectNode(staticContainer);
    window.getSelection().removeAllRanges();
    window.getSelection().addRange(rangeClone);
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
      newSelection = this.selectionEnd.leftHorizontalNeighbor();
    } else {
      newSelection = this.selectionEnd.rightHorizontalNeighbor();
    }
    if (newSelection.element === null) {
      return false;
    }
    this.selectionEnd = newSelection;
    this.selectionStartExpanded = this.selectionStart;
    if (this.selectionStart.element.parent !== this.selectionEnd.element.parent) {
      let parentOfSelectionStart = this.selectionStart.element.commonParent(
        this.selectionEnd.element
      );
      this.selectionStartExpanded = new AtomWithPosition(
        parentOfSelectionStart.parent.children[parentOfSelectionStart.indexInParent],
        - 1,
      );
    }
    this.highlightSelection();
    return true;
  }

  /**@return{boolean} */
  selectionStartToTheLeftOfSelectionEnd() {
    if (this.selectionStartExpanded.element === null || this.selectionEnd.element === null) {
      return true;
    }
    return this.selectionStartExpanded.element.isToTheLeftOf(this.selectionEnd.element);
  }

  highlightSelection() {
    if (this.selectionStartExpanded.element === null || this.selectionEnd.element === null) {
      return;
    }
    this.selectBetween(
      this.selectionStartExpanded,
      this.selectionEnd,
    );
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
    /** @type{number}*/
    this.stretchFactor = 1;
    /** @type{string}*/
    this.transform = "";
    /** @type{string}*/
    this.transformOrigin = "";
    /** @type{number}*/
    this.heightBeforeTransform = - 1;
    /** @type{number}*/
    this.widthBeforeTransform = - 1;
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
};

class LatexWithAnnotation {
  constructor(
    /**@type{string} */
    latex,
    /**@type{number} */
    selectionStart,
    /**@type{number} */
    selectionEnd,
  ) {
    /** @type {string} */
    this.latex = latex;
    /** @type {number} */
    this.selectionStart = selectionStart;
    /** @type {number} */
    this.selectionEnd = selectionEnd;
  }

  accountOwner(
    /** @type{MathNode} */
    owner,
  ) {
    if (owner === owner.equationEditor.selectionEnd.element) {
      if (owner.equationEditor.selectionStartToTheLeftOfSelectionEnd()) {
        this.selectionEnd = this.latex.length;
      } else {
        this.selectionEnd = 0;
      }
    }
    if (owner === owner.equationEditor.selectionStartExpanded.element) {
      if (owner.equationEditor.selectionStartToTheLeftOfSelectionEnd()) {
        this.selectionStart = 0;
      } else {
        this.selectionEnd = this.latex.length;
      }
    }
  }

  accountChild(
    /** @type{LatexWithAnnotation} */
    child,
    /** @type{number} */
    shift,
  ) {
    if (child.selectionStart !== - 1) {
      this.selectionStart = shift + child.selectionStart;
    }
    if (child.selectionEnd !== - 1) {
      this.selectionEnd = shift + child.selectionEnd;
    }
  }

  toString() {
    return `${this.latex} [[${this.selectionStart}, ${this.selectionEnd}]]`;
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
    if (this.type.minHeightScale !== 0) {
      this.element.style.minHeight = this.type.minHeightScale * fontSize / 1.6;
    }
    if (this.type.minWidth !== "") {
      this.element.style.minWidth = this.type.minWidth;
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
      this.element.textContent = this.initialContent;
    }
    if (this.type.position !== "") {
      this.element.style.position = this.type.position;
    }
    this.element.setAttribute("mathTagName", this.type.type);
    if (this.type.type === knownTypes.atom.type) {
      this.element.addEventListener("copy", (e) => {
        this.copyToClipboard(e);
      });
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
    this.element.addEventListener("click", (e) => {
      this.handleSingleClick(e);
    });
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
    if (!this.equationEditor.options.editable) {
      return;
    }
    if (this.type.type === knownTypes.atom.type) {
      e.stopPropagation();
      return;
    }
    e.stopPropagation();
    e.preventDefault();
    this.focus(1);
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
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  computeDimensionsCancel() {
    let content = this.children[1];
    this.boundingBox.height = content.boundingBox.height;
    this.boundingBox.width = content.boundingBox.width;
    this.boundingBox.fractionLineHeight = content.boundingBox.fractionLineHeight;
    let cancelSign = this.children[0];
    cancelSign.computeDimensionsAtomicNoTransform();
    let stretchX = this.boundingBox.width / cancelSign.boundingBox.widthBeforeTransform;
    let stretchY = this.boundingBox.height / cancelSign.boundingBox.heightBeforeTransform;
    cancelSign.element.style.transform = `matrix(${stretchX},0,0,${stretchY}, 0, 0)`;
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

    topLeft.element.style.borderTopLeftRadius = radiusString;
    topRight.element.style.borderTopRightRadius = radiusString;
    bottomLeft.element.style.borderBottomLeftRadius = radiusString;
    bottomRight.element.style.borderBottomRightRadius = radiusString;

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
    // Top border adds extra 1px to the height, which shifts the fraction line by 0.5px.
    this.boundingBox.fractionLineHeight = this.children[0].boundingBox.fractionLineHeight + 0.5;
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

  computeDimensionsFraction() {
    let numerator = this.children[0];
    let denominator = this.children[1];
    let extraSpaceBetweenNumeratorAndDenominator = 4;
    this.boundingBox.fractionLineHeight = numerator.boundingBox.height + 2;
    this.boundingBox.height = numerator.boundingBox.height + denominator.boundingBox.height + extraSpaceBetweenNumeratorAndDenominator;
    this.boundingBox.width = Math.max(numerator.boundingBox.width, denominator.boundingBox.width);
    this.boundingBox.needsMiddleAlignment = true;
    numerator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.top = numerator.boundingBox.height + extraSpaceBetweenNumeratorAndDenominator;
    numerator.computeBoundingBoxLeftSingleChild();
    denominator.computeBoundingBoxLeftSingleChild();
    this.boundingBox.width += 4;
    numerator.boundingBox.left += 2;
    denominator.boundingBox.left += 2;
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

  computeDimensionsOperatorWithSuperAndSubscript() {
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

  computeDimensionsOperatorWithSubscript() {
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

  computeDimensionsBaseWithExponent() {
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
    this.computeDimensionsAtomicNoTransform();
    if (heightToEnclose === 0) {
      heightToEnclose = this.boundingBox.height;
      fractionLineHeightEnclosed = this.boundingBox.height / 2;
    }
    let scaleParenthesis = 1.32;
    let scaleHeight = 1.1;
    heightToEnclose = Math.max(fractionLineHeightEnclosed * 2, (heightToEnclose - fractionLineHeightEnclosed) * 2);
    let heightToStretchTo = heightToEnclose * scaleParenthesis;
    if (heightToEnclose !== 0) {
      this.boundingBox.stretchFactor = heightToStretchTo / this.boundingBox.heightBeforeTransform;
    }
    this.boundingBox.height = heightToEnclose * scaleHeight;
    // For many fonts, the parenteses's middle appears to be below 
    // the middle of the line. We therefore translate the parenthesis up 
    // (negative translation) with a % of its bounding box height.
    let translateUpPercent = (scaleParenthesis - 1) / 2.15;
    this.boundingBox.fractionLineHeight = this.boundingBox.height * 0.5;
    let translateVertical = - heightToStretchTo * translateUpPercent;
    this.boundingBox.transformOrigin = "top left";
    this.boundingBox.transform = `matrix(1,0,0,${this.boundingBox.stretchFactor}, 0, ${translateVertical})`;
  }

  computeDimensionsRootNode() {
    this.computeDimensionsStandard();
    if (!this.boundingBox.needsMiddleAlignment || this.equationEditor.options.editable) {
      return;
    }
    let bottomDistance = this.boundingBox.height - this.boundingBox.fractionLineHeight;
    if (bottomDistance > this.boundingBox.fractionLineHeight) {
      this.boundingBox.height = bottomDistance * 2;
      this.boundingBox.top += bottomDistance - this.boundingBox.fractionLineHeight;
      this.boundingBox.fractionLineHeight = bottomDistance;
    } else {
      this.boundingBox.height = this.boundingBox.fractionLineHeight * 2;
    }
  }

  computeDimensionsNumerator() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height;
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
    radicalExponentBox.boundingBox.top = - 0.15 * radicalExponentBox.boundingBox.height;
    let widthSqrtSign = 0.99;// 0.92;
    sqrtSign.boundingBox.left = extraWidth;
    underTheRadical.boundingBox.left = sqrtSign.boundingBox.left + sqrtSign.boundingBox.width * widthSqrtSign;
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = underTheRadical.boundingBox.height * 1.15;
    this.boundingBox.fractionLineHeight = underTheRadical.boundingBox.fractionLineHeight + 2.2;
    let extraSpaceAfterRadical = 4;
    this.boundingBox.width = underTheRadical.boundingBox.left + underTheRadical.boundingBox.width + extraSpaceAfterRadical;
    this.boundingBox.needsMiddleAlignment = underTheRadical.boundingBox.needsMiddleAlignment;
  }

  computeDimensionsMatrix() {
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
      row.mergeBoundingBoxesHorizontallyAlignedElements();
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
    if (this.isDelimiter()) {
      this.computeDimensionsDelimiter();
      return;
    }
    if (this.type.type === knownTypes.error.type) {
      this.computeDimensionsAtomic();
      return;
    }
    if (this.type.type === knownTypes.cancel.type) {
      this.computeDimensionsCancel();
      return;
    }
    if (this.type.type === knownTypes.sqrt.type) {
      this.computeDimensionsSqrt();
      return;
    }
    if (this.type.type === knownTypes.sqrtSign.type) {
      this.computeDimensionsSqrtSign();
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
    if (this.type.type === knownTypes.fraction.type) {
      this.computeDimensionsFraction();
      return;
    }
    if (this.type.type === knownTypes.operatorWithSuperAndSubscript.type) {
      this.computeDimensionsOperatorWithSuperAndSubscript();
      return;
    }
    if (this.type.type === knownTypes.operatorWithSubscript.type) {
      this.computeDimensionsOperatorWithSubscript();
      return;
    }
    if (this.type.type === knownTypes.operatorStandalone.type) {
      this.computeDimensionsOperatorStandalone();
      return;
    }
    if (this.type.type === knownTypes.numerator.type) {
      this.computeDimensionsNumerator();
    }
    if (this.type.type === knownTypes.baseWithExponent.type) {
      this.computeDimensionsBaseWithExponent();
      return;
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
    if (this.type.type === knownTypes.root.type) {
      this.computeDimensionsRootNode();
      return;
    }
    if (this.type.type === knownTypes.horizontalMath.type) {
      this.computeDimensionsHorizontalMath();
      return;
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
    let enclosedFractionLineHeights = [];
    /** @type {number[]} */
    let indicesOpenedParentheses = [];
    for (let i = 0; i < this.children.length; i++) {
      this.computeDimensionsHorizontalMathParenthesesAccountChild(
        i, enclosedHeights, enclosedFractionLineHeights, indicesOpenedParentheses,
      );
    }
  }

  computeDimensionsHorizontalMathParenthesesAccountChild(
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

  mergeBoundingBoxesHorizontallyAlignedElements() {
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
      let heightFromChild = child.boundingBox.height - child.boundingBox.fractionLineHeight + this.boundingBox.fractionLineHeight;
      this.boundingBox.height = Math.max(this.boundingBox.height, heightFromChild);
    }
  }

  computeDimensionsHorizontalMath() {
    this.boundingBox = new BoundingBox();
    this.computeDimensionsHorizontalMathParenthesesHeight();
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.boundingBox.left = this.boundingBox.width;
      this.boundingBox.width += child.boundingBox.width;
    }
    this.mergeBoundingBoxesHorizontallyAlignedElements();
    this.computeMiddleAlignment();
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

  updateParentDOM(/** @type {HTMLElement|null} */ oldElement) {
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

  handleFocus(e) {
    this.element.style.background = "#f0f0f0";
    this.focused = true;
  }

  handleBlur(e) {
    this.element.style.background = "";
    this.equationEditor.backslashSequenceStarted = false;
    this.focused = false;
  }

  handleKeyDown(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let key = event.key;
    if (key === "Shift" || key === "Ctrl") {
      event.stopPropagation();
      return;
    }
    this.storeCaretPosition(event.key, event.shiftKey);
    event.stopPropagation();
    let doUpdateAlignment = true;
    if (this.handleKeyDownCases(event)) {
      event.preventDefault();
      doUpdateAlignment = false;
    }
    // While we no longer propagate the event, we need to 
    // release the thread so the browser can finish computations
    // with the released element. 
    setTimeout(() => {
      this.storeCaretPosition(event.key, event.shiftKey);
      if (doUpdateAlignment) {
        this.equationEditor.updateAlignment();
      }
      this.equationEditor.writeLatexToInput();
      this.equationEditor.writeDebugInfo(null);
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
  processBackslash(
    /** @type {string} */
    key,
    /** @type{boolean} */
    shiftHeld,
  ) {
    if (key === "Shift") {
      return false;
    }
    if (key === "\\" && !shiftHeld) {
      this.equationEditor.backslashSequenceStarted = true;
      this.equationEditor.backslashSequence = "";
      this.equationEditor.backslashPosition = this.positionCaretBeforeKeyEvents;
      return false;
    }
    if (!this.equationEditor.backslashSequenceStarted) {
      return false;
    }
    if (key === " " && this.equationEditor.backslashSequenceStarted) {
      this.equationEditor.backslashSequenceStarted = false;
      return this.makeBackslashSequence();
    }
    if (this.equationEditor.backslashSequence === "" && (key === "{" || key === "}")) {
      this.equationEditor.backslashSequence = key;
      this.equationEditor.backslashSequenceStarted = false;
      this.makeBackslashSequence();
      return true;
    }
    if (latexConstants.isLatinCharacter(key)) {
      this.equationEditor.backslashSequence += key;
      return false;
    }
    if (key === "Backspace") {
      if (this.equationEditor.backslashSequence === "") {
        this.equationEditor.backslashSequenceStarted = false;
      } else {
        this.equationEditor.backslashSequence = this.equationEditor.backslashSequence.slice(
          0, this.equationEditor.backslashSequence.length - 1,
        );
      }
      return false;
    }
    this.equationEditor.backslashSequenceStarted = false;
    return false;
  }

  copyToClipboard(event) {
    let latexWithAnnotation = this.equationEditor.rootNode.toLatexWithAnnotation();
    let toBeCopied = "";
    if (latexWithAnnotation.selectionStart === - 1 || latexWithAnnotation.selectionEnd === - 1) {
      toBeCopied = latexWithAnnotation.latex;
    } else {
      let start = latexWithAnnotation.selectionStart;
      let end = latexWithAnnotation.selectionEnd;
      if (start > end) {
        let original = start;
        start = end;
        end = original;
      }
      let sliced = latexWithAnnotation.latex.slice(start, end);
      toBeCopied = sliced;
    }
    event.clipboardData.setData('text/plain', toBeCopied);
    console.log("Copying to clipboard: " + toBeCopied);
    event.preventDefault();
  }

  pasteFromClipboard(event) {
    event.preventDefault();
    let data = event.clipboardData.getData('text');
    event.preventDefault();
    this.writeLatex(data);
  }

  writeLatex(
    /** @type {string} */
    data,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      return;
    }
    console.log("DEBUG: pasting: " + data);
    this.storeCaretPosition();
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
      this.focusRestore();
      return;
    }
    let split = this.splitByCaretEmptyAtoms();
    split[1].desiredCaretPosition = 0;
    let horizontalContent = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], newContent, split[1]]);
    horizontalContent.normalizeHorizontalMath();
    let parent = this.parent;
    parent.replaceChildAtPosition(this.indexInParent, horizontalContent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
  }

  /** @returns {boolean} whether default should be prevented. */
  handleKeyDownCases(
    /** @type {KeyboardEvent} */
    event,
  ) {
    let key = event.key;
    let shiftHeld = event.shiftKey;
    if (this.processBackslash(key, shiftHeld)) {
      return true;
    }
    switch (key) {
      case "/":
        this.makeFractionNumerator();
        return true;
      case "*":
      case "+":
      case "-":
      case "=":
      case ">":
      case "<":
        this.makeHorizontalOperatorCorrectInput(key);
        return true;
      case "^":
        this.makeBaseWithExponent();
        return true;
      case "_":
        this.makeBaseWithSubscript();
        return true;
      case "ArrowLeft":
      case "ArrowRight":
      case "ArrowUp":
      case "ArrowDown":
        return this.arrow(key, shiftHeld);
      case "|":
        this.makeDelimiterAmbiguous(key);
        return true;
      case "(":
      case "[":
        this.makeDelimiterLeft(key);
        return true;
      case ")":
      case "]":
        this.makeDelimiterRight(key);
        return true;
      case "Enter":
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

  /** @returns{boolean} whether the default should be prevented. */
  arrow(
    /** @type {string} */
    key,
    /** @type {boolean} */
    shiftHeld,
  ) {
    if (shiftHeld) {
      this.setSelectionStart();
      this.equationEditor.setSelectionEnd(key, shiftHeld);
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
      return (new AtomWithPosition(this, this.textContentOrInitialContent().length)).rightNeighbor();
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

  storeCaretPosition(
    /** @type{string} */
    key,
    /** @type{boolean} */
    shiftHeld,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      this.positionCaretBeforeKeyEvents = - 1;
      return;
    }
    let previousPosition = this.positionCaretBeforeKeyEvents;

    let selection = window.getSelection();
    let range = selection.getRangeAt(0);
    let rangeClone = range.cloneRange();
    rangeClone.selectNodeContents(this.element);
    rangeClone.setEnd(range.endContainer, range.endOffset);
    this.positionCaretBeforeKeyEvents = rangeClone.toString().length;// range.endOffset;
    if (key === "ArrowRight" && previousPosition === this.positionCaretBeforeKeyEvents && !shiftHeld) {
      this.positionCaretBeforeKeyEvents = range.startOffset;
    }
    this.equationEditor.positionDebugString = `Computed position: ${this.positionCaretBeforeKeyEvents}.`
    this.equationEditor.positionDebugString += `Range: [${range}], clone: [${rangeClone}], previous position: ${previousPosition}.`;
    this.equationEditor.positionDebugString += `end offset: ${range.endOffset}, start offset: ${range.startOffset}`;
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

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftNumerator() {
    if (this.type.type !== knownTypes.numerator.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftNumeratorOrDenominator();
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftDenominator() {
    if (this.type.type !== knownTypes.denominator.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftNumeratorOrDenominator();
  }

  /** @returns {boolean} whether reduction occurred. */
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

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheRightOfOperatorWithSuperAndSubscript() {
    if (this.type.type !== knownTypes.operatorWithSuperAndSubscript.type) {
      return false;
    }
    this.children[2].focus(1);
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheRightSqrt() {
    if (this.type.type !== knownTypes.sqrt.type) {
      return false;
    }
    this.children[2].focus(1);
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheRightCancel() {
    if (this.type.type !== knownTypes.cancel.type) {
      return false;
    }
    this.children[1].focus(1);
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftEndOfOperatorSubscript() {
    if (
      this.type.type !== knownTypes.operatorSubscript.type &&
      this.type.type !== knownTypes.operatorSuperscript.type
    ) {
      return false;
    }
    let operatorWithSuperAndSubscript = this.parent;
    let superscript = operatorWithSuperAndSubscript.children[0];
    let subscript = operatorWithSuperAndSubscript.children[2];
    let horizontal = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [superscript.children[0], subscript.children[0]]);
    horizontal.normalizeHorizontalMath();
    let parent = operatorWithSuperAndSubscript.parent;
    let indexOperator = operatorWithSuperAndSubscript.indexInParent;
    parent.replaceChildAtPosition(indexOperator, horizontal);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftOfExponent() {
    if (this.type.type !== knownTypes.exponent.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftOfExponentOrSubscript();
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftOfSubscript() {
    if (this.type.type !== knownTypes.subscript.type) {
      return false;
    }
    return this.applyBackspaceToTheLeftOfExponentOrSubscript();
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftOfExponentOrSubscript() {
    let baseWithExponent = this.parent;
    let indexBaseWithExponent = baseWithExponent.indexInParent;
    let base = baseWithExponent.children[0];
    this.children[0].children[0].desiredCaretPosition = 0;
    base.appendChild(this.children[0]);
    base.normalizeHorizontalMath();
    let parent = baseWithExponent.parent;
    parent.replaceChildAtPosition(indexBaseWithExponent, base);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftOfSqrtSign() {
    if (this.type.type !== knownTypes.radicalUnderBox.type) {
      return false;
    }
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

  /** @returns {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftOfCancelSign() {
    if (this.type.type !== knownTypes.cancelUnderBox.type) {
      return false;
    }
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

  /** @returns {boolean} whether reduction occurred. */
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
    if (this.applyBackspaceToTheLeftOfExponent()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftOfSubscript()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftOfSqrtSign()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftOfCancelSign()) {
      return true;
    }
    if (this.applyBackspaceToTheLeftEndOfOperatorSubscript()) {
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

  /** @returns {boolean} whether backspace was applied */
  applyBackspaceToTheRightFraction() {
    if (this.type.type !== knownTypes.fraction.type) {
      return false;
    }
    this.parent.children[this.indexInParent + 1].focusCancelOnce();
    this.children[1].focus(1);
    return true;
  }

  /** @returns {boolean} whether backspace was applied */
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
    if (this.applyBackspaceToTheRightOfOperatorWithSuperAndSubscript()) {
      return true;
    }
    if (this.applyBackspaceToTheRightMatrix()) {
      return true;
    }
    if (this.applyBackspaceToTheRightSqrt()) {
      return true;
    }
    if (this.applyBackspaceToTheRightCancel()) {
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
    if (!this.hasHorizintalMathParent()) {
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
      true
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
    if (!this.hasHorizintalMathParent()) {
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

  makeSqrt() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.makeSqrtFromSelection(splitBySelection);
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

  makeSqrtFromSelection(
    /** @type{SplitBySelectionResult} */
    splitBySelection,
  ) {
    let ancestor = splitBySelection.ancestor;
    ancestor.removeAllChildren();
    let underTheRadical = mathNodeFactory.horizontalMathFromArray(this.equationEditor, splitBySelection.split);
    underTheRadical.ensureEditableAtoms();
    let sqrt = mathNodeFactory.sqrt(this.equationEditor, underTheRadical, null);
    ancestor.appendChildren(splitBySelection.beforeSplit);
    ancestor.appendChild(sqrt);
    ancestor.appendChildren(splitBySelection.afterSplit);
    ancestor.ensureEditableAtoms();
    ancestor.updateDOM();
    sqrt.children[2].focusStandard(0);
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

  /** @returns {MathNode[]} */
  splitByPositionChopOffCharacters(
    /** @type{number} */
    position,
    /** @type{number} */
    charactersToRemove,
  ) {
    if (!this.isAtomEditable()) {
      if (position <= 0) {
        return [null, this];
      } else {
        return [this, null];
      }
    }
    let content = this.contentIfAtom();
    let leftContent = content.slice(0, position);
    let rightContent = content.slice(position + charactersToRemove, content.length);
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
    /**@type {number}*/
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
    delimiterReplaced.initialContent = delimiterString;
    this.ensureEditableAtoms();
    this.updateDOM();
    delimiterReplaced.focus(1);
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

  /** @returns {boolean} whether the default should be prevented. */
  makeBackslashSequence() {
    let backslashSequence = this.equationEditor.backslashSequence;
    let split = this.splitByPositionChopOffCharacters(
      this.equationEditor.backslashPosition,
      backslashSequence.length + 1,
    );
    if (backslashSequence in latexConstants.operatorWithSuperAndSubscript) {
      let command = latexConstants.operatorWithSuperAndSubscript[backslashSequence];
      this.makeOperatorWithSuperscriptAndSubscriptFromSplit(command, split);
      return true;
    }
    if (backslashSequence in latexConstants.operatorsWithSubscript) {
      let command = latexConstants.operatorsWithSubscript[backslashSequence];
      this.makeOperatorWithSubscriptFromSplit(command, split);
      return true;
    }
    if (backslashSequence in latexConstants.latexBackslashOperators) {
      let operator = latexConstants.latexBackslashOperators[backslashSequence];
      this.makeHorizontalOperatorFromSplit(operator, split);
      return true;
    }
    let backslashSequenceWithBackslash = "\\" + backslashSequence;
    if (backslashSequenceWithBackslash in latexConstants.leftDelimiters) {
      let delimiter = latexConstants.leftDelimiters[backslashSequenceWithBackslash];
      let atomNode = mathNodeFactory.atom(this.equationEditor, "");
      let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], atomNode, split[1]]);
      let parent = this.parent;
      parent.replaceChildAtPosition(this.indexInParent, node);
      node.makeDelimiterLeft(delimiter);
      parent.normalizeHorizontalMath();
      return true;
    }
    if (backslashSequenceWithBackslash in latexConstants.rightDelimiters) {
      let delimiter = latexConstants.rightDelimiters[backslashSequenceWithBackslash];
      let atomNode = mathNodeFactory.atom(this.equationEditor, "");
      let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], atomNode, split[1]]);
      let parent = this.parent;
      parent.replaceChildAtPosition(this.indexInParent, node);
      node.makeDelimiterRight(delimiter);
      parent.normalizeHorizontalMath();
      return true;
    }
    if (backslashSequence in latexConstants.latexBackslashAtomsEditable) {
      let content = latexConstants.latexBackslashAtomsEditable[backslashSequence];
      let atomNode = mathNodeFactory.atom(this.equationEditor, content);
      atomNode.desiredCaretPosition = content.length;
      let node = mathNodeFactory.horizontalMathFromArray(this.equationEditor, [split[0], atomNode, split[1]]);
      node.normalizeHorizontalMath();
      let parent = this.parent;
      parent.replaceChildAtPosition(this.indexInParent, node);
      parent.normalizeHorizontalMath();
      parent.updateDOM();
      parent.focusRestore();
      return true;
    }
    if (backslashSequence === "sqrt") {
      this.makseSqrtFromSplit(split);
      return true;
    }
    if (backslashSequence === "cancel") {
      this.makeCancelFromSplit(split);
      return true;
    }
    if (backslashSequence === "pmatrix" || backslashSequence === "matrix") {
      this.makeMatrixFromSplit(2, 2, split);
      return true;
    }
    return false;
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
      this.setRangeStart(range, 0);
    }
    // console.log(`Position: ${position}, range ${range}, collapseToStart: ${collapseToStart} start offset: ${range.startOffset}, end offset: ${range.endOffset}, text len: ${this.element.textContent.length}`);
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
    result.push(content);
    for (let i = 0; i < this.children.length; i++) {
      result.push(this.children[i].toHtml(indentationLevel + 1));
    }
    return result.join("\n<br>\n");
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotationAtomic() {
    let selectionStart = - 1;
    let selectionEnd = - 1;
    if (this === this.equationEditor.selectionStartExpanded.element) {
      let position = this.equationEditor.selectionStartExpanded.position;
      if (this.equationEditor.selectionStartToTheLeftOfSelectionEnd()) {
        selectionStart = position;
      } else {
        selectionEnd = position;
      }
    }
    if (this === this.equationEditor.selectionEnd.element) {
      let position = this.equationEditor.selectionEnd.position;
      if (this.equationEditor.selectionStartToTheLeftOfSelectionEnd()) {
        selectionEnd = position;
      } else {
        selectionStart = position;
      }
    }
    return latexConstants.convertUtf8ToLatex(
      this.contentIfAtomic(),
      selectionStart,
      selectionEnd,
    );
  }

  /** @returns{string} */
  toLatex() {
    return this.toLatexWithAnnotation().latex;
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    let toJoin = [];
    let charactersSoFar = 0;
    let result = new LatexWithAnnotation("", - 1, - 1);
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i] === null) {
        toJoin.push("[[null]]");
        continue;
      }
      let childLatex = this.children[i].toLatexWithAnnotation();
      result.accountChild(childLatex, charactersSoFar);
      toJoin.push(childLatex.latex);
      charactersSoFar += childLatex.latex.length;
    }
    result.latex = toJoin.join("");
    result.accountOwner(this);
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

class MathNodeAtom extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.atom);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    return this.toLatexWithAnnotationAtomic();
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
  toLatexWithAnnotation() {
    return this.toLatexWithAnnotationAtomic();
  }
}

class MathNodeFraction extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.fraction);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    let numerator = this.children[0].toLatexWithAnnotation();
    let denominator = this.children[1].toLatexWithAnnotation();
    let result = new LatexWithAnnotation(`\\frac{${numerator.latex}}{${denominator.latex}}`, - 1, - 1);
    result.accountChild(numerator, 6);
    result.accountChild(denominator, 6 + numerator.latex.length + 2);
    result.accountOwner(this);
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
}

class MathNodeBaseWithExponent extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithExponent);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    return new LatexWithAnnotation(
      `{${this.children[0].toLatex()}}^{${this.children[1].toLatex()}}`,
      - 1,
      - 1,
    );
  }
}

class MathNodeHorizontalMath extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.horizontalMath);
  }
}

class MathNodeGenericBox extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.genericMathBox);
  }
}

class MathNodeRoot extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.root);
  }
}

class MathNodeError extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.error);
  }
}

class MathNodeCancel extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.cancel);
  }

  toLatexWithAnnotation() {
    let childLatex = this.children[1].toLatex();
    return new LatexWithAnnotation(`\\cancel{${childLatex}}`, - 1, - 1);
  }
}

class MathNodeSqrtSign extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrtSign);
  }
}

class MathNodeSqrt extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrt);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    if (this.element === null) {
      return new LatexWithAnnotation("[null(]", - 1, - 1);
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
      return new LatexWithAnnotation(`\\sqrt[${exponent}]{${underTheRadical}}`, - 1, - 1);
    }
    return new LatexWithAnnotation(`\\sqrt{${underTheRadical}}`, - 1, - 1);
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
}

class MathNodeBaseWithSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithSubscript);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    return new LatexWithAnnotation(`{${this.children[0].toLatex()}}_{${this.children[1].toLatex()}}`, - 1, - 1);
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
  toLatexWithAnnotation() {
    return new LatexWithAnnotation(this.textContentOrInitialContent(), - 1, - 1);
  }
}

class MathNodeBaseRightDelimiter extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.rightDelimiter);
  }

  /** @returns {LatexWithAnnotation} */
  toLatexWithAnnotation() {
    return new LatexWithAnnotation(this.textContentOrInitialContent(), - 1, - 1);
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
  toLatexWithAnnotation() {
    if (this.element === null) {
      return new LatexWithAnnotation("[null)]", - 1, - 1);
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
    return new LatexWithAnnotation(result.join(""), - 1, - 1);
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
  toLatexWithAnnotation() {
    let top = this.children[0].toLatex();
    let result = this.children[1].toLatex();
    let bottom = this.children[2].toLatex();
    if (bottom !== "") {
      result += `_{${bottom}}`;
    }
    if (top !== "") {
      result += `^{${top}}`;
    }
    return new LatexWithAnnotation(result, - 1, - 1);
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
  toLatexWithAnnotation() {
    latexConstants.computeUtf8ToLatexMap();
    const content = this.textContentOrInitialContent();
    if (content in latexConstants.utf8ToLatexMap) {
      return new LatexWithAnnotation(latexConstants.utf8ToLatexMap[content]);
    }
    return new LatexWithAnnotation(`${content}`, - 1, - 1);
  }
}

class MathNodeOperatorWithSubscript extends MathNode {
  constructor(
    /** @type {EquationEditor} */
    equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorWithSubscript);
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
      newChildren.push(mathTag);
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
    /**@type{HTMLElement} */
    toBeModified,
  ) {
    this.elementProcessed = toBeModified;
    this.startTime = (new Date()).getTime();
    this.lastTimeSample = this.startTime;
    if (this.elementProcessed) {
      this.convertTagsRecursive(this.elementProcessed, 0);
    }
    this.typesetMathTags();
  }

  /**@returns{boolean} */
  postponeTypeset() {
    let currentTime = (new Date()).getTime();
    let elapsedTime = currentTime - this.lastTimeSample;
    if (
      elapsedTime < this.typesetTimeout ||
      this.typesetTotal >= this.elementsToTypeset
    ) {
      return false;
    }
    this.lastTimeSample = currentTime;
    setTimeout(this.typesetMathTags.bind(this), 10);
    if (this.logTiming) {
      console.log(`Typeset ${this.typesetTotal} out of ${this.elementsToTypeset} elements.`);
    }
    return true;
  }

  typesetMathTags() {
    let mathElements = document.getElementsByClassName("mathcalculator");
    if (this.elementsToTypeset < 0) {
      this.elementsToTypeset = mathElements.length;
      this.typesetTotal = 0;
    }
    for (; this.typesetTotal < mathElements.length; this.typesetTotal++) {
      if (this.postponeTypeset()) {
        return;
      }
      /** @type {HTMLElement} */
      let element = mathElements[this.typesetTotal];
      if (element["typeset"] === "true") {
        continue;
      }
      element["typeset"] = "true";
      let startTime = (new Date()).getTime();
      mathFromElement(element, false, this.sanitizeLatexSource, this.removeDisplayStyle);
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
  /** @type {string} */
  style,
  /**@type{boolean} whether to convert the original latex to parsed one.*/
  sanitizeLatexSource,
  /**@type{boolean} whether to remove \\displaystyle from latex source.*/
  removeDisplayStyle,
  /**@type{boolean} whether to log in the console timing statistics.*/
  logTiming,
) {
  if (style === "") {
    style = "font-family:'Times New Roman'; display:inline-block;";
  }
  new MathTagCoverter(style, sanitizeLatexSource, removeDisplayStyle, logTiming).typeset(toBeModified);
}

function initializeTestPage(
  /** @type{string} */
  equationEditorId,
  /** @type{string} */
  latexInputId,
  /** @type{string} */
  debugId,
) {
  let editorElement = document.getElementById(equationEditorId);
  const options = new EquationEditorOptions({
    latexInput: document.getElementById(latexInputId),
    debugLogContainer: document.getElementById(debugId),
  });
  let editor = new EquationEditor(editorElement, options);
  editor.updateDOM();
  editor.rootNode.focus(- 1);

  setTimeout(() => {
    editor.sendKeys([/*
      "(", "1", "/", "1",
      "1", "/", "1", "ArrowUp", "ArrowUp", "ArrowUp", "(",
      "1", "+", "1",
      "1", "+", "1", "/", "1", "+", "1", "/",
      "1", "+", "1", "/", "1",
      "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp", "ArrowUp",
      "+", "1", "/", "1",*/
    ]);
  }, 300);
}

module.exports = {
  initializeTestPage,
  typeset,
  EquationEditor,
  EquationEditorOptions,
  mathFromLatex,
  mathFromElement,
  latexConstants,
};