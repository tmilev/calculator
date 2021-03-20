"use strict";
const submitRequests = require("./submit_requests");
const panels = require("./panels");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");
const equation_editor = require("./equation_editor");
const storage = require("./storage");
const EquationEditorAction = require("./equation_editor").EquationEditorAction;
const EquationEditor = require("./equation_editor").EquationEditor;
const EquationEditorButtonFactory = require("./equation_editor").EquationEditorButtonFactory;
const EquationEditorOptions = require("./equation_editor").EquationEditorOptions;
const datePicker = require("./date_picker").datePicker;

let keyWordsKnownToMathQuill = [
  'sqrt',
  'frac',
  'cdot',
  'left',
  'right',
  'infty',
  'otimes',
  'times',
  'oplus',
  'pmatrix',
  'int',
  'begin',
  'end',
  'alpha',
  'pi',
  'beta',
  'cup',
  'cap',
];

let charsToSplit = ['x', 'y'];
let panelsCollapseStatus = {};
let calculatorSeparatorLeftDelimiters = {
  '(': true,
  '{': true
};

function processMathQuillLatex(theText) {
  for (let i = 0; i < theText.length; i++) {
    if (i + 1 < theText.length) {
      if ((theText[i] === '_' || theText[i] === '^') && theText[i + 1] !== '{') {
        theText = theText.slice(0, i + 1) + '{' + theText[i + 1] + '}' + theText.slice(i + 2);
      }
      if (theText[i] === '\\' && theText[i + 1] === '\\') {
        theText = theText.slice(0, i + 2) + ' ' + theText.slice(i + 2);
      }
    }
  }
  if (charsToSplit !== undefined) {
    for (let i = 0; i < theText.length - 1; i++) {
      for (let j = 0; j < charsToSplit.length; j++) {
        if (
          theText[i] === charsToSplit[j] && theText[i + 1] !== ' ' &&
          theText[i + 1] !== '\\' && theText[i + 1] !== '+' &&
          theText[i + 1] !== '*' && theText[i + 1] !== '/' &&
          theText[i + 1] !== '-' && theText[i + 1] !== '='
        ) {
          if (theText[i] === 'x') {
            if (theText.slice(i - 5, i + 1) === 'matrix') {
              continue;
            }
          }
          theText = theText.slice(0, i + 1) + " " + theText.slice(i + 1);
        }
      }
    }
  }
  return theText;
}

function accountCalculatorDelimiterReturnMustEndSelection(character, calculatorSeparatorCounts) {
  if (character in calculatorSeparatorLeftDelimiters) {
    calculatorSeparatorCounts.leftSeparators++;
  }
  if (character in calculatorSeparatorLeftDelimiters) {
    calculatorSeparatorCounts.rightSeparators++;
  }
  return calculatorSeparatorCounts.leftSeparators > calculatorSeparatorCounts.rightSeparators;
}

function initializeAccordionButtons() {
  ///initializing accordions
  if (localStorage !== undefined) {
    if (localStorage.panels !== undefined) {
      panelsCollapseStatus = JSON.parse(localStorage.panels)
      let theProps = Object.getOwnPropertyNames(panelsCollapseStatus);
      for (let i = 0; i < theProps.length; i++) {
        let current = panelsCollapseStatus[theProps[i]];
        if (current.isCollapsed) {
          panels.toggleHeight(document.getElementById(current.button), theProps[i]);
        }
      }
    }
  }
  let acc = document.getElementsByClassName("accordion");
  for (let i = 0; i < acc.length; i++) {
    acc[i].onclick = function () {
      if (this.firstLoad === undefined) {
        this.firstLoad = true;
        let theDeadlines = this.nextElementSibling.getElementsByClassName("modifyDeadlineInput");
        for (let j = 0; j < theDeadlines.length; j++) {
          datePicker.createDatePicker(theDeadlines[j].id);
        }
        this.nextElementSibling.style.display = "inline-block";
      }
      this.classList.toggle("active");
      this.nextElementSibling.classList.toggle("show");
    }
  }
}

function initializeButtons() {
  initializeAccordionButtons();
}

class ButtonCollection {
  constructor(
    /**@type {Object} */
    keywords
  ) {
    this.selected = false;
    this.keywords = keywords;
  }
  updateFlags(
    /** @type{Object.<string, string} */
    allFlags,
  ) {
    for (let keyword in this.keywords) {
      if (keyword in allFlags) {
        this.selected = true;
        return;
      }
    }
  }
}

class InputPanelData {
  constructor(
    /**@type{{
     * pureLatexElement?:HTMLElement|null,
     * }} */
    input,
  ) {
    /** @type{string} Id of component where the editor is placed.*/
    this.idEquationEditorElement = input.idEquationEditorElement;
    if (this.idEquationEditorElement === "") {
      this.idEquationEditorElement = "";
    }
    /**@type{HTMLElement|null} */
    this.equationEditorContainer = null;
    if (input.equationEditorContainer !== undefined) {
      this.equationEditorContainer = input.equationEditorContainer;
    }
    this.idEditorComments = input.idEditorComments;
    if (this.idEditorComments === "") {
      this.idEditorComments = "";
    }
    this.problemId = input.problemId;
    if (this.problemId === "") {
      this.problemId = "";
    }
    this.idPureLatex = input.idPureLatex;
    /**@type{HTMLElement|null} */
    this.pureLatexElement = null;
    if (input.pureLatexElement !== undefined && input.pureLatexElement !== null) {
      this.pureLatexElement = input.pureLatexElement;
    }
    /**@type{EquationEditorButtonFactory[]} */
    this.buttonFactories = [];
    this.valueChangeHandler = null;
    if (input.valueChangeHandler !== undefined) {
      this.valueChangeHandler = input.valueChangeHandler;
    }
    this.buttonsPerLine = input.buttonsPerLine;
    if (this.buttonsPerLine === null || this.buttonsPerLine === undefined) {
      this.buttonsPerLine = 4;
    }
    this.flagRendered = false;

    this.requestedButtons = [];
    this.flagButtons = {
      noPreference: new ButtonCollection({}),
      all: new ButtonCollection({
        "all": true,
      }),
      none: new ButtonCollection({
        "none": true,
      }),
      trigonometry: new ButtonCollection({
        "trig": true,
        "trigonometry": true,
      }),
      algebra: new ButtonCollection({
        "algebra": true,
      }),
      inverseTrigonometry: new ButtonCollection({
        "inversetrig": true,
        "inverse_trig": true,
        "inverse-trig": true,
        "inversetrigonometry": true,
        "inverse_trigonometry": true,
        "inverse-trigonometry": true,
        "inverse trigonometry": true,
      }),
      comma: new ButtonCollection({
        "brackets": true,
        "comma": true,
        "commas": true,
        "intervals": true,
      }),
      logarithms: new ButtonCollection({
        "logarithms": true,
        "logarithm": true,
      }),
      brackets: new ButtonCollection({
        "brackets": true,
        "intervals": true,
      }),
      variables: new ButtonCollection({
        "variables": true,
      }),
      complex: new ButtonCollection({
        "complex": true,
        "imaginary": true,
      }),
      infinity: new ButtonCollection({
        "infinity": true,
        "infty": true,
        "\\infty": true,
        "interval": true,
        "intervals": true,
        "limits": true,
        "limit": true,
      }),
      limits: new ButtonCollection({
        "limits": true,
        "limit": true,
      }),
      series: new ButtonCollection({
        "sum": true,
        "series": true,
      }),
      angles: new ButtonCollection({
        "angle": true,
        "angles": true,
      }),
      newtonsMethod: new ButtonCollection({
        "newtonsmethod": true,
        "newton": true,
        "newton's method": true,
        "newtons method": true,
      }),
      logical: new ButtonCollection({
        "interval": true,
        "intervals": true,
        "or": true,
      }),
      setOperations: new ButtonCollection({
        "interval": true,
        "intervals": true,
      }),
      matrix: new ButtonCollection({
        "matrix": true,
        "matrices": true,
      }),
    };
    /**@type{HTMLElement|null} */
    this.buttonSubmitHardCoded = input.buttonSubmitHardCoded;
    if (this.buttonSubmitHardCoded === undefined) {
      this.buttonSubmitHardCoded = null;
    }
    /**@type{string} */
    this.idButtonContainer = input.idButtonContainer;
    /**@type{HTMLElement|null} */
    this.buttonContainer = input.buttonContainer;
    if (this.buttonContainer === undefined || this.buttonContainer === null) {
      this.buttonContainer = document.getElementById(this.idButtonContainer);
    }
    this.idButtonSubmit = input.idButtonSubmit;
    this.idButtonInterpret = input.idButtonInterpret;
    this.idButtonAnswer = input.idButtonAnswer;
    this.idButtonSolution = input.idButtonSolution;
    this.idVerificationSpan = input.idVerificationSpan;
    this.answerHighlight = input.answerHighlight;
    this.answerPanelId = input.answerPanelId;
    this.randomSeed = input.randomSeed;

    this.htmlPureLatex = "";
    this.htmlButtonContainer = "";
    this.htmlButtonSubmit = "";
    this.htmlButtonInterpret = ""
    this.htmlButtonAnswer = "";
    this.htmlButtonSolution = "";
    this.htmlVerificationSpan = "";
    this.htmlSolution = "";
    this.htmlAnswerHighlight = "";
    this.htmlMQFieldEnclosure = "";

    this.properties = input.properties;
    this.previousAnswers = input.previousAnswers;
    this.mathQuillPanelOptions = input.mathQuillPanelOptions;
    this.flagAnswerPanel = input.flagAnswerPanel;
    this.flagCalculatorPanel = input.flagCalculatorPanel;
    this.forReal = (input[pathnames.urlFields.problem.forReal] === true);
    //just in case we forget some entry above:
    for (let key in input) {
      if (!(key in this)) {
        console.log("Missing key: " + key);
      }
    }
    /**@type{EquationEditor|null} */
    this.equationEditor = null;
    this.ignoreNextEditorEvent = false;
    this.flagAnswerPanel = input.flagAnswerPanel;
    this.flagInitialized = false;
    this.flagCalculatorPanel = input.flagCalculatorPanel;
    this.flagGenerateQuestionAndAnswerField = true;
    this.calculatorLeftString = "";
    this.calculatorRightString = "";
    this.theLaTeXString = null;
    this.selectionEnd = 0;
    this.javascriptInsertionAlreadyCalled = false;
    this.timerForPreviewAnswers = 0;

    if (this.flagCalculatorPanel === undefined) {
      this.flagCalculatorPanel = false;
    }
    if (this.idEditorComments === undefined) {
      this.idEditorComments = null;
    }
  }

  /**@returns{HTMLElement|null} */
  getButtonContainer() {
    if (this.buttonContainer !== null) {
      return this.buttonContainer;
    }
    this.buttonContainer = document.getElementById(this.idButtonContainer);
    return this.buttonContainer;
  }

  /**@returns{HTMLElement|null} */
  getEditorContainer() {
    if (this.equationEditorContainer !== null && this.equationEditorContainer !== undefined) {
      return this.equationEditorContainer;
    }
    this.equationEditorContainer = document.getElementById(this.idEquationEditorElement);
    return this.equationEditorContainer;
  }

  /**@returns{HTMLElement|null} */
  getPureLatexElement() {
    if (this.pureLatexElement !== null && this.pureLatexElement !== undefined) {
      return this.pureLatexElement;
    }
    this.pureLatexElement = document.getElementById(this.idPureLatex);
    return this.pureLatexElement;
  }

  editLaTeX() {
    // useful event handlers	
    this.ignoreNextEditorEvent = true;
    this.equationEditor.writeLatex(document.getElementById(this.idPureLatex).value + ' ');
    this.ignoreNextEditorEvent = false;
    this.submitPreviewWithTimeOut();
  }

  editLatexHook(
    /**@type{EquationEditor} */
    editor,
    /**@type{equation_editor.MathNode} */
    unusedNode,
  ) {
    if (this.ignoreNextEditorEvent) {
      return;
    }
    if (this.valueChangeHandler !== null) {
      this.valueChangeHandler(editor.rootNode.toLatex());
    }
  }

  initialize() {
    if (this.flagInitialized) {
      return;
    }
    let currentMQspan = this.getEditorContainer();
    let latexInput = null;
    if (!this.flagCalculatorPanel && !this.flagAnswerPanel) {
      latexInput = this.getPureLatexElement();
    }
    this.equationEditor = new EquationEditor(currentMQspan, new EquationEditorOptions({
      latexInput: latexInput,
      editHandler: (editor, node) => {
        this.editLatexHook(editor, node);
      },
    }));
    let forceShowAll = false;
    let forceShowNone = false;
    if (this.idEquationEditorElement === ids.domElements.pages.solve.editor) {
      forceShowAll = storage.storage.variables.solve.panel.forceShowAll.isTrue();
      forceShowNone = storage.storage.variables.solve.panel.forceShowNone.isTrue();
    }
    if (this.idEquationEditorElement === ids.domElements.pages.solveSocratic.editor) {
      forceShowAll = storage.storage.variables.solveSocratic.panel.forceShowAll.isTrue();
      forceShowNone = storage.storage.variables.solveSocratic.panel.forceShowNone.isTrue();
    }
    this.initializePartTwo(forceShowAll, forceShowNone);
    this.renderIfVisible();
  }

  renderIfVisible() {
    if (this.flagRendered) {
      return;
    }
    if (this.equationEditor.container.offsetParent === null || this.equationEditor.container.offsetParent === undefined) {
      return;
    }
    this.flagRendered = true;
    this.equationEditor.updateDOM();
    this.equationEditor.updateAlignment();
  }

  computeFlags(
    /**@type {boolean} */
    forceShowAll,
  ) {
    let currentButtonPanel = this.getButtonContainer();
    let buttonsNonSplit = currentButtonPanel.getAttribute("buttons");
    if (buttonsNonSplit === null) {
      buttonsNonSplit = "";
    }
    let buttonArray = buttonsNonSplit.split(/(?:,| ) +/);
    this.flagButtons.noPreference.selected = false;
    this.flagButtons.all.selected = false;
    if (forceShowAll) {
      this.flagButtons.all.selected = true;
    }
    let allFlags = {};
    for (let i = 0; i < buttonArray.length; i++) {
      allFlags[buttonArray[i].toLowerCase()] = true;
    }
    if (buttonArray.length === 0) {
      this.flagButtons.noPreference.selected = true;
    }
    if (buttonArray.length === 1 && "" in allFlags) {
      this.flagButtons.noPreference.selected = true;
    }
    for (let key in this.flagButtons) {
      let flag = this.flagButtons[key];
      flag.updateFlags(allFlags);
    }
  }

  addLatexCommand(
    /**@type{string} */
    command,
    /**@type{string} */
    label,
    /**@type{string} */
    additionalStyle,
  ) {
    let buttonFactory = new EquationEditorButtonFactory(
      command, false, label, additionalStyle,
    );
    this.buttonFactories.push(buttonFactory);
  }

  addKeySequence(
    /**@type{string[]} */
    keys,
    /**@type{string} */
    label,
    /**@type{string} */
    additionalStyle,
  ) {
    let buttonFactory = new EquationEditorButtonFactory(
      "", true, label, additionalStyle,
    );
    for (let i = 0; i < keys.length; i++) {
      buttonFactory.addEditorAction(new EquationEditorAction(keys[i], true));
    }
    this.buttonFactories.push(buttonFactory);
  }

  addButtons(
    /**@type{boolean} */
    forceShowAll,
    /**@type{boolean} */
    forceShowNone,
  ) {
    this.buttonFactories = [];
    if (forceShowNone) {
      return;
    }
    let noOptions = this.flagButtons.noPreference.selected;
    let includeAll = this.flagButtons.all.selected || forceShowAll;
    if (this.flagButtons.algebra.selected || noOptions || includeAll) {
      this.addLatexCommand("+", "+");
      this.addLatexCommand("-", "-");
      this.addLatexCommand("\\cdot", "*");
      this.addLatexCommand("\\frac{\\caret}{}", "/");
      this.addKeySequence(["/"], "(\u2022)/(\u2022)", "font-size:10px"); // \u2022 = bullet.
      this.addLatexCommand("\\sqrt{\\caret}", "\u221A");
      this.addKeySequence(["^"], "^");
      this.addKeySequence(["("], "(");
      this.addKeySequence([")"], ")");
    }
    if (this.flagButtons.trigonometry.selected || includeAll) {
      this.addLatexCommand("\\sin", "sin", "font-size:10px; ");
      this.addLatexCommand("\\cos", "cos", "font-size:10px; ");
      this.addLatexCommand("\\tan", "tan", "font-size:10px; ");
      this.addLatexCommand("\\cot", "cot", "font-size:10px; ");
      this.addLatexCommand("\\sec", "sec", "font-size:10px; ");
      this.addLatexCommand("\\csc", "csc", "font-size:10px; ");
    }
    if (this.flagButtons.inverseTrigonometry.selected || includeAll) {
      this.addLatexCommand("\\arcsin", "asin", "font-size:7px");
      this.addLatexCommand("\\arccos", "acos", "font-size:7px");
      this.addLatexCommand("\\arctan", "atan", "font-size:7px");
    }
    if (this.flagButtons.comma.selected || includeAll) {
      this.addLatexCommand(",", ",");
    }
    if (this.flagButtons.brackets.selected || includeAll) {
      this.addKeySequence(["["], "[");
      this.addKeySequence(["]"], "]");
    }
    if (this.flagButtons.complex.selected || includeAll) {
      this.addLatexCommand("i", "i");
    }
    if (this.flagButtons.variables.selected || includeAll) {
      this.addLatexCommand("x", "x");
      this.addLatexCommand("y", "y");
      this.addLatexCommand("=", "=");
    }
    if (this.flagButtons.logarithms.selected || noOptions || includeAll) {
      this.addLatexCommand("\\log_{} ", "log_", "font-size:10px; ");
      this.addLatexCommand("{}_{}", "_");
      this.addLatexCommand("\\ln", "ln");
      this.addLatexCommand("e", "e");
    }
    if (this.flagButtons.infinity.selected || includeAll || noOptions) {
      this.addLatexCommand("\\infty", "\u221E");
    }
    if (this.flagButtons.limits.selected || includeAll) {
      this.addLatexCommand(" DNE ", "DNE", "font-size: 7px");
    }
    if (this.flagButtons.series.selected || noOptions || includeAll) {
      this.addLatexCommand("\\sum", "\u03A3");
      this.addLatexCommand("!", "!");
      this.addLatexCommand("\\binom{\\caret}{}", "binom", "font-size : 7px;");
    }
    if (noOptions || includeAll) {
      this.addLatexCommand("\\circ", "\u2218");
    }
    if (this.flagButtons.logical.selected || noOptions || includeAll) {
      this.addLatexCommand(" or ", "or");
    }
    if (this.flagButtons.setOperations.selected || noOptions || includeAll) {
      this.addLatexCommand("\\emptyset", "\u2205");
      this.addLatexCommand("\\cup", "\u222A");
      this.addLatexCommand("\\in", "\u2208");
    }
    if (this.flagButtons.matrix.selected || includeAll) {
      this.addLatexCommand("\\begin{pmatrix} \\\\ \\end{pmatrix}", "2x1", "font-size : 7px;", true);
      this.addLatexCommand("\\begin{pmatrix} \\\\ \\\\ \\end{pmatrix}", "3x1", "font-size : 7px;", true);
      this.addLatexCommand("\\begin{pmatrix} & \\\\ & \\end{pmatrix}", "2x2", "font-size : 7px;", true);
      this.addLatexCommand("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}", "3x3", "font-size : 7px;", true);
    }
    if (this.flagButtons.angles.selected || noOptions || includeAll) {
      this.addLatexCommand("\\alpha", "\u03B1");
      this.addLatexCommand("\\beta", "\u03B2");
      this.addLatexCommand("\\gamma", "\u03B3");
      this.addLatexCommand("\\theta", "\u03B8");
      this.addLatexCommand("\\pi", "\u03C0");
      this.addKeySequence(["^", "\\circ"], "^\u2218");
    }
    if (this.flagButtons.newtonsMethod.selected || includeAll) {
      this.addLatexCommand(["NewtonsMethod(\\caret,,)"], "Newton", "font-size: 6px", false);
    }
  }

  initializePartTwo(
    /**@type{boolean} */
    forceShowAll,
    /**@type{boolean} */
    forceShowNone,
  ) {
    if (this.idEquationEditorElement === ids.domElements.pages.solve.editor) {
      storage.storage.variables.solve.panel.forceShowAll.setAndStore(forceShowAll);
      storage.storage.variables.solve.panel.forceShowNone.setAndStore(forceShowNone);
    }
    if (this.idEquationEditorElement === ids.domElements.pages.solveSocratic.editor) {
      storage.storage.variables.solveSocratic.panel.forceShowAll.setAndStore(forceShowAll);
      storage.storage.variables.solveSocratic.panel.forceShowNone.setAndStore(forceShowNone);
    }
    this.computeFlags(forceShowAll);
    this.addButtons(forceShowAll, forceShowNone);
    let currentButtonPanel = this.getButtonContainer();
    /** @type{HTMLTableElement} */
    let table = document.createElement("TABLE");
    table.style.margin = "auto";
    let currentRow = null;
    for (let j = 0; j < this.buttonFactories.length; j++) {
      if (j % this.buttonsPerLine === 0) {
        currentRow = table.insertRow();
      }
      let cell = currentRow.insertCell();
      let button = this.buttonFactories[j].getButton(this.equationEditor);
      button.className = "buttonMQ";
      cell.appendChild(button);
    }
    let toggles = [];
    if (!forceShowAll) {
      toggles.push(this.getShowAllToggle());
    }
    if (forceShowNone || forceShowAll) {
      toggles.push(this.getShowDefaultToggle());
    }
    if (!forceShowNone && !this.flagButtons.none.selected) {
      toggles.push(this.getShowNoneToggle());
    }
    let oldHeight = window.getComputedStyle(currentButtonPanel).height;
    currentButtonPanel.style.maxHeight = "";
    currentButtonPanel.style.height = "";
    currentButtonPanel.innerHTML = "";
    currentButtonPanel.appendChild(table);
    for (let i = 0; i < toggles.length; i++) {
      currentButtonPanel.appendChild(toggles[i]);
    }
    if (oldHeight !== 0 && oldHeight !== "0px") {
      let newHeight = window.getComputedStyle(currentButtonPanel).height;
      currentButtonPanel.style.maxHeight = oldHeight;
      currentButtonPanel.style.height = oldHeight;
      setTimeout(function () {
        panels.modifyHeightForTimeout(currentButtonPanel, newHeight)
      }, 0);
    }
    return false;
  }

  /**@returns{HTMLButtonElement} */
  getShowAllToggle() {
    let toggle = document.createElement("button");
    toggle.addEventListener(
      'click',
      () => {
        this.initializePartTwo(true, false);
      }
    );
    toggle.className = "buttonShowExpandMQPanel";
    toggle.style.fontSize = "x-small";
    toggle.textContent = "all";
    return toggle;
  }

  /**@returns{HTMLButtonElement} */
  getShowNoneToggle() {
    let toggle = document.createElement("button");
    toggle.addEventListener(
      'click',
      () => {
        this.initializePartTwo(false, true);
      }
    );
    toggle.className = "buttonShowExpandMQPanel";
    toggle.style.fontSize = "x-small";
    toggle.textContent = "none";
    return toggle;
  }

  /**@returns{HTMLButtonElement} */
  getShowDefaultToggle() {
    let toggle = document.createElement("button");
    toggle.addEventListener(
      'click',
      () => {
        this.initializePartTwo(false, false);
      }
    );
    toggle.className = "buttonShowExpandMQPanel";
    toggle.style.fontSize = "x-small";
    toggle.textContent = "default";
    return toggle;
  }
}

function isSeparatorCharacter(theChar) {
  if (theChar[0] >= 'a' && theChar[0] <= 'z') {
    return false;
  }
  if (theChar[0] >= 'A' && theChar[0] <= 'Z') {
    return false;
  }
  return true;
}

function isKeyWordStartKnownToMathQuill(input) {
  for (let i = 0; i < keyWordsKnownToMathQuill.length; i++) {
    if (keyWordsKnownToMathQuill[i].startsWith(input)) {
      return true;
    }
  }
  return false;
}

function isKeyWordEndKnownToMathQuill(input) {
  for (let i = 0; i < keyWordsKnownToMathQuill.length; i++) {
    if (keyWordsKnownToMathQuill[i].endsWith(input)) {
      return true;
    }
  }
  return false;
}

module.exports = {
  initializeAccordionButtons,
  initializeButtons,
  InputPanelData,
  processMathQuillLatex,
};