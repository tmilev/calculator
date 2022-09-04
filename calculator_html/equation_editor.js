// Licensed to you under the Apache 2.0 license.
//
// Documentation of the equation editor can be found in:
//
// https://calculator-algebra.org:8166/calculator_html/test_equation_editor.html
//
// The source code of the editor can be found in:
//
// https://github.com/tmilev/calculator
//
// The formatting in this file follows
// an auto-code formatting associated with the closure compiler.
//
// We are not using 'use strict' as that creates
// a "uselessCode" compiler error with
// the closure compiler.

class MathNodeType {
  /**
   * Inserts default values in the input object to make it a valid
   * MathNodeType.
   */
  normalizeConstructorInput(input) {
    for (let label in knownTypeDefaults) {
      if (!(label in input)) {
        input[label] = knownTypeDefaults[label];
      }
    }
    if (input['arrows'] === undefined) {
      input['arrows'] = knownTypeDefaultsArrows;
      return;
    }
    for (let label in knownTypeDefaultsArrows) {
      if (!(label in input['arrows'])) {
        input['arrows'][label] = knownTypeDefaultsArrows[label];
      }
    }
  }

  /** Constructs a MathNodeType, used to construct a MathNode. */
  constructor(input) {
    this.normalizeConstructorInput(input);
    /** @type {string} */
    this.arrows = input['arrows'];
    /** @type {string} */
    this.type = input['type'];
    /** @type {string} */
    this.width = input['width'];
    /** @type {string} */
    this.height = input['height'];
    /** @type {string} */
    this.display = input['display'];
    /** @type {string} */
    this.verticalAlign = input['verticalAlign'];
    /** @type {string} */
    this.outline = input['outline'];
    /** @type {number} */
    this.fontSizeRatio = input['fontSizeRatio'];
    /** @type {string} */
    this.fontWeight = input['fontWeight'];
    /** @type {string} */
    this.whiteSpace = input['whiteSpace'];
    /** @type {string} */
    this.textAlign = input['textAlign'];
    /** @type {string} */
    this.position = input['position'];
    /** @type {string} */
    this.minWidth = input['minWidth'];
    /** @type {string} */
    this.cursor = input['cursor'];
    /** @type {string} */
    this.overflow = input['overflow'];
    /** @type {string} */
    this.boxSizing = input['boxSizing'];
    /** @type {string} */
    this.colorImplied = input['colorImplied'];
    /** @type {string} */
    this.colorText = input['colorText'];
    // Padding.
    /** @type {string} */
    this.paddingBottom = input['paddingBottom'];
    /** @type {number} */
    this.paddingBottomRatioForSVG = input['paddingBottomRatioForSVG'];
    /** @type {string} */
    this.paddingTop = input['paddingTop'];
    /** @type {string} */
    this.paddingRight = input['paddingRight'];
    /** @type {string} */
    this.paddingLeft = input['paddingLeft'];
    /** @type {string} */
    this.padding = input['padding'];
    // Margins.
    /** @type {string} */
    this.margin = input['margin'];
    /** @type {string} */
    this.marginBottom = input['marginBottom'];
    /** @type {string} */
    this.marginRight = input['marginRight'];
    /** @type {string} */
    this.marginLeft = input['marginLeft'];
    // Borders
    /** @type {string} */
    this.borderTopLeftRadius = input['borderTopLeftRadius'];
    /** @type {string} */
    this.borderTopRightRadius = input['borderTopRightRadius'];
    /** @type {string} */
    this.borderBottomLeftRadius = input['borderBottomLeftRadius'];
    /** @type {string} */
    this.borderBottomRightRadius = input['borderBottomRightRadius'];
    /** @type {string} */
    this.borderStyle = input['borderStyle'];
    /** @type {string} */
    this.borderColor = input['borderColor'];
    /** @type {string} */
    this.borderBottom = input['borderBottom'];
    /** @type {string} */
    this.borderTop = input['borderTop'];
    /** @type {string} */
    this.borderLeft = input['borderLeft'];
    /** @type {string} */
    this.borderRight = input['borderRight'];
  }

  /** @return {MathNodeType!} */
  clone() {
    let result = new MathNodeType({});
    Object.assign(result, this);
    return result;
  }
}

const knownTypeDefaults = {
  'width': '',
  'height': '',
  'minWidth': '',
  'display': 'inline-block',
  'verticalAlign': '',
  'fontSizeRatio': 1,
  'fontWeight': '',
  'whiteSpace': '',
  'textAlign': '',
  'position': 'absolute',
  'cursor': '',
  'outline': '',
  'overflow': '',
  'boxSizing': '',
  // Colors
  'colorText': '',
  'colorImplied': '',
  // Padding
  'paddingBottom': '',
  'paddingBottomRatioForSVG': 1,
  'paddingTop': '',
  'paddingLeft': '',
  'paddingRight': '',
  'padding': '',
  // Margins
  'marginBottom': '',
  'marginLeft': '',
  'marginRight': '',
  'margin': '',
  // Borders.
  'borderBottomLeftRadius': '',
  'borderBottomRightRadius': '',
  'borderTopLeftRadius': '',
  'borderTopRightRadius': '',
  'borderStyle': '',
  'borderColor': '',
  'borderBottom': '',
  'borderTop': '',
  'borderLeft': '',
  'borderRight': '',
};

class ArrowMotionTypes {
  constructor() {
    /** @type {string} */
    this.parentForward = 'parentForward';
    /** @type {string} */
    this.firstAtomToTheLeft = 'firstAtomToTheLeft';
    /** @type {string} */
    this.firstAtomToTheRight = 'firstAtomToTheRight';
    /** @type {string} */
    this.firstAtomUp = 'firstAtomUp';
    /** @type {string} */
    this.firstAtomDown = 'firstAtomDown';
  }
}

let arrowMotion = new ArrowMotionTypes();

const knownTypeDefaultsArrows = {
  'ArrowUp': arrowMotion.parentForward,
  'ArrowDown': arrowMotion.parentForward,
  'ArrowLeft': arrowMotion.firstAtomToTheLeft,
  'ArrowRight': arrowMotion.firstAtomToTheRight,
};

const defaultFractionScale = 0.9;

/**
 * @type {{
 * root:MathNodeType!,
 * atom:MathNodeType!,
 * formInput:MathNodeType!,
 * atomImmutable:MathNodeType!,
 * eventCatcher:MathNodeType!,
 * error:MathNodeType!,
 * leftDelimiterMark:MathNodeType!,
 * leftDelimiter:MathNodeType!,
 * rightDelimiterMark:MathNodeType!,
 * rightDelimiter:MathNodeType!,
 * cancelSign:MathNodeType!,
 * sqrtSignDecoration:MathNodeType!,
 * sqrtSign:MathNodeType!,
 * sqrtSignLeft:MathNodeType!,
 * sqrtSignRight:MathNodeType!,
 * radicalExponentBox:MathNodeType!,
 * topLeftQuarterCircle:MathNodeType!,
 * horizontalLineBottomMargin:MathNodeType!,
 * bottomRightQuarterCircle:MathNodeType!,
 * bottomLeftQuarterCircle:MathNodeType!,
 * topRightQuarterCircle:MathNodeType!,
 * horizontalBraceUp:MathNodeType!,
 * horizontalMath:MathNodeType!,
 * verticalLineLeftMargin:MathNodeType!,
 * verticalLineRightMargin:MathNodeType!,
 * operatorStandalone:MathNodeType!,
 * matrixTable:MathNodeType!,
 * fraction:MathNodeType!,
 * matrix:MathNodeType!,
 * baseWithExponent:MathNodeType!,
 * sqrt:MathNodeType!,
 * matrixRow:MathNodeType!,
 * radicalUnderBox:MathNodeType!,
 * numerator:MathNodeType!,
 * denominator:MathNodeType!,
 * overLinedBox:MathNodeType!,
 * overBrace:MathNodeType!,
 * baseWithSubscript:MathNodeType!,
 * genericMathBox:MathNodeType!,
 * cancel:MathNodeType!,
 * cancelUnderBox:MathNodeType!,
 * exponent:MathNodeType!,
 * subscript:MathNodeType!,
 * matrixRowEntry:MathNodeType!,
 * verticalLineInTable:MathNodeType!,
 * operatorWithSuperAndSubscript:MathNodeType!,
 * operatorWithSubscript:MathNodeType!,
 * operatorSubscript:MathNodeType!,
 * operatorSuperscript:MathNodeType!,
 * }}
 */
const knownTypes = {
  root: new MathNodeType({
    'type': 'root',
    'padding': '2px',
    'margin': '2px',
    'cursor': 'text',
    'minWidth': '30px',
  }),
  // A math expression with no children such as "x", "2".
  // This is the only element type that has contentEditable = true;
  atom: new MathNodeType({
    'type': 'atom',
    'outline': '0px solid transparent',
    'width': 'auto',
    'height': 'auto',
    'minWidth': '4px',
    'verticalAlign': 'text-bottom',
    'overflow': 'hidden',
    'boxSizing': 'border-box',
  }),
  // An input element: rather than being a contentEditable div,
  // this is a text-only input box.
  formInput: new MathNodeType({
    'type': 'formInput',
    'boxSizing': 'border-box',
  }),
  // A non-editable math expression/operator such as "+" or "-".
  atomImmutable: new MathNodeType({
    'type': 'atomImmutable',
    'paddingLeft': `${0.1}em`,
    'paddingRight': `${0.1}em`,
    'outline': '0px solid transparent',
    'textAlign': 'center',
    'width': 'auto',
    'height': 'auto',
    'boxSizing': 'border-box',
  }),
  eventCatcher: new MathNodeType({
    'type': 'eventCatcher',
    'width': '0px',
    'maxWidth': '0px',
    'height': '0px',
    'maxHeight': '0px',
    'display': 'hidden',
    'overflow': 'hidden',
  }),
  error: new MathNodeType({
    'type': 'error',
    'colorText': 'red',
    'whiteSpace': 'nowrap',
  }),
  leftDelimiterMark: new MathNodeType({
    'type': 'leftDelimiterMark',
    'borderLeft': '2px solid',
    'colorImplied': 'silver',
    'boxSizing': 'border-box',
  }),
  // Left delimiter (parentheses, bracket, ...)
  leftDelimiter: new MathNodeType({
    'type': 'leftDelimiter',
    'colorImplied': 'silver',
  }),
  // Right delimiter mark surrounded by two spacers.
  rightDelimiterMark: new MathNodeType({
    'type': 'rightDelimiterMark',
    'borderRight': '2px solid',
    'colorImplied': 'silver',
    'boxSizing': 'border-box',
  }),
  // Right delimiter (parentheses, bracket, ...)
  /** @type {MathNodeType!} */
  rightDelimiter: new MathNodeType({
    'type': 'rightDelimiter',
    'colorImplied': 'silver',
  }),
  // Horizontally laid out math such as "x+2".
  // The example "x+2" consists of the three elements "x", "+" and 2.
  // Not allowed to contain other horizontally laid out math elements.
  horizontalMath: new MathNodeType({
    'type': 'horizontalMath',
    'whiteSpace': 'nowrap',
    'verticalAlign': 'text-bottom',
    'textAlign': 'center',
  }),
  verticalLineInTable: new MathNodeType({
    'type': 'verticalLine',
    'borderLeft': '1px solid black',
  }),
  // Represents expressions such as "x/y" or "\frac{x}{y}".
  fraction: new MathNodeType({
    'type': 'fraction',
  }),
  cancel: new MathNodeType({
    'type': 'cancel',
  }),
  cancelSign: new MathNodeType({
    'type': 'cancelSign',
    'borderLeft': '2px solid black',
  }),
  cancelUnderBox: new MathNodeType({
    'type': 'cancelUnderBox',
  }),
  // Represents the numerator x of a fraction x/y.
  numerator: new MathNodeType({
    'type': 'numerator',
    'borderBottom': '1px solid black',
    'fontSizeRatio': defaultFractionScale,
    'arrows': {
      'ArrowUp': arrowMotion.firstAtomToTheLeft,
      'ArrowDown': arrowMotion.firstAtomToTheRight,
    },
    'whiteSpace': 'nowrap',
    'textAlign': 'center',
  }),
  // Represents the denominator y of a fraction x/y.
  denominator: new MathNodeType({
    'type': 'denominator',
    'fontSizeRatio': defaultFractionScale,
    'arrows': {
      'ArrowUp': arrowMotion.firstAtomToTheLeft,
      'ArrowDown': arrowMotion.firstAtomToTheRight,
    },
    'textAlign': 'center',
  }),
  overLinedBox: new MathNodeType({
    'type': 'overLinedBox',
    'borderTop': '1px solid black',
    'boxSizing': 'border-box',
  }),
  genericMathBox: new MathNodeType({
    'type': 'genericMathBox',
  }),
  baseWithExponent: new MathNodeType({
    'type': 'baseWithExponent',
  }),
  exponent: new MathNodeType({
    'type': 'exponent',
    'fontSizeRatio': 0.75,
  }),
  baseWithSubscript: new MathNodeType({
    'type': 'baseWithSubscript',
  }),
  underBrace: new MathNodeType({
    'type': 'underBrace',
  }),
  overBrace: new MathNodeType({
    'type': 'overBrace',
  }),
  horizontalBraceUp: new MathNodeType({
    'type': 'horizontalBraceUp',
  }),
  topRightQuarterCircle: new MathNodeType({
    'type': 'topRightQuarterCircle',
    'borderTopRightRadius': '4px',
  }),
  topLeftQuarterCircle: new MathNodeType({
    'type': 'topLeftQuarterCircle',
    'borderTopLeftRadius': '4px',
  }),
  bottomRightQuarterCircle: new MathNodeType({
    'type': 'bottomRightQuarterCircle',
    'borderBottomRightRadius': '4px',
  }),
  bottomLeftQuarterCircle: new MathNodeType({
    'type': 'bottomLeftQuarterCircle',
    'borderBottomLeftRadius': '4px',
  }),
  horizontalLineBottomMargin: new MathNodeType({
    'type': 'horizontalLineBottomMargin',
    'borderBottom': '2px solid black',
  }),
  verticalLineLeftMargin: new MathNodeType({
    'type': 'verticalLineLeftMargin',
    'borderLeft': 'solid',
  }),
  verticalLineRightMargin: new MathNodeType({
    'type': 'verticalLineRightMargin',
    'borderRight': 'solid',
  }),
  subscript: new MathNodeType({
    'type': 'subscript',
    'fontSizeRatio': 0.75,
  }),
  sqrt: new MathNodeType({
    'type': 'sqrt',
    'margin': '2px',
  }),
  sqrtSign: new MathNodeType({
    'type': 'sqrtSign',
  }),
  sqrtSignDecoration: new MathNodeType({
    'type': 'sqrtSignDecoration',
    'borderTop': '1px solid black',
  }),
  sqrtSignLeft: new MathNodeType({
    'type': 'sqrtSignLeft',
    'borderLeft': '1px solid black',
  }),
  sqrtSignRight: new MathNodeType({
    'type': 'sqrtSignRight',
    'borderLeft': '1px solid black',
  }),
  radicalExponentBox: new MathNodeType({
    'type': 'radicalExponentBox',
    'fontSizeRatio': 0.75,
  }),
  radicalUnderBox: new MathNodeType({
    'type': 'radicalUnderBox',
    'borderTop': '1px solid black',
  }),
  matrix: new MathNodeType({
    'type': 'matrix',
  }),
  matrixTable: new MathNodeType({
    'type': 'matrixTable',
  }),
  matrixRow: new MathNodeType({
    'type': 'matrixRow',
  }),
  matrixRowEntry: new MathNodeType({
    'type': 'matrixRowEntry',
    'arrows': {
      'ArrowUp': arrowMotion.firstAtomUp,
      'ArrowDown': arrowMotion.firstAtomDown,
    },
  }),
  operatorWithSuperAndSubscript: new MathNodeType({
    'type': 'operatorWithSuperAndSubscript',
  }),
  operatorWithSubscript: new MathNodeType({
    'type': 'operatorWithSubscript',
  }),
  operatorStandalone: new MathNodeType({
    'type': 'operatorStandalone',
    'fontSizeRatio': 1.8,
  }),
  operatorSubscript: new MathNodeType({
    'type': 'operatorSubscript',
    'fontSizeRatio': 0.55,
  }),
  operatorSuperscript: new MathNodeType({
    'type': 'operatorSuperscript',
    'fontSizeRatio': 0.55,
  }),
};

class MathNodeFactory {
  /**
   * Constructs horizontal math.
   * @return {MathNodeHorizontalMath!}
   */
  horizontalMath(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      content,
  ) {
    const result = new MathNodeHorizontalMath(equationEditor);
    if (content === null) {
      content = this.atom(equationEditor, '');
    }
    result.appendChild(content);
    return result;
  }

  /**
   * Constructs a generic math box.
   * @return {MathNodeGenericBox!}
   */
  genericMathBox(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      content,
  ) {
    const result = new MathNodeGenericBox(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, content));
    return result;
  }

  /**
   * @return {MathNodeHorizontalMath!}
   */
  // Returns a horizontal math with content given by the input array.
  // Normalizes the input but does not ensure editable atoms at the ends.
  horizontalMathFromArray(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {Array.<MathNode?>!} */
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
      result.appendChild(/** @type {MathNode!} */ (content[i]));
    }
    result.normalizeHorizontalMath();
    return result;
  }

  /**
   * Constructs math atom.
   *
   * @return {MathNodeAtom!}
   */
  atom(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      initialContent,
  ) {
    const result = new MathNodeAtom(equationEditor);
    result.positionCursorBeforeKeyEvents = initialContent.length;
    result.initialContent = initialContent;
    return result;
  }

  /**
   * Constructs immutable math atom.
   *
   * @return {MathNodeAtomImmutable!}
   */
  atomImmutable(
      /** @type {EquationEditor!} */
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
    if (operator in latexConstants.operatorsExtraVerticalPadding) {
      result.type.paddingBottom =
          latexConstants.operatorsExtraVerticalPadding[operator];
    }
    if (operator in latexConstants.operatorsExtraVerticalPaddingForSVG) {
      result.type.paddingBottomRatioForSVG =
          latexConstants.operatorsExtraVerticalPaddingForSVG[operator];
    }
    result.initialContent = operator;
    return result;
  }

  /** @return {MathNodeFraction!} */
  fraction(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?}*/
      numeratorContent,
      /** @type {MathNode?}*/
      denominatorContent,
  ) {
    const fraction = new MathNodeFraction(equationEditor);
    const numerator = new MathNodeNumerator(equationEditor);
    const denominator = new MathNodeDenominator(equationEditor);
    numerator.appendChild(
        this.horizontalMath(equationEditor, numeratorContent));
    denominator.appendChild(
        this.horizontalMath(equationEditor, denominatorContent));
    fraction.appendChild(numerator);
    fraction.appendChild(denominator);
    return fraction;
  }

  /** @return {MathNodeRoot!} */
  rootMath(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    const result = new MathNodeRoot(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, null));
    return result;
  }

  /** @return {MathNodeError!} */
  error(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      initialContent,
  ) {
    const result = new MathNodeError(equationEditor);
    result.initialContent = initialContent;
    return result;
  }

  /** @return {MathNodeCancel!} */
  cancel(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?}*/
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

  /** @return {MathNodeSqrtSign!} */
  sqrtSign(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    let result = new MathNodeSqrtSign(equationEditor);
    result.appendChild(
        new MathNode(equationEditor, knownTypes.sqrtSignDecoration));
    result.appendChild(new MathNode(equationEditor, knownTypes.sqrtSignLeft));
    result.appendChild(new MathNode(equationEditor, knownTypes.sqrtSignRight));
    return result;
  }

  /** @return {MathNodeSqrt!} */
  sqrt(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      underTheRadicalContent,
      /** @type {MathNode?} */
      exponentContent,
  ) {
    const sqrt = new MathNodeSqrt(equationEditor);
    const radicalExponentBox =
        new MathNode(equationEditor, knownTypes.radicalExponentBox);
    radicalExponentBox.appendChild(
        this.horizontalMath(equationEditor, exponentContent));
    sqrt.appendChild(radicalExponentBox);
    sqrt.appendChild(mathNodeFactory.sqrtSign(equationEditor));
    const underTheRadical = new MathNodeRadicalUnderBox(equationEditor);
    underTheRadical.appendChild(
        mathNodeFactory.horizontalMath(equationEditor, underTheRadicalContent));
    sqrt.appendChild(underTheRadical);
    return sqrt;
  }

  /** @return {MathNodeFormInput!} */
  formInput(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      content,
  ) {
    let nameAndContent = content.split(',');
    if (nameAndContent.length < 2) {
      nameAndContent.push('');
      nameAndContent.push('');
    }
    nameAndContent[0] = nameAndContent[0].trim();
    nameAndContent[1] = nameAndContent[1].trim();
    const formInput = new MathNodeFormInput(
        equationEditor,
        nameAndContent[0],
        nameAndContent[1],
    );
    return formInput;
  }

  /** @return {MathNodeOverLine!} */
  overLine(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      content,
  ) {
    const result = new MathNodeOverLine(equationEditor);
    const horizontalMath = this.horizontalMath(equationEditor, content);
    result.appendChild(horizontalMath);
    return result;
  }

  /** @return {MathNodeOverBrace!} */
  overBrace(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      content,
      /** @type {MathNode?} */
      overBraceContent,
  ) {
    const result = new MathNodeOverBrace(equationEditor);
    const base = mathNodeFactory.horizontalMath(equationEditor, content);
    // 6 components of overbrace line
    const horizontalBraceTopLeft =
        new MathNode(equationEditor, knownTypes.topLeftQuarterCircle);
    const horizontalBraceLeft =
        new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceBottomRight =
        new MathNode(equationEditor, knownTypes.bottomRightQuarterCircle);
    const horizontalBraceBottomLeft =
        new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle);
    const horizontalBraceRight =
        new MathNode(equationEditor, knownTypes.horizontalLineBottomMargin);
    const horizontalBraceTopRight =
        new MathNode(equationEditor, knownTypes.topRightQuarterCircle);

    const horizontalBrace =
        new MathNode(equationEditor, knownTypes.horizontalBraceUp);
    horizontalBrace.appendChildren([
      horizontalBraceTopLeft,
      horizontalBraceLeft,
      horizontalBraceBottomRight,
      horizontalBraceBottomLeft,
      horizontalBraceRight,
      horizontalBraceTopRight,
    ]);
    let superscript =
        mathNodeFactory.genericMathBox(equationEditor, overBraceContent);
    result.appendChild(base);
    result.appendChild(horizontalBrace);
    result.appendChild(superscript);
    return result;
  }

  /** @return {MathNodeBaseWithExponent!} */
  baseWithExponent(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      base,
      /** @type {MathNode?}*/
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
    const baseHorizontal = new MathNodeHorizontalMath(equationEditor);
    baseHorizontal.appendChild(base);
    baseHorizontal.normalizeHorizontalMath();
    // The base with the exponent.
    baseWithExponent.appendChild(baseHorizontal);
    baseWithExponent.appendChild(exponentWrapped);
    return baseWithExponent;
  }

  /** @return {MathNodeBaseWithSubscript!} */
  baseWithSubscript(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode!} */
      base,
      /** @type {MathNode?} */
      subscript,
  ) {
    // Horizontal math wrapper for the exponent.
    const subscriptWrapped = new MathNodeSubscript(equationEditor);
    let subscriptContainer = this.horizontalMath(equationEditor, subscript);
    subscriptContainer.normalizeHorizontalMath();
    subscriptContainer.ensureEditableAtoms();
    subscriptWrapped.appendChild(subscriptContainer);
    // Horizontal math wrapper for the base.
    const baseHorizontal = new MathNodeHorizontalMath(equationEditor);
    baseHorizontal.appendChild(base);
    baseHorizontal.normalizeHorizontalMath();
    // The base with the subscript.
    const baseWithSubscript = new MathNodeBaseWithSubscript(equationEditor);
    baseWithSubscript.appendChild(baseHorizontal);
    baseWithSubscript.appendChild(subscriptWrapped);
    return baseWithSubscript;
  }

  /** @return {MathNodeLeftDelimiter!} */
  leftParenthesis(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      implied,
  ) {
    return this.leftDelimiter(equationEditor, '(', implied);
  }

  /** @return {MathNodeLeftDelimiter!} */
  leftDelimiter(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      content,
      /** @type {boolean} */
      implied,
  ) {
    const leftDelimiter = new MathNodeLeftDelimiter(equationEditor);
    leftDelimiter.implied = implied;
    if (content === '') {
      return leftDelimiter;
    }
    leftDelimiter.appendChild(
        this.delimiterMark(equationEditor, content, true, implied));
    leftDelimiter.extraData = content;
    return leftDelimiter;
  }

  /**
   * @return {MathNodeAbsoluteValue!|MathNodeAngleBrackets!|
   * MathNodeCurlyBrace!|MathNodeParenthesis!|MathNodeSquareBrackets!}
   */
  delimiterMark(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      content,
      /** @type {boolean} */
      left,
      /** @type {boolean} */
      implied,
  ) {
    let result = null;
    switch (content) {
      case '|':
        result = new MathNodeAbsoluteValue(equationEditor, left);
        break;
      case '\\rangle':
      case '\\langle':
      // utf16 version of \\rangle
      case '\u27E9':
      // utf16 version of \\langle
      case '\u27E8':
        result = new MathNodeAngleBrackets(equationEditor, left);
        if (left) {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
        } else {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
        }
        break;
      case '[':
      case ']':
        result = new MathNodeSquareBrackets(equationEditor, left);
        break;
      case '(':
      case ')':
        result = new MathNodeParenthesis(equationEditor, left);
        if (left) {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
        } else {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineRightMargin));
        }
        break;
      case '{':
      case '}':
      default:
        result = new MathNodeCurlyBrace(equationEditor, left);
        if (left) {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.topLeftQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(
              equationEditor, knownTypes.bottomRightQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle));
        } else {
          result.appendChild(
              new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.bottomLeftQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.topRightQuarterCircle));
          result.appendChild(
              new MathNode(equationEditor, knownTypes.verticalLineLeftMargin));
          result.appendChild(new MathNode(
              equationEditor, knownTypes.bottomRightQuarterCircle));
        }
        break;
    }
    result.implied = implied;
    return result;
  }

  /** @return {MathNodeRightDelimiter!} */
  rightParenthesis(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      implied,
  ) {
    return this.rightDelimiter(equationEditor, ')', implied);
  }

  /** @return {MathNodeRightDelimiter!} */
  rightDelimiter(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      content,
      /** @type {boolean} */
      implied,
  ) {
    const rightDelimiter = new MathNodeRightDelimiter(equationEditor);
    rightDelimiter.implied = implied;
    if (content === '') {
      return rightDelimiter;
    }
    rightDelimiter.appendChild(
        this.delimiterMark(equationEditor, content, false, implied));
    rightDelimiter.extraData = content;
    return rightDelimiter;
  }

  /** @return {MathNodeMatrix!} */
  matrix(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {number} */
      rows,
      /** @type {number} */
      columns,
      /**
       * For the environment \begin{array}{rcl}a&=&b\end{array},
       * columnStyle contains the
       * rcl
       * column spec string
       * @type {string}
       */
      columnStyle,
      /** @type{string} */
      matrixEnvironment,
  ) {
    const matrixTable = new MathNode(equationEditor, knownTypes.matrixTable);
    for (let i = 0; i < rows; i++) {
      matrixTable.appendChild(this.matrixRow(equationEditor, columns));
    }
    let leftDelimiter = null;
    let rightDelimiter = null;
    if (columnStyle === '') {
      if (matrixEnvironment === 'bmatrix') {
        leftDelimiter = this.leftDelimiter(equationEditor, '[', false);
        rightDelimiter = this.rightDelimiter(equationEditor, ']', false);
      } else if (
          matrixEnvironment === 'pmatrix' || matrixEnvironment == 'binom') {
        leftDelimiter = this.leftParenthesis(equationEditor, false);
        rightDelimiter = this.rightParenthesis(equationEditor, false);
      } else if (matrixEnvironment === 'cases') {
        leftDelimiter = this.leftDelimiter(equationEditor, '{', false);
        rightDelimiter = this.rightDelimiter(equationEditor, '', false);
      } else {
        leftDelimiter = this.leftDelimiter(equationEditor, '', false);
        rightDelimiter = this.rightDelimiter(equationEditor, '', false);
      }
    } else {
      leftDelimiter = this.leftDelimiter(equationEditor, '', false);
      rightDelimiter = this.rightDelimiter(equationEditor, '', false);
    }
    let parenthesesLayout = this.horizontalMath(equationEditor, leftDelimiter);
    parenthesesLayout.appendChild(matrixTable);
    parenthesesLayout.appendChild(rightDelimiter);
    let result = new MathNodeMatrix(equationEditor, matrixEnvironment);
    result.latexExtraStyle = columnStyle;
    result.appendChild(parenthesesLayout);
    result.appendChildren(
        new LatexColumnStyleIterator(columnStyle)
            .verticalSeparators(equationEditor),
    );
    return result;
  }

  /** @return {MathNodeMatrixRow!} */
  matrixRow(
      /** @type {EquationEditor!} */
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

  /** @return {MathNodeRowEntry!} */
  matrixRowEntry(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNode?} */
      content,
  ) {
    let result = new MathNodeRowEntry(equationEditor);
    result.appendChild(this.horizontalMath(equationEditor, content));
    return result;
  }

  /** @return {MathNodeVerticalLine!} */
  matrixVerticalLine(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {number} */
      columnIndex,
      /** @type {number} */
      numberOfStrips,
  ) {
    let result = new MathNodeVerticalLine(equationEditor);
    result.extraData = new VerticalBarData(columnIndex, numberOfStrips);
    return result;
  }

  /** @return {MathNodeOperatorWithSuperAndSubscript!} */
  operatorWithSuperAndSubscript(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      operator,
      /** @type {MathNode?} */
      superscript,
      /** @type {MathNode?} */
      subscript,
  ) {
    let result = new MathNodeOperatorWithSuperAndSubscript(equationEditor);
    let superscriptNode = new MathNodeOperatorSuperscript(equationEditor);
    let subscriptNode = new MathNodeOperatorSubscript(equationEditor);
    let operatorNode = new MathNodeOperatorStandalone(equationEditor);
    superscriptNode.appendChild(
        this.horizontalMath(equationEditor, superscript));
    subscriptNode.appendChild(this.horizontalMath(equationEditor, subscript));
    operatorNode.appendChild(this.atomImmutable(equationEditor, operator));
    result.appendChild(superscriptNode);
    result.appendChild(operatorNode);
    result.appendChild(subscriptNode);
    return result;
  }

  /** @return {MathNodeOperatorWithSubscript!} */
  operatorWithSubscript(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      operator,
      /** @type {MathNode?} */
      subscript,
  ) {
    let result = new MathNodeOperatorWithSubscript(equationEditor);
    let subscriptNode = new MathNodeOperatorSubscript(equationEditor);
    let subscriptScale = 0.8;
    subscriptNode.type.fontSizeRatio = subscriptScale;
    let operatorNode = new MathNodeOperatorStandalone(equationEditor);
    let operatorScale = 1;
    operatorNode.type.fontSizeRatio = operatorScale;
    subscriptNode.appendChild(this.horizontalMath(equationEditor, subscript));
    operatorNode.appendChild(this.atomImmutable(equationEditor, operator));
    result.appendChild(operatorNode);
    result.appendChild(subscriptNode);
    return result;
  }
}

class MathNodeWithCursorPosition {
  constructor(
      /** @type {MathNode?} */
      element,
      /** @type {number} */
      position,
  ) {
    /** @type {MathNode?} */
    this.element = element;
    /** @type {number} */
    this.position = position;
    if (this.element === null) {
      this.position = -1;
    }
  }

  assign(
      /** @type {MathNodeWithCursorPosition?} */
      other,
  ) {
    if (other === null) {
      this.element = null;
      this.position = -1;
      return;
    }
    this.element = other.element;
    this.position = other.position;
  }

  /** @return {number} */
  getPositionWholeItemIfNegative(
      /**
       * @type {number} If position is negative,
       * determines which end to select:
       * negative direction for left end, non-negative for right.
       */
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

  /** @return {number} */
  nextPositionInDirection(
      /** @type {number} */
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
    return -1;
  }

  /** @return {MathNodeWithCursorPosition!} */
  leftHorizontalNeighborBalanced(
      /** @type {boolean} */
      shrinking,
  ) {
    if (this.element === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    if (this.position > 0 && this.element.hasHorozintalMathParent()) {
      return new MathNodeWithCursorPosition(
          this.element, this.nextPositionInDirection(-1));
    }
    let next = null;
    if (shrinking &&
        this.element.type.type === knownTypes.rightDelimiter.type) {
      let matching = this.element.findMatchingDelimiter();
      if (matching === null) {
        return new MathNodeWithCursorPosition(null, -1);
      }
      next = matching.firstAtomSiblingOrUncle(-1);
    } else {
      next = this.element.firstAtomSiblingOrUncle(-1);
    }
    if (next === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    return new MathNodeWithCursorPosition(next, -1);
  }

  /** @return {MathNodeWithCursorPosition!} */
  rightHorizontalNeighborBalanced(
      /** @type {boolean} */
      shrinking,
  ) {
    if (this.element === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    if (this.position >= 0 &&
        this.position < this.element.lengthContentIfAtom() &&
        this.element.hasHorozintalMathParent()) {
      return new MathNodeWithCursorPosition(
          this.element, this.nextPositionInDirection(1));
    }
    let next = null;
    if (shrinking && this.element.type.type === knownTypes.leftDelimiter.type) {
      let matching = this.element.findMatchingDelimiter();
      if (matching === null) {
        return new MathNodeWithCursorPosition(null, -1);
      }
      next = matching.firstAtomSiblingOrUncle(1);
    } else {
      next = this.element.firstAtomSiblingOrUncle(1);
    }
    if (next === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    return new MathNodeWithCursorPosition(next, -1);
  }

  /** @return {MathNodeWithCursorPosition!} */
  leftNeighbor() {
    if (this.element === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    if (this.position > 0) {
      return new MathNodeWithCursorPosition(
          this.element, this.nextPositionInDirection(-1));
    }
    let resultElement = this.element.firstAtomToTheLeft();
    if (resultElement === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    return new MathNodeWithCursorPosition(
        resultElement, resultElement.element.textContent.length);
  }

  /** @return {MathNodeWithCursorPosition!} */
  rightNeighbor() {
    if (this.element === null) {
      return new MathNodeWithCursorPosition(null, -1);
    }
    if (this.position < this.element.lengthContentIfAtom()) {
      return new MathNodeWithCursorPosition(
          this.element, this.nextPositionInDirection(1));
    }
    return new MathNodeWithCursorPosition(
        this.element.firstAtomToTheRight(), 0);
  }

  /** @return {string} */
  toString() {
    if (this.element === null) {
      return `[null, ${this.position}]`;
    }
    let detached = '';
    if (this.element.isDetached()) {
      detached = ',<b style=\'color:red\'> detached</b>';
    }
    return `[${this.element.toString()}, ${this.position}${detached}]`;
  }
}

/** @return {EquationEditor!} */
// Converts the textContent of an html element to typeset math.
function mathFromElement(
    /** @type {HTMLElement!} */
    container,
    /** @type {boolean} */
    editable,
    /** @type {boolean} */
    sanitizeLatexSource,
    /** @type {boolean} whether to remove \\displaystyle from latex source.*/
    removeDisplayStyle,
    /** @type {Function?} callback after the element has been typeset.*/
    callback,
    /** @type {HTMLElement?} */
    containerSVG,
    /** @type {boolean} */
    copyButton,
) {
  let content = container.textContent;
  if (content === null) {
    content = '';
  }
  return mathFromLatex(
      container, content, editable, sanitizeLatexSource, removeDisplayStyle,
      callback, containerSVG, copyButton);
}

/** @return {EquationEditor!} Returns typeset math.*/
function mathFromLatex(
    /** @type {HTMLElement!} */
    container,
    /** @type {string} */
    latex,
    /** @type {boolean} */
    editable,
    /** @type {boolean} */
    sanitizeLatexSource,
    /** @type {boolean} whether to remove \\displaystyle from latex source.*/
    removeDisplayStyle,
    /** @type {Function?} callback after the element has been typeset*/
    callback,
    /** @type {HTMLElement?} */
    containerSVG,
    /** @type {boolean} */
    copyButton,
) {
  let lineBreakWidthString = container.getAttribute('lineBreakWidth');
  let lineBreakWidth = 0;
  if (lineBreakWidthString !== '') {
    lineBreakWidth = parseInt(lineBreakWidthString, 10);
  }
  let result = new EquationEditor(
      container, new EquationEditorOptions({
        editable: editable,
        sanitizeLatexSource: sanitizeLatexSource,
        removeDisplayStyle: removeDisplayStyle,
        logTiming: true,
        lineBreakWidth: lineBreakWidth,
        copyButton: copyButton,
      }),
      containerSVG);
  result.writeLatex(latex);
  if (callback !== undefined && callback !== null) {
    callback(result);
  }
  return result;
}

/** @return {Element!}*/
function boldElement(
    /** @type {string} */
    text,
    /** @type {string} */
    color,
) {
  /** @type {Element!} */
  let result = document.createElement('b');
  result.textContent = text;
  result.style.color = color;
  return result;
}

class SyntacticElement {
  constructor(
      /** @type {MathNode?} */
      node,
      /** @type {string} */
      content,
      /** @type {string} */
      syntacticRole,
  ) {
    /** @type {MathNode?} contains parsed math content. */
    this.node = node;
    /** @type {string} contains non-parsed atomic content. */
    this.content = content;
    /** @type {string} */
    this.syntacticRole = syntacticRole;
  }

  /** @return {Array<HTMLElement!>!} */
  toHtmlDebugData() {
    let result = [];
    if (this.node !== null) {
      let next = this.node.toLatex();
      if (next === '') {
        next = `[${this.node.type.type}]`;
      }
      result.push(document.createTextNode(next));
    }
    if (this.content !== '') {
      result.push(boldElement(this.content, 'orange'));
    }
    if (this.syntacticRole !== '') {
      result.push(boldElement(this.syntacticRole, 'red'));
    }
    return result;
  }

  /** @return {boolean} */
  isExpression() {
    return this.syntacticRole === '' && this.node !== null;
  }

  /** @return {boolean} */
  isMatrixEnder() {
    return this.syntacticRole in latexConstants.matrixEnder;
  }
}

class ToLatexOptions {
  constructor(/** @type {boolean}*/ useCursorCommand) {
    /** @type {boolean}*/
    this.useCursorCommand = useCursorCommand;
  }
}

let latexOptionsWithCursor = new ToLatexOptions(true);

class LaTeXConstants {
  constructor() {
    this.latinCharactersString =
        'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    this.latinCharacters = {};
    for (let i = 0; i < this.latinCharactersString.length; i++) {
      this.latinCharacters[this.latinCharactersString[i]] = true;
    }
    this.digitString = '0123456789';
    this.digits = {};
    for (let i = 0; i < this.digitString.length; i++) {
      this.digits[this.digitString[i]] = true;
    }
    this.characterReplacingSelectionString =
        'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 +-*';

    /** @type {Object<string, boolean>!}*/
    this.characterReplacingSelection = {};
    for (let i = 0; i < this.characterReplacingSelectionString.length; i++) {
      this.characterReplacingSelection[this.characterReplacingSelectionString
                                           [i]] = true;
    }
    /** @type {Object.<string, string>!} */
    this.operatorsExtraPadding = {
      '=': '0.3em',
      // leq
      '\u2264': '0.3em',
      // geq
      '\u2265': '0.3em',
      // Latex to (right arrow).
      '\u2192': '0.3em',
      '>': '0.3em',
      '<': '0.3em',
      'mod': '0.5em',
      // neq
      '\u2260': '0.3em',
      // ellipsis dots
      '\u2026': '0.3em',
    };
    /**
     * @type {Object.<string, string>!}
     * Used as bottom padding to the operator to aid with
     * vertical centering.
     */
    this.operatorsExtraVerticalPadding = {
      // Latex to (right arrow).
      '\u2192': '0.3em',
    };
    /**
     * @type {Object.<string, number>!}
     * A factor to multiply the SVG height with.
     * to aid with vertical centering.
     */
    this.operatorsExtraVerticalPaddingForSVG = {
      // Latex to (right arrow).
      '\u2192': 0.85,
    };
    /** @type {Object.<string, string>!} */
    this.operatorsNormalized = {
      // Full-width plus sign, wider and taller plus sign.
      '\uFF0B': '+',
      '+': '+',
      ';': ';',
      ':': ':',
      ',': ',',
      '.': '.',
      // \pm
      '\u00B1': '\u00B1',
      // Mathematical minus, a wider dash.
      '\u2212': '\u2212',
      '-': '\u2212',
      '*': '\u00B7',
      // A vertically centered small solid dot.
      '\u00B7': '\u00B7',
      '=': '=',
      '>': '>',
      '<': '<',
    };
    /** @type {Object.<string, string>!} */
    this.operatorsFromUft8 = {
      '\u2212': '-',
    };
    /** @type {Object.<string, string>!} */
    this.latexSyntacticValues = {
      '{': '{',
      '}': '}',
      '^': '^',
      '_': '_',
      '(': '(',
      ')': ')',
      '\\': '\\',
      '&': '&',
      '[': '[',
      ']': ']',
      '|': '|',
    };
    /** @type {Object.<string, string>!} */
    this.latexBackslashCommands = {
      'left': '\\left',
      'right': '\\right',
      'cancel': '\\cancel',
      'sqrt': '\\sqrt',
      'begin': '\\begin',
      // Special command reserved for indicating the
      // cursor position when the latex is used in an editable box.
      'cursor': '\\cursor',
      // Special command reserved for generation of input box.
      'formInput': '\\formInput',
      'end': '\\end',
      'frac': '\\frac',
      'mathcal': '\\mathcal',
      'mathfrak': '\\mathfrak',
      'mathbb': '\\mathbb',
      'langle': '\\langle',
      'rangle': '\\rangle',
      '{': '\\{',
      'binom': '\\binom',
      'stackrel': '\\stackrel',
      'overbrace': '\\overbrace',
      'overline': '\\overline',
      'color': '\\color',
      'mathbf': '\\mathbf',
      'hline': '\\hline',
    };
    /** @type {Object.<string, string>!} */
    this.latexBackslashOperators = {
      'bullet': '\u2022',
      'otimes': '\u2297',
      'oplus': '\u2295',
      'times': '\u00D7',
      'circ': '\u2218',
      'cdot': '\u00B7',
      'leq': '\u2264',
      'neq': '\u2260',
      'dots': '\u2026',
      'vdots': '\u22EE',
      'approx': '~',
      'equiv': '\u2261',
      'subset': '\u2282',
      'supset': '\u2283',
      'setminus': '\\',
      'lt': '<',
      'gt': '>',
      'sqcup': '\u2294',
      'to': '\u2192',
      'mapsto': '\u21A6',
      'nwarrow': '\u2196',
      'nearrow': '\u2197',
      'searrow': '\u2198',
      'swarrow': '\u2199',
      'perp': '\u27C2',
      'pm': '\u00B1',
      'det': 'det',
      'geq': '\u2265',
      'sin': 'sin',
      'cos': 'cos',
      'tan': 'tan',
      'cot': 'cot',
      'sec': 'sec',
      'csc': 'csc',
      'ln': 'ln',
      'log': 'log',
      'arctan': 'arctan',
      'arccos': 'arccos',
      'arcsin': 'arcsin',
      'in': '\u2208',
      'cap': '\u2229',
      'cup': '\u222A',
      'mod': 'mod',
    };
    /** @type {Object.<string, string>!} */
    this.latexBackslashOperatorsBackslashed = {};
    for (let operator in this.latexBackslashOperators) {
      this.latexBackslashOperatorsBackslashed['\\' + operator] =
          this.latexBackslashOperators[operator];
    }

    /** @type {Object.<string, string>!} */
    this.operatorWithSuperAndSubscript = {
      'sum': '\u2211',
      'int': '\u222B',
    };
    /** @type {Object.<string, string>!} */
    this.operatorWithSuperAndSubscriptBackslashed = {};
    for (let operator in this.operatorWithSuperAndSubscript) {
      this.operatorWithSuperAndSubscriptBackslashed['\\' + operator] =
          this.operatorWithSuperAndSubscript[operator];
    }

    /** @type {Object.<string, string>!} */
    this.operatorsWithSubscript = {
      'lim': 'lim',
    };
    /** @type {Object.<string, string>!} */
    this.operatorsWithSubscriptBackslashed = {};
    for (let operator in this.operatorsWithSubscript) {
      this.operatorsWithSubscriptBackslashed['\\' + operator] =
          this.operatorsWithSubscript[operator];
    }

    /** @type {Object.<string, string>!} */
    this.latexBackslashAtomsEditable = {
      'alpha': '\u03B1',
      'beta': '\u03B2',
      'gamma': '\u03B3',
      'delta': '\u03B4',
      'epsilon': '\u03B5',
      'varepsilon': '\u03B5',
      'zeta': '\u03B6',
      'eta': '\u03B7',
      'theta': '\u03B8',
      'vartheta': '',
      'iota': '\u03B9',
      'emptyset': '\u2205',
      'kappa': '\u03BA',
      'lambda': '\u03BB',
      'mu': '\u03BC',
      'nu': '\u03BD',
      'xi': '\u03BE',
      'omicron': '\u03BF',
      'pi': '\u03C0',
      'rho': '\u03C1',
      'sigma': '\u03C3',
      'tau': '\u03C4',
      'upsilon': '\u03C5',
      'phi': '\u03C6',
      'varphi': '\u03D5',
      'chi': '\u03C7',
      'psi': '\u03C8',
      'omega': '\u03C9',
      'Alpha': '\u0391',
      'Beta': '\u0392',
      'Gamma': '\u0393',
      'Delta': '\u0394',
      'Epsilon': '\u0395',
      'Zeta': '\u0396',
      'Eta': '\u0397',
      'Theta': '\u0398',
      'Iota': '\u0399',
      'Kappa': '\u039A',
      'Lambda': '\u039B',
      'Mu': '\u039C',
      'Nu': '\u039D',
      'Xi': '\u039E',
      'Omicron': '\u039F',
      'Pi': '\u03A0',
      'Rho': '\u03A1',
      'Sigma': '\u03A3',
      'Tau': '\u03A4',
      'Upsilon': '\u03A5',
      'Phi': '\u03A6',
      'Chi': '\u03A7',
      'Psi': '\u03A8',
      'Omega': '\u03A9',
      'infty': '\u221E',
      'S': '\u00A7',
      'partial': '\u2202',
    };
    /** @type {Object.<string, string>!} */
    this.latexBackslashAtomsEditableBackslashed = {};
    for (let operator in this.latexBackslashAtomsEditable) {
      this.latexBackslashAtomsEditableBackslashed['\\' + operator] =
          this.latexBackslashAtomsEditable[operator];
    }

    /** @type {Object.<string, string>!} */
    this.mathcalEquivalents = {
      'A': '\uD835\uDC9C',
      'C': '\uD835\uDC9E',
      'D': '\uD835\uDC9F',
      'G': '\uD835\uDCA2',
      'J': '\uD835\uDCA5',
      'K': '\uD835\uDCA6',
      'L': '\u2112',
      'N': '\uD835\uDCA9',
      'O': '\uD835\uDCAA',
      'P': '\uD835\uDCAB',
      'Q': '\uD835\uDCAC',
      'S': '\uD835\uDCAE',
      'T': '\uD835\uDCAF',
      'U': '\uD835\uDCB0',
      'V': '\uD835\uDCB1',
      'W': '\uD835\uDCB2',
      'X': '\uD835\uDCB3',
      'Y': '\uD835\uDCB4',
      'Z': '\uD835\uDCB5',
    };
    /** @type {Object.<string, string>!} */
    this.mathfrakEquivalents = {
      'a': '\uD835\uDD1E',
      'b': '\uD835\uDD1F',
      'c': '\uD835\uDD20',
      'd': '\uD835\uDD21',
      'e': '\uD835\uDD22',
      'f': '\uD835\uDD23',
      'g': '\uD835\uDD24',
      'h': '\uD835\uDD25',
      'i': '\uD835\uDD26',
      'j': '\uD835\uDD27',
      'k': '\uD835\uDD28',
      'l': '\uD835\uDD29',
      'm': '\uD835\uDD2A',
      'n': '\uD835\uDD2B',
      'o': '\uD835\uDD2C',
      'p': '\uD835\uDD2D',
      'q': '\uD835\uDD2E',
      'r': '\uD835\uDD2F',
      's': '\uD835\uDD30',
      't': '\uD835\uDD31',
      'u': '\uD835\uDD32',
      'v': '\uD835\uDD33',
      'w': '\uD835\uDD34',
      'x': '\uD835\uDD35',
      'y': '\uD835\uDD36',
      'z': '\uD835\uDD37',
    };
    /** @type {Object.<string, string>!} */
    this.mathbbEquivalents = {
      'A': '\uD835\uDD38',
      'C': '\u2102',
      'H': '\u210D',
      'N': '\u2115',
      'O': '\uD835\uDD46',
      'P': '\u2119',
      'Q': '\u211A',
      'R': '\u211D',
      'Z': '\u2124',
    };
    /** @type {Object.<string, string>!} */
    this.leftDelimiters = {
      '\\left.': '',
      '\\langle': '\u27E8',
      '(': '(',
      '[': '[',
      '|': '|',
      '\\{': '{',
    };
    /** @type {Object.<string, string>!} */
    this.delimitersAmbiguous = {
      '|': '|',
    };
    /** @type {Object.<string, string>!} */
    this.rightDelimiters = {
      '\\right.': '',
      '\\rangle': '\u27E9',
      ')': ')',
      '|': '|',
      ']': ']',
      '\\}': '}',
    };
    /** @type {Object.<string, string>!} */
    this.leftRightDelimiterPair = {
      '\\langle': '\\rangle',
      '\u27E8': '\u27E9',  // langle, rangle
      '[': ']',
      '(': ')',
      '{': '}',
      '|': '|',
    };
    /** @type {Object.<string, string>!} */
    this.rightLeftDelimiterPair = {
      '\\rangle': '\\langle',
      '\u27E9': '\u27E8',  // rangle, langle
      ']': '[',
      ')': '(',
      '}': '{',
    };
    this.latexCommandsIgnored = {
      'displaystyle': true,
      'text': true,
      'mathrm': true,
      'phantom': true,
      'limits': true,
    };
    this.beginEndEnvironments = {
      'matrix': 'matrix',
      'pmatrix': 'pmatrix',
      'bmatrix': 'bmatrix',
      'array': 'array',
      'cases': 'cases',
      'align': 'align',
    };
    /** @type {Object.<string, boolean>!} */
    this.matrixEnder = {
      '\\end{pmatrix}': true,
      '\\end{matrix}': true,
      '\\end{bmatrix}': true,
      '\\end{array}': true,
      '\\end{align}': true,
      '\\end{cases}': true,
    };
    /** @type {Object.<string, boolean>!} */
    // The boolean indicates that the white space should be ignored.
    this.whiteSpaceCharactersIgnored = {
      '~': false,
      ' ': true,
      '\t': true,
      '\u00A0': true,
      '\n': true,
    };
    this.whiteSpaceUtf16 = {
      '\u00A0': ' ',
      '\u200A': ' ',
      '\u200B': ' ',
      '\u2009': ' ',
    };
    /** @type {Object.<string, string>?} */
    this.utf16ToLatexMap = null;

    /** @type {Object.<string, boolean>!} */
    this.recognizedCommandsKeyInput = {
      '\\langle': true,
      '\\rangle': true,
      '\\matrix': true,
      '\\pmatrix': true,
      '\\bmatrix': true,
      '\\cancel': true,
      '\\sqrt': true,
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

  /** Computes the value of this.utf16ToLatexMap. */
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
      this.utf16ToLatexMap[key] = ' ';
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

  /** @return {LatexWithAnnotation!} */
  convertUtf16ToLatex(
      /** @type {string} */
      input,
      /** @type {number} */
      positionCursor,
  ) {
    this.computeUtf16ToLatexMap();
    let result = [];
    if (positionCursor === 0) {
      result.push('\\cursor ');
    }
    for (let i = 0; i < input.length; i++) {
      let current = '';
      for (let j = 0; j < 4 && i + j < input.length; j++) {
        current += input[i + j];
        if (current in this.utf16ToLatexMap) {
          current = this.utf16ToLatexMap[current];
          i += j;
          result.push(current);
          current = '';
          break;
        }
      }
      if (current !== '') {
        result.push(current);
      }
      if (i + 1 === positionCursor) {
        result.push('\\cursor ');
      }
    }
    return new LatexWithAnnotation(result.join(''));
  }

  /** @return {boolean} */
  isCharacterReplacingSelection(
      /** @type {string} */
      input,
  ) {
    return input in this.characterReplacingSelection;
  }

  /** @return {boolean} */
  isLatinCharacter(
      /** @type {string}*/
      input,
  ) {
    return input in this.latinCharacters;
  }

  /** @return {boolean} */
  isDigit(
      /** @type {string}*/
      input,
  ) {
    return input in this.digits;
  }

  /** @return {boolean} */
  isWhiteSpace(
      /** @type {string} */ input,
  ) {
    return (input in this.whiteSpaceCharactersIgnored);
  }

  /** @return {boolean} */
  isWhiteSpaceIgnored(
      /** @type {string} */ input,
  ) {
    if (!(input in this.whiteSpaceCharactersIgnored)) {
      return false;
    }
    return this.whiteSpaceCharactersIgnored[input];
  }

  /** @return {string} */
  normalizeOperatorToUtf8(
      /** @type {string} */
      input,
  ) {
    if (input in this.operatorsNormalized) {
      return this.operatorsNormalized[input];
    }
    return input;
  }
}

/** @type {LaTeXConstants!}*/
const latexConstants = new LaTeXConstants();

class LaTeXParser {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      latex,
  ) {
    /** @type {EquationEditor!} */
    this.equationEditor = equationEditor;
    /** @type {string} */
    this.latex = latex;
    /** @type {MathNode!} */
    this.result = new MathNode(equationEditor, knownTypes.horizontalMath);
    /** @type {Array.<string>!} */
    this.words = [];
    /** @type {Array.<SyntacticElement!>!} */
    this.parsingStack = [];
    /** @type {number} */
    // Dummy elements are prepended to the parsing stack
    // so parsing rules can safely assume a minimal number
    // of syntanctic elements.
    this.dummyParsingElements = 6;
    /** @type {boolean} */
    this.debug = false;
    /** @type {Array.<Array.<HTMLElement!>!>!} */
    this.reductionLog = [];
    /** @type {string} */
    this.lastRuleName = '';
    /** @type {number} */
    this.startTime = 0;
    /** @type {boolean} */
    // Whether the special command \cursor is found.
    // Used to indicate the position of the
    // cursor when the latex is used in an editable box.
    this.cursorFound = false;
  }

  /** Initializes the parser. */
  initialize() {
    this.parsingStack = [];
    for (let i = 0; i < this.dummyParsingElements; i++) {
      this.parsingStack.push(new SyntacticElement(null, '', ''));
    }
  }

  /** Parses all words. */
  parseWords() {
    let next = [];
    for (let i = 0; i < this.latex.length; i++) {
      let nextCharacter = this.latex[i];
      if (latexConstants.isLatinCharacter(nextCharacter)) {
        next.push(nextCharacter);
        continue;
      }
      if (next.length > 0) {
        this.words.push(next.join(''));
      }
      this.words.push(nextCharacter);
      next = [];
    }
    if (next.length > 0) {
      this.words.push(next.join(''));
    }
  }

  /** @return {MathNode!} */
  parse() {
    this.startTime = new Date().getTime();
    this.initialize();
    this.parseWords();
    this.result = new MathNode(this.equationEditor, knownTypes.horizontalMath);
    let startingNode =
        mathNodeFactory.horizontalMath(this.equationEditor, null);
    this.parsingStack.push(new SyntacticElement(startingNode, '', ''));
    for (let i = 0; i < this.words.length; i++) {
      if (!this.reduceStack(new SyntacticElement(null, this.words[i], ''))) {
        return this.constructError();
      }
    }
    if (!this.reduceStack(new SyntacticElement(null, '', 'parsingEnd'))) {
      return this.constructError();
    }
    if (this.parsingStack.length !== this.dummyParsingElements + 2) {
      if (this.equationEditor.options.debugLogContainer !== null) {
        console.log(`Failed to parse ${
            this.latex}: not all syntactic elements were reduced.`);
      }
      return this.constructError();
    }
    let secondToLastElement = this.parsingStack[this.parsingStack.length - 2];
    if (secondToLastElement.node === null) {
      if (this.equationEditor.options.debugLogContainer !== null) {
        console.log(`Failed to parse ${
            this.latex}: final syntactic element is not a node.`);
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

  /** @return {MathNode!} */
  constructError() {
    let error = mathNodeFactory.error(this.equationEditor, this.latex);
    let left = mathNodeFactory.atom(this.equationEditor, '');
    let right = mathNodeFactory.atom(this.equationEditor, '');
    return mathNodeFactory.horizontalMathFromArray(
        this.equationEditor, [left, error, right]);
  }

  /** @return {boolean} */
  reduceStack(
      /** @type {SyntacticElement!} */
      syntacticElement,
  ) {
    let numberOfRuleApplications = 0;
    let startingLength = this.parsingStack.length;
    this.parsingStack.push(syntacticElement);
    if (this.debug) {
      this.reductionLog.push(this.toHtmlStackDebug('append'));
      this.lastRuleName = '';
    }
    while (this.applyOneRule()) {
      if (this.debug) {
        this.reductionLog.push(this.toHtmlStackDebug(this.lastRuleName));
        this.lastRuleName = '';
      }
      numberOfRuleApplications++;
      let stackReduction = startingLength - this.parsingStack.length;
      let maximumRuleApplications = Math.max(10, stackReduction * 10);
      if (numberOfRuleApplications > maximumRuleApplications) {
        // Too many rules to reduce an expression: no more than 10 rule
        // applications per unit of stack reduction. Perhaps we have a
        // substitution loop in our parsing rules?
        console.log('Too many parsing rule applications.');
        return false;
      }
    }
    return true;
  }

  /** @return {boolean} */
  decreaseParsingStack(
      /** @type {number} */
      elementsToRemove,
  ) {
    this.parsingStack.length = this.parsingStack.length - elementsToRemove;
    return true;
  }

  /** @return {boolean} */
  replaceParsingStackTop(
      /** @type {MathNode?} */
      node,
      /** @type {string} */
      syntancticValue,
      /** @type {number} */
      indexToRemove,
  ) {
    return this.replaceParsingStackRange(
        node, syntancticValue, indexToRemove, -1);
  }

  /** @return {boolean} */
  replaceParsingStackRange(
      /** @type {MathNode?} */
      node,
      /** @type {string} */
      syntacticRole,
      /** @type {number} */
      firstIndexToRemove,
      /** @type {number} */
      lastIndexToRemove,
  ) {
    if (firstIndexToRemove < 0) {
      firstIndexToRemove += this.parsingStack.length;
    }
    if (lastIndexToRemove < 0) {
      lastIndexToRemove += this.parsingStack.length;
    }
    let incoming = new SyntacticElement(node, '', syntacticRole);
    this.parsingStack[firstIndexToRemove] = incoming;
    let nodesToRemove = lastIndexToRemove - firstIndexToRemove;
    for (let i = lastIndexToRemove + 1; i < this.parsingStack.length; i++) {
      this.parsingStack[i - nodesToRemove] = this.parsingStack[i];
    }
    this.parsingStack.length -= nodesToRemove;
    return true;
  }

  /** @return {Array.<HTMLElement!>!} */
  toHtmlStackDebug(
      /** @type {string} */
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

  /** @return {Array.<HTMLElement!>!} */
  toHtmlDebug() {
    let result = [];
    for (let i = 0; i < this.reductionLog.length; i++) {
      for (let j = 0; j < this.reductionLog[i].length; j++) {
        result.push(this.reductionLog[i][j]);
      }
      result.push(document.createElement('hr'));
    }
    return result;
  }

  /** @return {boolean} */
  specialFont(
      /** @type {Object.<string, string>!} */
      specialFontEquivalents,
  ) {
    let last = this.parsingStack[this.parsingStack.length - 1];

    if (latexConstants.isLatinCharacter(last.content)) {
      if (!(last.content in specialFontEquivalents)) {
        return false;
      }
      this.lastRuleName = 'special font';
      let node = mathNodeFactory.atom(
          this.equationEditor, specialFontEquivalents[last.content]);
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (!last.isExpression() ||
        last.node.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    if (last.node.children.length !== 1) {
      return false;
    }
    let contentIfAtom = last.node.children[0].contentIfAtom();
    if (!(contentIfAtom in specialFontEquivalents)) {
      return false;
    }
    this.lastRuleName = 'special font horizontal math';
    let node = mathNodeFactory.atom(
        this.equationEditor, specialFontEquivalents[contentIfAtom]);
    return this.replaceParsingStackTop(node, '', -2);
  }

  /** @return {boolean} */
  // Applies the first possible rule to the top of the parsing stack.
  applyOneRule() {
    let last = this.parsingStack[this.parsingStack.length - 1];
    if (latexConstants.isWhiteSpace(last.content)) {
      if (latexConstants.isWhiteSpaceIgnored(last.content)) {
        this.lastRuleName = 'clean whitespace';
        this.parsingStack.length = this.parsingStack.length - 1;
        return true;
      } else {
        this.lastRuleName = 'create whitespace';
        let node = mathNodeFactory.atom(this.equationEditor, ' ');
        return this.replaceParsingStackTop(node, '', -1);
      }
    }
    if (last.content in latexConstants.latexSyntacticValues) {
      this.lastRuleName = 'built-in syntactic element';
      return this.replaceParsingStackTop(
          null, latexConstants.latexSyntacticValues[last.content], -1);
    }

    let secondToLast = this.parsingStack[this.parsingStack.length - 2];
    if (secondToLast.syntacticRole === '\\') {
      if (last.syntacticRole === '\\') {
        this.lastRuleName = 'double backslash';
        return this.replaceParsingStackTop(null, '\\\\', -2);
      }
      if (last.syntacticRole === '{') {
        this.lastRuleName = '{ delimiter';
        return this.replaceParsingStackTop(null, '\\{', -2);
      }
      if (last.syntacticRole === '}') {
        this.lastRuleName = '} delimiter';
        return this.replaceParsingStackTop(null, '\\}', -2);
      }
    }
    let thirdToLast = this.parsingStack[this.parsingStack.length - 3];
    let fourthToLast = this.parsingStack[this.parsingStack.length - 4];
    if (secondToLast.syntacticRole === '{' && last.isExpression()) {
      if (last.node.type.type !== knownTypes.horizontalMath.type) {
        this.lastRuleName = 'horizontal math after curly brace';
        let node =
            mathNodeFactory.horizontalMath(this.equationEditor, last.node);
        return this.replaceParsingStackTop(node, '', -1);
      }
    }
    if (thirdToLast.syntacticRole === '{' && secondToLast.isExpression() &&
        last.syntacticRole === '}') {
      this.lastRuleName = 'remove curly braces';
      let node = secondToLast.node;
      if (node.type.type !== knownTypes.horizontalMath.type) {
        node = mathNodeFactory.horizontalMath(this.equationEditor, node);
      }
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (secondToLast.syntacticRole === '\\' &&
        last.content in latexConstants.latexBackslashCommands) {
      this.lastRuleName = 'latex command';
      return this.replaceParsingStackTop(
          null, latexConstants.latexBackslashCommands[last.content], -2);
    }
    if (secondToLast.syntacticRole === '\\left' &&
        last.syntacticRole in latexConstants.leftDelimiters) {
      this.lastRuleName = '\\left combined with left delimiter';
      this.parsingStack[this.parsingStack.length - 2] = last;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === '\\color' && last.isExpression()) {
      if (secondToLast.node === null) {
        this.lastRuleName = 'set color';
        // We have a situation along the lines of \color{red} with last.node =
        // red. Write the last node as the node that determines the color.
        secondToLast.node = last.node;
        return this.decreaseParsingStack(1);
      } else {
        this.lastRuleName = 'apply color';
        // We already have the color set along the lines of \\color{red}{last}
        // with secondToLast = \color{red} and secondToLast.node = red.
        let node =
            mathNodeFactory.genericMathBox(this.equationEditor, last.node);
        node.type.colorText = secondToLast.node.toLatex();
        return this.replaceParsingStackTop(node, '', -2);
      }
    }
    if (secondToLast.syntacticRole === '\\mathbf' && last.isExpression()) {
      this.lastRuleName = 'apply bold';
      // We already have the color set along the lines of \\color{red}{last}
      // with secondToLast = \color{red} and secondToLast.node = red.
      let node = mathNodeFactory.genericMathBox(this.equationEditor, last.node);
      node.type.fontWeight = 'bold';
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (secondToLast.syntacticRole === '\\right' &&
        last.syntacticRole in latexConstants.rightDelimiters) {
      this.lastRuleName = '\\right combined with right delimiter';
      this.parsingStack[this.parsingStack.length - 2] = last;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === '\\left' && last.content === '.') {
      return this.replaceParsingStackTop(null, '\\left.', -2);
    }
    if (secondToLast.syntacticRole === '\\right' && last.content === '.') {
      return this.replaceParsingStackTop(null, '\\right.', -2);
    }
    if (secondToLast.syntacticRole === '\\mathcal') {
      if (this.specialFont(latexConstants.mathcalEquivalents)) {
        return true;
      }
    }
    if (secondToLast.syntacticRole === '\\mathfrak') {
      if (this.specialFont(latexConstants.mathfrakEquivalents)) {
        return true;
      }
    }
    if (secondToLast.syntacticRole === '\\mathbb') {
      if (this.specialFont(latexConstants.mathbbEquivalents)) {
        return true;
      }
    }
    if (last.content in latexConstants.latexCommandsIgnored &&
        secondToLast.syntacticRole === '\\') {
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === '\\' &&
        last.content in latexConstants.operatorWithSuperAndSubscript) {
      this.lastRuleName = 'operator with superscript and subscript';
      let operatorSymbol =
          latexConstants.operatorWithSuperAndSubscript[last.content];
      let syntacticElement = new SyntacticElement(
          null, operatorSymbol, 'operatorWithSuperAndSubscript');
      this.parsingStack[this.parsingStack.length - 2] = syntacticElement;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === '\\' &&
        last.content in latexConstants.operatorsWithSubscript) {
      this.lastRuleName = 'operator with subscript';
      let operatorSymbol = latexConstants.operatorsWithSubscript[last.content];
      let syntacticElement =
          new SyntacticElement(null, operatorSymbol, 'operatorWithSubscript');
      this.parsingStack[this.parsingStack.length - 2] = syntacticElement;
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === '\\overline' && last.isExpression()) {
      this.lastRuleName = 'over-line';
      let node = mathNodeFactory.overLine(this.equationEditor, last.node);
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (fourthToLast.syntacticRole === '\\overbrace' &&
        thirdToLast.isExpression() && secondToLast.syntacticRole === '^' &&
        last.isExpression()) {
      this.lastRuleName = 'over-brace';
      let node = mathNodeFactory.overBrace(
          this.equationEditor,
          /** @type {MathNode!} */ (thirdToLast.node), last.node);
      return this.replaceParsingStackTop(node, '', -4);
    }
    let fifthToLast = this.parsingStack[this.parsingStack.length - 5];
    if (fifthToLast.syntacticRole === 'operatorWithSuperAndSubscript' &&
        fourthToLast.syntacticRole === '^' && thirdToLast.isExpression() &&
        secondToLast.syntacticRole === '_' && last.isExpression()) {
      this.lastRuleName =
          'operatorWithSuperAndSubscript combines with super and subscript.';
      let node = mathNodeFactory.operatorWithSuperAndSubscript(
          this.equationEditor, fifthToLast.content, thirdToLast.node,
          last.node);
      return this.replaceParsingStackTop(node, '', -5);
    }
    if (fifthToLast.syntacticRole === 'operatorWithSuperAndSubscript' &&
        fourthToLast.syntacticRole === '_' && thirdToLast.isExpression() &&
        secondToLast.syntacticRole === '^' && last.isExpression()) {
      this.lastRuleName =
          'operatorWithSuperAndSubscript combines with sub and superscript.';
      let node = mathNodeFactory.operatorWithSuperAndSubscript(
          this.equationEditor, fifthToLast.content, last.node,
          thirdToLast.node);
      return this.replaceParsingStackTop(node, '', -5);
    }
    if (secondToLast.syntacticRole === 'operatorWithSuperAndSubscript' &&
        last.syntacticRole !== '^' && last.syntacticRole !== '_' &&
        last.syntacticRole !== '\\') {
      this.lastRuleName = 'operator standalone';
      let node = mathNodeFactory.operatorWithSuperAndSubscript(
          this.equationEditor, secondToLast.content, null, null);
      return this.replaceParsingStackRange(node, '', -2, -2);
    }
    if (fourthToLast.syntacticRole === 'operatorWithSuperAndSubscript' &&
        thirdToLast.syntacticRole === '^' && secondToLast.isExpression() &&
        last.syntacticRole !== '_') {
      this.lastRuleName = 'operator with superscript only';
      let node = mathNodeFactory.operatorWithSuperAndSubscript(
          this.equationEditor, fourthToLast.content, secondToLast.node, null);
      return this.replaceParsingStackRange(node, '', -4, -2);
    }
    if (fourthToLast.syntacticRole === 'operatorWithSuperAndSubscript' &&
        thirdToLast.syntacticRole === '_' && secondToLast.isExpression() &&
        last.syntacticRole !== '^') {
      this.lastRuleName = 'operator with subscript only (allows superscript)';
      let node = mathNodeFactory.operatorWithSuperAndSubscript(
          this.equationEditor, fourthToLast.content, null, secondToLast.node);
      return this.replaceParsingStackRange(node, '', -4, -2);
    }
    if (thirdToLast.syntacticRole === 'operatorWithSubscript' &&
        secondToLast.syntacticRole === '_' && last.isExpression()) {
      this.lastRuleName = 'operator with subscript only';
      let node = mathNodeFactory.operatorWithSubscript(
          this.equationEditor, thirdToLast.content, last.node);
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (secondToLast.syntacticRole === '\\' &&
        last.content in latexConstants.latexBackslashOperators) {
      this.lastRuleName = 'atom immutable from backslash';
      let node = mathNodeFactory.atomImmutable(
          this.equationEditor,
          latexConstants.latexBackslashOperators[last.content],
      );
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (secondToLast.syntacticRole === '\\' &&
        last.content in latexConstants.latexBackslashAtomsEditable) {
      this.lastRuleName = 'latex symbol';
      let node = mathNodeFactory.atom(
          this.equationEditor,
          latexConstants.latexBackslashAtomsEditable[last.content],
      );
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (last.syntacticRole === '\\cursor') {
      this.lastRuleName = 'cursor location';
      let node = mathNodeFactory.atom(this.equationEditor, '');
      node.desiredCursorPosition = 0;
      return this.replaceParsingStackTop(node, '', -1);
    }
    if ((thirdToLast.syntacticRole === '\\begin' ||
         thirdToLast.syntacticRole === '\\end') &&
        secondToLast.syntacticRole === '{' &&
        last.content in latexConstants.beginEndEnvironments) {
      this.lastRuleName = 'begin or end environment';
      return this.replaceParsingStackTop(
          null, latexConstants.beginEndEnvironments[last.content], -1);
    }
    if (fourthToLast.syntacticRole === '\\begin' &&
        thirdToLast.syntacticRole === '{' &&
        secondToLast.syntacticRole in latexConstants.beginEndEnvironments &&
        last.syntacticRole === '}') {
      let environment =
          latexConstants.beginEndEnvironments[secondToLast.syntacticRole];
      return this.replaceParsingStackTop(null, `\\begin{${environment}}`, -4);
    }
    if (fourthToLast.syntacticRole === '\\end' &&
        thirdToLast.syntacticRole === '{' &&
        secondToLast.syntacticRole in latexConstants.beginEndEnvironments &&
        last.syntacticRole === '}') {
      let environment =
          latexConstants.beginEndEnvironments[secondToLast.syntacticRole];
      return this.replaceParsingStackTop(null, `\\end{${environment}}`, -4);
    }
    if (last.syntacticRole === '\\begin{pmatrix}') {
      this.lastRuleName = 'begin pmatrix to matrix builder';
      let matrix =
          mathNodeFactory.matrix(this.equationEditor, 1, 0, '', 'pmatrix');
      return this.replaceParsingStackTop(matrix, 'matrixBuilder', -1);
    }
    if (last.syntacticRole === '\\begin{matrix}' ||
        last.syntacticRole === '\\begin{align}') {
      this.lastRuleName = 'begin matrix to matrix builder';
      let matrix =
          mathNodeFactory.matrix(this.equationEditor, 1, 0, '', 'align');
      return this.replaceParsingStackTop(matrix, 'matrixBuilder', -1);
    }
    if (last.syntacticRole === '\\begin{bmatrix}') {
      this.lastRuleName = 'begin bmatrix to matrix builder';
      let matrix =
          mathNodeFactory.matrix(this.equationEditor, 1, 0, '', 'bmatrix');
      return this.replaceParsingStackTop(matrix, 'matrixBuilder', -1);
    }
    if (last.syntacticRole === '\\begin{cases}') {
      this.lastRuleName = 'begin cases to matrix builder';
      let matrix =
          mathNodeFactory.matrix(this.equationEditor, 1, 0, '', 'cases');
      return this.replaceParsingStackTop(matrix, 'matrixBuilder', -1);
    }
    if (secondToLast.syntacticRole === 'matrixBuilder' &&
        last.syntacticRole === '\\hline') {
      this.lastRuleName = 'matrixBuilder hline';
      /** @type{MathNodeMatrix!} */
      let builder = /** @type{MathNodeMatrix!} */ (secondToLast.node);
      builder.appendHorizontalAboveLastRow();
      return this.decreaseParsingStack(1);
    }
    if (fourthToLast.syntacticRole === '\\begin{array}' &&
        thirdToLast.syntacticRole === '{' && secondToLast.isExpression() &&
        last.syntacticRole === '|') {
      if (secondToLast.node.type.type === knownTypes.horizontalMath.type) {
        secondToLast.node.appendChild(
            mathNodeFactory.atom(this.equationEditor, last.syntacticRole));
        return this.decreaseParsingStack(1);
      }
    }
    if (secondToLast.syntacticRole === '\\begin{array}' &&
        last.isExpression()) {
      this.lastRuleName = 'begin array to matrix builder';
      let matrix = mathNodeFactory.matrix(
          this.equationEditor, 1, 0, last.node.toLatex(), 'array');
      return this.replaceParsingStackTop(matrix, 'matrixBuilder', -2);
    }
    if (thirdToLast.syntacticRole === 'matrixBuilder' &&
        secondToLast.isExpression() && last.syntacticRole === '&') {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      let incomingEntry = mathNodeFactory.matrixRowEntry(
          this.equationEditor, secondToLast.node);
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (thirdToLast.node);
      matrix.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === 'matrixBuilder' &&
        last.syntacticRole === '&') {
      // Modify secondToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      this.lastRuleName = 'matrix builder ampersand';
      let incomingEntry = mathNodeFactory.matrixRowEntry(
          this.equationEditor, mathNodeFactory.atom(this.equationEditor, ''));
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (secondToLast.node);
      matrix.getLastMatrixRow().appendChild(incomingEntry);
      return this.decreaseParsingStack(1);
    }
    if (last.syntacticRole === '\\\\' && secondToLast.isExpression() &&
        thirdToLast.syntacticRole === 'matrixBuilder') {
      // Modify thirdToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      this.lastRuleName = 'matrix builder expression double backslash';
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (thirdToLast.node);
      let lastRow = matrix.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(
          this.equationEditor, secondToLast.node);
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
      return this.decreaseParsingStack(2);
    }
    if (last.syntacticRole === '\\\\' &&
        secondToLast.syntacticRole === 'matrixBuilder') {
      // Modify secondToLast.node in place for performance reasons:
      // copying it may cause unwanted quadratic complexity.
      this.lastRuleName = 'matrix builder double backslash';
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (secondToLast.node);
      let lastRow = matrix.getLastMatrixRow();
      let incomingEntry = mathNodeFactory.matrixRowEntry(
          this.equationEditor, mathNodeFactory.atom(this.equationEditor, ''));
      lastRow.appendChild(incomingEntry);
      let newRow = mathNodeFactory.matrixRow(this.equationEditor, 0);
      lastRow.parent.appendChild(newRow);
      return this.decreaseParsingStack(1);
    }
    if (last.syntacticRole === '\\\\') {
      // Double backslah indicates a line break, which we choose to ignore.
      this.lastRuleName = 'remove double backslash';
      return this.decreaseParsingStack(1);
    }
    if (thirdToLast.syntacticRole === 'matrixBuilder' &&
        secondToLast.isExpression() && last.isMatrixEnder()) {
      let incomingEntry = mathNodeFactory.matrixRowEntry(
          this.equationEditor, secondToLast.node);
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (thirdToLast.node);
      matrix.getLastMatrixRow().appendChild(incomingEntry);
      // Normalize the matrix: ensure all rows have same number of columns, no
      // last empty row, etc.
      matrix.normalizeMatrix();
      (new LatexColumnStyleIterator(matrix.latexExtraStyle))
          .applyStyleToMatrix(matrix);
      // Mark the matrix as a regular expression.
      thirdToLast.syntacticRole = '';
      this.lastRuleName = 'finalize matrix builder';
      return this.decreaseParsingStack(2);
    }
    if (secondToLast.syntacticRole === 'matrixBuilder' &&
        last.isMatrixEnder()) {
      this.lastRuleName = 'finish matrix';
      /** @type {MathNodeMatrix!} */
      let matrix = /** @type {MathNodeMatrix!} */ (secondToLast.node);
      // Normalize the matrix: ensure all rows have same number of columns, no
      // last empty row, etc.
      matrix.normalizeMatrix();
      (new LatexColumnStyleIterator(secondToLast.node.latexExtraStyle))
          .applyStyleToMatrix(secondToLast.node);
      // Mark the matrix as a regular expression.
      secondToLast.syntacticRole = '';
      return this.decreaseParsingStack(1);
    }
    if (secondToLast.syntacticRole === '\\sqrt' && last.isExpression()) {
      this.lastRuleName = 'Square root';
      let node = mathNodeFactory.sqrt(
          this.equationEditor, last.node, secondToLast.node);
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (secondToLast.syntacticRole === '\\cancel' && last.isExpression()) {
      this.lastRuleName = 'cancel expression';
      let node = mathNodeFactory.cancel(this.equationEditor, last.node);
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (secondToLast.syntacticRole === '\\formInput' && last.isExpression() &&
        !this.equationEditor.options.editable) {
      this.lastRuleName = 'form input';
      let node =
          mathNodeFactory.formInput(this.equationEditor, last.node.toLatex());
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (secondToLast.syntacticRole === '{' && last.syntacticRole === '}') {
      this.lastRuleName = '{} to empty atom';
      let node = mathNodeFactory.atom(this.equationEditor, '');
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (fourthToLast.syntacticRole === '\\sqrt' && fourthToLast.node === null &&
        thirdToLast.syntacticRole === '[' && secondToLast.isExpression() &&
        last.syntacticRole === ']') {
      this.lastRuleName = 'nth radical';
      fourthToLast.node = secondToLast.node;
      return this.decreaseParsingStack(3);
    }
    if (secondToLast.isExpression() &&
        this.areMatchingDelimiters(thirdToLast, last)) {
      this.lastRuleName = 'parenthetic expression to expression';
      let leftDelimiter =
          latexConstants.leftDelimiters[thirdToLast.syntacticRole];
      let rightDelimiter = latexConstants.rightDelimiters[last.syntacticRole];
      let left = mathNodeFactory.leftDelimiter(
          this.equationEditor, leftDelimiter, false);
      let right = mathNodeFactory.rightDelimiter(
          this.equationEditor, rightDelimiter, false);
      let horizontal = mathNodeFactory.horizontalMathFromArray(
          this.equationEditor, [left, secondToLast.node, right]);
      return this.replaceParsingStackTop(horizontal, '', -3);
    }
    if (secondToLast.syntacticRole in latexConstants.leftDelimiters &&
        last.syntacticRole in latexConstants.rightDelimiters) {
      this.lastRuleName = 'parenthetic expression to expression';
      let leftDelimiter =
          latexConstants.leftDelimiters[secondToLast.syntacticRole];
      let rightDelimiter = latexConstants.rightDelimiters[last.syntacticRole];
      let left = mathNodeFactory.leftDelimiter(
          this.equationEditor, leftDelimiter, false);
      let right = mathNodeFactory.rightDelimiter(
          this.equationEditor, rightDelimiter, false);
      let atom = mathNodeFactory.atom(this.equationEditor, '\u200B');
      let horizontal = mathNodeFactory.horizontalMathFromArray(
          this.equationEditor, [left, atom, right]);
      return this.replaceParsingStackTop(horizontal, '', -2);
    }
    if (last.isExpression() && secondToLast.syntacticRole === '_' &&
        thirdToLast.isExpression()) {
      this.lastRuleName = 'make subscript';
      let node = mathNodeFactory.baseWithSubscript(
          this.equationEditor, /** @type {MathNode!} */ (thirdToLast.node),
          last.node);
      return this.replaceParsingStackTop(node, '', -3);
    }

    if (thirdToLast.syntacticRole === '\\frac' && secondToLast.isExpression() &&
        last.isExpression()) {
      let node = mathNodeFactory.fraction(
          this.equationEditor, secondToLast.node, last.node);
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (last.isExpression() && secondToLast.isExpression() &&
        latexConstants.isDigit(last.node.contentIfAtom())) {
      if (secondToLast.node.type.type === knownTypes.horizontalMath.type &&
          secondToLast.node.children.length > 0) {
        if (latexConstants.isDigit(
                secondToLast.node
                    .children[secondToLast.node.children.length - 1]
                    .contentIfAtom())) {
          this.lastRuleName = 'merge digits into horizontal math';
          secondToLast.node.appendChild(last.node);
          return this.decreaseParsingStack(1);
        }
      }
    }
    if (last.isExpression() && secondToLast.isExpression() &&
        latexConstants.isDigit(last.node.contentIfAtom()) &&
        latexConstants.isDigit(secondToLast.node.contentIfAtom())) {
      this.lastRuleName = 'merge two digits';
      let node = mathNodeFactory.horizontalMath(
          this.equationEditor, secondToLast.node);
      node.appendChild(last.node);
      return this.replaceParsingStackTop(node, '', -2);
    }
    if (thirdToLast.isExpression() && secondToLast.syntacticRole === '^' &&
        last.isExpression()) {
      this.lastRuleName = 'make exponent';
      let node = mathNodeFactory.baseWithExponent(
          this.equationEditor, /** @type {MathNode!} */ (thirdToLast.node),
          last.node);
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (thirdToLast.syntacticRole === '\\binom' &&
        secondToLast.isExpression() && last.isExpression()) {
      let node = mathNodeFactory.matrix(this.equationEditor, 2, 1, '', 'binom');
      node.getMatrixCell(0, 0).children[0].appendChild(secondToLast.node);
      node.getMatrixCell(1, 0).children[0].appendChild(last.node);
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (thirdToLast.syntacticRole === '\\stackrel' &&
        secondToLast.isExpression() && last.isExpression()) {
      let node =
          mathNodeFactory.matrix(this.equationEditor, 3, 1, '', 'stackrel');
      node.getMatrixCell(0, 0).children[0].appendChild(secondToLast.node);
      node.getMatrixCell(1, 0).children[0].appendChild(last.node);
      node.getMatrixCell(2, 0).children[0].appendChild(
          mathNodeFactory.atom(this.equationEditor, '\u00A0'));
      node.children[0].replaceChildAtPosition(
          0, mathNodeFactory.leftDelimiter(this.equationEditor, '', false));
      node.children[0].replaceChildAtPosition(
          2, mathNodeFactory.rightDelimiter(this.equationEditor, '', false));
      return this.replaceParsingStackTop(node, '', -3);
    }
    if (last.content in latexConstants.operatorsNormalized) {
      this.lastRuleName = 'atom immutable';
      let node = mathNodeFactory.atomImmutable(
          this.equationEditor,
          latexConstants.operatorsNormalized[last.content],
      );
      return this.replaceParsingStackTop(node, '', -1);
    }
    if (last.content !== '' && last.syntacticRole === '' &&
        last.node === null) {
      this.lastRuleName = 'construct atom';
      let node = mathNodeFactory.atom(this.equationEditor, last.content);
      return this.replaceParsingStackTop(node, '', -1);
    }
    if (thirdToLast.isExpression() && secondToLast.isExpression() &&
        last.syntacticRole !== '^' && last.syntacticRole !== '_') {
      // Absorb atom / immutable atom to preceding horizontal math.
      if (thirdToLast.node.type.type === knownTypes.horizontalMath.type) {
        this.lastRuleName = 'merge node into horizontal math';
        thirdToLast.node.appendChild(secondToLast.node);
        return this.replaceParsingStackRange(thirdToLast.node, '', -3, -2);
      } else {
        this.lastRuleName = 'merge expressions nodes into horizontal math';
        let node = mathNodeFactory.horizontalMathFromArray(
            this.equationEditor, [thirdToLast.node, secondToLast.node]);
        return this.replaceParsingStackRange(node, '', -3, -2);
      }
    }
    return false;
  }

  /** @return {boolean} */
  areMatchingDelimiters(
      /** @type {SyntacticElement!} */
      left,
      /** @type {SyntacticElement!} */
      right,
  ) {
    if (!(left.syntacticRole in latexConstants.leftDelimiters) ||
        !(right.syntacticRole in latexConstants.rightDelimiters)) {
      return false;
    }
    if (left.syntacticRole === '|' || right.syntacticRole === '|') {
      return left.syntacticRole === right.syntacticRole;
    }
    return true;
  }
}

class EquationEditorOptions {
  constructor(
      /**
       * @type {{
       * editable: boolean,
       * removeDisplayStyle: (boolean|undefined),
       * sanitizeLatexSource: (boolean|undefined),
       * debugLogContainer: (HTMLElement?|null|undefined),
       * latexInput: (HTMLInputElement?|null|undefined),
       * editHandler: (Function?|null|undefined),
       * lineBreakWidth: (number|undefined),
       * logTiming: (boolean|undefined),
       * copyButton: (boolean|undefined),
       * extraKeyHandlers: Object<string, function (EquationEditor, boolean)>
       * }}
       */
      options,
  ) {
    /** @type {boolean} */
    this.editable = options.editable;
    /** @type {boolean|undefined} */
    this.removeDisplayStyle = options.removeDisplayStyle;
    /** @type {boolean|undefined} */
    this.sanitizeLatexSource = options.sanitizeLatexSource;
    /** @type {HTMLElement?|undefined} */
    this.debugLogContainer = options.debugLogContainer;
    /** @type {HTMLInputElement?|undefined} */
    this.latexInput = options.latexInput;
    /** @type {boolean|undefined} */
    this.logTiming = options.logTiming;
    /** @type {number} */
    this.lineBreakWidth = 0;
    /** @type {boolean} */
    this.copyButton = false;
    if (options.lineBreakWidth !== null ||
        options.lineBreakWidth !== undefined) {
      this.lineBreakWidth = /** @type {number} */ (options.lineBreakWidth);
    }
    /**
     * @type {Function?|null|undefined}
     * Called on modification of the editor.
     * Will provide two arguments:
     * the editor and the MathNode being edited.
     */
    this.editHandler = options.editHandler;
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
    if (options.copyButton === true) {
      this.copyButton = true;
    }
    /** @type {boolean} */
    this.showLatexOnDoubleClick = !this.editable && !this.copyButton;
    /** @type {Object<string, function(EquationEditor,boolean)>|undefined} */
    this.extraKeyHandlers = {};
    if (options.extraKeyHandlers !== undefined && options.extraKeyHandlers !== null) {
      for (let label in options.extraKeyHandlers) {
        this.extraKeyHandlers[label] = options.extraKeyHandlers[label];
      }
    }
  }
}

class KeyHandlerResult {
  constructor(
      /** @type {boolean} */
      preventDefault,
      /** @type {boolean} */
      updateAlignment,
  ) {
    this.preventDefault = preventDefault;
    this.updateAlignment = updateAlignment;
  }
}

class CircularBuffer {
  constructor(
      /** @type {number} */
      capacity,
  ) {
    /** @type {Array.<string>!} */
    this.elements = [];
    /** @type {number} */
    this.lowestIndex = 0;
    /** @type {number} */
    this.highestIndex = -1;
    /** @type {number} */
    this.capacity = capacity;
  }

  push(
      /** @type {string} */
      input,
  ) {
    this.highestIndex++;
    if (this.totalElements() > this.capacity) {
      this.lowestIndex++;
    }
    this.setElement(this.highestIndex, input);
  }

  /**
   * Pops an element of the buffer.
   *
   * @return {string?}
   */
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
      /** @type {number} */
      index,
      /** @type {string} */
      input,
  ) {
    let circularIndex = this.indexToCircular(index);
    for (let i = this.elements.length; i <= circularIndex; i++) {
      this.elements.push('');
    }
    this.elements[circularIndex] = input;
  }

  /**
   * Reduces an element index modulo the size of the buffer.
   *
   * @return {number}
   */
  indexToCircular(
      /** @type {number} */
      index,
  ) {
    return index % this.capacity;
  }

  /** @return {string|null} */
  get(
      /** @type {number} */
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

  /** @return {number} */
  totalElements() {
    return this.highestIndex - this.lowestIndex + 1;
  }

  /** @return {string} */
  toString() {
    let elementsToShowAtEachEnd = 2;
    if (this.totalElements() <= elementsToShowAtEachEnd * 2) {
      let result = [];
      for (let i = this.lowestIndex; i <= this.highestIndex; i++) {
        result.push(this.get(i));
      }
      return result.join(', ');
    }
    let resultLeft = [];
    for (let i = this.lowestIndex;
         i < this.lowestIndex + elementsToShowAtEachEnd; i++) {
      resultLeft.push(this.get(i));
    }
    let resultRight = [];
    for (let i = this.highestIndex - elementsToShowAtEachEnd + 1;
         i <= this.highestIndex; i++) {
      resultRight.push(this.get(i));
    }
    let left = resultLeft.join(', ');
    let right = resultRight.join(', ');
    let omitted = this.totalElements() - 2 * elementsToShowAtEachEnd;
    return `${left} ...(${omitted} omitted)...${right}`;
  }
}

class FocusInformation {
  constructor() {
    /** @type {MathNode?} Used to write to the last focused node.*/
    this.lastFocused = null;
    /** @type {MathNodeWithCursorPosition!} */
    this.lastSelectionStart = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition!} */
    this.lastSelectionStartExpanded = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition!} */
    this.lastSelectionEnd = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition!} */
    this.lastSelectionEndExpanded = new MathNodeWithCursorPosition(null, -1);
  }

  setLastFocused(
      /** @type {MathNode?} */
      lastFocused,
  ) {
    this.lastFocused = lastFocused;
  }

  setLastSelection(
      /** @type {EquationEditor!} */
      editor,
  ) {
    this.lastSelectionStart.assign(editor.selectionStart);
    this.lastSelectionStartExpanded.assign(editor.selectionStartExpanded);
    this.lastSelectionEnd.assign(editor.selectionEnd);
    this.lastSelectionEndExpanded.assign(editor.selectionEndExpanded);
  }

  restoreSelection(
      /** @type {EquationEditor!} */
      editor,
  ) {
    editor.selectionStart.assign(this.lastSelectionStart);
    editor.selectionStartExpanded.assign(this.lastSelectionStartExpanded);
    editor.selectionEnd.assign(this.lastSelectionEnd);
    editor.selectionEndExpanded.assign(this.lastSelectionEndExpanded);
  }

  /** @return {Array.<HTMLElement!>!} */
  toHtmlDebugData() {
    let result = [];
    if (this.lastFocused === null) {
      return result;
    }
    result.push(document.createElement('br'));
    result.push(document.createTextNode(
        `Last focused: ${this.lastFocused.toString()}, position: ${
            this.lastFocused.positionCursorBeforeKeyEvents}`));
    if (this.lastFocused.isDetached()) {
      result.push(document.createElement('br'));
      result.push(boldElement('Detached last focused.', 'red'));
    }
    return result;
  }
}

class CopyButton {
  constructor(
      /** @type {EquationEditor?} */
      equationEditor,
  ) {
    /**@type{EquationEditor?} */
    this.equationEditor = equationEditor;
    /** @type {HTMLElement?} */
    this.button = null;
    /** @type {HTMLElement?} */
    this.container = null;
  }

  /** Initializes the copy button. Call to create the button. */
  initialize() {
    this.container =
        /** @type {HTMLElement!}*/ (document.createElement('span'));
    this.button = /** @type {HTMLElement!}*/ (document.createElement('button'));
    this.container.style.fontSize = '6px';
    this.button.style.fontSize = '6px';
    this.container.appendChild(this.button);
    this.button.style.opacity = '0';
    this.button.style.transition = 'all 1s';
    this.equationEditor.container.addEventListener('mouseover', () => {
      this.button.style.opacity = '1';
    });
    this.equationEditor.container.addEventListener('mouseleave', () => {
      this.button.style.opacity = '0';
    });
    this.button.addEventListener('click', () => {
      this.copy();
    });
    this.button.textContent = '\uD83D\uDCCB';
    this.button.style.cursor = 'pointer';
    this.container.style.position = 'absolute';
    this.container.style.left = '100%';
    this.equationEditor.container.appendChild(this.container);
  }

  /**
   * Copies the latex of the equationEditor to the clipboard. Equivalent to
   * select+all and doing ctrl+c.
   */
  copy() {
    this.equationEditor.copyToClipboard();
    this.equationEditor.container.style.transition = 'all 1s';
    this.equationEditor.container.style.backgroundColor = 'lightgreen';
    setTimeout(() => {
      this.equationEditor.container.style.backgroundColor = '';
      setTimeout(() => {
        this.equationEditor.container.style.transition = '';
      }, 1000);
    }, 1000);
  }
}

class EquationEditor {
  constructor(
      /** @type {HTMLElement!} */
      containerGiven,
      /** @type {EquationEditorOptions?} */
      options,
      /** @type {HTMLElement?} */
      containerSVGGiven,
  ) {
    /** @type {HTMLElement!} */
    this.container = containerGiven;
    /** @type {HTMLElement?} */
    this.containerSVG = containerSVGGiven;
    if (this.containerSVG === undefined) {
      this.containerSVG = null;
    }
    /** @type {EquationEditorOptions!} */
    this.options = new EquationEditorOptions({
      editable: true,
    });
    if (options !== null && options !== undefined) {
      this.options = options;
    }
    /** @type {HTMLElement?} */
    this.latexContainer = null;
    this.initializeContainer(this.container);
    this.initializeContainer(this.containerSVG);
    /** @type {number} */
    this.containerDesiredHeight = 0;
    /** @type {number} */
    this.containerDesiredWidth = 0;
    /** @type {MathNodeRoot!} */
    this.rootNode = mathNodeFactory.rootMath(this);
    if (!this.options.editable) {
      this.rootNode.type.borderStyle = '';
      this.rootNode.type.padding = '';
      this.rootNode.type.margin = '';
      this.rootNode.type.cursor = '';
      this.rootNode.type.minWidth = '';
    }
    /**
     * @type {boolean} whether there's a pending writeLatex event to be
     * trigerred when the event is visible.
     */
    this.pendingWriteLatex = false;
    /** @type {IntersectionObserver?} */
    this.observer = null;
    /** @type {boolean} */
    this.preventEditorBlur = false;
    /** @type {boolean} */
    this.mouseSelectionStarted = false;
    /** @type {boolean} */
    this.selectionNoMoreDefault = false;
    /** @type {boolean} */
    this.mouseIgnoreNextClick = false;
    /** @type {boolean} */
    this.mouseSelectionVisible = false;
    /** @type {MathNodeWithCursorPosition?} */
    this.selectionStart = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition?} */
    this.selectionStartExpanded = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition?} */
    this.selectionEnd = new MathNodeWithCursorPosition(null, -1);
    /** @type {MathNodeWithCursorPosition?} */
    this.selectionEndExpanded = new MathNodeWithCursorPosition(null, -1);
    /** @type {string} */
    this.latexLastWritten = '';
    /** @type {string} */
    this.latexLastEdit = '';
    /** @type {string} */
    this.latexLastEditWithCursor = '';
    /** @type {CircularBuffer!} */
    this.history = new CircularBuffer(5000);
    /** @type {Array.<string>!} */
    this.redoBuffer = [];
    /** @type {boolean}*/
    this.backslashSequenceStarted = false;
    /** @type {string}*/
    this.backslashSequence = '';
    /** @type {number} */
    this.backslashPosition = -1;
    /** @type {string} */
    this.positionDebugString = '';
    /** @type {string} */
    this.fontFamily = '';
    /** @type {boolean} */
    this.hasFocusDOM = false;
    /**
     * @type {boolean} If changing this.container's font size, set this to true
     * before the change, and back to false after.
     */
    this.resizingEditorFont = false;
    if (this.options.latexInput !== null) {
      this.options.latexInput.addEventListener('keyup', (e) => {
        this.writeLatexFromInput();
      });
    }
    this.focusInformation = new FocusInformation();
    /** @type {string} */
    this.lastSelectionAction = '';
    /** @type {number} */
    this.standardAtomHeight = 0;
    /** @type {string} */
    this.lastCopied = '';
    /** @type {MathNode?} */
    this.eventCatcher = null;
    this.prepareEventCatcher();
    /** @type{CopyButton?} */
    this.copyButtonContainer = null;
    this.prepareCopyButton();
  }

  initializeContainer(
      /** @type {HTMLElement?} */
      inputContainer,
  ) {
    if (inputContainer === null) {
      return;
    }
    inputContainer.style.display = 'inline-block';
    inputContainer.style.position = 'relative';
    inputContainer.textContent = '';
    if (this.options.editable) {
      inputContainer.style.margin = '2px';
      inputContainer.style.padding = '2px';
    }
  }

  /**
   * Extracts the font family string.
   *
   * @return {string}
   */
  getFontFamily() {
    if (this.fontFamily === '') {
      this.fontFamily = this.container.style.fontFamily;
    }
    return this.fontFamily;
  }

  /**
   * Focuses the event catcher DOM component.
   *
   * The event catcher is a very small (~1px size) component that is used as a
   * dummy event absorber.
   *
   * @return {boolean}
   */
  focusEventCatcher() {
    if (this.eventCatcher.element === null) {
      return false;
    }
    this.eventCatcher.element.focus();
    return true;
  }

  /**
   * Restores the focus on the currently active math component.
   */
  focusRestore() {
    if (!this.rootNode.focusRestore()) {
      this.resetSelectionFocusBestChoice();
    }
  }

  /**
   * Executes an "undo" action in the equation editor.
   */
  undo() {
    if (this.history.totalElements() <= 0) {
      return;
    }
    this.redoBuffer.push(this.latexLastEditWithCursor);
    let element = /** @type{string} */ (this.history.pop());

    this.writeLatex(element);
    this.latexLastEditWithCursor = element;
    this.latexLastEdit = this.rootNode.toLatexWithAnnotation(null).latex;
    this.focusRestore();
  }

  /**
   * Executes a "redo" action in the equation editor.
   */
  redo() {
    if (this.redoBuffer.length === 0) {
      return;
    }
    let element = this.redoBuffer.pop();
    this.writeLatex(element);
    this.writeLatexToInput(false);
    this.focusRestore();
  }

  /** @return {KeyHandlerResult!} */
  pasteFromClipboard() {
    if (!this.hasSelection() || !this.selectionEscapedOriginalAtom()) {
      return new KeyHandlerResult(false, false);
    }
    navigator.clipboard.readText().then((text) => {
      this.deleteSelection(text);
    });
    return new KeyHandlerResult(true, true);
  }

  /**
   * Executes a copy-to-clipboard operation.
   * @return {string}
   */
  copyToClipboard() {
    let toBeCopied = '';
    if (this.selectionEndExpanded.element === null ||
        this.selectionStartExpanded.element === null) {
      toBeCopied = this.rootNode.toLatexWithAnnotation(null).latex;
    } else {
      /** @type {MathNode!} */
      let left = this.selectionStartExpanded.element;
      /** @type {MathNode!} */
      let right = this.selectionEndExpanded.element;
      if (right.isToTheLeftOf(left)) {
        let copy = right;
        right = left;
        left = copy;
      }
      left = left.beefUpToHorizontalParent();
      right = right.beefUpToHorizontalParent();
      /** @type {Array.<string>!} */
      let latexContent = [];
      /** @type {MathNode?} */
      let current = left;
      for (;;) {
        latexContent.push(current.toLatexWithAnnotation(null).latex);
        if (current === right) {
          break;
        }
        current = current.horizontalSibling(1);
        if (current === null) {
          break;
        }
      }
      toBeCopied = latexContent.join('');
    }
    this.lastCopied = toBeCopied;
    navigator.clipboard.writeText(toBeCopied);
    this.writeDebugInfo(null);
    return toBeCopied;
  }

  /** Computes the height of a white-space text in a DOM. */
  computeStandardAtomHeight() {
    // In firefox, empty space can be interpretted to have zero height;
    // not so for non-breaking space.
    this.eventCatcher.initialContent = '\u200A';
    this.eventCatcher.element =
        /** @type {HTMLElement!}*/ (document.createElement('div'));
    this.eventCatcher.element.textContent = this.eventCatcher.initialContent;
    this.container.appendChild(this.eventCatcher.element);
    let boundingBox = this.eventCatcher.element.getBoundingClientRect();
    this.standardAtomHeight = boundingBox.height;
    this.container.removeChild(this.eventCatcher.element);
    this.eventCatcher.element = null;
  }

  /** Prepares a stand-alone copy button that copies the formula. */
  prepareCopyButton() {
    if (!this.options.copyButton) {
      return;
    }
    this.copyButtonContainer = new CopyButton(this);
    this.copyButtonContainer.initialize();
  }

  /** Initializes the event catcher. */
  prepareEventCatcher() {
    this.eventCatcher = new MathNode(this, knownTypes.eventCatcher);
    this.computeStandardAtomHeight();
    if (!this.options.editable) {
      return;
    }
    this.eventCatcher.createDOMElementIfMissing();
    this.container.appendChild(this.eventCatcher.element);
    this.eventCatcher.element.style.position = 'absolute';
    this.eventCatcher.element.style.left = '';
    this.eventCatcher.element.style.top = '';
    this.eventCatcher.element.addEventListener('copy', (_) => {
      this.copyToClipboard();
    });
    this.eventCatcher.element.addEventListener('paste', (e) => {
      this.pasteFromClipboard();
    });
    this.eventCatcher.element.addEventListener('keydown', (e) => {
      this.handleKeyDownCatchAll(/** @type {KeyboardEvent!}*/ (e));
    });
    this.container.addEventListener('keydown', (e) => {
      this.handleKeyDownCatchAll(/** @type {KeyboardEvent!}*/ (e));
    });
    this.eventCatcher.element.addEventListener('blur', (e) => {
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

  /** Resets the math selection. */
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

  /** Stores the last selection. To be used if the window loses focus. */
  storeSelection() {
    this.focusInformation.setLastSelection(this);
  }

  /** Ensures the DOM matches the MathNode tree. */
  updateDOM() {
    this.rootNode.updateDOM();
  }

  /** Writes an SVG component to the svg container specified at creation. */
  writeSVG() {
    if (this.containerSVG === null) {
      return;
    }
    this.containerSVG.textContent = '';
    // this.containerSVG.contentEditable = true;
    let graphicsWriter = new ScalableVectorGraphicsWriter();
    let svgElement = graphicsWriter.typeset(this.rootNode);
    svgElement.style.display = 'inline-block';
    svgElement.style.position = 'absolute';
    svgElement.style.left = `${this.rootNode.boundingBox.left}px`;
    svgElement.style.top = `${this.rootNode.boundingBox.top}px`;
    this.containerSVG.appendChild(svgElement);
    this.containerSVG.style.width = this.containerDesiredWidth;
    this.containerSVG.style.height = this.containerDesiredHeight;
    if (this.rootNode.boundingBox.needsMiddleAlignment) {
      this.containerSVG.style.verticalAlign = 'middle';
    } else {
      this.containerSVG.style.verticalAlign = 'text-bottom';
    }
  }

  /**
   * Writes parsing debug information to a DOM component.
   *
   * Use for manual debugging of the parser.
   */
  writeDebugInfo(
      /** @type {LaTeXParser?} */
      parser,
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
      result.push(document.createElement('hr'));
    }
    let debugPart = this.toHtmlDebugData();
    for (let i = 0; i < debugPart.length; i++) {
      result.push(debugPart[i]);
    }
    this.options.debugLogContainer.textContent = '';
    for (let i = 0; i < result.length; i++) {
      this.options.debugLogContainer.appendChild(result[i]);
    }
  }

  writeLatexToInput(
      /** @type {boolean} */
      wipeOffRedoBuffer,
  ) {
    if (this.backslashSequenceStarted) {
      return;
    }
    let latexWithAnnotationNoCursor = this.rootNode.toLatexWithAnnotation(null);
    if (this.latexLastEdit === latexWithAnnotationNoCursor.latex) {
      return;
    }
    if (wipeOffRedoBuffer) {
      this.redoBuffer = [];
    }
    this.history.push(this.latexLastEditWithCursor);
    this.latexLastEditWithCursor =
        this.rootNode.toLatexWithAnnotation(latexOptionsWithCursor).latex;
    this.latexLastEdit = latexWithAnnotationNoCursor.latex;
    if (this.options.latexInput !== null) {
      this.options.latexInput.value = latexWithAnnotationNoCursor.latex;
    }
  }

  /**
   * Fetches latex from a managed plain text component and writes it to the
   * equation editor.
   */
  writeLatexFromInput() {
    if (this.options.latexInput === null) {
      return;
    }
    let inputContent = this.options.latexInput.value;
    this.writeLatex(inputContent);
  }

  /** Writes latex to the currently-focused component. */
  writeLatex(
      /** @type {string} */
      latex,
  ) {
    if (this.container === null) {
      throw new Error('Attempt to write to non-initialized equation editor.');
    }
    if (this.observer === null) {
      // Please note: Intersection observer is a recent addition to web
      // browsers. It "fires events" when the visibility of an element changes.
      // The purpose of this snippet is the following.
      // Suppose we want to write latex to an element that is nested in another
      // element with visibility:none. Then the getBoundingClientRect() bounding
      // rectangles of all atoms in our editor will have zero dimensions, thus
      // throwing off the typesetting computation. So, we need to postpone the
      // writeLatex() function to the time when the element comes visible, hence
      // the observer below. Please note: common sense suggests that
      // this.container removed from the DOM tree, both this.container and the
      // observer below will be garbage-collected. However, this has not been
      // 100% tested to be the case. Since in most pages this.container will not
      // be removed from the DOM tree, it seems that the question of garbage
      // collection correctness can be ignored.
      this.observer = new IntersectionObserver((unused1, unused2) => {
        this.writeLatexPartTwo(latex);
      }, {
        root: document.documentElement,
      });
      this.observer.observe(this.container);
    }
    this.pendingWriteLatex = true;
    if (this.container.style.minWidth === '') {
      this.container.style.minWidth = '1px';
    }
    this.writeLatexPartTwo(latex);
  }

  /** @return {boolean} */
  writeLatexPartTwo(
      /** @type {string} */
      latex,
  ) {
    if (this.pendingWriteLatex !== true) {
      return false;
    }
    let boundingRectangle = this.container.getBoundingClientRect();
    if (boundingRectangle.width === 0) {
      return false;
    }
    this.pendingWriteLatex = false;
    this.latexLastWritten = latex;
    this.rootNode.removeAllChildren();
    if (this.rootNode.element !== null) {
      this.rootNode.element.textContent = '';
    }
    let parser = new LaTeXParser(this, latex);
    if (this.options.debugLogContainer !== null) {
      parser.debug = true;
    }
    let newContent = parser.parse();
    if (newContent === null) {
      this.writeDebugInfo(parser);
      console.log(`Failed to construct node from your input ${latex}.`);
      return false;
    }
    this.rootNode.appendChild(newContent);
    this.updateDOM();
    this.updateAlignment();
    this.writeSVG();
    this.writeDebugInfo(parser);
    this.container.setAttribute('latexsource', latex);
    if (this.containerSVG !== null) {
      this.containerSVG.setAttribute('latexsource', latex);
    }
    this.setLastFocused(this.rootNode.rightmostAtomChild());
    return true;
  }

  writeLatexLastFocused(
      /** @type {string} */
      latex,
  ) {
    if (this.focusInformation.lastFocused === null) {
      let toWriteTo = this.rootNode.rightmostAtomChild();
      if (toWriteTo === null) {
        console.log('Unexpected failure to find atom child.');
        return;
      }
      toWriteTo.positionCursorBeforeKeyEvents =
          toWriteTo.textContentOrInitialContent().length;
      toWriteTo.writeLatex(latex);
      this.writeLatexToInput(false);
      return;
    }
    let lastFocusedReference = this.focusInformation.lastFocused;
    // Ensure reference to last modified is wiped early.
    this.focusInformation.lastFocused = null;
    lastFocusedReference.writeLatex(latex);
    this.writeLatexToInput(false);
  }

  accountFrameTime(
      /** @type {number} */
      frameStart,
  ) {
    let now = new Date().getTime();
    let elapsedTime = now - frameStart;
    if (elapsedTime > 100) {
      console.log(
          `Warning: last equation editor frame took a full ${elapsedTime}ms.`);
    }
  }

  setLastFocused(
      /** @type {MathNode?} */
      lastFocused,
  ) {
    this.focusInformation.setLastFocused(lastFocused);
  }

  /** @return {Array.<HTMLElement!>!} */
  toHtmlDebugData() {
    let latexWithAnnotation = this.rootNode.toLatexWithAnnotation(null);
    let result = [];
    result.push(document.createTextNode(`Latex: ${latexWithAnnotation.latex}`));
    let backslashEscaped = latexWithAnnotation.latex.split('\\').join('\\\\');
    result.push(document.createTextNode(
        `Latex, backslash-escaped: ${backslashEscaped}`));
    result.push(document.createElement('br'));
    result.push(document.createTextNode(
        `URL-encoded: ${encodeURIComponent(latexWithAnnotation.latex)}`));
    if (this.lastCopied !== '') {
      result.push(document.createElement('br'));
      result.push(document.createTextNode(`Last copied: ${this.lastCopied}`));
    }
    let focusInformation = this.focusInformation.toHtmlDebugData();
    for (let i = 0; i < focusInformation.length; i++) {
      result.push(focusInformation[i]);
    }
    result.push(document.createElement('br'));
    result.push(document.createTextNode(`History: ${this.toStringHistory()}`));
    if (this.lastSelectionAction !== '') {
      result.push(document.createElement('br'));
      result.push(document.createTextNode(
          `Last selection action: ${this.lastSelectionAction}`));
    }
    result.push(document.createElement('br'));
    result.push(document.createTextNode(this.toStringSelection()));
    result.push(document.createElement('br'));
    result.push(document.createTextNode(`Structure: `));
    let elements = this.rootNode.toHtmlDebugData(0);
    for (let i = 0; i < elements.length; i++) {
      result.push(document.createElement('br'));
      result.push(elements[i]);
    }
    result.push(document.createElement('br'));
    result.push(document.createTextNode(`Backslash position: ${
        this.backslashPosition}; started: ${
        this.backslashSequenceStarted}, sequence: ${this.backslashSequence}.`));
    result.push(document.createElement('br'));
    result.push(document.createTextNode(
        `Position computation string: ${this.positionDebugString}.`));
    return result;
  }

  /**
   * Computes a string representation of the redo-undo buffer.
   *
   * @return {string}
   */
  toStringHistory() {
    let result = this.history.toString();
    if (this.redoBuffer.length > 0) {
      result += `<br>Redo buffer: ${this.redoBuffer.join(', ')}`;
    }
    return result;
  }

  /**
   * Resets the selection and dispatches a key string to the last focused /
   * best element.
   *
   * @param {string} key
   */
  dispatchKey(
      key,
  ) {
    let focused = this.restoreSelectionOrFocusBestChoice();
    this.dispatchKeyToNode(focused, key);
  }

  dispatchKeyToNode(
      /** @type {MathNode?}*/
      focused,
      /** @type {string} */
      key,
  ) {
    if (focused === null) {
      return;
    }
    if (key === 'ArrowLeft' && focused.positionCursorBeforeKeyEvents > 0) {
      focused.focus(0);
      return;
    }
    let event = new KeyboardEvent('keydown', {
      key: key,
    });
    if (focused === this.eventCatcher) {
      this.handleKeyDownCatchAll(event);
      return;
    }
    focused.handleKeyDownDontComputeCursorPosition(event);
  }

  /** @return {MathNode?} */
  getLastFocused() {
    if (this.focusInformation.lastFocused === null) {
      return this.rootNode.rightmostAtomChild();
    }
    if (this.focusInformation.lastFocused.isDetached()) {
      return this.rootNode.rightmostAtomChild();
    }
    return this.focusInformation.lastFocused;
  }

  /** @return {KeyHandlerResult!} */
  arrowShiftHeld(
      /** @type {MathNode?} */
      target,
      /** @type {string} */
      arrowKey,
  ) {
    this.initializeSelectionStart(target);
    let preventDefault = this.computeSelectionEndFromShiftKey(arrowKey);
    return new KeyHandlerResult(preventDefault, preventDefault);
  }

  /** @return {MathNode!} The node that was focused or the event catcher. */
  restoreSelectionOrFocusBestChoice() {
    if (this.hasSelection()) {
      this.eventCatcher.focus(0);
      return this.eventCatcher;
    }
    return this.resetSelectionFocusBestChoice();
  }

  /** @return {MathNode!} The node that was focused. */
  resetSelectionFocusBestChoice() {
    if (this.hasSelection()) {
      let lastSelected = this.selectionEndExpanded.element;
      let direction = -1;
      if (this.selectionStartToTheLeftOfSelectionEnd()) {
        direction = 1;
      }
      this.resetSelectionFullSelectEventCatcher();
      lastSelected.focus(direction);
      return lastSelected;
    }
    let result = this.getLastFocused();
    this.resetSelectionFullSelectEventCatcher();
    let position = -1;
    if (result.isAtomEditable()) {
      position = result.positionCursorBeforeKeyEvents;
    }
    result.focus(1);
    if (position !== -1) {
      result.positionCursorBeforeKeyEvents = position;
    }
    return result;
  }

  makeSqrtFromSelection(
      /** @type {SplitBySelectionResult?} */
      splitBySelection,
  ) {
    if (splitBySelection === null) {
      return;
    }
    let ancestor = splitBySelection.ancestor;
    ancestor.removeAllChildren();
    let underTheRadical =
        mathNodeFactory.horizontalMathFromArray(this, splitBySelection.split);
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
      /** @type {SplitBySelectionResult?} */
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
    fraction.children[1].focus(-1);
  }

  /**
   * Determines whether to delete a selection with non-default editable action
   * when a key is pressed.
   *
   * @param {string} key
   *
   * @return {boolean}
   */
  shouldDeleteSelection(
      key,
  ) {
    return this.hasSelection() && this.selectionEscapedOriginalAtom() &&
        latexConstants.isCharacterReplacingSelection(key);
  }

  /** @return {KeyHandlerResult!} */
  handleControlKeys(
      /** @type {KeyboardEvent!} */
      e,
  ) {
    if (!e.ctrlKey) {
      return new KeyHandlerResult(false, false);
    }
    switch (e.key) {
      case 'Z':
        this.redo();
        return new KeyHandlerResult(true, true);
      case 'z':
        this.undo();
        return new KeyHandlerResult(true, true);
      case 'c':
        this.copyToClipboard();
        return new KeyHandlerResult(true, false);
      case 'x':
        this.copyToClipboard();
        return this.deleteSelection(null);
      case 'v':
        this.pasteFromClipboard();
        return new KeyHandlerResult(false, false);
      default:
        return new KeyHandlerResult(false, false);
    }
  }

  handleKeyDownCatchAll(
      /** @type {KeyboardEvent!} */
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
      case 'ArrowLeft':
      case 'ArrowRight':
      case 'ArrowUp':
      case 'ArrowDown':
        if (e.shiftKey) {
          this.arrowShiftHeld(this.getLastFocused(), key);
          return;
        }
        this.resetSelectionFocusBestChoice();
        return;
      case '/':
        this.makeFractionNumeratorFromSelection(this.splitAtomsBySelection());
        return;
      case '\\sqrt':
        if (!this.hasSelection()) {
          this.resetSelectionFocusBestChoice().makeSqrt();
          return;
        }
        this.makeSqrtFromSelection(this.splitAtomsBySelection());
        return;
      case 'Delete':
      case 'Backspace':
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

  /** @return {boolean} */
  hasSelection() {
    return this.selectionStart.element !== null;
  }

  /** @return {boolean} */
  selectionEscapedOriginalAtom() {
    return this.selectionStartExpanded.position === -1;
  }

  /**
   * @return {KeyHandlerResult!} whether the default should be prevented.
   *
   * @param {string|null} keyOrLatex A Latex snippet, latin character,
   *     whitespace or digit to replace the selection.
   */
  deleteSelection(
      keyOrLatex,
  ) {
    if (!this.hasSelection()) {
      return new KeyHandlerResult(false, false);
    }
    if (!this.selectionEscapedOriginalAtom()) {
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let expanded =
        this.selectionStartExpanded.element.beefUpToHorizontalParent();
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
      console.log('Unexpected horizontal math element without parent.');
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let splitBySelection = this.splitAtomsBySelection();
    if (splitBySelection === null) {
      console.log('Unexpected failure to split by selection');
      this.resetSelectionLeaveRangesIntact();
      return new KeyHandlerResult(false, false);
    }
    let horizontalReplacement = mathNodeFactory.horizontalMathFromArray(
        this, splitBySelection.beforeSplit);
    /** @type {MathNode?} */
    let replacing = null;
    if (keyOrLatex !== null) {
      replacing = new LaTeXParser(this, keyOrLatex).parse();
    } else {
      replacing = mathNodeFactory.atom(this, '');
    }
    if (replacing.type.type === knownTypes.atom.type) {
      replacing.desiredCursorPosition =
          replacing.textContentOrInitialContent().length;
    } else {
      replacing.desiredCursorPosition = 1;
    }
    horizontalReplacement.appendChild(replacing);
    horizontalReplacement.appendChildren(splitBySelection.afterSplit);
    horizontalReplacement.normalizeHorizontalMath();
    horizontalReplacement.ensureEditableAtoms();
    parent.replaceChildAtPosition(
        horizontalOwner.indexInParent, horizontalReplacement);
    this.resetSelectionLeaveRangesIntact();
    parent.updateDOM();
    parent.focusRestore();
    this.writeLatexToInput(true);
    this.writeDebugInfo(null);
    return new KeyHandlerResult(true, true);
  }

  /** Computes the dimensions of the DOM element container. */
  computeContainerDimensions() {
    let boundingRectangle = this.rootNode.element.getBoundingClientRect();
    // Bounding box may not be computed correctly at this point in Firefox.
    this.containerDesiredHeight = Math.max(
        boundingRectangle.height, this.rootNode.boundingBox.height,
        this.standardAtomHeight);
    if (this.options.editable) {
      this.containerDesiredHeight += 2;
    }
    // Bounding box may not be computed correctly at this point in Firefox.
    this.containerDesiredWidth =
        Math.max(boundingRectangle.width, this.rootNode.boundingBox.width);
  }

  /** Updates the vertical alignment of the container DOM component. */
  updateAlignment() {
    this.rootNode.computeBoundingBox();
    this.rootNode.doAlign();
    this.computeContainerDimensions();
    this.container.style.height = `${this.containerDesiredHeight}px`;
    this.container.style.width = `${this.containerDesiredWidth}px`;
    if (this.rootNode.boundingBox.needsMiddleAlignment &&
        !this.options.editable) {
      this.container.style.verticalAlign = 'middle';
    } else {
      this.container.style.verticalAlign = 'sub';
    }
  }

  /** @return {string} */
  toStringSelection() {
    if (this.selectionStart.element === null) {
      return '';
    }
    let startAnnotationString = '[expanded selection null]';
    if (this.selectionStartExpanded.element !== null) {
      startAnnotationString =
          this.selectionStartExpanded.element.toLatexWithAnnotation(null).latex;
    }
    let endAnnotationString =
        this.selectionEnd.element.toLatexWithAnnotation(null).latex;
    let result =
        `Selection: from: ${this.selectionStartExpanded.toString()} to: ${
            this.selectionEndExpanded.toString()}.`;
    result += ` Actually selected: ${this.selectionStart.toString()} to: ${
        this.selectionEnd.toString()}.`;
    result += ` Latex from: ${startAnnotationString} to ${endAnnotationString}`;
    return result;
  }

  /**
   * Splits a text atom based on how much of it is selected.
   * @return {SplitBySelectionResult?}
   */
  splitOneAtomBySelection() {
    if (this.selectionStartExpanded.element === null) {
      return null;
    }
    if (this.selectionEnd.element !== this.selectionStartExpanded.element ||
        !this.selectionEnd.element.isAtomEditable()) {
      return null;
    }
    let atom = this.selectionEnd.element;
    if (!atom.hasHorozintalMathParent()) {
      console.log('Atom has non-horizontal math parent.');
      return null;
    }
    /** @type {MathNode!} */
    let parent = /** @type {MathNode!} */ (atom.parent);
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
      console.log('Unexpected negative left position.');
      return null;
    }
    let rightPositionShifted = rightPosition - leftPosition;
    if (rightPositionShifted < 0) {
      console.log('Unexpected negative shifted right position.');
      return null;
    }

    // Suppose our atom has content: "leftmiddleright".
    // Suppose the middle is selected, i.e., the selection borders are as
    // indicated: "left|middle|right".
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

  /**
   * Splits multiple atoms based on a selection.
   *
   * @return {SplitBySelectionResult?}
   */
  splitAtomsBySelection() {
    if (this.selectionStartExpanded.element === null ||
        this.selectionEndExpanded.element === null) {
      return null;
    }
    /** @type {MathNodeWithCursorPosition!} */
    let start = this.selectionStartExpanded;
    /** @type {MathNodeWithCursorPosition!} */
    let end = this.selectionEndExpanded;

    if (end.element.isToTheLeftOf(/** @type {MathNode!} */ (start.element))) {
      // Swap end and start
      let temporary = start;
      start = end;
      end = temporary;
    }
    if (this.selectionStartExpanded.element ===
            this.selectionEndExpanded.element &&
        this.selectionStartExpanded.element.isAtomEditable() &&
        !this.selectionEscapedOriginalAtom()) {
      // Selection is contained within a single atom.
      return this.splitOneAtomBySelection();
    }
    let ancestor = start.element.commonAncestor(end.element);
    if (ancestor === null) {
      console.log(`Not expected: no common ancestor element between ${
          start} and ${end}. `);
      return null;
    }
    if (ancestor.type.type !== knownTypes.horizontalMath.type) {
      ancestor = ancestor.beefUpToHorizontalParent().parent;
      if (ancestor === null) {
        // The common ancestor is the root node.
        let result = new SplitBySelectionResult(
            /** @type {MathNode!} */ (this.rootNode.children[0]));
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
    for (let i = start.element.indexInParent; i <= end.element.indexInParent;
         i++) {
      result.split.push(ancestor.children[i]);
    }
    for (let i = end.element.indexInParent + 1; i < ancestor.children.length;
         i++) {
      result.afterSplit.push(ancestor.children[i]);
    }
    return result;
  }

  /** @return {MathNode!} */
  elementFromPath(
      /** @type {Array.<number>!} */
      path,
  ) {
    let current = this.rootNode;
    for (let i = 0; i < path.length; i++) {
      current = current.children[path[i]];
    }
    return /** @type {MathNode!} */ (current);
  }

  handleDoubleClick(
      /** @type {MouseEvent?} */
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
    this.latexContainer =
        /** @type {HTMLElement!} */ (document.createElement('SPAN'));
    let latex = '';
    let writeOriginal = false;
    let removeDisplayStyle = false;
    if (this.rootNode.children[0].type.type === knownTypes.error.type) {
      writeOriginal = true;
    } else if (
        !this.options.editable && this.latexLastWritten !== '' &&
        !this.options.sanitizeLatexSource) {
      writeOriginal = true;
      removeDisplayStyle = true;
    }
    if (writeOriginal) {
      if (removeDisplayStyle) {
        latex = this.latexLastWritten.split('\\displaystyle').join('');
      } else {
        latex = this.latexLastWritten;
      }
    } else {
      latex = this.rootNode.toLatex();
    }
    this.latexContainer.textContent = latex;
    this.latexContainer.style.position = 'absolute';
    this.latexContainer.style.left = `${this.rootNode.element.style.width}`;
    this.latexContainer.style.whiteSpace = 'nowrap';
    this.container.appendChild(this.latexContainer);
    let newWidth = this.rootNode.element.getBoundingClientRect().width +
        this.latexContainer.getBoundingClientRect().width;
    this.container.style.width = newWidth;
    let range = window.getSelection().getRangeAt(0);
    let rangeClone = range.cloneRange();
    rangeClone.selectNode(this.latexContainer);
    this.resetSelectionDOM();
    window.getSelection().addRange(rangeClone);
  }

  /** Resets a selection without touching the text selection in the DOM. */
  resetSelectionLeaveRangesIntact() {
    if (this.selectionStart.element !== null) {
      this.rootNode.unSelectMouseRecursive();
    }
    this.selectionStart.element = null;
    this.selectionStart.position = -1;
    this.selectionEnd.element = null;
    this.selectionEnd.position = -1;
    this.selectionStartExpanded.element = null;
    this.selectionStartExpanded.position = -1;

    this.mouseSelectionStarted = false;
    this.mouseIgnoreNextClick = false;
    this.selectionNoMoreDefault = false;
    this.mouseSelectionVisible = false;
  }

  initializeSelectionStart(
      /** @type {MathNode?} */
      start,
  ) {
    if (start === null) {
      return;
    }
    if (this.selectionStart.element !== null) {
      return;
    }
    this.resetSelectionLeaveRangesIntact();
    this.selectionStart = new MathNodeWithCursorPosition(
        start, start.positionCursorBeforeKeyEvents);
    this.selectionEnd = new MathNodeWithCursorPosition(
        start, start.positionCursorBeforeKeyEvents);
  }

  /** @return {MathNodeWithCursorPosition?} */
  computeNewSelectionFromShiftKey(
      /** @type {string} */
      key,
  ) {
    if (key === 'ArrowLeft' || key === 'ArrowUp') {
      let shrinking = this.selectionStartToTheLeftOfSelectionEnd();
      return this.selectionEnd.leftHorizontalNeighborBalanced(shrinking);
    } else {
      let shrinking = !this.selectionStartToTheLeftOfSelectionEnd();
      return this.selectionEnd.rightHorizontalNeighborBalanced(shrinking);
    }
  }

  /** @return {boolean} whether the default should be prevented. */
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
      this.selectFromElement(/** @type {MathNode!} */ (newSelection.element));
    }, 0);
    if (this.selectionNoMoreDefault) {
      return true;
    }
    if (newSelection.element !== this.selectionStart.element) {
      return true;
    }
    return false;
  }

  /**
   * @return {boolean} whether the default browser selection action should be
   * prevented.
   */
  selectFromElement(
      /** @type {MathNode!} */
      element,
  ) {
    this.mouseIgnoreNextClick = true;
    if (element === this.selectionStart.element &&
        !this.selectionNoMoreDefault) {
      this.selectionStartExpanded.assign(this.selectionStart);
      this.selectionEnd.element.storeCursorPosition('', false);
      this.selectionEnd.position =
          this.selectionEnd.element.positionCursorBeforeKeyEvents;
      if (this.selectionEnd.position === this.selectionStart.position &&
          this.selectionEnd.element.selectionLength !== 0) {
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
    this.eventCatcher.focus(0);
    // The selection has escaped the original element.
    // Once we are out of the original element, we
    // can only select an entire atom.
    this.selectionStart.position = -1;
    this.selectionEnd.element = element;
    this.selectionEnd.position = -1;
    this.computeSelectionExpanded();
    this.highlightSelectionMouse();
    if (this.options.debugLogContainer !== null) {
      this.lastSelectionAction =
          `Mouse move selection above ${element.toString()}.`;
    }
    this.storeSelection();
    this.writeDebugInfo(null);
    this.preventEditorBlur = false;
    return true;
  }

  /** @return {MathNodeWithCursorPosition!} */
  expandElementForSelection(
      /** @type {MathNode!} */
      toBeExpanded,
      /** @type {MathNode!} */
      peer,
  ) {
    let parent = toBeExpanded.commonParent(peer);
    if (parent.parent === null) {
      return new MathNodeWithCursorPosition(this.rootNode, -1);
    }
    let container = parent.parent.children[parent.indexInParent];
    return new MathNodeWithCursorPosition(
        container.beefUpToHorizontalParent(),
        -1,
    );
  }

  /** Expands the actual selection to a balanced one. */
  computeSelectionExpanded() {
    if (this.selectionStart.element === null ||
        this.selectionEnd.element === null) {
      return;
    }

    this.selectionStartExpanded = this.expandElementForSelection(
        this.selectionStart.element, this.selectionEnd.element);
    this.selectionEndExpanded = this.expandElementForSelection(
        this.selectionEnd.element,
        /** @type {MathNode!} */ (this.selectionStartExpanded.element));
    this.balanceSelection();
  }

  /** Balances a selection without expansion. */
  balanceSelection() {
    let startElement = this.selectionStartExpanded.element;
    let endElement = this.selectionEndExpanded.element;
    let parent = startElement.parent;
    if (parent === null) {
      // We have selected the entire equation.
      return;
    }
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      // If the parent node is the root node, this is normal.
      if (parent.type.type !== knownTypes.root.type) {
        console.log(
            'Unexpected: null, non-horizontal parent or different parent.');
        console.log(`Parent type: ${parent.type.type}`);
        console.log(this.toStringSelection());
      }
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
        /** @type {MathNode!} */
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
        /** @type {MathNode!} */
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
      accountChildToTheRight(/** @type {MathNode!}*/ (parent.children[i]));
    }
    while (leftOpenCount > 0 || rightOpenCount > 0) {
      if (leftOpenCount > 0) {
        right++;
        if (right >= parent.children.length) {
          console.log('Missing right deliimiter');
          return;
        }
        accountChildToTheRight(
            /** @type {MathNode!}*/ (parent.children[right]));
      }
      if (rightOpenCount > 0) {
        left--;
        if (left < 0) {
          console.log('Missing left delimiter');
          return;
        }
        accountChildToTheLeft(/** @type {MathNode!}*/ (parent.children[left]));
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

  /** @return {boolean} */
  selectionStartToTheLeftOfSelectionEnd() {
    if (this.selectionStartExpanded.element === null ||
        this.selectionEnd.element === null) {
      return true;
    }
    return this.selectionStartExpanded.element.isToTheLeftOf(
        this.selectionEnd.element);
  }

  /** Highlights selection from the mouse position. */
  highlightSelectionMouse() {
    if (this.selectionStartExpanded.element === null ||
        this.selectionEnd.element === null) {
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
    /** @type {MathNode?} */
    let current = left;
    this.rootNode.unSelectMouseRecursive();
    this.rootNode.blurRecursive();
    for (;;) {
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

  /** Updates the editor selection to match mouse moves. */
  handleMouseMoveSelection(
      /** @type {MouseEvent!} */
      e,
      /** @type {MathNode!} */
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

  /** Starts a mouse selection. */
  handleMouseSelectionStart(
      /** @type {MouseEvent!} */
      e,
      /** @type {MathNode!} */
      element,
  ) {
    this.preventEditorBlur = true;
    if (!this.hasFocusDOM) {
      this.eventCatcher.focus(1);
      this.hasFocusDOM = true;
    }
    e.stopPropagation();
    // e.preventDefault();
    element.storeCursorPosition('', false);
    // Discard previous selection data.
    this.resetSelectionLeaveRangesIntact();
    this.initializeSelectionStart(element);
    this.resetSelectionDOMSelectEventCatcher();
    this.mouseSelectionStarted = true;
    this.mouseSelectionVisible = true;
    if (this.options.debugLogContainer !== null) {
      this.lastSelectionAction =
          `Mouse selection start over ${element.toString()}, position:${
              element.positionCursorBeforeKeyEvents}.`;
    }
    // The mouse down event may fire before the built-in element ranges are
    // updated. So, release the event handling to allow built-in processing to
    // take place, and schedule an element update immediately after.
    setTimeout(() => {
      element.storeCursorPosition('', false);
      this.selectionStart.position = element.positionCursorBeforeKeyEvents;
      this.writeDebugInfo(null);
    }, 0);
    this.writeDebugInfo(null);
    return;
  }

  /** Handles the end of a mouse selection. */
  handleMouseSelectionEnd(
      /** @type {MouseEvent!} */
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

  /** Blurs the selected math component. */
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
      /** @type {MathNode!} */
      ancestor,
  ) {
    /** @type {Array.<MathNode?>!} */
    this.beforeSplit = [];
    /** @type {Array.<MathNode?>!} */
    this.afterSplit = [];
    /** @type {Array.<MathNode?>!} */
    this.split = [];
    /** @type {MathNode!} */
    this.ancestor = ancestor;
  }
}

class BoundingBox {
  constructor() {
    /** @type {number} */
    this.left = 0;
    /** @type {number} */
    this.top = 0;
    /** @type {number} */
    this.width = 0;
    /** @type {number} */
    this.height = 0;
    /**
     * @type {number}
     * Used with broken lines to
     * indicate the height of the broken line.
     */
    this.lineHeight = -1;
    /**
     * @type {number}
     * The distance between the top of the bounding box and the expected
     * location of a fraction line.
     */
    this.fractionLineHeight = 0;

    /** @type {boolean} */
    this.needsMiddleAlignment = false;
    /** @type {Array.<number>!}*/
    // Used for math nodes that have columns (matrices, tables
    // (\begin{array}...)).
    this.columnOffsets = [];
    /** @type {number} */
    // In a^b the following measures the width of b.
    this.superScriptWidth = 0;
    /** @type {number} */
    // In a_b the following measures the width of b.
    this.subScriptWidth = 0;
    /** @type {string}*/
    this.transform = '';
    /** @type {string}*/
    this.transformOrigin = '';
    /** @type {number} */
    this.heightBeforeTransform = -1;
    /** @type {number} */
    this.widthBeforeTransform = -1;
    /** @type {number} */
    this.fontSizeInPixels = 0;
  }

  /** @return {number} */
  getColumnOffset(
      /** @type {number} */
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

  /** @return {string} */
  toString() {
    return `w: ${this.width.toFixed(2)}, h: ${this.height.toFixed(2)}, fl:${
        this.fractionLineHeight.toFixed(2)}`;
  }
}

/**
 * Contains latex of a math node with metadata.
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
      /** @type {string} */
      latex,
  ) {
    /** @type {string} */
    this.latex = latex;
  }
}

class BackslashResult {
  constructor(
      /** @type {boolean} */
      keyAccountedCarryOutDefaultEvent,
      /** @type {string} */
      command,
  ) {
    this.keyAccountedCarryOutDefaultEvent = keyAccountedCarryOutDefaultEvent;
    this.command = command;
  }
}

class MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {MathNodeType!} */
      type,
  ) {
    if (!(equationEditor instanceof EquationEditor)) {
      throw new Error(`Unexpected equation editor ${equationEditor}`);
    }
    /** @type {Array<MathNode?>!} */
    this.children = [];
    /** @type {MathNodeType!} */
    this.type = type.clone();
    /** @type {HTMLElement?} */
    this.element = null;
    /** @type {MathNode?} */
    this.parent = null;
    /** @type {number} */
    this.indexInParent = -1;
    /** @type {number} */
    this.positionCursorBeforeKeyEvents = -1;
    /** @type {number} */
    this.selectionLength = -1;
    /** @type {number} */
    this.desiredCursorPosition = -1;
    /** @type {string} */
    this.initialContent = '';
    /** @type {boolean} */
    this.focused = false;
    /**
     * @type {boolean}
     * Indicates whether the node was explicitly entered by the user
     * or has been implied by a user action, such as parentheses auto-balancing.
     */
    this.implied = false;
    /** @type {boolean} */
    this.selectedByMouse = false;
    /** @type {EquationEditor!} */
    this.equationEditor = equationEditor;
    /** @type {BoundingBox!} */
    this.boundingBox = new BoundingBox();
    /**
     * @type {string} Used for matrix column formatting; additionally reserved
     *     for unforeseen use cases.
     */
    this.latexExtraStyle = '';
    /**
     * @type {VerticalBarData?|string} Reserved for data specific to
     * particular MathNode types.
     */
    this.extraData = null;
  }

  /** Creates a the DOM element if missing. */
  createDOMElementIfMissing() {
    if (this.element !== null) {
      // Element already created;
      return;
    }
    if (this.type.type === knownTypes.formInput.type) {
      this.element =
          /** @type {HTMLElement!}*/ (document.createElement('input'));
    } else {
      this.element = /** @type {HTMLElement!}*/ (document.createElement('div'));
      // this.element = document.createTextNode("");
    }
    if ((this.type.type === knownTypes.eventCatcher.type ||
         this.type.type === knownTypes.atom.type) &&
        this.equationEditor.options.editable) {
      this.element.contentEditable = true;
    }
    if (this.type.type === knownTypes.root.type &&
        this.equationEditor.options.editable) {
      // equationeditoreditable one word for maximum portability.
      this.element.classList.add('equationeditoreditable');
      if (getComputedStyle(this.element).borderWidth === '') {
        this.element.style.borderStyle = 'solid';
      }
    }

    // Padding.
    if (this.type.padding !== '') {
      this.element.style.padding = this.type.padding;
    }
    if (this.type.paddingBottom !== '') {
      this.element.style.paddingBottom = this.type.paddingBottom;
    }
    if (this.type.paddingTop !== '') {
      this.element.style.paddingTop = this.type.paddingTop;
    }
    if (this.type.paddingLeft !== '') {
      this.element.style.paddingLeft = this.type.paddingLeft;
    }
    if (this.type.paddingRight !== '') {
      this.element.style.paddingRight = this.type.paddingRight;
    }
    if (this.type.colorText !== '') {
      this.element.style.color = this.type.colorText;
    }
    // Margins
    if (this.type.margin !== '') {
      this.element.style.margin = this.type.margin;
    }
    if (this.type.marginBottom !== '') {
      this.element.style.marginBottom = this.type.marginBottom;
    }
    if (this.type.marginLeft !== '') {
      this.element.style.marginLeft = this.type.marginLeft;
    }
    if (this.type.marginRight !== '') {
      this.element.style.marginRight = this.type.marginRight;
    }
    if (this.type.boxSizing !== '') {
      this.element.style.boxSizing = this.type.boxSizing;
    }
    // Borders.
    if (this.type.borderStyle !== '') {
      this.element.style.border = this.type.borderStyle;
    }
    if (this.type.borderColor !== '') {
      this.element.style.borderColor = this.type.borderColor;
    }
    if (this.type.borderBottom !== '') {
      this.element.style.borderBottom = this.type.borderBottom;
    }
    if (this.type.borderTopLeftRadius !== '') {
      this.element.style.borderTopLeftRadius = this.type.borderTopLeftRadius;
    }
    if (this.type.borderTopRightRadius !== '') {
      this.element.style.borderTopRightRadius = this.type.borderTopRightRadius;
    }
    if (this.type.borderBottomLeftRadius !== '') {
      this.element.style.borderBottomLeftRadius =
          this.type.borderBottomLeftRadius;
    }
    if (this.type.borderBottomRightRadius !== '') {
      this.element.style.borderBottomRightRadius =
          this.type.borderBottomRightRadius;
    }
    if (this.type.borderTop !== '') {
      this.element.style.borderTop = this.type.borderTop;
    }
    if (this.type.borderLeft !== '') {
      this.element.style.borderLeft = this.type.borderLeft;
    }
    if (this.type.borderRight !== '') {
      this.element.style.borderRight = this.type.borderRight;
    }
    this.element.style.width = this.type.width;
    this.element.style.height = this.type.height;
    this.element.style.display = this.type.display;
    if (this.type.minWidth !== '') {
      this.element.style.minWidth = this.type.minWidth;
    }
    if (this.type.overflow !== '') {
      this.element.style.overflow = this.type.overflow;
    }
    this.element.style.verticalAlign = this.type.verticalAlign;
    this.element.style.outline = this.type.outline;
    if (this.type.cursor !== '') {
      this.element.style.cursor = this.type.cursor;
    }
    if (this.type.textAlign !== '') {
      this.element.style.textAlign = this.type.textAlign;
    }
    if (this.type.whiteSpace !== '') {
      this.element.style.whiteSpace = this.type.whiteSpace;
    }
    // Fonts.
    if (this.type.fontWeight !== '') {
      this.element.style.fontWeight = this.type.fontWeight;
    }
    if (this.type.fontSizeRatio !== 1) {
      this.element.style.fontSize = `${this.type.fontSizeRatio * 100}%`;
    }
    if (this.initialContent !== '') {
      if (this.type.type === knownTypes.formInput.type) {
        (/** @type {HTMLInputElement!}*/ (this.element)).value =
            this.initialContent;
      } else {
        this.element.textContent = this.initialContent;
      }
    }
    if (this.type.position !== '') {
      this.element.style.position = this.type.position;
    }
    this.element.setAttribute('mathTagName', this.type.type);
    if (this.equationEditor.options.editable) {
      this.element.addEventListener('copy', (e) => {
        this.equationEditor.copyToClipboard();
      });
      if (this.type.type === knownTypes.atom.type) {
        this.element.addEventListener('paste', (e) => {
          this.pasteFromClipboard(e);
        });
        this.element.addEventListener('keyup', (e) => {
          this.handleKeyUp(/** @type {KeyboardEvent!}*/ (e));
        });
        this.element.addEventListener('keydown', (e) => {
          this.handleKeyDown(/** @type {KeyboardEvent!}*/ (e));
        });
        this.element.addEventListener('focus', (e) => {
          this.handleFocus(e);
        });
        this.element.addEventListener('blur', (e) => {
          this.handleBlur(e);
        });
      }
      if (this.type.type !== knownTypes.eventCatcher.type) {
        this.element.addEventListener('click', (e) => {
          this.handleSingleClick(/** @type {MouseEvent!}*/ (e));
        });
        this.element.addEventListener('mousedown', (e) => {
          this.equationEditor.handleMouseSelectionStart(
              /** @type {MouseEvent!}*/ (e), this);
        });
        this.element.addEventListener('mouseup', (e) => {
          this.equationEditor.handleMouseSelectionEnd(
              /** @type {MouseEvent!}*/ (e));
        });
        this.element.addEventListener('mousemove', (e) => {
          this.equationEditor.handleMouseMoveSelection(
              /** @type {MouseEvent!}*/ (e), this);
        });
      }
      if (this.type.type === knownTypes.root.type) {
        this.element.addEventListener('focusin', () => {
          // equationeditoreditablehighlighted one word for portability.
          this.element.classList.add('equationeditoreditablehighlighted');
        });
        this.element.addEventListener('focusout', () => {
          this.element.classList.remove('equationeditoreditablehighlighted');
        });
      }
    }
    if (this.type.type === knownTypes.root.type) {
      this.element.addEventListener('dblclick', (e) => {
        this.equationEditor.handleDoubleClick(/** @type {MouseEvent!}*/ (e));
      });
    }
  }

  handleSingleClick(
      /** @type {MouseEvent!} */
      e,
  ) {
    e.stopPropagation();
    e.preventDefault();
    if (this.equationEditor.mouseIgnoreNextClick) {
      this.equationEditor.mouseIgnoreNextClick = false;
      if (this.equationEditor.selectionEscapedOriginalAtom()) {
        this.equationEditor.resetSelectionDOMSelectEventCatcher();
        if (this.equationEditor.options.debugLogContainer !== null) {
          this.equationEditor.lastSelectionAction =
              'Set mouse ignore next to true.';
        }
      }
      this.equationEditor.writeDebugInfo(null);
      return;
    }
    if (this.equationEditor.options.debugLogContainer !== null) {
      this.equationEditor.lastSelectionAction =
          'Reset selection on single click.';
    }
    this.equationEditor.resetSelectionLeaveRangesIntact();
    this.storeCursorPosition('', false);
    if (this.type.type === knownTypes.atom.type) {
      return;
    }
    this.focus(1);
  }

  /** @return {MathNode?} */
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

  /** Selects a math node using the mouse. */
  selectElementByMouse() {
    if (!this.selectedByMouse && this.element !== null) {
      this.element.style.backgroundColor = 'lightskyblue';
    }
    this.selectedByMouse = true;
  }

  /** Un-selects a math node using the mouse. */
  unSelectElementByMouse() {
    if (this.selectedByMouse && this.element !== null) {
      this.element.style.backgroundColor = '';
    }
    this.selectedByMouse = false;
  }

  /** Un-selects a math node recursively. */
  unSelectMouseRecursive() {
    this.unSelectElementByMouse();
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i] === null) {
        continue;
      }
      this.children[i].unSelectMouseRecursive();
    }
  }

  /** Focuses the DOM element. */
  focusElement() {
    this.element.style.background = '#f0f0f0';
    this.focused = true;
    this.equationEditor.hasFocusDOM = true;
  }

  /** @return {MathNode!} */
  beefUpToHorizontalParent() {
    let parentWithIndex = this.findHorizontalMathParent();
    if (parentWithIndex.parent === null) {
      return this;
    }
    return /** @type {MathNode!}*/ (
        parentWithIndex.parent.children[parentWithIndex.indexInParent]);
  }

  /** Blurs the DOM element. */
  blurElement() {
    this.focused = false;
    if (this.element === null) {
      return;
    }
    this.element.style.background = '';
  }

  /** Blurs the DOM element and all children DOM elements. */
  blurRecursive() {
    this.blurElement();
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].blurRecursive();
    }
  }

  /** Updates the DOM to match the MathNode computed data. */
  updateDOM() {
    this.updateDOMRecursive(0);
    this.equationEditor.updateAlignment();
  }

  /**
   * @return {number}
   * Returns the length of the content of an atom.
   * Returns -1 if the element is not an atom.
   */
  lengthContentIfAtom() {
    if (!this.isAtomEditable()) {
      return -1;
    }
    if (this.element === null) {
      return this.initialContent.length;
    }
    if (this.element.textContent === null) {
      return this.initialContent.length;
    }
    return this.element.textContent.length;
  }

  /**
   * Returns the text content if atomic, else returns the empty string.
   *
   * @return {string}
   */
  contentIfAtom() {
    if (!this.isAtomEditable()) {
      return '';
    }
    return this.textContentOrInitialContent();
  }

  /** @return {string} */
  textContentOrInitialContent() {
    if (this.element === null) {
      if (this.initialContent === null) {
        console.log('Unexpected: initial content is null.');
        return '[null]';
      }
      return this.initialContent;
    }
    let result = this.element.textContent;
    if (result === null) {
      return '';
    }
    return result;
  }

  /** @return {string} */
  contentIfAtomic() {
    if (!this.isAtomic()) {
      return '';
    }
    return this.textContentOrInitialContent();
  }

  /** @return {boolean} */
  isAtomicNonEmptyOrHorizontalMathWithNonEmptyAtomic() {
    if (this.isAtomic()) {
      return this.contentIfAtomic() !== '';
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
      if (child.contentIfAtomic() !== '') {
        numberOfAtomicNonEmpty++;
        if (numberOfAtomicNonEmpty > 1) {
          return false;
        }
      }
    }
    return numberOfAtomicNonEmpty === 1;
  }

  /** @return {boolean} */
  isAtomic() {
    return (
        this.type.type === knownTypes.atom.type ||
        this.type.type === knownTypes.atomImmutable.type ||
        this.type.type === knownTypes.sqrtSign.type);
  }

  /** @return {boolean} */
  isDelimiter() {
    return (
        this.type.type === knownTypes.leftDelimiter.type ||
        this.type.type === knownTypes.rightDelimiter.type);
  }

  /** @return {boolean} */
  isAtomEditable() {
    return this.type.type === knownTypes.atom.type;
  }

  /** @return {boolean} */
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
    if (this.type.type === knownTypes.atom.type ||
        this.type.type === knownTypes.atomImmutable.type ||
        this.type.type === knownTypes.formInput.type) {
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

  /** Computes the bounding box of the element. */
  computeBoundingBox() {
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.computeBoundingBox();
    }
    this.computeDimensions();
  }

  /**
   * Computes the bounding box of an element that is a delimiter mark
   * (parenthesis, bracket), ...
   */
  computeDimensionsDelimiter() {
    let boundingRecangleDOM = this.element.getBoundingClientRect();
    this.boundingBox.width = boundingRecangleDOM.width;
  }

  /** Computes the dimensions of an atomic math node. */
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
      /** @type {number} */
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

  /** Computes the dimensions of an overline math node. */
  computeDimensionsOverLine() {
    this.computeDimensionsStandard();
    // The border add 1 extra pixel of height.
    this.boundingBox.fractionLineHeight =
        this.children[0].boundingBox.fractionLineHeight + 1;
  }

  /** Computes the dimensions of an overbrace math node. */
  computeDimensionsOverBrace() {
    let base = this.children[0];
    let brace = this.children[1];
    let superscript = this.children[2];
    brace.computeDimensionsHorizontalBrace(base.boundingBox.width);
    superscript.boundingBox.width = base.boundingBox.width;
    superscript.computeBoundingBoxLeftSingleChild();
    this.boundingBox.width = base.boundingBox.width;
    this.boundingBox.height = base.boundingBox.height +
        brace.boundingBox.height + superscript.boundingBox.height;
    this.boundingBox.fractionLineHeight = brace.boundingBox.height +
        superscript.boundingBox.height + base.boundingBox.fractionLineHeight;
    brace.boundingBox.top = superscript.boundingBox.height;
    base.boundingBox.top =
        superscript.boundingBox.height + brace.boundingBox.height;
  }

  /**
   * Computes whether the node or any of its children contains a fraction line.
   * @return {boolean}
   */
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

  /**
   * Whether the expression requires a tall exponent.
   *
   * By tall exponent we mean how high is an exponent relative to the
   * height of base: in x^2, the square should not be too tall, else the formula
   * looks unnatural.
   *
   * In \left(\frac{1}{2}\right)^2, the exponent 2 needs to
   * be high (close to the tip of the parenthesis)
   * for the formula to look natural.
   *
   * @return {boolean}
   */
  requiresTallExponent() {
    return false;
  }

  /**
   * Computes+updates the bounding box of the left-most child of a node.
   */
  computeBoundingBoxLeftSingleChild() {
    let child = this.children[0];
    child.boundingBox.left =
        (this.boundingBox.width - child.boundingBox.width) / 2;
  }

  /**
   * Vertically stretch is a to-be-overloaded method that stretches the node to
   * enclose adjacen horizontal content. For example, left, right parentheses
   * would be vertically stretched to enclose the content that they enclose.
   */
  verticallyStretch(unused1, unused2) {
    throw new Error('verticallyStretch not defined for a general MathNode');
  }

  verticallyStretchParenthesis(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
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

  /** Computes the dimensions of a denominator of a fraction. */
  computeDimensionsNumerator() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.children[0].boundingBox.height;
  }

  /** Computes the dimensions of a matrix. */
  computeDimensionsMatrix() {
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = this.children[0].boundingBox.height;
    this.boundingBox.width = this.children[0].boundingBox.width;
    this.boundingBox.fractionLineHeight =
        this.children[0].boundingBox.fractionLineHeight;
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

  /** Computes the dimensions of a matrix table. */
  computeDimensionsMatrixTable() {
    this.boundingBox = new BoundingBox();
    let betweenColumns = 10;
    let betweenRows = 10;
    if (this.children.length <= 0) {
      // empty matrix;
      this.boundingBox.height = betweenRows;
      this.boundingBox.width = betweenColumns;
      this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
      return;
    }
    let firstRow = this.children[0];
    let numberOfColumns = firstRow.children.length;
    let numberOfRows = this.children.length;
    let left = 0;
    for (let i = 0; i < numberOfColumns; i++) {
      this.boundingBox.columnOffsets.push(left);
      let width = 0;
      for (let j = 0; j < numberOfRows; j++) {
        width = Math.max(width, this.children[j].children[i].boundingBox.width);
      }
      for (let j = 0; j < numberOfRows; j++) {
        let child = this.children[j].children[i];
        let extraShift = (width - child.boundingBox.width) / 2;
        if (child.latexExtraStyle === 'l') {
          extraShift = 0;
        } else if (child.latexExtraStyle === 'r') {
          extraShift = width - child.boundingBox.width;
        }
        child.boundingBox.left = left + extraShift;
        child.computeBoundingBoxLeftSingleChild();
      }
      left += width + betweenColumns;
    }
    let rowWidth = left - betweenColumns;
    let top = 0;
    for (let i = 0; i < numberOfRows; i++) {
      let row = this.children[i];
      row.boundingBox = new BoundingBox();
      row.mergeVerticalComponentsBoundingBoxesHorizontallyAlignedElements();
      row.boundingBox.top = top;
      row.boundingBox.width = rowWidth;
      top += row.boundingBox.height + betweenRows;
    }
    this.boundingBox.height = top - betweenRows;
    this.boundingBox.width = rowWidth;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }

  /** Computes the dimensions of a box under a radical sign. */
  computeDimensionsRadicalUnderBox() {
    this.computeDimensionsStandard();
    let child = this.children[0];
    this.boundingBox.height = child.boundingBox.height + 1;
    this.boundingBox.fractionLineHeight =
        child.boundingBox.fractionLineHeight + 1;
  }

  /** Computes the dimensions of the bounding box. */
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
      throw new Error(
          'This case should be handled by MathNodeSqrt.computeDimensions()');
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
    if (this.type.type === knownTypes.numerator.type) {
      this.computeDimensionsNumerator();
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

  /**
   * A default non-dummy implementation of computeDimensions() that makes some
   * reasonable assumptions about the math bounding box.
   */
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

  /**
   * Computes the dimensions of the parentheses required to enclose the present
   * node.
   */
  computeDimensionsParenthesesHeight() {
    // Compute parentheses height
    /** @type {Array.<number>!} */
    let enclosedHeights = [];
    let enclosedFractionLineHeights = [];
    /** @type {Array.<number>!} */
    let indicesOpenedParentheses = [];
    for (let i = 0; i < this.children.length; i++) {
      this.computeDimensionsParenthesesAccountChild(
          i,
          enclosedHeights,
          enclosedFractionLineHeights,
          indicesOpenedParentheses,
      );
    }
  }

  computeDimensionsParenthesesAccountChild(
      /** @type {number} */
      index,
      /** @type {Array.<number>!} */
      enclosedHeights,
      /** @type {Array.<number>!} */
      enclosedFractionLineHeights,
      /** @type {Array.<number>!} */
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
        console.log('Warning: unbalanced right parenthesis.');
        return;
      }
      let currentHeight = enclosedHeights.pop();
      let currentFractionLineHeight = enclosedFractionLineHeights.pop();
      child.verticallyStretchParenthesis(
          currentHeight, currentFractionLineHeight);
      let leftCounterpart = this.children[indicesOpenedParentheses.pop()];
      leftCounterpart.verticallyStretchParenthesis(
          currentHeight, currentFractionLineHeight);
    }
    if (enclosedHeights.length > 0) {
      enclosedHeights[enclosedHeights.length - 1] = Math.max(
          child.boundingBox.height,
          enclosedHeights[enclosedHeights.length - 1]);
      enclosedFractionLineHeights[enclosedFractionLineHeights.length - 1] =
          Math.max(
              child.boundingBox.fractionLineHeight,
              enclosedFractionLineHeights
                  [enclosedFractionLineHeights.length - 1]);
    }
  }

  /**
   * Merges the vertical dimensions of a list of boxes that are aligned as
   * adjacent horizontal boxes.
   */
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
      child.boundingBox.top = this.boundingBox.fractionLineHeight -
          child.boundingBox.fractionLineHeight;
    }
    // Compute present element height.
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      let heightFromChild = child.boundingBox.height -
          child.boundingBox.fractionLineHeight +
          this.boundingBox.fractionLineHeight;
      this.boundingBox.height =
          Math.max(this.boundingBox.height, heightFromChild);
    }
  }

  /** Computes whether a DOM element should have middle vertical alignment.*/
  computeMiddleAlignment() {
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      if (child.boundingBox.needsMiddleAlignment) {
        this.boundingBox.needsMiddleAlignment = true;
        return;
      }
    }
  }

  /** Resizes the DOM element to match the computed bounding box. */
  doAlign() {
    if (this.boundingBox.transform !== '') {
      this.element.style.transformOrigin = this.boundingBox.transformOrigin;
      this.element.style.transform = this.boundingBox.transform;
    }
    if (this.element.style.width !== 'auto') {
      this.element.style.width = `${this.boundingBox.width}px`;
    }
    if (this.element.style.height !== 'auto') {
      this.element.style.height = `${this.boundingBox.height}px`;
    }
    this.element.style.left = `${this.boundingBox.left}px`;
    this.element.style.top = `${this.boundingBox.top}px`;
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].doAlign();
    }
  }

  updateParentDOM(
      /** @type {HTMLElement?} */
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

  /** Handles the focus of the DOM element. */
  handleFocus(_) {
    this.focusElement();
    this.storeCursorPosition('', false);
  }

  /** Handles the blur of the DOM element. */
  handleBlur(_) {
    this.blurElement();
    this.equationEditor.blur();
  }

  /** Handles a key press event received by the DOM component. */
  handleKeyDown(
      /** @type {KeyboardEvent!} */
      event,
  ) {
    if (event.key === 'Shift' || event.key === 'Ctrl') {
      event.stopPropagation();
      return;
    }
    this.storeCursorPosition(event.key, event.shiftKey);
    this.handleKeyDownDontComputeCursorPosition(event);
  }

  /** Handles a key down event without recomputing the cursor position. */
  handleKeyDownDontComputeCursorPosition(
      /** @type {KeyboardEvent!} */
      event,
  ) {
    let frameStart = new Date().getTime();
    event.stopPropagation();
    let handlerResult = this.handleKeyDownCases(event);
    if (handlerResult.preventDefault) {
      event.preventDefault();
      this.equationEditor.accountFrameTime(frameStart);
    }
    // While we no longer propagate the event, we need to
    // release the thread so the browser can finish computations
    // with the released element.
    setTimeout(() => {
      this.equationEditor.storeSelection();
      if (handlerResult.updateAlignment) {
        this.storeCursorPosition(event.key, event.shiftKey);
        this.element.style.maxWidth = '';
        this.element.style.maxHeight = '';
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

  /** Handles a key up even received by the DOM. */
  handleKeyUp(
      /** @type {KeyboardEvent!} */
      event,
  ) {
    if (!event.shiftKey && event.key !== 'Shift') {
      this.equationEditor.selectionStart =
          new MathNodeWithCursorPosition(null, -1);
      this.equationEditor.selectionEnd =
          new MathNodeWithCursorPosition(null, -1);
    }
    let keyHandlers = this.equationEditor.options.extraKeyHandlers;
    if (event.key in keyHandlers) {
      keyHandlers[event.key](this.equationEditor, event.shiftKey);
    }
  }

  /** @return {BackslashResult!} */
  processBackslash(
      /** @type {string} */
      key,
      /** @type {boolean} */
      shiftHeld,
  ) {
    let result = new BackslashResult(false, key);
    if (key === 'Shift') {
      return result;
    }
    if (this.type.type !== knownTypes.atom.type) {
      return result;
    }
    if (key === '\\' && !shiftHeld) {
      this.equationEditor.backslashSequenceStarted = true;
      this.equationEditor.backslashSequence = '\\';
      this.equationEditor.backslashPosition =
          this.positionCursorBeforeKeyEvents;
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    if (!this.equationEditor.backslashSequenceStarted) {
      return result;
    }
    if (key === ' ' && this.equationEditor.backslashSequenceStarted) {
      this.equationEditor.backslashSequenceStarted = false;
      if (this.equationEditor.backslashSequence in
          latexConstants.recognizedCommandsKeyInput) {
        return this.removeBackslashSequence();
      }
      return result;
    }
    if (latexConstants.isLatinCharacter(key)) {
      this.equationEditor.backslashSequence += key;
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    if (key === 'Backspace') {
      if (this.equationEditor.backslashSequence === '\\') {
        this.equationEditor.backslashSequenceStarted = false;
      } else {
        this.equationEditor.backslashSequence =
            this.equationEditor.backslashSequence.slice(
                0,
                this.equationEditor.backslashSequence.length - 1,
            );
      }
      result.keyAccountedCarryOutDefaultEvent = true;
      return result;
    }
    this.equationEditor.backslashSequenceStarted = false;
    return result;
  }

  /** Handles a paste event. */
  pasteFromClipboard(event) {
    event.preventDefault();
    let data = event.clipboardData.getData('text');
    event.preventDefault();
    this.storeCursorPosition('', false);
    this.writeLatex(data);
  }

  writeLatex(
      /** @type {string} */
      data,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      return;
    }
    if (this.equationEditor === null) {
      throw new Error('Equation editor cannot be null here');
    }
    let parser = new LaTeXParser(
        /** @type {EquationEditor!} */ (this.equationEditor), data);
    let newContent = parser.parse();
    if (newContent === null) {
      let content = this.textContentOrInitialContent();
      let leftSlice = content.slice(0, this.positionCursorBeforeKeyEvents);
      let rightSlice = content.slice(this.positionCursorBeforeKeyEvents);
      let endContent = leftSlice + data + rightSlice;
      this.element.textContent = endContent;
      this.desiredCursorPosition =
          this.positionCursorBeforeKeyEvents + data.length;
      this.updateDOM();
      this.equationEditor.updateAlignment();
      this.focusRestore();
      return;
    }
    let split = this.splitByCursorEmptyAtoms();
    /** @type {MathNode?} */
    let newRightMostAtom = newContent.rightmostAtomChild();
    if (newRightMostAtom === null) {
      split[1].desiredCursorPosition = 0;
    } else {
      newRightMostAtom.desiredCursorPosition =
          newRightMostAtom.textContentOrInitialContent().length;
    }
    let horizontalContent = mathNodeFactory.horizontalMathFromArray(
        this.equationEditor, [split[0], newContent, split[1]]);
    horizontalContent.normalizeHorizontalMath();
    let parent = this.parent;
    parent.replaceChildAtPosition(this.indexInParent, horizontalContent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    this.equationEditor.updateAlignment();
    parent.focusRestore();
  }

  /**
   * Inserts a string in an atomic element.
   *
   * @return {boolean}
   */
  insertString(
      /** @type {string} */
      input,
  ) {
    let split = this.splitByCursor();
    let atomNode = mathNodeFactory.atom(this.equationEditor, input);
    atomNode.desiredCursorPosition = input.length;
    let node = mathNodeFactory.horizontalMathFromArray(
        this.equationEditor, [split[0], atomNode, split[1]]);
    node.normalizeHorizontalMath();
    let parent = this.parent;
    parent.replaceChildAtPosition(this.indexInParent, node);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** @return {KeyHandlerResult!} whether default should be prevented. */
  handleKeyDownCases(
      /** @type {KeyboardEvent!} */
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
      let operator =
          latexConstants.operatorWithSuperAndSubscriptBackslashed[command];
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
    if (command in latexConstants.leftDelimiters &&
        !(command in latexConstants.delimitersAmbiguous)) {
      this.makeDelimiterLeft(command);
      return new KeyHandlerResult(true, false);
    }
    if (command in latexConstants.rightDelimiters &&
        !(command in latexConstants.delimitersAmbiguous)) {
      this.makeDelimiterRight(command);
      return new KeyHandlerResult(true, false);
    }
    if (command in latexConstants.latexBackslashAtomsEditableBackslashed) {
      let content =
          latexConstants.latexBackslashAtomsEditableBackslashed[command];
      this.insertString(content);
      return new KeyHandlerResult(true, false);
    }
    switch (backslashPreprocessing.command) {
      case '/':
        this.makeFractionNumerator();
        return new KeyHandlerResult(true, false);
      case ';':
      case ':':
      case '*':
      case '+':
      case '-':
      case '=':
      case '>':
      case '<':
      case ',':
      case '.':
        this.makeHorizontalOperatorCorrectInput(key);
        return new KeyHandlerResult(true, false);
      case '^':
        this.makeBaseWithExponent();
        return new KeyHandlerResult(true, false);
      case '_':
        this.makeBaseWithSubscript();
        return new KeyHandlerResult(true, false);
      case 'ArrowLeft':
      case 'ArrowRight':
      case 'ArrowUp':
      case 'ArrowDown':
        return this.arrow(key, shiftHeld);
      case '|':
        this.makeDelimiterAmbiguous(key);
        return new KeyHandlerResult(true, false);
      case 'Enter':
        return new KeyHandlerResult(true, false);
      case 'Delete':
        return this.deleteButton();
      case 'Backspace':
        return this.backspace();
      case '{':
        this.makeDelimiterLeft('{');
        return new KeyHandlerResult(true, false);
      case '}':
        this.makeDelimiterRight('}');
        return new KeyHandlerResult(true, false);
      case '\\sqrt':
        this.makeSqrt();
        return new KeyHandlerResult(true, false);
      case '\\cancel':
        this.makeCancel();
        return new KeyHandlerResult(true, false);
      case '\\pmatrix':
        this.makeMatrix(2, 2, 'pmatrix');
        return new KeyHandlerResult(true, false);
      case '\\bmatrix':
        this.makeMatrix(2, 2, 'bmatrix');
        return new KeyHandlerResult(true, false);
      case '\\matrix':
        this.makeMatrix(2, 2, 'matrix');
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

  /**
   * @return {Array.<number>!} The path needed to get from the current node to
   * the root node.
   */
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

  /** @return {Array.<number>?} */
  commonPathToRoot(
      /** @type {MathNode!} */
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

  /** @return {MathNode?} returns the common ancestor of two nodes. */
  commonAncestor(
      /** @type {MathNode?} */
      other,
  ) {
    if (this === other || other === null) {
      return this;
    }
    if (this.equationEditor !== other.equationEditor) {
      return null;
    }
    let commonPath =
        /** @type {Array<number>!}*/ (this.commonPathToRoot(other));
    return this.equationEditor.elementFromPath(commonPath);
  }

  /**
   * @return {ParentWithIndex!}
   * returns the common ancestor of two nodes
   * with the index of the ancestor's child
   * that contains this node.
   */
  commonParent(
      /** @type {MathNode!} */
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

  /**
   * @return {boolean} Given two atoms, elements whether this is to the left of
   * the other atom.
   */
  isToTheLeftOf(
      /** @type {MathNode!} */ other,
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

  /** @return {KeyHandlerResult!} whether the default should be prevented. */
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
      if (key === 'ArrowLeft') {
        this.positionCursorBeforeKeyEvents--;
      } else {
        this.positionCursorBeforeKeyEvents++;
      }
      return new KeyHandlerResult(false, false);
    }
    /** @type {MathNodeWithCursorPosition!} */
    const toFocus = this.getAtomToFocus(key);
    if (toFocus.element !== null) {
      toFocus.element.positionCursorBeforeKeyEvents =
          toFocus.element.element.textContent.length + 1;
      toFocus.element.focus(toFocus.position);
    }
    return new KeyHandlerResult(true, true);
  }

  /**
   * Returns whether the arrow key should be automatically handled by the
   * default DOM behavior.
   *
   * @return {boolean}
   */
  arrowAbsorbedByAtom(
      /** @type {string} */
      key,
  ) {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    if (this.element.textContent === ' ') {
      // Single space bar textcontent does not appear to respect left-right
      // arrow navigation. Tested in Chrome and Firefox.
      return false;
    }
    if (key === 'ArrowLeft') {
      return this.positionCursorBeforeKeyEvents !== 0;
    }
    if (key === 'ArrowRight') {
      return this.positionCursorBeforeKeyEvents !==
          this.element.textContent.length;
    }
    return false;
  }

  /** @return {MathNodeWithCursorPosition!} */
  getAtomToFocus(/** @type {string} */ key) {
    return this.getAtomToFocusFromAction(key, this.type.arrows[key]);
  }

  /** @return {MathNodeWithCursorPosition!} */
  getAtomToFocusFromAction(
      /** @type {string} */ key,
      /** @type {string} */ arrowType,
  ) {
    return this.getAtomToFocusFromActionDefault(key, arrowType);
  }

  /** @return {MathNodeWithCursorPosition!} */
  getAtomToFocusFromActionDefault(
      /** @type {string} */ key,
      /** @type {string} */ arrowType,
  ) {
    if (arrowType === arrowMotion.parentForward) {
      if (this.parent === null) {
        return new MathNodeWithCursorPosition(null, -1);
      }
      return this.parent.getAtomToFocus(key);
    }
    if (arrowType === arrowMotion.firstAtomToTheLeft) {
      return (new MathNodeWithCursorPosition(this, 0)).leftNeighbor();
    }
    if (arrowType === arrowMotion.firstAtomToTheRight) {
      return (new MathNodeWithCursorPosition(
                  this, this.textContentOrInitialContent().length))
          .rightNeighbor();
    }
    return new MathNodeWithCursorPosition(null, -1);
  }

  /**
   * @return {MathNode?} returns sibling, right one if direction is
   * positive, left one otherwise.
   */
  firstAtom(
      /** @type {number} */
      direction,
  ) {
    if (direction > 0) {
      return this.firstAtomToTheRight();
    } else {
      return this.firstAtomToTheLeft();
    }
  }

  /**
   * @return {MathNode?}
   * Returns first atom uncle to the left or right that is a child of a
   * horizontal ancestor. Negative direction indicates we are looking for uncles
   * to the left, else we are looking for uncles to the right of the this
   * element.
   */
  firstAtomSiblingOrUncle(
      /** @type {number} */
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
      for (let nextIndex = horizontalAncestor.indexInParent + direction;
           nextIndex >= 0 &&
           nextIndex < horizontalAncestor.parent.children.length;
           nextIndex += direction) {
        let candidate = horizontalAncestor.parent.children[nextIndex];
        if (candidate.isAtomic()) {
          return candidate;
        }
      }
      current = horizontalAncestor.parent;
    }
  }

  /**
   * Returns first MathNode atom that lies to the right of  the present element
   * or null if there is no such element.
   *
   * @return {MathNode?}
   */
  firstAtomToTheRight() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.firstAtomToTheRightOf(this.indexInParent);
  }

  /** @return {MathNode?} */
  firstAtomToTheRightOf(
      /** @type {number} */
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

  /** @return {MathNode?} */
  firstAtomToTheLeft() {
    if (this.parent === null) {
      return null;
    }
    return this.parent.firstAtomToTheLeftOf(this.indexInParent);
  }

  /** @return {MathNode?} */
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

  /** @return {MathNode?} */
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

  /** @return {MathNode?} */
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

  storeCursorPositionPreExisingRange(
      /** @type {string} */
      key,
      /** @type {boolean} */
      shiftHeld,
  ) {
    let previousPosition = this.positionCursorBeforeKeyEvents;
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
    this.positionCursorBeforeKeyEvents =
        rangeClone.toString().length;  // range.endOffset;
    this.selectionLength = range.toString().length;
    this.equationEditor.setLastFocused(this);
    this.equationEditor.positionDebugString =
        `Computed position: ${this.positionCursorBeforeKeyEvents}.`;
    this.equationEditor.positionDebugString += `Range: [${range}], clone: [${
        rangeClone}], previous position: ${previousPosition}.`;
    this.equationEditor.positionDebugString +=
        `end offset: ${range.endOffset}, start offset: ${range.startOffset}`;
    this.equationEditor.writeDebugInfo(null);
  }

  storeCursorPosition(
      /** @type {string} */
      key,
      /** @type {boolean} */
      shiftHeld,
  ) {
    if (this.type.type === knownTypes.eventCatcher.type) {
      return;
    }
    if (this.type.type !== knownTypes.atom.type) {
      this.positionCursorBeforeKeyEvents = -1;
      this.selectionLength = 0;
      this.equationEditor.setLastFocused(null);
      return;
    }
    try {
      this.storeCursorPositionPreExisingRange(key, shiftHeld);
    } catch (e) {
      console.log(`Failed to store cursor position ${e}.`);
    }
  }

  appendChild(
      /** @type {MathNode?} */
      child,
  ) {
    if (child === null) {
      return;
    }
    if (child.parent !== this && child.parent !== null &&
        child.indexInParent >= 0) {
      child.parent.children[child.indexInParent] = null;
    }
    child.parent = this;
    child.indexInParent = this.children.length;
    this.children.push(child);
  }

  appendChildren(
      /** @type {Array.<MathNode?>!} */
      newChildren,
  ) {
    for (let i = 0; i < newChildren.length; i++) {
      if (newChildren[i] === null) {
        continue;
      }
      this.appendChild(newChildren[i]);
    }
  }

  /** Ensures that horizonal math elements have editable atoms on both ends. */
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

  /**
   * Normalizes horizontal math by collapsing multiple embedded horizontal math
   * elements into a single horizontal math element.
   */
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

  /**
   * Carries out the horizontal math normalization.
   *
   * @return {boolean}
   */
  normalizeHorizontalMath() {
    if (this.type.type !== knownTypes.horizontalMath.type) {
      return false;
    }
    let found = false;
    while (this.normalizeHorizontalMathInHorizontalMathOnce() ||
           this.normalizeHorizontalMathAtomNextToAtomOnce()) {
      found = true;
    }
    return found;
  }

  /** @return {boolean} Whether normalization of horizontal math is needed. */
  normalizeHorizontalMathInHorizontalMathOnceAccountOneChild(
      /** @type {Array.<MathNode!>!} */
      normalizedChildren,
      /** @type {MathNode!} */
      current,
  ) {
    if (current.type.type !== knownTypes.horizontalMath.type) {
      normalizedChildren.push(current);
      return false;
    }
    for (let j = 0; j < current.children.length; j++) {
      if (current.desiredCursorPosition === 0 && j === 0) {
        current.children[j].desiredCursorPosition = 0;
      } else if (
          current.desiredCursorPosition > 0 &&
          j === current.children.length - 1) {
        current.children[j].desiredCursorPosition = 1;
      }
      normalizedChildren.push(/** @type {MathNode!}*/ (current.children[j]));
    }
    return true;
  }

  /** @return {boolean} */
  normalizeHorizontalMathInHorizontalMathOnce() {
    let normalizedChildren = [];
    let found = false;
    for (let i = 0; i < this.children.length; i++) {
      if (this.normalizeHorizontalMathInHorizontalMathOnceAccountOneChild(
              normalizedChildren,
              /** @type {MathNode!}*/ (this.children[i]),
              )) {
        found = true;
      }
    }
    if (!found) {
      return false;
    }
    this.replaceChildRangeWithChildren(
        0, this.children.length - 1, normalizedChildren);
    return true;
  }

  mergeAtomContentToTheRight(
      /** @type {MathNode!} */
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
    if (right.desiredCursorPosition !== -1) {
      this.desiredCursorPosition =
          thisContent.length + right.desiredCursorPosition;
    }
    thisContent += rightContent;
    if (this.element === null) {
      this.initialContent = thisContent;
    } else {
      this.element.textContent = thisContent;
    }
  }

  /** @return {boolean} */
  normalizeHorizontalMathAtomNextToAtomOnce() {
    for (let i = 0; i < this.children.length - 1; i++) {
      let current = this.children[i];
      let next = this.children[i + 1];
      if (current.type.type !== knownTypes.atom.type ||
          next.type.type !== knownTypes.atom.type) {
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
      /** @type {Array.<MathNode?>!} */
      newChildren,
  ) {
    this.replaceChildRangeWithChildren(index, index, newChildren);
  }

  replaceChildAtPosition(
      /** @type {number} */
      index,
      /** @type {MathNode?} */
      child,
  ) {
    this.replaceChildRangeWithChildren(index, index, [child]);
  }

  insertChildAtPosition(
      /** @type {number} */
      index,
      /** @type {MathNode?} */
      child,
  ) {
    this.replaceChildRangeWithChildren(index, index - 1, [child]);
  }

  /** @return {MathNode!} */
  removeChildReplaceWithNull(
      /** @type {number} */
      childIndex,
  ) {
    let result = this.children[childIndex];
    result.parent = null;
    result.indexInParent = -1;
    this.children[childIndex] = null;
    return result;
  }

  removeChild(
      /** @type {number} */
      index,
  ) {
    this.replaceChildRangeWithChildren(index, index, []);
  }

  removeChildRange(
      /** @type {number} */
      fromIndex,
      /** @type {number} */
      toIndex,
  ) {
    this.replaceChildRangeWithChildren(fromIndex, toIndex, []);
  }

  replaceChildRangeWithChildren(
      /** @type {number} */
      fromIndex,
      /** @type {number} */
      toIndex,
      /** @type {Array<MathNode?>!} */
      inputChildren,
  ) {
    // Please do not modify removed as removed can
    // be in use as a member of a sub-tree of the inputChildren.
    let toBeShiftedDown = [];
    // Disown all removed children.
    for (let i = fromIndex; i <= toIndex && i >= 0 && i < this.children.length;
         i++) {
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

  /** @return {boolean} */
  isAtomWithLeftmostCursor() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    let offset = window.getSelection().getRangeAt(0).startOffset;
    return offset === 0;
  }

  /** @return {boolean} */
  isAtomWithRightmostCursor() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    let offset = window.getSelection().getRangeAt(0).startOffset;
    return offset === this.element.textContent.length;
  }

  /** @return {KeyHandlerResult!} whether the default should be prevented. */
  deleteButton() {
    if (this.equationEditor.hasSelection()) {
      return this.equationEditor.deleteSelection(null);
    }
    if (this.positionCursorBeforeKeyEvents !==
            this.element.textContent.length ||
        this.type.type !== knownTypes.atom.type) {
      return new KeyHandlerResult(false, true);
    }
    let cousinAtom = this.firstAtom(1);
    if (cousinAtom === null) {
      return new KeyHandlerResult(false, false);
    }
    let cousinLeftSibling = cousinAtom.previousHorizontalSibling();
    if (cousinLeftSibling !== null) {
      if (cousinLeftSibling.implied) {
        cousinAtom.desiredCursorPosition = 0;
        this.parent.focusRestore();
        return new KeyHandlerResult(true, false);
      }
    }
    let rightSibling = this.nextHorizontalSibling();
    if (rightSibling !== null &&
        rightSibling.type.type === knownTypes.baseWithExponent.type) {
      rightSibling.focus(-1);
      return new KeyHandlerResult(true, false);
    }
    this.positionCursorBeforeKeyEvents = -1;
    cousinAtom.positionCursorBeforeKeyEvents = 0;
    return cousinAtom.backspace();
  }

  /** @return {KeyHandlerResult!} whether the default should be prevented. */
  backspace() {
    if (this.equationEditor.hasSelection()) {
      return this.equationEditor.deleteSelection(null);
    }
    if (this.positionCursorBeforeKeyEvents !== 0 ||
        this.type.type !== knownTypes.atom.type) {
      return new KeyHandlerResult(false, true);
    }
    this.desiredCursorPosition = 0;
    let result = this.applyBackspaceToTheLeft();
    return new KeyHandlerResult(result, !result);
  }

  /** @return {boolean} whether reduction occurred. */
  applyBackspaceToTheLeftChildWithSiblingWrapper() {
    let parent = this.parent;
    let parentIndexInParent = parent.indexInParent;
    let base = parent.children[0];
    this.children[0].children[0].desiredCursorPosition = 0;
    base.appendChild(this.children[0]);
    base.normalizeHorizontalMath();
    let parentParent = parent.parent;
    parentParent.replaceChildAtPosition(parentIndexInParent, base);
    parentParent.normalizeHorizontalMath();
    parentParent.updateDOM();
    parentParent.focusRestore();
    return true;
  }

  /** @return {boolean} whether reduction occurred. */
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

  /** @return {boolean} whether backspace was applied */
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

  /** @return {boolean} whether backspace was applied */
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

  /** @return {boolean} */
  applyBackspaceToTheRightAsLeftArrow() {
    let sibling = this.nextHorizontalSibling();
    if (sibling !== null) {
      sibling.focusCancelOnce();
    }
    this.rightmostAtomChild().focus(1);
    return true;
  }

  /** @return {boolean} */
  applyBackspaceToTheLeftAsLeftArrow() {
    let sibling = this.previousHorizontalSibling();
    if (sibling === null) {
      return false;
    }
    sibling.rightmostAtomChild().focus(1);
    return true;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return false;
  }

  /** @return {boolean} */
  applyBackspaceToTheRightDelimiter() {
    if (this.type.type !== knownTypes.rightDelimiter.type &&
        this.type.type !== knownTypes.leftDelimiter.type) {
      return false;
    }
    let matchingIndex =
        this.parent.findIndexMatchingDelimiter(this.indexInParent);
    if (matchingIndex === -1) {
      console.log('Unexpected failure to find matching left parenthesis.');
      return false;
    }
    if (!this.parent.children[matchingIndex].implied) {
      this.implied = true;
      this.children[0].implied = true;
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

  /**
   * Returns the position of the operator.
   *
   * - 1 stands for start ("+11"), 0 for middle ("1+1"), 1 for end ("11+").
   *
   * @return {number}
   */
  computePositionOfOperator() {
    let positionOperator = 1;
    if (this.positionCursorBeforeKeyEvents === 0 &&
        this.element.textContent.length > 0) {
      positionOperator = -1;
    } else if (
        this.positionCursorBeforeKeyEvents > 0 &&
        this.positionCursorBeforeKeyEvents < this.element.textContent.length) {
      positionOperator = 0;
    }
    return positionOperator;
  }

  /** @return {ParentWithIndex!} */
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
    this.makeHorizontalOperator(
        latexConstants.normalizeOperatorToUtf8(contentTransformedToMathSymbol));
  }

  makeHorizontalOperator(
      /** @type {string} */
      key,
  ) {
    let split = this.splitByCursorEmptyAtoms();
    this.makeHorizontalOperatorFromSplit(key, split);
  }

  makeHorizontalOperatorFromSplit(
      /** @type {string} */
      key,
      /** @type {Array.<MathNode!>!} */
      split,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorozintalMathParent()) {
      console.log(
          'Warning: horizontal operator made on element not contained in horizontal math.');
      return;
    }
    let parent = this.parent;
    let operator = mathNodeFactory.atomImmutable(this.equationEditor, key);
    parent.replaceChildAtPositionWithChildren(this.indexInParent, [
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
      /** @type {string} */
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
      /** @type {string} */
      leftDelimiter,
  ) {
    this.makeDelimiterCommon(
        leftDelimiter, latexConstants.leftRightDelimiterPair[leftDelimiter],
        true, false);
  }

  makeDelimiterRight(
      /** @type {string} */
      rightDelimiter,
  ) {
    this.makeDelimiterCommon(
        latexConstants.rightLeftDelimiterPair[rightDelimiter], rightDelimiter,
        false, false);
  }

  makeMatrix(
      /** @type {number} */
      rows,
      /** @type {number} */
      columns,
      /** @type {string}*/
      matrixEnvironment,
  ) {
    let split = this.splitByCursorEmptyAtoms();
    this.makeMatrixFromSplit(rows, columns, split, matrixEnvironment);
  }

  makeMatrixFromSplit(
      /** @type {number} */
      rows,
      /** @type {number} */
      columns,
      /** @type {Array.<MathNode!>!} */
      split,
      /** @type {string}*/
      matrixEnvironment,
  ) {
    // Find closest ancestor node that's of type horizontal math.
    if (!this.hasHorozintalMathParent()) {
      console.log(
          'Warning: horizontal operator made on element not contained in horizontal math.');
      return;
    }
    let parent = this.parent;
    let matrix = mathNodeFactory.matrix(
        this.equationEditor, rows, columns, '', matrixEnvironment);
    parent.replaceChildAtPositionWithChildren(this.indexInParent, [
      split[0],
      matrix,
      split[1],
    ]);
    parent.normalizeHorizontalMath();
    parent.ensureEditableAtoms();
    parent.updateDOM();
    matrix.children[0].children[1].children[0].children[0].focus(0);
  }

  /** Constructs a cancel box from selection. */
  makeCancel() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.makeCancelFromSplit(splitBySelection.split);
      return;
    }
    this.makeCancelFromSplit(this.splitByCursor());
  }

  /** Constructs a sqrt box from selection. */
  makeSqrt() {
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection !== null) {
      this.equationEditor.makeSqrtFromSelection(splitBySelection);
    } else {
      this.makeSqrtFromCursor();
    }
  }

  /** Constructs a sum from selection. */
  makeSum() {
    this.makeOperatorWithSuperscriptAndSubscript(
        latexConstants.operatorWithSuperAndSubscript['sum']);
  }

  /** Constructs an operator with super- and subscript. */
  makeOperatorWithSuperscriptAndSubscript(
      /** @type {string} */
      operator,
  ) {
    let split = this.splitByCursor();
    this.makeOperatorWithSuperscriptAndSubscriptFromSplit(operator, split);
  }

  makeOperatorWithSuperscriptAndSubscriptFromSplit(
      /** @type {string} */
      operator,
      /** @type {Array.<MathNode?>!} */
      split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let operatorNode = mathNodeFactory.operatorWithSuperAndSubscript(
        this.equationEditor, operator, null, null);
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
    let split = this.splitByCursor();
    this.makeOperatorWithSubscriptFromSplit(operator, split);
  }

  makeOperatorWithSubscriptFromSplit(
      /** @type {string} */
      operator,
      /** @type {Array.<MathNode?>!}*/
      split,
  ) {
    let parent = this.parent;
    let oldIndex = this.indexInParent;
    let operatorNode = mathNodeFactory.operatorWithSubscript(
        this.equationEditor, operator, null);
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
      /** @type {Array.<MathNode?>!} */
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
    cancel.children[1].focusStandard(-1);
  }

  /** Makes a sqrt box from the position of the cursor. */
  makeSqrtFromCursor() {
    let split = this.splitByCursor();
    this.makseSqrtFromSplit(split);
  }

  /** Makes a sqrt box from split math nodes. */
  makseSqrtFromSplit(
      /** @type {Array.<MathNode?>!} */
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
      /** @type {string} */
      leftDelimiter,
      /** @type {string} */
      rightDelimiter,
      /** @type {boolean} */
      isLeft,
      /** @type {boolean} */
      isAmbiguous,
  ) {
    let positionOperator = this.computePositionOfOperator();
    // Find closest ancestor node that's of type horizontal math.
    let parentAndIndex = this.findHorizontalMathParent();
    if (parentAndIndex.parent === null) {
      // No ancestor is of type horizontal math.
      console.log('Warning: could not find ancestor of type horizontal math.');
      return;
    }
    let oldIndexInParent = parentAndIndex.indexInParent;
    let parent = parentAndIndex.parent;
    parent.children[oldIndexInParent].doMakeDelimiterCommon(
        leftDelimiter, rightDelimiter, positionOperator, isLeft, isAmbiguous);
  }

  /** @return {number} */
  findIndexToInsertRightDelimiter(
      /** @type {number} */
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

  /** @return {number} */
  findIndexToInsertLeftDelimiter(
      /** @type {number} */
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

  /** @return {MathNode?} */
  findMatchingDelimiter() {
    if (this.type.type !== knownTypes.rightDelimiter.type &&
        this.type.type !== knownTypes.leftDelimiter.type) {
      return null;
    }
    if (!this.hasHorozintalMathParent()) {
      return null;
    }
    let parent = this.parent;
    let index = parent.findIndexMatchingDelimiter(this.indexInParent);
    if (index === -1) {
      return null;
    }
    return parent.children[index];
  }

  /** @return {number}*/
  findIndexMatchingDelimiter(
      /** @type {number} */
      indexStartingDelimiter,
  ) {
    let startingDelimiter = knownTypes.leftDelimiter.type;
    let endingDelimiter = knownTypes.rightDelimiter.type;
    let direction = 1;
    if (this.children[indexStartingDelimiter].type.type ===
        knownTypes.rightDelimiter.type) {
      endingDelimiter = knownTypes.leftDelimiter.type;
      startingDelimiter = knownTypes.rightDelimiter.type;
      direction = -1;
    }
    let openDelimiters = 1;
    for (let i = indexStartingDelimiter + direction;
         i < this.children.length && i >= 0; i += direction) {
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
    return -1;
  }

  /** @return {Array.<MathNode?>!} */
  splitByCursor() {
    return this.splitByPosition(this.positionCursorBeforeKeyEvents);
  }

  /** @return {Array.<MathNode?>!} */
  splitByCursorEmptyAtoms() {
    return this.splitByPositionEmptyAtoms(this.positionCursorBeforeKeyEvents);
  }

  /** @return {Array.<MathNode?>!} */
  splitByPositionEmptyAtoms(
      /** @type {number} */
      position,
  ) {
    let result = this.splitByPosition(position);
    if (result[0] === null) {
      result[0] = mathNodeFactory.atom(this.equationEditor, '');
    }
    if (result[1] === null) {
      result[1] = mathNodeFactory.atom(this.equationEditor, '');
    }
    return result;
  }

  /** @return {Array.<MathNode?>!} */
  splitByPosition(
      /** @type {number} */
      position,
  ) {
    return this.splitByPositionChopOffCharacters(position, 0);
  }

  /** @return {Array.<string>!} */
  splitByPositionIntoStringsChopOffCharacters(
      /** @type {number} */
      position,
      /** @type {number} */
      charactersToRemove,
  ) {
    if (!this.isAtomEditable() || position < 0) {
      if (position <= 0) {
        return ['', ''];
      } else {
        return ['', ''];
      }
    }
    let content = this.contentIfAtom();
    let leftContent = content.slice(0, position);
    let rightContent =
        content.slice(position + charactersToRemove, content.length);
    return [leftContent, rightContent];
  }

  /** @return {Array.<MathNode?>!} */
  splitByPositionChopOffCharacters(
      /** @type {number} */
      position,
      /** @type {number} */
      charactersToRemove,
  ) {
    if (!this.isAtomEditable() || position < 0) {
      if (position <= 0) {
        return [null, this];
      } else {
        return [this, null];
      }
    }
    let splitStrings = this.splitByPositionIntoStringsChopOffCharacters(
        position, charactersToRemove);
    let leftContent = splitStrings[0];
    let rightContent = splitStrings[1];
    let leftNode = mathNodeFactory.atom(this.equationEditor, leftContent);
    let rightNode = mathNodeFactory.atom(this.equationEditor, rightContent);
    if (leftContent === '') {
      return [null, rightNode];
    }
    if (rightContent === '') {
      return [leftNode, null];
    }
    return [leftNode, rightNode];
  }

  doMakeDelimiterCommon(
      /** @type {string} */
      leftDelimiterString,
      /** @type {string} */
      rightDelimiterString,
      /** @type {number} */
      positionOperator,
      /** @type {boolean} */
      isLeft,
      /** @type {boolean} */
      isAmbiguous,
  ) {
    let parent = this.parent;
    if (parent.type.type !== knownTypes.horizontalMath.type) {
      console.log('Warning: making parentheses in non-horizontal math.');
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
      let leftAndRight = this.splitByCursor();
      parent.replaceChildAtPositionWithChildren(
          oldIndexInParent,
          leftAndRight,
      );
    }
    let indexLeftDelimiter = -1;
    let indexRightDelimiter = -1;
    if (isAmbiguous) {
      for (let i = oldIndexInParent; i >= 0; i--) {
        let child = parent.children[i];
        if (child.type.type === knownTypes.leftDelimiter.type &&
            child.initialContent === '|' && !child.implied) {
          isLeft = true;
        }
      }
      if (isLeft) {
        if (oldIndexInParent + 1 < parent.children.length) {
          let child = parent.children[oldIndexInParent + 1];
          if (child.type.type === knownTypes.rightDelimiter.type &&
              child.implied === true) {
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
      if (parent.replaceImpliedLeftDelimiter(
              leftDelimiterString, indexLeftDelimiter)) {
        return;
      }
      indexRightDelimiter =
          parent.findIndexToInsertRightDelimiter(indexLeftDelimiter);
    } else {
      if (positionOperator === 1) {
        indexRightDelimiter = oldIndexInParent + 1;
      } else if (positionOperator === 0) {
        indexRightDelimiter = oldIndexInParent + 1;
      } else {
        indexRightDelimiter = oldIndexInParent;
      }
      if (parent.replaceImpliedRightDelimiter(
              rightDelimiterString, indexRightDelimiter)) {
        return;
      }
      indexLeftDelimiter =
          parent.findIndexToInsertLeftDelimiter(indexRightDelimiter);
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

  /** @return {boolean} */
  replaceImpliedLeftDelimiter(
      /** @type {string} */
      delimiterString,
      /** @type {number} */
      delimiterIndex,
  ) {
    let openDelimiters = 0;
    if (delimiterIndex >= this.children.length) {
      delimiterIndex = this.children.length - 1;
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
          return this.moveDelimiterMarkExplicit(
              delimiterString, delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @return {boolean} */
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
          return this.moveDelimiterMarkExplicit(
              delimiterString, delimiterIndex, i);
        }
        return false;
      }
    }
    return false;
  }

  /** @return {boolean} */
  moveDelimiterMarkExplicit(
      /** @type {string} */
      delimiterString,
      /** @type {number} */
      toIndex,
      /** @type {number} */
      fromIndex,
  ) {
    let isLeft =
        this.children[fromIndex].type.type === knownTypes.leftDelimiter.type;
    this.removeChild(fromIndex);
    if (toIndex > fromIndex) {
      toIndex--;
    }
    let incomingDelimiter = null;
    if (isLeft) {
      incomingDelimiter = mathNodeFactory.leftDelimiter(
          this.equationEditor, delimiterString, false);
    } else {
      incomingDelimiter = mathNodeFactory.rightDelimiter(
          this.equationEditor, delimiterString, false);
    }
    this.insertChildAtPosition(toIndex, incomingDelimiter);
    this.ensureEditableAtoms();
    this.updateDOM();
    incomingDelimiter.focus(1);
    return true;
  }

  /** @return {boolean} */
  hasExponentOrSubscriptParent() {
    if (this.parent === null) {
      return false;
    }
    if (this.indexInParent !== 0) {
      return false;
    }
    let type = this.parent.type.type;
    return type === knownTypes.baseWithExponent.type ||
        type === knownTypes.baseWithSubscript.type;
  }

  /** Ensures editable atoms on the end of a horizontal math. */
  ensureEditableAtoms() {
    if (this.type.type !== knownTypes.horizontalMath.type) {
      console.log(
          'Warning: call ensureEditableAtomToTheRight on non-horizontal math. ');
      return;
    }
    let correctedChildren = [];
    if (this.children.length === 0) {
      correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ''));
    } else if (!this.children[0].isAtomEditable()) {
      if (!this.hasExponentOrSubscriptParent()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ''));
      }
    }
    for (let i = 0; i < this.children.length - 1; i++) {
      let child = this.children[i];
      let next = this.children[i + 1];
      correctedChildren.push(child);
      if (!child.isAtomEditable() && !next.isAtomEditable()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ''));
      }
    }
    correctedChildren.push(this.children[this.children.length - 1]);
    // Editable atom not inserted in the end if the parent is exponent or
    // subscript.
    if (!correctedChildren[correctedChildren.length - 1].isAtomEditable()) {
      if (!this.hasExponentOrSubscriptParent()) {
        correctedChildren.push(mathNodeFactory.atom(this.equationEditor, ''));
      }
    }
    if (correctedChildren.length > this.children.length) {
      this.replaceChildRangeWithChildren(
          0, this.children.length, correctedChildren);
    }
  }

  setTextContent(
      /** @type {string} */
      input,
  ) {
    if (this.element === null) {
      this.initialContent = input;
      return;
    }
    this.element.textContent = input;
  }

  /** @return {BackslashResult!} */
  removeBackslashSequence() {
    let backslashSequence = this.equationEditor.backslashSequence;
    let split = this.splitByPositionIntoStringsChopOffCharacters(
        this.equationEditor.backslashPosition,
        backslashSequence.length,
    );
    this.positionCursorBeforeKeyEvents = this.equationEditor.backslashPosition;
    this.setTextContent(split[0] + split[1]);
    return new BackslashResult(false, backslashSequence);
  }

  /**
   * Constructs a fraction numerator from the default selection or cursor
   * position.
   */
  makeFractionNumerator() {
    if (this.parent === null) {
      return;
    }
    let splitBySelection = this.equationEditor.splitAtomsBySelection();
    if (splitBySelection === null) {
      this.makeFractionNumeratorFromCursorPosition();
    } else {
      this.equationEditor.makeFractionNumeratorFromSelection(splitBySelection);
    }
  }

  /** Constructs a fraction numerator from the cursor position. */
  makeFractionNumeratorFromCursorPosition() {
    const oldParent = this.parent;
    const oldIndexInParent = this.indexInParent;
    let fraction = null;
    let childIndexToFocus = 1;
    if (
      this.positionCursorBeforeKeyEvents === 0 &&
      this.element.textContent.length > 0
    ) {
      childIndexToFocus = 0;
    } 
    let split = this.splitByCursor();

    let previous = this.previousHorizontalSibling();
    let next = this.nextHorizontalSibling();
    let leftWhiteSpace = mathNodeFactory.atom(this.equationEditor, '');
    let rightWhiteSpace = mathNodeFactory.atom(this.equationEditor, '');
    if (
      split[0] === null &&
      previous !== null &&
      previous.type.type === knownTypes.rightDelimiter.type
    ) {
      let rightIndex = oldIndexInParent - 1;
      let leftIndex = oldParent.findIndexMatchingDelimiter(rightIndex);
      let slice = oldParent.children.slice(leftIndex, rightIndex + 1);
      let numerator = mathNodeFactory.horizontalMathFromArray(
        this.equationEditor, slice
      );
      numerator.ensureEditableAtoms();
      numerator.normalizeHorizontalMath();
      fraction = mathNodeFactory.fraction(
        this.equationEditor, numerator, split[1]
      );
      oldParent.replaceChildRangeWithChildren(
        leftIndex, rightIndex + 1, [
        leftWhiteSpace, fraction, rightWhiteSpace
      ]);
      childIndexToFocus = 1;
    } else if (
      split[1] === null && 
      next !== null &&
      next.type.type === knownTypes.leftDelimiter.type
    ) {
      let leftIndex = oldIndexInParent + 1;
      let rightIndex = oldParent.findIndexMatchingDelimiter(leftIndex);
      let slice = oldParent.children.slice(leftIndex, rightIndex + 1);
      let denominator = mathNodeFactory.horizontalMathFromArray(this.equationEditor, slice);
      denominator.ensureEditableAtoms();
      denominator.normalizeHorizontalMath();
      fraction = mathNodeFactory.fraction(
        this.equationEditor, split[0], denominator
      );
      oldParent.replaceChildRangeWithChildren(
        oldIndexInParent, rightIndex, [
        leftWhiteSpace, fraction, rightWhiteSpace
      ]);
      childIndexToFocus = 1;
    } else {
      fraction =
        mathNodeFactory.fraction(this.equationEditor, split[0], split[1]);
      oldParent.replaceChildAtPosition(
        oldIndexInParent, mathNodeFactory.atom(this.equationEditor, ''));
      oldParent.insertChildAtPosition(oldIndexInParent + 1, fraction);
      oldParent.insertChildAtPosition(
        oldIndexInParent + 2, mathNodeFactory.atom(this.equationEditor, ''));
    }
    fraction.parent.updateDOM();
    fraction.children[childIndexToFocus].focus(-1);
  }

  /** Removes all children of a node. */
  removeAllChildren() {
    this.removeChildRange(0, this.children.length - 1);
  }

  /** @return {boolean} */
  isEmptyAtom() {
    if (this.type.type !== knownTypes.atom.type) {
      return false;
    }
    if (this.element === null) {
      return false;
    }
    return this.element.textContent === '';
  }

  /**
   * Returns an immediate horizontal math sibling, provided the node
   * sits in horizontally laid out math and has a previous sibling.
   * @return {MathNode?}
   */
  previousHorizontalSibling() {
    return this.horizontalSibling(-1);
  }

  /**
   * Returns an immediate horizontal math sibling, provided the node
   * sits in horizontally laid out math and has a next sibling.
   * @return {MathNode?}
   */
  nextHorizontalSibling() {
    return this.horizontalSibling(1);
  }

  /** @return {MathNode?} */
  horizontalSibling(
      /** @type {number} */
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

  /** Makes a base with exponent node at the cursor position. */
  makeBaseWithExponent() {
    if (this.parent === null) {
      return;
    }
    if (this.positionCursorBeforeKeyEvents > 0) {
      this.makeBaseDefaultWithExponentNoDelimiter();
      return;
    }
    let previous = this.previousHorizontalSibling();
    if (previous === null) {
      this.makeBaseDefaultWithExponentNoDelimiter();
      return;
    }
    if (previous.type.type === knownTypes.rightDelimiter.type) {
      previous.makeBaseWithExponentRightDelimiter();
      return;
    }
    if (previous.type.type === knownTypes.matrix.type) {
      previous.makeBaseDefaultWithExponent();
      return;
    }
    if (previous.isAtomEditable()) {
      previous.makeBaseDefaultWithExponentNoDelimiter();
      return;
    }
    this.makeBaseDefaultWithExponentNoDelimiter();
  }

  /**
   * Makes a base with exponent node where the base ends on a right delimiter.
   */
  makeBaseWithExponentRightDelimiter() {
    let rightIndex = this.indexInParent;
    let originalParent = this.parent;
    let leftIndex = originalParent.findIndexMatchingDelimiter(rightIndex);
    let baseContent = originalParent.children.slice(leftIndex, rightIndex + 1);
    let base = mathNodeFactory.horizontalMathFromArray(
        this.equationEditor,
        baseContent,
    );
    const baseWithExponent =
        mathNodeFactory.baseWithExponent(this.equationEditor, base, null);
    baseWithExponent.children[1].children[0].children[0].desiredCursorPosition =
        0;
    originalParent.replaceChildRangeWithChildren(
        leftIndex, rightIndex, [baseWithExponent]);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /**
   * Makes a base with exponent ignoring delimiters.
   */
  makeBaseDefaultWithExponentNoDelimiter() {
    if (!this.isAtomEditable()) {
      this.makeBaseDefaultWithExponent();
      return;
    }
    let split = this.splitByCursor();
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    let baseWithExponent = mathNodeFactory.baseWithExponent(
        this.equationEditor, split[0], split[1]);
    if (split[0] === null) {
      baseWithExponent.children[0].appendChild(
          mathNodeFactory.atom(this.equationEditor, ''));
      baseWithExponent.children[0].children[0].desiredCursorPosition = 0;
    } else {
      baseWithExponent.children[1]
          .children[0]
          .children[0]
          .desiredCursorPosition = 0;
    }
    originalParent.replaceChildAtPosition(
        originalIndexInParent, baseWithExponent);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /**
   * Makes a base with exponent, default behavior that ignores all delimiters.
   */
  makeBaseDefaultWithExponent() {
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithExponent =
        mathNodeFactory.baseWithExponent(this.equationEditor, this, null);
    baseWithExponent.children[1].children[0].children[0].desiredCursorPosition =
        0;
    originalParent.replaceChildAtPosition(
        originalIndexInParent, baseWithExponent);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /** Makes a base with subscript. */
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
    if (this.type.type === knownTypes.rightDelimiter.type) {
      this.makeBaseWithSubscriptRightDelimiter();
      return;
    }
    this.makeBaseDefaultWithSubscript();
  }

  /** Makes a base with subscript where the base ends on a right delimiter. */
  makeBaseWithSubscriptRightDelimiter() {
    let rightIndex = this.indexInParent;
    let originalParent = this.parent;
    let leftIndex = originalParent.findIndexMatchingDelimiter(rightIndex);
    let baseContent = originalParent.children.slice(leftIndex, rightIndex + 1);
    let base = mathNodeFactory.horizontalMathFromArray(
        this.equationEditor,
        baseContent,
    );
    const baseWithSubscript =
        mathNodeFactory.baseWithSubscript(this.equationEditor, base, null);
    baseWithSubscript.children[1]
        .children[0]
        .children[0]
        .desiredCursorPosition = 0;
    originalParent.replaceChildRangeWithChildren(
        leftIndex, rightIndex, [baseWithSubscript]);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /** Makes a base with subscript, default behavior. */
  makeBaseDefaultWithSubscript() {
    let originalParent = this.parent;
    let originalIndexInParent = this.indexInParent;
    const baseWithSubscript =
        mathNodeFactory.baseWithSubscript(this.equationEditor, this, null);
    baseWithSubscript.children[1]
        .children[0]
        .children[0]
        .desiredCursorPosition = 0;
    originalParent.replaceChildAtPosition(
        originalIndexInParent, baseWithSubscript);
    originalParent.ensureEditableAtoms();
    originalParent.updateDOM();
    originalParent.focusRestore();
  }

  /** @return {boolean} Focuses the cursor on an atom. */
  focusAtom(
      /** @type {number} */
      endToFocus,
  ) {
    if (this.element === null) {
      this.desiredCursorPosition = -1;
      return false;
    }
    // The call to focus(null) will wipe the desiredCursorPosition.
    let originalDesiredPosition = this.desiredCursorPosition;
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
    this.setCursorPosition(position);
    this.equationEditor.setLastFocused(this);
    this.desiredCursorPosition = -1;
    return true;
  }

  /**
   * Focuses the HTMLElement that belongs to the math node.
   *
   * The endToFocus parameter denotes where the focus should occur.
   * At the moment, negative endToFocus indicates the cursor should
   * be on the element's left, and positive endToFocus indicates the cursor
   * should go on the right. Zero indicates to focus at
   * this.positionCursorBeforeKeyEvents.
   *
   * @return {boolean} Whether the focus was successful.
   */
  focus(
      /** @type {number} */
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

  /**
   * Finds an editable child to the left or right, if possible, and focuses it.
   *
   * @return {boolean}
   */
  focusStandard(
      /** @type {number} */
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
    return sibling.focus(-endToFocus);
  }

  /** @return {boolean} whether focus request was found. */
  focusCancelOnce() {
    if (this.desiredCursorPosition !== -1) {
      this.desiredCursorPosition = -1;
      return true;
    }
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].focusCancelOnce()) {
        return true;
      }
    }
    return false;
  }

  /** @return {boolean} whether focused child was found. */
  focusRestore() {
    if (this.desiredCursorPosition !== -1) {
      this.focus(0);
      return true;
    }
    this.desiredCursorPosition = -1;
    for (let i = 0; i < this.children.length; i++) {
      if (this.children[i].focusRestore()) {
        return true;
      }
    }
    let sibling = null;
    if (this.desiredCursorPosition > 0) {
      sibling = this.firstAtomToTheRight();
    } else if (this.desiredCursorPosition === 0) {
      sibling = this.firstAtomToTheLeft();
    }
    if (sibling !== null) {
      sibling.focus(0);
      return true;
    }
    return false;
  }

  /** Sets the range start to be a given element. */
  setRangeStartEntireElement(
      /** @type {Range!}  */
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

  /** Sets the range end to be a given element. */
  setRangeEndEntireElement(
      /** @type {Range!}  */
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
      /** @type {Range?}  */
      range,
      /** @type {number} */
      position,
  ) {
    if (range !== null && (!this.isAtomEditable() || position < 0)) {
      this.setRangeStartEntireElement(range);
    } else if (this.element.childNodes.length > 0) {
      range.setStart(this.element.childNodes[0], position);
    } else {
      range.setStart(this.element, 0);
    }
  }

  setRangeEnd(
      /** @type {Range!}  */
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

  setCursorPosition(
      /** @type {number} */
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
    // console.log(`Position: ${position}, range ${range}, collapseToStart:
    // ${collapseToStart} start offset: ${range.startOffset}, end offset:
    // ${range.endOffset}, text len: ${this.element.textContent.length}`);
    range.collapse(collapseToStart);
    this.equationEditor.resetSelectionDOM();
    selection.addRange(range);
    this.positionCursorBeforeKeyEvents = position;
    //    this.element.focus();
  }

  /** @return {boolean} */
  isDetached() {
    let ancestor = this;
    while (ancestor.parent !== null) {
      ancestor = ancestor.parent;
    }
    return ancestor.type.type !== knownTypes.root.type;
  }

  /** @return {BoundingBox!} */
  boundingBoxForChildren(
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    let result = new BoundingBox();
    result.left = this.boundingBox.left + boundingBoxFromParent.left;
    result.top = this.boundingBox.top + boundingBoxFromParent.top;
    result.width = this.boundingBox.width;
    result.height = this.boundingBox.height;
    result.fontSizeInPixels =
        this.type.fontSizeRatio * boundingBoxFromParent.fontSizeInPixels;
    return result;
  }

  toScalableVectorGraphicsAtomic(
      /** @type {SVGSVGElement!}*/
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    let text = new ScalableVectorGraphicsTextElement();
    let width = boundingBoxFromParent.left + this.boundingBox.left;
    let height = boundingBoxFromParent.top + this.boundingBox.top;
    let heightFromBox = this.boundingBox.height;
    if (this.type.paddingBottomRatioForSVG !== 1) {
      heightFromBox *= this.type.paddingBottomRatioForSVG;
    }
    height += heightFromBox;
    text.setX(width);
    text.setY(height);
    text.setAlignmentBaseline('text-after-edge');
    text.setFontFamily(this.equationEditor.getFontFamily());
    let fontSize =
        this.type.fontSizeRatio * boundingBoxFromParent.fontSizeInPixels;
    if (fontSize != 0) {
      text.setFontSize(fontSize);
    }
    if (this.type.colorText !== '') {
      text.setFontColor(this.type.colorText);
    }
    let content = this.textContentOrInitialContent();
    let element = document.createTextNode(content);
    text.appendTextChild(element);
    container.appendChild(text.element);
  }

  toScalableVectorGraphicsBase(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    let shiftedBoundingBox = this.boundingBoxForChildren(boundingBoxFromParent);
    for (let i = 0; i < this.children.length; i++) {
      this.children[i].toScalableVectorGraphics(container, shiftedBoundingBox);
    }
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
  }

  /** @return {string} */
  toString() {
    const result = [];
    if (this.isAtomic()) {
      result.push(this.textContentOrInitialContent());
    }
    result.push(`[${this.type.type}]`);
    if (this.children.length > 0) {
      result.push('{');
      for (let i = 0; i < this.children.length; i++) {
        if (this.children[i] === null) {
          result.push('null');
          continue;
        }
        result.push(this.children[i].toString());
        if (i !== this.children.length - 1) {
          result.push(',');
        }
      }
      result.push('}');
    }
    return result.join('');
  }

  /** @return {Array.<HTMLElement!>!} */
  toHtmlDebugData(
      /** @type {number} */
      indentationLevel,
  ) {
    if (indentationLevel === null || indentationLevel === undefined) {
      indentationLevel = 0;
    }
    const indentation = '-'.repeat(indentationLevel);
    const result = [];
    let content = `${indentation}${this.type.type}[${this.constructor.name}]`;
    if (this.element !== null) {
      content += `[${this.element.textContent}]`;
    }
    if (this.focused) {
      content += ', F';
    }
    if (this.desiredCursorPosition !== -1) {
      content += `, FD[${this.desiredCursorPosition}]`;
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
      content += `, cursor: ${this.positionCursorBeforeKeyEvents}, `;
    }
    result.push(document.createTextNode(content));
    for (let i = 0; i < this.children.length; i++) {
      let childrenElements =
          this.children[i].toHtmlDebugData(indentationLevel + 1);
      for (let j = 0; j < childrenElements.length; j++) {
        result.push(childrenElements[j]);
      }
    }
    return result;
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotationAtomic(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let positionCursor = -1;
    if (options !== null && options.useCursorCommand) {
      positionCursor = this.desiredCursorPosition;
    }
    let content = this.contentIfAtomic();
    if (this.type.type === knownTypes.atomImmutable.type) {
      if (content in latexConstants.latexBackslashOperators) {
        return new LatexWithAnnotation(`\\${content} `);
      }
    }
    let result = latexConstants.convertUtf16ToLatex(
        content,
        positionCursor,
    );
    return result;
  }

  /** @return {string} */
  toLatex() {
    return this.toLatexWithAnnotation(null).latex;
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let toJoin = [];
    let result = new LatexWithAnnotation('');
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      if (child === null) {
        toJoin.push('[[null]]');
        continue;
      }
      let childLatex = child.toLatexWithAnnotation(options);
      toJoin.push(childLatex.latex);
    }
    result.latex = toJoin.join('');
    return result;
  }

  /** @return {MathNode?} */
  getMatrixCell(
      /** @type {number} */
      rowIndex,
      /** @type {number} */
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

  /** @return {number} */
  matrixColumnCount() {
    if (this.type.type !== knownTypes.matrix.type) {
      return -1;
    }
    let matrixTable = this.children[0].children[1];
    let columnCount = 0;
    let numberOfRows = matrixTable.children.length;
    for (let i = 0; i < numberOfRows; i++) {
      columnCount =
          Math.max(columnCount, matrixTable.children[i].children.length);
    }
    return columnCount;
  }
}

class ScalableVectorGraphicsBase {
  constructor(
      /** @type {SVGSVGElement!} */
      element,
  ) {
    /** @type {SVGSVGElement!} */
    this.element = element;
  }
  appendChild(
      /** @type {SVGSVGElement!} */
      input,
  ) {
    this.element.appendChild(input);
  }
  appendTextChild(
      /** @type {Text!} */
      input,
  ) {
    this.element.appendChild(input);
  }
}

class ScalableVectorGraphicsElementGeneral extends ScalableVectorGraphicsBase {
  constructor() {
    super(/** @type {SVGSVGElement!} */ (
        document.createElementNS('http://www.w3.org/2000/svg', 'svg')));
  }
  setWidth(
      /** @type {number} */
      width,
  ) {
    this.element.setAttribute('width', width);
  }
  setHeight(
      /** @type {number} */
      height,
  ) {
    this.element.setAttribute('height', height);
  }
}

class ScalableVectorGraphicsTextElement extends ScalableVectorGraphicsBase {
  constructor() {
    super(/** @type {SVGSVGElement!} */ (
        document.createElementNS('http://www.w3.org/2000/svg', 'text')));
  }
  setX(
      /** @type {number} */ x,
  ) {
    this.element.setAttribute('x', x);
  }
  setY(
      /** @type {number} */ y,
  ) {
    this.element.setAttribute('y', y);
  }
  setAlignmentBaseline(
      /** @type {string} */ input,
  ) {
    this.element.setAttribute('alignment-baseline', input);
  }
  setFontFamily(
      /** @type {string} */
      input,
  ) {
    this.element.setAttribute('font-family', input);
  }
  setFontSize(
      /** @type {number} */
      fontSize,
  ) {
    this.element.setAttribute('font-size', fontSize);
  }
  setFontColor(
      /** @type {string} */
      color,
  ) {
    this.element.setAttribute('font-color', color);
  }
}

class ScalableVectorGraphicsLine extends ScalableVectorGraphicsBase {
  constructor() {
    super(/** @type {SVGSVGElement!} */ (
        document.createElementNS('http://www.w3.org/2000/svg', 'line')));
  }
  setX1(
      /** @type {number} */
      x1,
  ) {
    this.element.setAttribute('x1', x1);
  }
  setY1(
      /** @type {number} */
      y1,
  ) {
    this.element.setAttribute('y1', y1);
  }
  setX2(
      /** @type {number} */
      x2,
  ) {
    this.element.setAttribute('x2', x2);
  }
  setY2(
      /** @type {number} */
      y2,
  ) {
    this.element.setAttribute('y2', y2);
  }

  setStroke(/** @type {string} */ color) {
    this.element.setAttribute('stroke', color);
  }
}

class ScalableVectorGraphicsPath extends ScalableVectorGraphicsBase {
  constructor() {
    super(/** @type {SVGSVGElement!} */ (
        document.createElementNS('http://www.w3.org/2000/svg', 'path')));
  }
  setPathString(
      /** @type {string} */
      path,
  ) {
    this.element.setAttribute('d', path);
  }
  setStrokeColor(/** @type {string} */ color) {
    this.element.setAttribute('stroke', color);
  }
  setFillColor(/** @type {string} */ color) {
    this.element.setAttribute('fill', color);
  }
}

class ScalableVectorGraphicsWriter {
  constructor() {
    /** @type {ScalableVectorGraphicsElementGeneral?} */
    this.underConstruction = null;
  }

  /** @return {SVGSVGElement!} */
  typeset(
      /** @type {MathNode!} */
      input,
  ) {
    this.underConstruction = new ScalableVectorGraphicsElementGeneral();
    input.toScalableVectorGraphics(
        this.underConstruction.element, new BoundingBox());
    this.underConstruction.setWidth(input.boundingBox.width);
    this.underConstruction.setHeight(input.boundingBox.height);
    return this.underConstruction.element;
  }
}

class MathNodeAtom extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.atom);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    return this.toLatexWithAnnotationAtomic(options);
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsAtomic(container, boundingBoxFromParent);
  }
}

class MathNodeAtomImmutable extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.atomImmutable);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    return this.toLatexWithAnnotationAtomic(options);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.children[this.indexInParent + 1].desiredCursorPosition = 0;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsAtomic(container, boundingBoxFromParent);
  }
}

class MathNodeFraction extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.fraction);
    this.extraSpaceBetweenNumeratorAndDenominator = 3;
    this.extraWidth = 4;
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    let numerator = this.children[0];
    let denominator = this.children[1];
    this.boundingBox.fractionLineHeight = numerator.boundingBox.height + 1;
    this.boundingBox.height = numerator.boundingBox.height +
        denominator.boundingBox.height +
        this.extraSpaceBetweenNumeratorAndDenominator;
    this.boundingBox.width =
        Math.max(numerator.boundingBox.width, denominator.boundingBox.width) +
        this.extraWidth;
    this.boundingBox.needsMiddleAlignment = true;
    numerator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.width = this.boundingBox.width;
    denominator.boundingBox.top = numerator.boundingBox.height +
        this.extraSpaceBetweenNumeratorAndDenominator;
    numerator.computeBoundingBoxLeftSingleChild();
    denominator.computeBoundingBoxLeftSingleChild();
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsLine();
    result.setX1(boundingBoxFromParent.left + this.boundingBox.left);
    result.setX2(
        boundingBoxFromParent.left + this.boundingBox.left +
        this.boundingBox.width + this.extraWidth / 2);
    result.setY1(
        boundingBoxFromParent.top + this.boundingBox.top +
        this.boundingBox.fractionLineHeight);
    result.setY2(
        boundingBoxFromParent.top + this.boundingBox.top +
        this.boundingBox.fractionLineHeight);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStroke(color);
    container.appendChild(result.element);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let numerator = this.children[0].toLatexWithAnnotation(options);
    let denominator = this.children[1].toLatexWithAnnotation(options);
    let result = new LatexWithAnnotation(
        `\\frac{${numerator.latex}}{${denominator.latex}}`);
    if (this.children.length <= 2) {
      return result;
    }
    // This is not expected to happen: a fraction should have exactly two
    // children. console.log("Warning: corrupt fraction.");
    result.latex += '[';
    for (let i = 2; i < this.children.length; i++) {
      result.latex += this.children[i].toLatex();
    }
    result.latex += ']';
    return result;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }
}

class MathNodeNumerator extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.numerator);
  }
  /** @return {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftWrapperChildWithSiblingWrapper();
  }
}

class MathNodeDenominator extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.denominator);
  }
  /** @return {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftWrapperChildWithSiblingWrapper();
  }
}

class MathNodeBaseWithExponent extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithExponent);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
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
      return new LatexWithAnnotation(
          `{${baseLatex.latex}}^{${exponentLatex.latex}}`);
    } else {
      return new LatexWithAnnotation(
          `${baseLatex.latex}^{${exponentLatex.latex}}`);
    }
  }

  /**
   * Applies backspace to the right.
   *
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftAsLeftArrow();
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    let base = this.children[0];
    let exponent = this.children[1];
    let overlapRatio = 0.35;
    if (base.requiresTallExponent()) {
      overlapRatio = 0.1;
    }
    let overlap = base.boundingBox.height * overlapRatio;
    this.boundingBox.height =
        exponent.boundingBox.height + base.boundingBox.height - overlap;
    if (exponent.boundingBox.height > this.boundingBox.height) {
      this.boundingBox.height = exponent.boundingBox.height;
    }
    base.boundingBox.top = exponent.boundingBox.height - overlap;
    if (base.boundingBox.top < 0) {
      // The exponent is so small relative to the base that it
      // fits entirely in the overlap between the base and the exponent.
      exponent.boundingBox.top = -base.boundingBox.top;
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
      this.boundingBox.width =
          base.boundingBox.width + exponent.boundingBox.width;
      this.boundingBox.superScriptWidth = exponent.boundingBox.width;
      exponent.boundingBox.left = base.boundingBox.width;
    } else {
      this.boundingBox.width = Math.max(
          baseWithSubscript.boundingBox.width + exponent.boundingBox.width -
              baseWithSubscript.boundingBox.subScriptWidth,
          baseWithSubscript.boundingBox.width,
      );
      this.boundingBox.superScriptWidth = 0;
      this.boundingBox.subScriptWidth = 0;
      exponent.boundingBox.left =
          base.boundingBox.width - baseWithSubscript.boundingBox.subScriptWidth;
    }
    this.boundingBox.fractionLineHeight =
        base.boundingBox.top + base.boundingBox.fractionLineHeight;
    this.computeMiddleAlignment();
  }

  /** @return {boolean} */
  requiresTallExponent() {
    return this.children[0].requiresTallExponent();
  }
}

class MathNodeHorizontalMath extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.horizontalMath);
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    let boundingBox = this.boundingBoxForChildren(boundingBoxFromParent);
    for (let i = 0; i < this.children.length; i++) {
      let child = this.children[i];
      child.toScalableVectorGraphics(container, boundingBox);
    }
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsHorizontalMath();
  }

  /**
   * Same as computeDimensions but easier to code search.
   */
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

  /** Computes bonding box data due to line breaks. */
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
      /** @type {number} */
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
      this.boundingBox.lineHeight = -1;
    }
  }

  /** @return {boolean} */
  requiresTallExponent() {
    for (let i = this.children.length - 1; i >= 0; i--) {
      let child = this.children[i];
      let childType = child.type.type;
      if (childType === knownTypes.matrix.type) {
        return true;
      }
      if (childType === knownTypes.rightDelimiter.type) {
        return this.boundingBox.needsMiddleAlignment;
      }
      if (childType === knownTypes.baseWithSubscript.type ||
          childType === knownTypes.baseWithExponent.type) {
        return child.requiresTallExponent();
      }
      if (childType === knownTypes.atom.type) {
        if (child.textContentOrInitialContent() === '') {
          continue;
        }
      }
      return false;
    }
    return false;
  }
}

class MathNodeGenericBox extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.genericMathBox);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let childLatex = this.children[0].toLatexWithAnnotation(options);
    let color = this.type.colorText;
    if (color === '') {
      return new LatexWithAnnotation(`{${childLatex.latex}}`);
    }
    return new LatexWithAnnotation(`{\\color{${color}}{${childLatex.latex}}}`);
  }
}

class MathNodeRoot extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.root);
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsStandard();
    this.boundingBox.lineHeight = this.children[0].boundingBox.lineHeight;
    if (!this.boundingBox.needsMiddleAlignment ||
        this.equationEditor.options.editable) {
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
      this.boundingBox.top +=
          bottomDistance - this.boundingBox.fractionLineHeight;
      this.boundingBox.fractionLineHeight = bottomDistance;
    } else {
      this.boundingBox.height +=
          this.boundingBox.fractionLineHeight * 2 - lineHeight;
    }
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    let boundingBox = new BoundingBox();
    boundingBox.top = boundingBoxFromParent.top;
    boundingBox.left = boundingBoxFromParent.left;
    let fontSizeString = window.getComputedStyle(this.element, null)
                             .getPropertyValue('font-size');
    boundingBox.fontSizeInPixels = parseInt(fontSizeString, 10);
    this.children[0].toScalableVectorGraphics(container, boundingBox);
  }
}

class MathNodeError extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.error);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.children[this.indexInParent + 1].desiredCursorPosition = 0;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeCancel extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.cancel);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let childLatex = this.children[1].toLatexWithAnnotation(options);
    return new LatexWithAnnotation(`\\cancel{${childLatex.latex}}`);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    let content = this.children[1];
    content.boundingBox.left = 0;
    this.boundingBox.height = content.boundingBox.height;
    this.boundingBox.width = content.boundingBox.width;
    this.boundingBox.fractionLineHeight =
        content.boundingBox.fractionLineHeight;
    let cancelSign = this.children[0];
    cancelSign.boundingBox.width = 4;
    cancelSign.boundingBox.height = content.boundingBox.height;
    cancelSign.boundingBox.left = 0;
    if (cancelSign.element !== null) {
      cancelSign.element.style.width = cancelSign.boundingBox.width;
      cancelSign.element.style.height = cancelSign.boundingBox.height;
    }
    let stretch = content.boundingBox.width / cancelSign.boundingBox.height;
    cancelSign.boundingBox.transformOrigin = 'top left';
    cancelSign.boundingBox.transform =
        `matrix(1,0,${- stretch},1,${content.boundingBox.width},0)`;
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let leftX = boundingBoxFromParent.left + this.boundingBox.left;
    let leftY = boundingBoxFromParent.top + this.boundingBox.top +
        this.boundingBox.height;
    let rightX = boundingBoxFromParent.left + this.boundingBox.left +
        this.boundingBox.width;
    let rightY = boundingBoxFromParent.top + this.boundingBox.top;

    let result = new ScalableVectorGraphicsLine();
    result.setX1(leftX);
    result.setY1(leftY);
    result.setX2(rightX);
    result.setY2(rightY);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStroke(color);
    container.appendChild(result.element);
  }
}

class MathNodeCancelUnderBox extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.cancelUnderBox);
  }
  /** @return {boolean} whether reduction occurred. */
  applyBackspaceToTheLeft() {
    let cancel = this.parent;
    let indexCancel = cancel.indexInParent;
    let content = this.children[0];
    content.children[0].desiredCursorPosition = 0;
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
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrtSign);
  }

  verticallyStretchSqrt(
      /** @type {number} */
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

    left.boundingBox.transformOrigin = 'top left';
    right.boundingBox.transformOrigin = 'top left';
    let leftSkew = left.boundingBox.width / left.boundingBox.height;
    left.boundingBox.transform = `matrix(1,0,${leftSkew},1,0,0)`;
    let rightSkew = preferredWidth / right.boundingBox.height;
    right.boundingBox.transform =
        `matrix(1,0,${- rightSkew},1,${preferredWidth},0)`;
    this.boundingBox.height = heightToEnclose;
    this.boundingBox.width =
        left.boundingBox.width + preferredWidth + decoration.boundingBox.width;
  }
}

class MathNodeSqrt extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.sqrt);
    this.sqrtProportionOverlapped = 0.7;
    this.widthSqrtSign = 0.99;
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    if (this.element === null) {
      return new LatexWithAnnotation('[null(]');
    }
    let result = new LatexWithAnnotation('');
    let exponent = null;
    let underTheRadical = null;
    if (this.children.length > 0) {
      exponent = this.children[0].toLatexWithAnnotation(options);
    }
    if (this.children.length > 2) {
      underTheRadical = this.children[2].toLatexWithAnnotation(options);
    }
    if (exponent !== null) {
      if (exponent.latex !== '') {
        result.latex = `\\sqrt[${exponent.latex}]{${underTheRadical.latex}}`;
        return result;
      }
    }
    result.latex = `\\sqrt{${underTheRadical.latex}}`;
    return result;
  }

  /** @return {number} */
  sqrtSignLeftShift() {
    let radicalExponentBox = this.children[0];
    let sqrtSign = this.children[1];
    return Math.max(
        0,
        radicalExponentBox.boundingBox.width -
            sqrtSign.boundingBox.width * this.sqrtProportionOverlapped);
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    let radicalExponentBox = this.children[0];
    let sqrtSign = /** @type {MathNodeSqrtSign!} */ (this.children[1]);
    let underTheRadical = this.children[2];
    // The proportion of the width of the sqrt sign that overlaps with the
    // radical exponent. The top of the sqrt sign may not connect perfectly with
    // the overline of the under-the-radical content. The following variable
    // compensates for that.
    sqrtSign.verticallyStretchSqrt(underTheRadical.boundingBox.height);
    radicalExponentBox.boundingBox.top =
        -0.15 * radicalExponentBox.boundingBox.height;
    sqrtSign.boundingBox.left = this.sqrtSignLeftShift();
    underTheRadical.boundingBox.left = sqrtSign.boundingBox.left +
        sqrtSign.boundingBox.width * this.widthSqrtSign;
    this.boundingBox = new BoundingBox();
    this.boundingBox.height = underTheRadical.boundingBox.height * 1.15;
    this.boundingBox.fractionLineHeight =
        underTheRadical.boundingBox.fractionLineHeight + 2.2;
    let extraSpaceAfterRadical = 4;
    this.boundingBox.width = underTheRadical.boundingBox.left +
        underTheRadical.boundingBox.width + extraSpaceAfterRadical;
    this.boundingBox.needsMiddleAlignment =
        underTheRadical.boundingBox.needsMiddleAlignment;
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsPath();
    let sqrtSign = this.children[1];
    let decoration = sqrtSign.children[0];
    let leftStroke = sqrtSign.children[1];
    let rightStroke = sqrtSign.children[2];
    let overline = this.children[2];
    let sqrtTop = boundingBoxFromParent.top + this.boundingBox.top +
        sqrtSign.boundingBox.top;
    let sqrtLeft = boundingBoxFromParent.left + this.boundingBox.left +
        sqrtSign.boundingBox.left;
    let bottom =
        sqrtTop + leftStroke.boundingBox.top + leftStroke.boundingBox.height;
    let midX = sqrtLeft + rightStroke.boundingBox.left;
    let decorationY = sqrtTop + decoration.boundingBox.top;
    let decorationLeft = sqrtLeft + decoration.boundingBox.left;
    let decorationRight = decorationLeft + decoration.boundingBox.width;
    let topY = sqrtTop + rightStroke.boundingBox.top;
    let rightX = boundingBoxFromParent.left + this.boundingBox.left +
        overline.boundingBox.left;
    let overlineRight = boundingBoxFromParent.left + this.boundingBox.left +
        overline.boundingBox.left + overline.boundingBox.width;
    let path = `M ${decorationLeft} ${decorationY} L ${decorationRight} ${
        decorationY} `;
    path += `L ${decorationRight} ${decorationY} `;
    path += `L ${midX} ${bottom} `;
    path += `L ${rightX} ${topY} `;
    path += `L ${overlineRight} ${topY} `;
    result.setPathString(path);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStrokeColor(color);
    result.setFillColor('none');
    container.appendChild(result.element);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }
}

class MathNodeOverLine extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.overLinedBox);
  }
}

class MathNodeFormInput extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      value,
      /** @type {string} */
      name,
  ) {
    super(equationEditor, knownTypes.formInput);
    this.value = value;
    this.name = name;
    this.initialContent = this.value;
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsStandard();
    this.boundingBox.height = this.equationEditor.standardAtomHeight;
    if (this.boundingBox.height === 0) {
      this.boundingBox.height = 20;
    }
    this.boundingBox.width = this.value.length * this.boundingBox.height / 2;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }
}

class MathNodeOverBrace extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.overBrace);
  }
}

class MathNodeExponent extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.exponent);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftChildWithSiblingWrapper();
  }
}

class MathNodeSubscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.subscript);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    return this.applyBackspaceToTheLeftChildWithSiblingWrapper();
  }
}

class MathNodeBaseWithSubscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.baseWithSubscript);
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsBaseWithSubscript();
  }

  /** Same as computeDimensions but easier to code-search. */
  computeDimensionsBaseWithSubscript() {
    let base = this.children[0];
    let subscript = this.children[1];
    let overlapRatio = 0.35;
    if (base.requiresTallExponent()) {
      overlapRatio = 0.1;
    }
    let overlap = base.boundingBox.height * overlapRatio;
    this.boundingBox.height =
        subscript.boundingBox.height + base.boundingBox.height - overlap;
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
    this.boundingBox.width =
        base.boundingBox.width + subscript.boundingBox.width;
    subscript.boundingBox.top = base.boundingBox.height * (1 - overlapRatio);
    if (baseWithExponent === null) {
      subscript.boundingBox.left = base.boundingBox.width;
      this.boundingBox.subScriptWidth = subscript.boundingBox.width;
    } else {
      subscript.boundingBox.left = base.boundingBox.width -
          baseWithExponent.boundingBox.superScriptWidth;
      this.boundingBox.superScriptWidth = 0;
      this.boundingBox.subScriptWidth = 0;
      this.boundingBox.width = Math.max(
          baseWithExponent.boundingBox.width + subscript.boundingBox.width -
              baseWithExponent.boundingBox.superScriptWidth,
          baseWithExponent.boundingBox.width,
      );
    }
    this.boundingBox.fractionLineHeight = base.boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let result = null;
    let numerator = this.children[0].toLatexWithAnnotation(options);
    let denominator = this.children[1].toLatexWithAnnotation(options);
    if (this.children[0].isAtomicNonEmptyOrHorizontalMathWithNonEmptyAtomic()) {
      result =
          new LatexWithAnnotation(`${numerator.latex}_{${denominator.latex}}`);
    } else {
      result = new LatexWithAnnotation(
          `{${numerator.latex}}_{${denominator.latex}}`);
    }
    return result;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /** @return {boolean} */
  requiresTallExponent() {
    return this.children[0].requiresTallExponent();
  }
}

class MathNodeLeftDelimiter extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.leftDelimiter);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    /** @type {LatexWithAnnotation?} */
    let result = null;
    if (this.children.length === 0) {
      result = new LatexWithAnnotation('\\left.');
    } else if (this.extraData === '{') {
      result = new LatexWithAnnotation('\\{');
    } else {
      result = new LatexWithAnnotation(/** @type {string} */ (this.extraData));
    }
    return result;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightDelimiter();
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsDelimiter();
  }
}

class MathNodeRightDelimiter extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.rightDelimiter);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    /** @type {LatexWithAnnotation?} */
    let result = null;
    if (this.children.length === 0) {
      result = new LatexWithAnnotation('\\right.');
    } else if (this.extraData === '{') {
      result = new LatexWithAnnotation('\\}');
    } else {
      result = new LatexWithAnnotation(/** @type {string} */ (this.extraData));
    }
    return result;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightDelimiter();
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsDelimiter();
  }
}

class MathNodeDelimiterMark extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(
        equationEditor,
        left ? knownTypes.leftDelimiterMark : knownTypes.rightDelimiterMark);
    this.parenthesisThickness = 0;
  }

  verticallyStretchCommon(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
      fractionLineHeightEnclosed,
  ) {
    let scaleHeight = 1.1;
    this.parenthesisThickness = heightToEnclose / 24;
    this.parenthesisThickness = Math.min(3, this.parenthesisThickness);
    this.parenthesisThickness = Math.max(1.5, this.parenthesisThickness);
    heightToEnclose = Math.max(
        2 * fractionLineHeightEnclosed,
        2 * (heightToEnclose - fractionLineHeightEnclosed));
    this.boundingBox.top = 0;
    this.boundingBox.height = heightToEnclose * scaleHeight;
    this.boundingBox.fractionLineHeight = this.boundingBox.height / 2;
  }
}

class MathNodeAbsoluteValue extends MathNodeDelimiterMark {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
      fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    this.boundingBox.width = Math.max(heightToEnclose / 6, 3);
    this.element.style.borderWidth = `${this.parenthesisThickness}px`;
  }
}

class MathNodeSquareBrackets extends MathNodeDelimiterMark {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
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
      this.boundingBox.left = -shift;
    }
    if (this.element !== null) {
      this.element.style.borderTop = `solid`;
      this.element.style.borderBottom = `solid`;
      this.element.style.borderWidth = `${this.parenthesisThickness}px`;
    }
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsPath();
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

    let command = `M ${xStart} ${yLow} L ${xMiddle} ${yLow} L ${xMiddle} ${
        yHigh} L ${xStart} ${yHigh}`;  // move to point.
    result.setPathString(command);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStrokeColor(color);
    result.setFillColor('none');
    container.appendChild(result.element);
  }
}

class MathNodeAngleBrackets extends MathNodeDelimiterMark {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
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

    topBar.boundingBox.transformOrigin = 'top left';
    bottomBar.boundingBox.transformOrigin = 'top left';
    let scale = width / halfHeight;

    if (this.left) {
      topBar.boundingBox.transform = `matrix(1,0,${- scale},1,${width},0)`;
      bottomBar.boundingBox.transform = `matrix(1,0,${scale},1,0,0)`;
    } else {
      topBar.boundingBox.transform = `matrix(1,0,${scale},1,0,0)`;
      bottomBar.boundingBox.transform = `matrix(1,0,${- scale},1,${width},0)`;
    }
    this.element.style.borderLeft = '';
    this.element.style.borderRight = '';
    this.boundingBox.width =
        Math.max(width + 2 * margin + 2 * this.parenthesisThickness, 3);
    this.element.style.borderWidth = `${this.parenthesisThickness}px`;
  }
}

class MathNodeParenthesis extends MathNodeDelimiterMark {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  verticallyStretch(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
      fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    this.boundingBox.width = Math.min(this.boundingBox.height / 4, 20);
    this.element.style.borderLeft = '';
    this.element.style.borderRight = '';
    let scale = this.scale();

    let radius = this.boundingBox.height / 2;
    let child = this.children[0];
    child.element.style.borderRadius = `${radius}px`;
    child.element.style.borderWidth = `${this.borderWidth()}px`;
    child.boundingBox.width = this.boundingBox.height;
    child.boundingBox.height = this.boundingBox.height;
    child.boundingBox.transformOrigin = 'top left';
    child.boundingBox.left += this.horizontalShift();
    child.boundingBox.transform = `matrix(${scale},0,0,1,0,0)`;
  }

  /** @return {number} */
  scale() {
    return this.boundingBox.width / this.boundingBox.height;
  }

  /** @return {number} */
  borderWidth() {
    return this.parenthesisThickness / this.scale();
  }

  /** @return {number} */
  horizontalShift() {
    let shift = this.boundingBox.width / 3.5;
    if (this.left) {
      return shift;
    } else {
      return -shift;
    }
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsParenthesis(container, boundingBoxFromParent);
  }

  // Same as toScalableVectorGraphics but easier to code search.
  toScalableVectorGraphicsParenthesis(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsPath();
    let extraShift = this.parenthesisThickness + 1;
    if (!this.left) {
      extraShift *= -1;
    }
    let x = boundingBoxFromParent.left + this.boundingBox.left +
        this.horizontalShift() + extraShift;
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

    let moveBottom = `M ${x} ${startY}`;  // move to point.
    let arcOuter = `A ${rx} ${ry} ${degrees} 1 1 ${x} ${endY}`;
    let arcInner = `A ${rxInner} ${ry} ${degrees} 0 0 ${x} ${startY}`;
    let command = `${moveBottom} ${arcOuter} ${arcInner}`;
    result.setPathString(command);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStrokeColor('none');
    result.setFillColor(color);
    container.appendChild(result.element);
  }
}

class MathNodeCurlyBrace extends MathNodeDelimiterMark {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {boolean} */
      left,
  ) {
    super(equationEditor, left);
    this.left = left;
  }

  /** @return {number} */
  radius() {
    let radius = Math.floor(this.boundingBox.height / 12);
    if (radius < 2) {
      radius = 2;
    }
    return radius;
  }

  verticallyStretch(
      /** @type {number} */
      heightToEnclose,
      /** @type {number} */
      fractionLineHeightEnclosed,
  ) {
    this.verticallyStretchCommon(heightToEnclose, fractionLineHeightEnclosed);
    let topBar = this.children[1];
    let bottomBar = this.children[4];
    let radius = this.radius();
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
    let allQuarterCircles = [
      topQuarterCircle, topMiddleQuarterCircle, bottomMiddleQuarterCircle,
      bottomQuarterCircle
    ];
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
    bottomMiddleQuarterCircle.boundingBox.left = this.left ? 0 : radius;

    bottomQuarterCircle.boundingBox.top = 2 * heightBar + 3 * radius;
    bottomQuarterCircle.boundingBox.left = this.left ? radius : 0;
    if (this.element !== null) {
      this.element.style.borderLeft = '';
      this.element.style.borderRight = '';
    }
    let borderStyleString = `solid ${borderWidthString}`;
    if (this.left) {
      topQuarterCircle.element.style.borderTopLeftRadius = radiusString;
      topQuarterCircle.element.style.borderLeft = borderStyleString;

      topMiddleQuarterCircle.element.style.borderBottomRightRadius =
          radiusString;
      topMiddleQuarterCircle.element.style.borderRight = borderStyleString;

      bottomMiddleQuarterCircle.element.style.borderTopRightRadius =
          radiusString;
      bottomMiddleQuarterCircle.element.style.borderRight = borderStyleString;

      bottomQuarterCircle.element.style.borderBottomLeftRadius = radiusString;
      bottomQuarterCircle.element.style.borderLeft = borderStyleString;
    } else {
      topQuarterCircle.element.style.borderTopRightRadius = radiusString;
      topQuarterCircle.element.style.borderRight = borderStyleString;

      topMiddleQuarterCircle.element.style.borderBottomLeftRadius =
          radiusString;
      topMiddleQuarterCircle.element.style.borderLeft = borderStyleString;

      bottomMiddleQuarterCircle.element.style.borderTopLeftRadius =
          radiusString;
      bottomMiddleQuarterCircle.element.style.borderLeft = borderStyleString;

      bottomQuarterCircle.element.style.borderBottomRightRadius = radiusString;
      bottomQuarterCircle.element.style.borderRight = borderStyleString;
    }
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsPath();
    let radius = this.radius();
    let leftShift = boundingBoxFromParent.left + this.boundingBox.left;
    let xMiddle = leftShift + radius;
    let xStickyPart = leftShift;
    let xEnd = leftShift;
    if (this.left) {
      xEnd += radius * 2;
    } else {
      xStickyPart += radius * 2;
    }
    let yHigh = boundingBoxFromParent.top + this.boundingBox.top;
    let yLow = yHigh + this.boundingBox.height;
    let yMiddle = (yHigh + yLow) / 2;
    let arcChoiceBottom = 0;
    let arcChoiceOther = 1;
    if (this.left) {
      arcChoiceBottom = 1;
      arcChoiceOther = 0;
    }

    let path = `M ${xEnd} ${yLow} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceBottom} ${xMiddle} ${
        yLow - radius} `;
    path += `L ${xMiddle} ${yMiddle + radius} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceOther} ${xStickyPart} ${
        yMiddle} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceOther} ${xMiddle} ${
        yMiddle - radius} `;
    path += `L ${xMiddle} ${yHigh + radius} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceBottom} ${xEnd} ${yHigh} `;

    let thicknessShift = this.parenthesisThickness;
    if (!this.left) {
      thicknessShift *= -1;
    }
    path += `A ${radius} ${radius} 0 0 ${arcChoiceOther} ${
        xMiddle + thicknessShift} ${yHigh + radius} `;
    path += `L ${xMiddle + thicknessShift} ${yMiddle - radius} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceBottom} ${
        xStickyPart + thicknessShift} ${yMiddle} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceBottom} ${
        xMiddle + thicknessShift} ${yMiddle + radius} `;
    path += `L ${xMiddle + thicknessShift} ${yLow - radius} `;
    path += `A ${radius} ${radius} 0 0 ${arcChoiceOther} ${xEnd} ${yLow} `;

    result.setPathString(path);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStrokeColor('none');
    result.setFillColor(color);
    container.appendChild(result.element);
  }
}

class MathNodeMatrix extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
      /** @type {string} */
      matrixEnvironment,
  ) {
    super(equationEditor, knownTypes.matrix);
    this.matrixEnvironment = matrixEnvironment;
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    if (this.element === null) {
      return new LatexWithAnnotation('[(null)]');
    }
    let matrixContent = this.children[0].children[1];
    if (this.isBinom()) {
      let first = matrixContent.children[0]
                      .children[0]
                      .toLatexWithAnnotation(options)
                      .latex;
      let second = matrixContent.children[1]
                       .children[0]
                       .toLatexWithAnnotation(options)
                       .latex;
      return new LatexWithAnnotation(`\\binom{${first}}{${second}}`);
    }
    let result = [];
    result.push(`\\begin{${this.matrixEnvironment}}`);
    if (this.matrixEnvironment === 'array') {
      result.push('{');
      result.push(this.latexExtraStyle);
      result.push('}');
    }
    let rows = [];
    for (let i = 0; i < matrixContent.children.length; i++) {
      let matrixRow = matrixContent.children[i];
      let currentRowStrings = [];
      for (let j = 0; j < matrixRow.children.length; j++) {
        let entry = matrixRow.children[j];
        currentRowStrings.push(entry.toLatexWithAnnotation(options).latex);
      }
      rows.push(currentRowStrings.join('&'));
    }
    result.push(rows.join('\\\\'));
    result.push(`\\end{${this.matrixEnvironment}}`);
    return new LatexWithAnnotation(result.join(''));
  }

  /**
   * Returns whether  the matrix is actually used to record a \binom{a}{b}
   * element.
   *
   * @return{boolean}
   */
  isBinom() {
    if (this.matrixEnvironment !== 'binom') {
      return false;
    }
    let matrixContent = this.children[0].children[1];
    if (matrixContent.children.length !== 2) {
      return false;
    }
    return matrixContent.children[0].children.length === 1 &&
        matrixContent.children[1].children.length === 1;
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    let parent = this.parent;
    parent.removeChild(this.indexInParent);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }

  /** Appends a horizontal line directly above the last row. */
  appendHorizontalAboveLastRow() {
    let rowContainer = this.children[0].children[1];
    if (rowContainer.children.length === 0) {
      return;
    }
    /** @type{MathNodeMatrixRow!} */
    let row = /** @type{MathNodeMatrixRow!} */ (
        rowContainer.children[rowContainer.children.length - 1]);
    row.addTopBorder();
  }

  /**
   * Fetches the last row of the matrix.
   * @return {MathNode?}
   */
  getLastMatrixRow() {
    let matrixTable = this.children[0].children[1];
    if (matrixTable.children.length === 0) {
      return null;
    }
    return matrixTable.children[matrixTable.children.length - 1];
  }

  /** Ensures that a matrix has rows with equal number of columns. */
  normalizeMatrix() {
    /** */
    let matrixTable = this.children[0].children[1];
    let columnCount = this.matrixColumnCount();
    let numberOfRows = matrixTable.children.length;
    // Last empty row is ignored. Previous empty rows are preserved.
    /** @type{MathNodeMatrixRow!} */
    let lastRow = /** @type{MathNodeMatrixRow!} */ (
        matrixTable.children[numberOfRows - 1]);
    if (lastRow.children.length === 0) {
      if (numberOfRows > 1 && lastRow.topLineCount > 0) {
        // We have a last row, that is empty, except for an \hline.
        // This means that the table has a bottom border, which
        // we create by appending a bottom border on the row above.
        let row = /** @type{MathNodeMatrixRow!}*/ (
            matrixTable.children[numberOfRows - 2]);
        row.addBottomBorder();
      }
      numberOfRows--;
      matrixTable.removeChild(numberOfRows);
    }
    // Expand rows to the colum count.
    for (let i = 0; i < matrixTable.children.length; i++) {
      let child = matrixTable.children[i];
      for (let j = child.children.length; j < columnCount; j++) {
        child.appendChild(
            mathNodeFactory.matrixRowEntry(this.equationEditor, null));
      }
    }
  }
}

class MathNodeRadicalUnderBox extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.radicalUnderBox);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    let sqrt = this.parent;
    let indexSqrt = sqrt.indexInParent;
    let exponent = sqrt.children[0].children[0];
    this.children[0].children[0].desiredCursorPosition = 0;
    let parent = sqrt.parent;
    parent.replaceChildAtPositionWithChildren(
        indexSqrt, [exponent, sqrt.children[2].children[0]]);
    parent.normalizeHorizontalMath();
    parent.updateDOM();
    parent.focusRestore();
    return true;
  }
}

class MathNodeMatrixRow extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.matrixRow);
    /** @type{number}*/
    this.topLineCount = 0;
    this.bottomLineCount = 0;
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    if (this.indexInParent > 0) {
      return this.parent.children[this.indexInParent - 1]
          .applyBackspaceToTheRightAsLeftArrow();
    }
    return this.parent.parent.parent.applyBackspaceToTheLeftAsLeftArrow();
  }

  /** Adds a border to the top edge of the matrix row. */
  addTopBorder() {
    this.topLineCount++;
    this.type.borderTop = '1px solid black';
  }

  /** Adds a border to the bottom edge of the matrix row. */
  addBottomBorder() {
    this.bottomLineCount++;
    this.type.borderBottom = '1px solid black';
  }
}

class MathNodeRowEntry extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.matrixRowEntry);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    if (this.indexInParent > 0) {
      return this.parent.children[this.indexInParent - 1]
          .applyBackspaceToTheRightAsLeftArrow();
    }
    return this.parent.applyBackspaceToTheLeft();
  }

  /** @return {MathNodeWithCursorPosition!} */
  getAtomToFocusFromAction(
      /** @type {string} */ key,
      /** @type {string} */ arrowType,
  ) {
    if (arrowType === arrowMotion.firstAtomDown) {
      let row = this.parent;
      let matrixTable = row.parent;
      if (row.indexInParent + 1 < matrixTable.children.length) {
        return new MathNodeWithCursorPosition(
            matrixTable.children[row.indexInParent + 1]
                .children[this.indexInParent],
            -1);
      }
    }
    if (arrowType === arrowMotion.firstAtomUp) {
      let row = this.parent;
      let matrixTable = row.parent;
      if (row.indexInParent > 0) {
        return new MathNodeWithCursorPosition(
            matrixTable.children[row.indexInParent - 1]
                .children[this.indexInParent],
            1);
      }
    }
    return this.getAtomToFocusFromActionDefault(key, arrowType);
  }
}

class MathNodeVerticalLine extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.verticalLineInTable);
  }

  toScalableVectorGraphics(
      /** @type {SVGSVGElement!} */
      container,
      /** @type {BoundingBox!} */
      boundingBoxFromParent,
  ) {
    this.toScalableVectorGraphicsBase(container, boundingBoxFromParent);
    let result = new ScalableVectorGraphicsLine();
    let x = this.boundingBox.left + boundingBoxFromParent.left;
    let yHigh = this.boundingBox.top + boundingBoxFromParent.top;
    let yLow = yHigh + this.boundingBox.height;

    result.setX1(x);
    result.setX2(x);
    result.setY1(yLow);
    result.setY2(yHigh);
    let color = this.type.colorText;
    if (color === '') {
      color = 'black';
    }
    result.setStroke(color);
    container.appendChild(result.element);
  }
}

class MathNodeOperatorWithSuperAndSubscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorWithSuperAndSubscript);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let top = this.children[0].toLatexWithAnnotation(options).latex;
    let result = this.children[1].toLatexWithAnnotation(options).latex;
    let bottom = this.children[2].toLatexWithAnnotation(options).latex;
    if (bottom !== '') {
      result += `_{${bottom}}`;
    }
    if (top !== '') {
      result += `^{${top}}`;
    }
    return new LatexWithAnnotation(result);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /** @return {boolean} whether reduction occurred. */
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

  /**
   * Computes the dimensions of the DOM.
   * @override
   */
  computeDimensions() {
    this.computeDimensionsOperatorWithSuperAndSubscript();
  }

  /**
   * Same as computeDimensions but easier to code search.
   */
  computeDimensionsOperatorWithSuperAndSubscript() {
    let superscript = this.children[0];
    let operator = this.children[1];
    let subscript = this.children[2];
    superscript.boundingBox.top = 0;
    operator.boundingBox.top = superscript.boundingBox.height;
    subscript.boundingBox.top =
        superscript.boundingBox.height + operator.boundingBox.height;
    this.boundingBox.height = superscript.boundingBox.height +
        operator.boundingBox.height + subscript.boundingBox.height;
    this.boundingBox.width = Math.max(
        superscript.boundingBox.width, operator.boundingBox.width,
        subscript.boundingBox.width);
    this.boundingBox.fractionLineHeight = superscript.boundingBox.height +
        operator.boundingBox.fractionLineHeight;
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
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorStandalone);
  }

  /** @return {LatexWithAnnotation!} */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    latexConstants.computeUtf16ToLatexMap();
    const content = this.textContentOrInitialContent();
    if (content in latexConstants.utf16ToLatexMap) {
      return new LatexWithAnnotation(latexConstants.utf16ToLatexMap[content]);
    }
    return new LatexWithAnnotation(`\\${content}`);
  }

  /** Computes the dimensions of the bounding box. */
  computeDimensions() {
    this.computeDimensionsOperatorStandalone();
  }

  /**
   * Same as computeDimensions but easier to code search.
   */
  computeDimensionsOperatorStandalone() {
    let child = this.children[0];
    // see latexConstants.operatorWithSuperAndSubscript
    // Measured as a proportion of child.boundingBox.height.
    let distanceTopToCharacterTipMap = {
      // sum
      '\u2211': 0.066,
      // integral
      '\u222B': 0.066,
    };
    let distanceTopToCharacterBottomMap = {
      // sum
      '\u2211': 1.033,
      // integral
      '\u222B': 1,
    };
    let distanceTopToCharacterTip = 0;
    let operatorName = child.initialContent;
    if (operatorName in distanceTopToCharacterTipMap) {
      distanceTopToCharacterTip = distanceTopToCharacterTipMap[operatorName];
    }
    let distanceTopToCharacterBottom = 1;
    if (operatorName in distanceTopToCharacterBottomMap) {
      distanceTopToCharacterBottom =
          distanceTopToCharacterBottomMap[operatorName];
    }
    let childHeight = child.boundingBox.height;
    child.boundingBox.top = -childHeight * distanceTopToCharacterTip;
    this.boundingBox.height = childHeight * (distanceTopToCharacterBottom);
    this.boundingBox.width = child.boundingBox.width;
    this.boundingBox.fractionLineHeight = childHeight *
        (distanceTopToCharacterBottom + distanceTopToCharacterTip) / 2;
  }
}

class MathNodeOperatorWithSubscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorWithSubscript);
  }

  /**
   * Applies backspace to the right.
   * @return {boolean}
   */
  applyBackspaceToTheRight() {
    return this.applyBackspaceToTheRightAsLeftArrow();
  }

  /** @return {boolean} whether reduction occurred. */
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
    let extraSpaceBelowOperator =
        operator.boundingBox.height * extraSpaceBelowOperatorPercent;
    operator.boundingBox.top = 0;
    subscript.boundingBox.top =
        operator.boundingBox.height + extraSpaceBelowOperator;
    this.boundingBox.height = operator.boundingBox.height +
        extraSpaceBelowOperator + subscript.boundingBox.height;
    this.boundingBox.width =
        Math.max(operator.boundingBox.width, subscript.boundingBox.width);
    this.boundingBox.fractionLineHeight =
        operator.boundingBox.fractionLineHeight;
    this.boundingBox.needsMiddleAlignment = true;
    subscript.boundingBox.width = this.boundingBox.width;
    operator.boundingBox.width = this.boundingBox.width;
    operator.computeBoundingBoxLeftSingleChild();
    subscript.computeBoundingBoxLeftSingleChild();
  }

  /**
   * @override
   * @return {LatexWithAnnotation!}
   */
  toLatexWithAnnotation(
      /** @type {ToLatexOptions?} */
      options,
  ) {
    let result = this.children[0].toLatexWithAnnotation(options).latex;
    let bottom = this.children[1].toLatexWithAnnotation(options).latex;
    if (bottom !== '') {
      result += `_{${bottom}}`;
    }
    return new LatexWithAnnotation(result);
  }
}

class MathNodeOperatorSubscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorSubscript);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    let parent =
        /** @type {MathNodeOperatorWithSuperAndSubscript!} */ (this.parent);
    return parent.applyBackspaceToTheLeftEndOfOperatorSubscript();
  }
}

class MathNodeOperatorSuperscript extends MathNode {
  constructor(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    super(equationEditor, knownTypes.operatorSuperscript);
  }

  /**
   * Applies backspace to the left.
   * @return {boolean}
   */
  applyBackspaceToTheLeft() {
    let parent =
        /** @type {MathNodeOperatorWithSuperAndSubscript!}*/ (this.parent);
    return parent.applyBackspaceToTheLeftEndOfOperatorSubscript();
  }
}

class VerticalBarData {
  constructor(
      /** @type {number} */
      columnIndex,
      /** @type {number} */
      stripIndex,
  ) {
    this.columnIndex = columnIndex;
    this.stripIndex = stripIndex;
  }
}

class LatexColumnStyleIterator {
  constructor(/** @type {string} */ style) {
    this.style = style;
    this.nextStyleStartCharacter = 0;
    this.currentColumnAlignment = '';
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    this.exhausted = false;
  }

  /** Resets the column iterator. */
  reset() {
    this.nextStyleStartCharacter = 0;
    this.currentColumnAlignment = '';
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    this.exhausted = false;
  }

  /** Iterates to the next column. */
  next() {
    this.currentColumnAlignment = '';
    this.verticalBarRightCount = 0;
    this.verticalBarLeftCount = 0;
    if (this.nextStyleStartCharacter >= this.style.length) {
      this.exhausted = true;
      return;
    }
    for (let i = this.nextStyleStartCharacter; i < this.style.length; i++) {
      let next = this.style[i];
      if (next === 'c' || next === 'r' || next === 'l') {
        if (this.currentColumnAlignment !== '') {
          return;
        }
        this.currentColumnAlignment = next;
      }
      this.nextStyleStartCharacter = i + 1;
      if (next === '|') {
        if (this.currentColumnAlignment === '') {
          this.verticalBarLeftCount++;
        } else {
          this.verticalBarRightCount++;
        }
      }
    }
  }

  applyStyleToMatrix(
      /** @type {MathNode!} */
      matrix,
  ) {
    if (matrix.type.type !== knownTypes.matrix.type) {
      return;
    }
    let matrixTable = matrix.children[0].children[1];
    // Expand rows to the colum count.
    for (let i = 0; i < matrixTable.children.length; i++) {
      let row = /** @type {MathNode!} */ (matrixTable.children[i]);
      this.applyStyleToRow(row);
    }
  }

  applyStyleToRow(
      /** @type {MathNode!} */
      row,
  ) {
    this.reset();
    this.next();
    for (let j = 0; j < row.children.length && !this.isExhausted();
         j++, this.next()) {
      row.children[j].latexExtraStyle = this.currentColumnAlignment;
    }
  }

  /**
   * Given matrix column style such as {c|c},
   * generates the vertical bars. Example: \begin{array}{c|c}1&2\end{array}.
   *
   * @return {Array.<MathNode!>!}
   */
  verticalSeparators(
      /** @type {EquationEditor!} */
      equationEditor,
  ) {
    this.reset();
    this.next();
    let result = [];
    let columnCount = 0;
    for (; !this.isExhausted(); this.next()) {
      for (let i = 0; i < this.verticalBarLeftCount; i++) {
        result.push(mathNodeFactory.matrixVerticalLine(
            equationEditor,
            columnCount,
            i,
            ));
      }
      columnCount++;
      for (let i = 0; i < this.verticalBarRightCount; i++) {
        result.push(mathNodeFactory.matrixVerticalLine(
            equationEditor,
            columnCount,
            i,
            ));
      }
    }
    return result;
  }

  /** @return {string} */
  borderFromVerticalBarCount(
      /** @type {number} */
      verticalBars,
  ) {
    if (verticalBars <= 0) {
      return '';
    }
    if (verticalBars === 1) {
      return '1px solid black';
    }
    return '1px double black';
  }

  /** @return {boolean} */
  isExhausted() {
    return this.exhausted;
  }
}

class ParentWithIndex {
  constructor(
      /** @type {MathNode?} */
      parent,
      /** @type {number} */
      indexInParent,
  ) {
    /** @type {MathNode?} */
    this.parent = parent;
    /** @type {number} */
    this.indexInParent = indexInParent;
  }
}

class MathTagData {
  constructor(
      /** @type {number} */
      startIndex,
      /** @type {number} */
      startIndexData,
      /** @type {number} */
      closeIndexData,
      /** @type {number} */
      closeIndex,
      /** @type {string} */
      ambientContent,
  ) {
    this.startIndex = startIndex;
    this.startIndexData = startIndexData;
    this.closeIndexData = closeIndexData;
    this.closeIndex = closeIndex;
    this.content =
        ambientContent.slice(this.startIndexData, closeIndexData + 1);
  }
}

class MathTagCoverter {
  constructor(
      /**
       * @type {{
       * style: (Object.<string,string>!|undefined),
       * sanitizeLatexSource: (boolean|undefined),
       * removeDisplayStyle: (boolean|undefined),
       * svgOnly: (boolean|undefined),
       * svgAndDOM: (boolean|undefined),
       * logTiming: (boolean|undefined),
       * copyButton: (boolean|undefined),
       * extraAttributes: (Object.<string, string>!|undefined)}|undefined}
       */
      // sanitizeLatexSource: whether to convert the original latex to parsed
      // one. removeDisplayStyle: whether to remove \\displaystyle from latex
      // source. logTiming: whether to log in the console timing statistics.
      // svgAndDOM: generate both a DOM and an SVG equation side-by side. Use
      // for debugging only. svgOnly: use svg instead of DOM output.
      // extraAttributes: an string-key string-value object.
      options,
  ) {
    if (options === null || options === undefined) {
      options = {};
    }
    /** @type {boolean} */
    this.sanitizeLatexSource =
        /** @type {boolean} */ (options.sanitizeLatexSource);
    if (this.sanitizeLatexSource === undefined) {
      this.sanitizeLatexSource = false;
    }
    /** @type {boolean} */
    this.removeDisplayStyle =
        /** @type {boolean} */ (options.removeDisplayStyle);
    if (this.removeDisplayStyle === undefined) {
      this.removeDisplayStyle = false;
    }
    /** @type {boolean} */
    this.logTiming = /** @type {boolean} */ (options.logTiming);
    if (this.logTiming === undefined || this.logTiming === null) {
      this.logTiming = false;
    }
    if (this.logTiming) {
      console.log(
          'Logging parsing speed times; to turn off, set logTiming=false.');
    }
    this.extraAttributes = options.extraAttributes;
    if (this.extraAttributes === null || this.extraAttributes === undefined) {
      this.extraAttributes = {};
    }
    /** @type {boolean} */
    this.svgAndDOM = /** @type {boolean} */ (options.svgAndDOM);
    if (this.svgAndDOM === undefined || this.svgAndDOM === null) {
      this.svgAndDOM = false;
    }
    /** @type {boolean} */
    this.svgOnly = /** @type {boolean} */ (options.svgOnly);
    if (this.svgOnly === null || this.svgOnly === undefined) {
      this.svgOnly = false;
    }
    /** @type {HTMLElement?} */
    this.elementProcessed = null;
    /** @type {HTMLElement?} */
    this.hiddenContainer = null;
    /** @type {number} */
    this.startTime = 0;
    /** @type {number} */
    this.lastTimeSample = 0;
    /** @type {number} */
    this.typesetTimeout = 50;
    /** @type {number} */
    this.elementsToTypeset = -1;
    /** @type {number} */
    this.typesetTotal = 0;
    let style = options.style;
    /** @type {boolean} */
    this.copyButton = false;
    if (options.copyButton === true) {
      this.copyButton = true;
    }
    if (style === null) {
      style = {fontFamily: 'Times New Roman', display: 'inline-block'};
    }
    let styleComputer = document.createElement('DIV');
    for (let label in style) {
      styleComputer.style[label] = style[label];
    }
    this.style = {
      fontFamily: styleComputer.style.fontFamily,
      display: styleComputer.style.display,
      fontSize: styleComputer.style.fontSize,
      verticalAlign: styleComputer.style.verticalAlign,
      marginBottom: styleComputer.style.marginBottom,
      maxWidth: styleComputer.style.maxWidth,
    };
    /** @type {Array.<HTMLElement!>!} */
    this.mathElementsDOM = [];
    /** @type {Array.<HTMLElement?>!} */
    this.mathElementsSVG = [];
  }

  /** @return {HTMLElement!} */
  getMathTagEmpty() {
    let mathTag = document.createElement('DIV');
    mathTag.style.fontFamily = this.style.fontFamily;
    mathTag.style.display = this.style.display;
    mathTag.style.fontSize = this.style.fontSize;
    mathTag.style.verticalAlign = this.style.verticalAlign;
    mathTag.style.marginBottom = this.style.marginBottom;
    mathTag.style.maxWidth = this.style.maxWidth;
    return /** @type {HTMLElement!} */ (mathTag);
  }

  convertTags(
      /** @type {HTMLElement!} */
      toBeModified,
      /** @type {Array.<MathTagData!>!} */
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
      let intermediateContent =
          content.slice(previousIndex, toBeConverted[i].startIndex);
      if (intermediateContent.length > 0) {
        newChildren.push(document.createTextNode(intermediateContent));
      }
      let mathTag = this.getMathTagEmpty();
      mathTag.textContent = toBeConverted[i].content;
      mathTag.className = 'mathcalculator';
      this.mathElementsDOM.push(mathTag);
      newChildren.push(mathTag);
      if (this.svgOnly) {
        this.mathElementsSVG.push(mathTag);
      } else if (this.svgAndDOM) {
        let mathTagSVG = this.getMathTagEmpty();
        mathTagSVG.className = 'mathcalculatorSVG';
        this.mathElementsSVG.push(mathTagSVG);
        newChildren.push(mathTagSVG);
      }
    }
    let previousIndex = toBeConverted[toBeConverted.length - 1].closeIndex + 1;
    let remainingContent = content.slice(previousIndex);
    if (remainingContent !== '' && remainingContent !== null) {
      newChildren.push(document.createTextNode(remainingContent));
    }
    toBeModified.textContent = '';
    let replacing = document.createElement('span');
    for (let i = 0; i < newChildren.length; i++) {
      replacing.appendChild(newChildren[i]);
    }
    toBeModified.parentElement.replaceChild(replacing, toBeModified);
  }

  processTextContent(
      /** @type {HTMLElement!} */
      toBeModified,
  ) {
    if (toBeModified.textContent === '' || toBeModified.textContent === null) {
      return;
    }
    let content = toBeModified.textContent;
    let openIndex = -1;
    /** @type {Array.<MathTagData!>!} */
    let toBeConverted = [];
    for (let i = 1; i < content.length; i++) {
      let previous = content[i - 1];
      let current = content[i];
      if (previous === '\\') {
        if (openIndex === -1) {
          if (current === '(') {
            openIndex = i;
            continue;
          }
        } else {
          if (current === ')') {
            toBeConverted.push(new MathTagData(
                openIndex - 1, openIndex + 1, i - 2, i, content));
            openIndex = -1;
          }
        }
      }
    }
    this.convertTags(toBeModified, toBeConverted);
  }

  convertTagsRecursive(
      /** @type {HTMLElement!|Node!} */
      toBeModified,
      /** @type {number} */
      recursionDepth,
  ) {
    if (recursionDepth > 40) {
      console.log('While converting mathtags, reached recursion depth limits');
      return;
    }
    if (toBeModified.childNodes.length === 0) {
      this.processTextContent(/** @type {HTMLElement!} */ (toBeModified));
      return;
    }
    for (let i = 0; i < toBeModified.childNodes.length; i++) {
      this.convertTagsRecursive(toBeModified.childNodes[i], recursionDepth + 1);
    }
  }

  typeset(
      /** @type {HTMLElement?} */
      toBeModified,
      /** @type {Function?} callback after the element has been typeset */
      callback,
  ) {
    this.elementProcessed = toBeModified;
    this.startTime = (new Date()).getTime();
    this.lastTimeSample = this.startTime;
    let mathElements = document.getElementsByClassName('mathcalculator');
    if (this.elementProcessed !== null) {
      this.convertTagsRecursive(this.elementProcessed, 0);
    }
    for (let i = 0; i < mathElements.length; i++) {
      this.mathElementsDOM.push(/** @type {HTMLElement!} */ (mathElements[i]));
      this.mathElementsSVG.push(null);
    }
    this.typesetMathTags(callback);
  }

  /** @return {boolean} */
  postponeTypeset(
      /** @type {Function?} callback after the element has been typeset*/
      callback,
  ) {
    let currentTime = (new Date()).getTime();
    let elapsedTime = currentTime - this.lastTimeSample;
    if (elapsedTime < this.typesetTimeout ||
        this.typesetTotal >= this.elementsToTypeset) {
      return false;
    }
    this.lastTimeSample = currentTime;
    setTimeout(() => {
      this.typesetMathTags(callback);
    }, 10);
    if (this.logTiming) {
      console.log(`Typeset ${this.typesetTotal} out of ${
          this.elementsToTypeset} elements.`);
    }
    return true;
  }

  typesetMathTags(
      /** @type {Function?} callback after the element has been typeset */
      callback,
  ) {
    if (this.elementsToTypeset < 0) {
      this.elementsToTypeset = this.mathElementsDOM.length;
      this.typesetTotal = 0;
    }
    for (; this.typesetTotal < this.mathElementsDOM.length;
         this.typesetTotal++) {
      if (this.postponeTypeset(callback)) {
        return;
      }
      /** @type {HTMLElement!} */
      let element = this.mathElementsDOM[this.typesetTotal];
      let elementSVG = this.mathElementsSVG[this.typesetTotal];
      if (element['typeset'] === 'true') {
        continue;
      }
      element['typeset'] = 'true';
      let startTime = (new Date()).getTime();
      mathFromElement(
          element,
          false,
          this.sanitizeLatexSource,
          this.removeDisplayStyle,
          callback,
          elementSVG,
          this.copyButton,
      );
      if (this.svgOnly) {
        console.log('svgOnly not implemented yet.');
      }
      let typeSetTime = (new Date()).getTime() - startTime;
      if (this.logTiming) {
        console.log(`Typeset of element ${this.typesetTotal + 1} out of ${
            this.elementsToTypeset} took ${typeSetTime} ms.`);
      }
    }
  }
}

function typeset(
    /** @type {HTMLElement!} */
    toBeModified,
    /** @type {Function?} */
    callbackEquationCreation,
    /**
     * @type {{style: Object.<string, string>!,
     * sanitizeLatexSource: boolean,
     * removeDisplayStyle: boolean,
     * svgOnly: boolean,
     * svgAndDOM: boolean,
     * extraAttributes: Object<string, string>!}}
     */
    // sanitizeLatexSource: whether to convert the original latex to parsed one.
    // removeDisplayStyle: whether to remove \\displaystyle from latex source.
    // logTiming: whether to log in the console timing statistics.
    // extraAttributes: an string-key string-value object.
    options,
) {
  if (callbackEquationCreation === undefined) {
    callbackEquationCreation = null;
  }
  new MathTagCoverter(
      options,
      )
      .typeset(
          toBeModified,
          callbackEquationCreation,
      );
}

class EquationEditorAction {
  constructor(
      /** @type {string} */
      command,
      /** @type {boolean} */
      isKeyPress,
  ) {
    this.command = command;
    this.isKeyPress = isKeyPress;
  }

  execute(
      /** @type {EquationEditor!} */
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

/** A template to  construct buttons that modify an equation editor. */
class EquationEditorButtonFactory {
  /**
   * @param{string} command The latex to write at the position of the cursor.
   * @param{boolean} isKeyPress Whether the input is to be interpreted in the
   * same way as a keyboard input. When this is true, editor-specific functions
   * such as auto-parentheses balancing will be triggered in addition to writing
   * the plain LaTeX.
   * @param{string} label The label of the generated button. Try to use
   * international mathematical signs, rather than English.
   * @param{Object.<string, string>?} additionalStyle A key-value map of
   * styles that will be applied to the button via .style[key]=value.
   * @param{string|undefined} className Classname to assign to the button; empty
   * -> do not assign.
   */
  constructor(
      command,
      isKeyPress,
      label,
      additionalStyle,
      className,
  ) {
    /** @type {Array.<EquationEditorAction!>!} */
    this.commands = [];
    if (command !== '' && command !== null) {
      this.commands.push(new EquationEditorAction(command, isKeyPress));
    }
    this.label = label;
    /** @type {Object.<string, string>!} */
    this.additionalStyle = {};
    if (additionalStyle !== null && additionalStyle !== undefined) {
      this.additionalStyle =
          /** @type {Object.<string, string>!} */ (additionalStyle);
    }
    /** @type {string} */
    this.className = '';
    if (className !== null && className !== undefined) {
      this.className = /** @type {string}*/ (className);
    }
  }

  addEditorAction(
      /** @type {EquationEditorAction!}*/
      command,
  ) {
    this.commands.push(command);
  }

  /** @return {HTMLButtonElement!} */
  getButton(
      /** @type {EquationEditor!} */
      editor,
  ) {
    let result =
        /** @type {HTMLButtonElement!} */ (document.createElement('button'));
    for (let label in this.additionalStyle) {
      result.style[label] = this.additionalStyle[label];
    }
    result.textContent = this.label;
    if (this.className !== '') {
      result.className = this.className;
    }
    return this.attachToClick(result, editor);
  }

  /** @return {HTMLButtonElement!} */
  attachToClick(
      /** @type {HTMLButtonElement!} */
      element,
      /** @type {EquationEditor!} */
      editor,
  ) {
    element.addEventListener('click', () => {
      this.clickFunction(editor);
    });
    return element;
  }

  /** @return {HTMLButtonElement?} */
  attachToClickById(
      /** @type {string} */
      buttonId,
      /** @type {EquationEditor!} */
      editor,
  ) {
    let button =
        /** @type {HTMLButtonElement!} */ (document.getElementById(buttonId));
    if (button === null) {
      return null;
    }
    return this.attachToClick(button, editor);
  }

  clickFunction(
      /** @type {EquationEditor!} */
      editor,
  ) {
    for (let i = 0; i < this.commands.length; i++) {
      this.commands[i].execute(editor);
    }
  }
}

/**
 * An alternative constructor that constructs a button factory from a sequence
 * of key presses.
 *
 * @param{Array.<string>!} keys The key presses generated by clicking the
 * button.
 * @param{string} label The button label. Try to use international mathematical
 * symbols (UTF8) rather than English.
 * @param{Object.<string, string>?} additionalStyle Additional style to apply to
 * the button. Passed to the constructor of the EquationEditorButtonFactory.
 * @param{string|undefined} className To be used as className of the constructed
 * button; passed on to the constructor of the EquationEditorButtonFactory.
 * @return {EquationEditorButtonFactory!}
 */
function equationEditorButtonFactoryFromKeySequence(
    keys,
    label,
    additionalStyle,
    className,
) {
  let result = new EquationEditorButtonFactory(
      '', true, label, additionalStyle, className);
  for (let i = 0; i < keys.length; i++) {
    result.addEditorAction(new EquationEditorAction(keys[i], true));
  }
  return result;
}

let buttonFactories = {
  'plus':
      new EquationEditorButtonFactory('+', true, '+', {'width': '100%'}, ''),
  'minus':
      new EquationEditorButtonFactory('-', true, '-', {'width': '100%'}, ''),
  'product':
      new EquationEditorButtonFactory('*', true, '*', {'width': '100%'}, ''),
  'divide':
      new EquationEditorButtonFactory('/', true, '/', {'width': '100%'}, ''),
  'exponent':
      new EquationEditorButtonFactory('^', true, '^', {'width': '100%'}, ''),
  'times': new EquationEditorButtonFactory(
      '\\times', true, '\u00D7', {'width': '100%'}, ''),
  'fraction': new EquationEditorButtonFactory(
      '\\frac{\\cursor}{}', false, '(\u2022)/(\u2022)', {'width': '100%'}, ''),
  'leftParenthesis':
      new EquationEditorButtonFactory('(', true, '(', {'width': '100%'}, ''),
  'rightParenthesis':
      new EquationEditorButtonFactory(')', true, ')', {'width': '100%'}, ''),
  'sqrt': new EquationEditorButtonFactory(
      '\\sqrt', true, '\u221A', {'width': '100%'}, ''),
  'integral': new EquationEditorButtonFactory(
      '\\int', false, '\u222B', {'width': '100%'}, ''),
  'sum': new EquationEditorButtonFactory(
      '\\sum', false, '\u03A3', {'width': '100%'}, ''),
  'prod': new EquationEditorButtonFactory(
      '\\prod', false, '\u03A0', {'width': '100%'}, ''),
  'matrix2x2': new EquationEditorButtonFactory(
      '\\begin{matrix}\\cursor&\\\\&\\end{matrix}', false, '2x2',
      {'width': '100%'}, ''),
  'pmatrix2x2': new EquationEditorButtonFactory(
      '\\begin{pmatrix}\\cursor&\\\\&\\end{pmatrix}', false, '(2x2)',
      {'width': '100%'}, ''),
  'bmatrix2x2': new EquationEditorButtonFactory(
      '\\begin{bmatrix}\\cursor&\\\\&\\end{bmatrix}', false, '[2x2]',
      {'width': '100%'}, ''),
  'bmatrix3x3': new EquationEditorButtonFactory(
      '\\begin{bmatrix}\\cursor&&\\\\&&\\\\&&\\end{bmatrix}', false, '[3x3]',
      {'width': '100%'}, ''),
  'bmatrix1x3': new EquationEditorButtonFactory(
      '\\begin{bmatrix}\\cursor \\\\~\\\\~ \\end{bmatrix}', false, '[1x3]',
      {'width': '100%'}, ''),
  'bmatrix1x2': new EquationEditorButtonFactory(
      '\\begin{bmatrix}\\cursor \\\\~\\end{bmatrix}', false, '[1x2]',
      {'width': '100%'}, ''),
  'bmatrix2x1': new EquationEditorButtonFactory(
      '\\begin{bmatrix}\\cursor &~ \\end{bmatrix}', false, '[2x1]',
      {'width': '100%'}, ''),
  'cases2x1': new EquationEditorButtonFactory(
      '\\begin{cases}\\cursor \\\\~ \\end{cases}', false, '{2x1',
      {'width': '100%'}, ''),
  'cases3x1': new EquationEditorButtonFactory(
      '\\begin{cases}\\cursor\\\\ ~\\\\ ~\\end{cases}', false, '{3x1',
      {'width': '100%'}, ''),
  'cases3x3': new EquationEditorButtonFactory(
      '\\begin{cases}\\cursor&=&\\\\ &=& \\\\ &=&\\end{cases}', false, '{3x3',
      {'width': '100%'}, ''),
  'array3x3': new EquationEditorButtonFactory(
      '\\begin{array}{rcl}\\cursor&=&\\\\ &=& \\\\ &=&\\end{array}', false,
      '3x3', {'width': '100%'}, ''),
  'align3x3': new EquationEditorButtonFactory(
      '\\begin{align}\\cursor&=&\\\\ &=& \\\\ &=&\\end{align}', false, 'al3x1',
      {'width': '100%'}, ''),
  'pi': new EquationEditorButtonFactory(
      '\\pi', false, '\u03C0', {'width': '100%'}, ''),
  'degrees': equationEditorButtonFactoryFromKeySequence(
      ['^', '\\circ'], '\u2218', {'width': '100%'}, ''),
  'alpha': new EquationEditorButtonFactory(
      '\\alpha', false, '\u03B1', {'width': '100%'}, ''),
  'beta': new EquationEditorButtonFactory(
      '\\beta', false, '\u03B2', {'width': '100%'}, ''),
  'gamma': new EquationEditorButtonFactory(
      '\\gamma', false, '\u03B3', {'width': '100%'}, ''),
  'underscore':
      new EquationEditorButtonFactory('_', true, '_', {'width': '100%'}, ''),
  'binom': new EquationEditorButtonFactory(
      '\\binom{\\cursor}{}', true, 'bin', {'width': '100%'}, ''),
  'cup': new EquationEditorButtonFactory(
      '\\cup', true, '\u222A', {'width': '100%'}, ''),
  'cap': new EquationEditorButtonFactory(
      '\\cap', true, '\u2229', {'width': '100%'}, ''),
  'limit': new EquationEditorButtonFactory(
      '\\lim_{\\cursor}', false, 'lim', {'width': '100%'}, ''),
};

/** Generates a panel of buttons to manipulate a given equation editor. */
class EquationEditorButtonPanel {
  /**
   * @param{EquationEditor!} editor The equation editor associated with the
   * panel.
   * @param{HTMLElement!} panelContainer The html element in which to write the
   * panel. The element's content will be cleared.
   * @param{{
   * numberOfColumns: (number|undefined),
   * useDefaultButtonFactories: (boolean|undefined),
   * expandButton: (HTMLButtonElement?|undefined),
   * collapseButton: (HTMLButtonElement?|undefined),
   * startsExpanded: (boolean|undefined),
   * }} options The options used to construct
   * the panel. numberOfColumns is the number of button columns to use in the
   * panel. useDefaultButtonFactories indicates that we should generate a
   * predefined set of buttons. collapseButton, expandButton provide a pair of
   * buttons which we will attach to collapsing/expanding the panel.
   * startsExpanded determines whether the panel starts expanded.
   */
  constructor(
      editor,
      panelContainer,
      options,
  ) {
    /** @type {EquationEditor!} */
    this.editor = editor;
    /** @type {HTMLElement!} */
    this.panelContainer = panelContainer;
    /** @type {HTMLElement?} */
    this.expandableDiv = null;
    /** @type {HTMLTableElement?} */
    this.buttonTable = null;
    /**
     * @type {string} Computed height property of the panel when expanded, if
     * known.
     */
    this.panelHeight = '';
    /** @type {HTMLButtonElement?} */
    this.collapseButton = (options.collapseButton === null ||
                           options.collapseButton === undefined) ?
        null : /** @type {HTMLButtonElement!} */
        (options.collapseButton);
    /** @type {HTMLButtonElement?} */
    this.expandButton =
        (options.expandButton === null || options.expandButton == undefined) ?
        null :
        /** @type {HTMLButtonElement!} */ (options.expandButton);
    /** @type {number} */
    this.numberOfColumns = (options.numberOfColumns === undefined) ?
        8 :
        /** @type {number} */ (options.numberOfColumns);
    /** @type {Array.<EquationEditorButtonFactory!>!}  */
    this.desiredButtons = [];
    /** @type {boolean} */
    this.startsExpanded = (options.startsExpanded === undefined) ?
        true :
        /** @type {boolean}*/ (options.startsExpanded);
    /** @type {boolean} */
    this.expanded = false;
    if (options.useDefaultButtonFactories === true ||
        options.useDefaultButtonFactories === undefined) {
      this.desiredButtons = [
        buttonFactories['plus'],
        buttonFactories['minus'],
        buttonFactories['product'],
        buttonFactories['divide'],
        buttonFactories['exponent'],
        buttonFactories['times'],
        buttonFactories['fraction'],
        buttonFactories['leftParenthesis'],
        buttonFactories['rightParenthesis'],
        buttonFactories['sqrt'],
        buttonFactories['integral'],
        buttonFactories['sum'],
        buttonFactories['limit'],
        buttonFactories['pi'],
        buttonFactories['degrees'],
        buttonFactories['alpha'],
        buttonFactories['beta'],
        buttonFactories['underscore'],
        buttonFactories['cup'],
        buttonFactories['cap'],
        buttonFactories['matrix2x2'],
        buttonFactories['pmatrix2x2'],
        buttonFactories['bmatrix3x3'],
        buttonFactories['bmatrix2x2'],
        buttonFactories['bmatrix1x3'],
        buttonFactories['bmatrix1x2'],
        buttonFactories['bmatrix2x1'],
        buttonFactories['cases2x1'],
        buttonFactories['cases3x1'],
        buttonFactories['cases3x3'],
        buttonFactories['array3x3'],
        buttonFactories['align3x3'],
      ];
    }
  }

  /**
   * Adds a button factory to the list of button factories. Call before
   * constructPanelDOM.
   */
  addButtonFactory(
      /** @type {string} */
      command,
      /** @type {boolean} */
      isKeyPress,
      /** @type {string} */
      label,
      /** @type {Object.<string, string>?} */
      additionalStyle,
      /** @type {string|undefined} */
      className,
  ) {
    this.desiredButtons.push(new EquationEditorButtonFactory(
        command, isKeyPress, label, additionalStyle, className));
  }

  /** Constructs a panel DOM element. */
  constructPanelDOM() {
    this.panelContainer.textContent = '';
    this.buttonTable =
        /** @type {HTMLTableElement!}*/ (document.createElement('table'));
    /** @type {HTMLTableRowElement!} */
    let currentRow =
        /** @type {HTMLTableRowElement!} */ (this.buttonTable.insertRow(-1));
    for (let i = 0; i < this.desiredButtons.length; i++) {
      if (currentRow.cells.length >= this.numberOfColumns) {
        currentRow = /** @type {HTMLTableRowElement!} */ (
            this.buttonTable.insertRow(-1));
      }
      let currentFactory = this.desiredButtons[i];
      currentRow.insertCell(-1).appendChild(
          currentFactory.getButton(this.editor));
    }
    this.expandableDiv =
        /** @type {HTMLElement!}*/ (document.createElement('div'));
    this.expandableDiv.appendChild(this.buttonTable);
    this.buttonTable.style.marginLeft = 'auto';
    this.buttonTable.style.marginRight = 'auto';
    this.expandableDiv.style.textAlign = 'center';
    this.expandableDiv.style.transition = 'all 1s';
    this.panelContainer.appendChild(this.expandableDiv);
    this.expanded = this.startsExpanded;
    if (this.collapseButton !== null) {
      this.collapseButton.addEventListener('click', () => {
        this.collapse();
      });
    }
    if (this.expandButton !== null) {
      this.expandButton.addEventListener('click', () => {
        this.expand();
      });
    }
    this.panelHeight = /** @type {string}*/ (
        window.getComputedStyle(this.expandableDiv).height);
    setTimeout(() => {
      this.matchExpandedStatus();
    }, 0);
  }

  /** Collapses a panel of buttons. */
  collapse() {
    this.expanded = false;
    this.matchExpandedStatus();
  }

  /** Expands a panel of buttons. */
  expand() {
    this.expanded = true;
    this.matchExpandedStatus();
  }

  /** Expands or collapses a panel to match the expanded flag. */
  matchExpandedStatus() {
    if (this.expanded) {
      this.expandableDiv.style.maxHeight = this.panelHeight;
      this.expandableDiv.style.height = this.panelHeight;
      setTimeout(() => {
        this.buttonTable.style.display = '';
      }, 1000);
      if (this.expandButton !== null) {
        this.expandButton.style.display = 'none';
      }
      if (this.collapseButton !== null) {
        this.collapseButton.style.display = '';
      }
    } else {
      this.expandableDiv.style.maxHeight = '0px';
      this.expandableDiv.style.height = '0px';
      this.buttonTable.style.display = 'none';
      if (this.expandButton !== null) {
        this.expandButton.style.display = '';
      }
      if (this.collapseButton !== null) {
        this.collapseButton.style.display = 'none';
      }
    }
  }
}

/** @type {MathNodeFactory!} */
let mathNodeFactory = new MathNodeFactory();

/**
 * When using as a commonJS module inside the calculator project, the content of
 * this file is inserted into a function body, which receives as an argument a
 * variable called module. Modifications to module.exports are then visible to
 * the caller of this functions, i.e., to external javascript files.
 */
var module;
if (module === undefined) {
  module = {
    exports : null,
  };
}

module.exports = {
  typeset,
  EquationEditor,
  EquationEditorButtonFactory,
  EquationEditorAction,
  buttonFactories,
  EquationEditorOptions,
  EquationEditorButtonPanel,
  mathFromLatex,
  mathFromElement,
  latexConstants,
  MathNode,
  knownTypes,
};
