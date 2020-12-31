"use strict";
const submitRequests = require("./submit_requests");
const panels = require("./panels");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous");
const equation_editor = require("./equation_editor");
const storage = require("./storage");
const EquationEditorAction = require("./equation_editor").EquationEditorAction;
const EquationEditor = require("./equation_editor").EquationEditor;
const EquationEditorButtonFactory = require("./equation_editor").EquationEditorButtonFactory;
const EquationEditorOptions = require("./equation_editor").EquationEditorOptions;
const datePicker = require("./date_picker").datePicker;

var keyWordsKnownToMathQuill = [
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

var charsToSplit = ['x', 'y'];
var panelsCollapseStatus = {};
var calculatorSeparatorLeftDelimiters = {
  '(': true,
  '{': true
};
var startingCharacterSectionUnderMathQuillEdit = '';

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
  constructor(input) {
    /** @type{string} Id of component where the editor is placed.*/
    this.idEquationEditorElement = input.idEquationEditorElement;
    this.idEquationEditorElementLocation = input.idEquationEditorElementLocation;
    this.idEditorComments = input.idEditorComments;
    this.problemId = input.problemId;
    /**@type{EquationEditorButtonFactory[]} */
    this.buttonFactories = [];

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
    this.idPureLatex = input.idPureLatex;
    this.idButtonContainer = input.idButtonContainer;
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
    this.layoutVertical = true;

    this.properties = input.properties;
    this.previousAnswers = input.previousAnswers;
    this.mathQuillPanelOptions = input.mathQuillPanelOptions;
    this.flagAnswerPanel = input.flagAnswerPanel;
    this.flagCalculatorPanel = input.flagCalculatorPanel;
    //just in case we forget some entry above:
    for (let key in input) {
      if (!(key in this)) {
        console.log("Missing key: " + key);
      }
    }
    /**@type{EquationEditor|null} */
    this.equationEditor = null;
    this.ignoreNextMathQuillUpdateEvent = false;
    this.flagAnswerPanel = input.flagAnswerPanel;
    this.flagInitialized = false;
    this.flagCalculatorPanel = input.flagCalculatorPanel;
    this.flagCalculatorMQStringIsOK = true;
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

  editorHelpCalculator() {
    this.getSemiColumnEnclosure();
    if (this.equationEditor === null) {
      return;
    }
    this.ignoreNextMathQuillUpdateEvent = true;
    if (this.flagCalculatorMQStringIsOK) {
      this.equationEditor.writeLatex(this.theLaTeXString);
    }
    this.ignoreNextMathQuillUpdateEvent = false;
  }

  submitOrPreviewAnswersCallback(outputComponent, input) {
    if (typeof outputComponent === "string") {
      outputComponent = document.getElementById(outputComponent);
    }
    let inputParsed = miscellaneous.jsonUnescapeParse(input);
    let resultHtml = "";
    if (inputParsed.error !== undefined && inputParsed.error !== null && inputParsed.error !== "") {
      resultHtml += `<b style = 'color:red'>Error.</b> ${inputParsed.error}`;
    }
    if (
      inputParsed.resultHtml !== "" &&
      inputParsed.resultHtml !== undefined &&
      inputParsed.resultHtml !== null
    ) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += inputParsed.resultHtml;
    }
    let crashReport = inputParsed.crashReport;
    if (crashReport !== undefined && crashReport !== null && crashReport) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += crashReport;
    }
    let timeComputation = inputParsed.timeComputation;
    if (timeComputation !== null && timeComputation !== undefined) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += `<br><span style = 'font-size:x-small;float:right'>Response: ${timeComputation} ms</small>`;
    }
    outputComponent.innerHTML = resultHtml;
    let spanVerification = document.getElementById(this.idVerificationSpan);
    let scripts = spanVerification.getElementsByTagName('script');
    let theHead = document.getElementsByTagName('head')[0];
    for (let i = 0; i < this.numInsertedJavascriptChildren; i++) {
      theHead.removeChild(theHead.lastChild);
    }
    this.numInsertedJavascriptChildren = 0;
    for (let i = 0; i < scripts.length; i++) {
      let scriptChild = document.createElement('script');
      scriptChild.innerHTML = scripts[i].innerHTML;
      scriptChild.type = 'text/javascript';
      theHead.appendChild(scriptChild);
      this.numInsertedJavascriptChildren++;
    }
    this.javascriptInsertionAlreadyCalled = true;
    typeset.typesetter.typesetSoft(outputComponent, "");
  }

  submitOrPreviewAnswers(requestQuery) {
    clearTimeout(this.timerForPreviewAnswers);
    let studentAnswer = document.getElementById(this.idPureLatex).value;
    let theURL = "";
    theURL += `${pathnames.urls.calculatorAPI}?${requestQuery}&`;
    theURL += `calculatorAnswer${this.idPureLatex}=${encodeURIComponent(studentAnswer)}&`;
    theURL += `${pathnames.urlFields.problem.fileName}=${this.problemId}&`;
    submitRequests.submitGET({
      url: theURL,
      progress: ids.domElements.spanProgressReportGeneral,
      callback: this.submitOrPreviewAnswersCallback.bind(this, this.idVerificationSpan),
    });
  }

  showSolution() {
    let theRequest = "";
    let thePage = window.calculator.mainPage;
    let currentProblem = thePage.getProblemById(this.problemId);
    if (!isForRealProblem(currentProblem)) {
      if (thePage.isLoggedIn()) {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemSolution}&`;
      } else {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemSolutionNoLogin}&`;
      }
      theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
    }
    this.submitOrPreviewAnswers(theRequest);
  }

  submitAnswers() {
    let theRequest = "";
    let thePage = window.calculator.mainPage;
    let currentProblem = thePage.getProblemById(this.problemId);
    if (thePage.isLoggedIn()) {
      if (isForRealProblem(currentProblem)) {
        theRequest = `${pathnames.urlFields.request}=${pathnames.urlFields.submitAnswers}`;
      } else {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExercise}&`;
        theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
      }
    } else {
      theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExerciseNoLogin}&`;
      theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
    }
    this.submitOrPreviewAnswers(theRequest);
  }

  submitGiveUp() {
    let thePage = window.calculator.mainPage;
    let currentProblem = thePage.getProblemById(this.problemId);
    let theRequest = "";
    if (thePage.isLoggedIn()) {
      theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemGiveUp}&`;
    } else {
      theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemGiveUpNoLogin}&`;
    }
    if (currentProblem.randomSeed === undefined) {
      throw ("Random seed not supposed to be undefined. ");
    }
    theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
    this.submitOrPreviewAnswers(theRequest);
  }

  submitPreview() {
    let thePage = window.calculator.mainPage;
    let theRequest = "";
    let currentProblem = thePage.getProblemById(this.problemId);
    if (thePage.isLoggedIn()) {
      if (isForRealProblem(currentProblem)) {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitAnswersPreview}&`;
      } else {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExercisePreview}&`;
        theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
      }
    } else {
      theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExercisePreviewNoLogin}&`;
      theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
    }
    this.submitOrPreviewAnswers(theRequest);
  }

  submitPreviewWithTimeOut() {
    clearTimeout(this.timerForPreviewAnswers);
    this.timerForPreviewAnswers = setTimeout(this.submitPreview.bind(this), 4000);
  }

  editMQFunction(
    /**@type{EquationEditor} */
    unused,
    /**@type{equation_editor.MathNode} */
    unusedNode,
  ) {
    // useful event handlers
    if (this.ignoreNextMathQuillUpdateEvent) {
      return;
    }
    let latexBox = document.getElementById(this.idPureLatex);
    if (latexBox === null) {
      return;
    }
    if (this.flagAnswerPanel) {
      latexBox.value = processMathQuillLatex(this.equationEditor.rootNode.toLatex());
      this.submitPreviewWithTimeOut();
      return;
    }
    if (this.flagCalculatorPanel) {
      if (!this.flagCalculatorMQStringIsOK) {
        return;
      }
      let theBoxContent = this.equationEditor.rootNode.toLatex();
      if (this.calculatorLeftString === null || this.calculatorRightString === null) {
        this.editorHelpCalculator();
      }
      let theInserted = processMathQuillLatex(theBoxContent);
      if (theInserted.length > 0 && startingCharacterSectionUnderMathQuillEdit.length > 0) {
        if (theInserted[0] !== ' ') {
          theInserted = ' ' + theInserted;
        }
      }
      document.getElementById(this.idPureLatex).value = this.calculatorLeftString + theInserted + this.calculatorRightString;
    }
  }

  initialize() {
    if (this.flagInitialized) {
      return;
    }
    let currentMQspan = document.getElementById(this.idEquationEditorElement);
    this.equationEditor = new EquationEditor(currentMQspan, new EquationEditorOptions({
      editHandler: (editor, node) => {
        this.editMQFunction(editor, node);
      },
    }));
    let forceShowAll = false;
    let forceShowNone = false;
    if (this.idEquationEditorElement === ids.domElements.pages.solve.editor) {
      forceShowAll = storage.storage.variables.solve.panel.forceShowAll.isTrue();
      forceShowNone = storage.storage.variables.solve.panel.forceShowNone.isTrue();
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

  chopStrings(
    /**@type{number} */
    calculatorLeftPosition,
    /**@type{number} */
    calculatorRightPosition,
  ) {
    let mqCommentsSpan = document.getElementById(this.idEditorComments);
    if (calculatorRightPosition - calculatorLeftPosition > 1000) {
      this.flagCalculatorMQStringIsOK = false;
      mqCommentsSpan.innerHTML = "<span style ='color:red'><b>Formula too big </b></span>";
      return;
    }
    this.flagCalculatorMQStringIsOK = true;
    mqCommentsSpan.innerHTML = "Equation assistant";
    document.getElementById(this.idEquationEditorElement).style.visibility = "visible";
    let calculatorInput = document.getElementById(this.idPureLatex);
    this.theLaTeXString = calculatorInput.value.substring(calculatorLeftPosition, calculatorRightPosition + 1);
    this.calculatorLeftString = calculatorInput.value.substring(0, calculatorLeftPosition);
    this.calculatorRightString = calculatorInput.value.substring(calculatorRightPosition + 1);
  }

  getSemiColumnEnclosure() {
    let startPos = this.selectionEnd;
    let calculatorInput = document.getElementById(this.idPureLatex);
    for (; startPos > 0 && startPos < calculatorInput.value.length; startPos--) {
      if (isSeparatorCharacter(calculatorInput.value[startPos])) {
        break;
      }
    }
    if (startPos >= calculatorInput.value.length) {
      startPos = calculatorInput.value.length - 1;
    }
    let rightPos = startPos;
    let lastSeparator = startPos;
    let lastWord = '';
    let currentChar = 'a';
    for (; rightPos < calculatorInput.value.length - 1; rightPos++) {
      currentChar = calculatorInput.value[rightPos];
      if (currentChar === ';') {
        if (rightPos > 0) {
          rightPos--;
        }
        break;
      }
      if (isSeparatorCharacter(currentChar)) {
        lastWord = '';
        lastSeparator = rightPos;
      } else {
        lastWord += currentChar;
      }
      if (lastWord.length > 3) {
        if (!isKeyWordStartKnownToMathQuill(lastWord)) {
          rightPos = lastSeparator;
          break;
        }
      }
    }
    let calculatorSeparatorCounts = {
      leftSeparators: 0,
      rightSeparators: 0
    };
    let leftPos = rightPos - 1;
    lastWord = '';
    lastSeparator = rightPos;
    for (; leftPos > 0; leftPos--) {
      currentChar = calculatorInput.value[leftPos];
      if (currentChar === ';') {
        leftPos++;
        break;
      }
      if (accountCalculatorDelimiterReturnMustEndSelection(calculatorInput.value[leftPos], calculatorSeparatorCounts)) {
        leftPos++;
        break;
      }
      if (isSeparatorCharacter(currentChar)) {
        lastWord = '';
        lastSeparator = leftPos;
      } else {
        lastWord = currentChar + lastWord;
      }
      if (lastWord.length > 3) {
        if (!isKeyWordEndKnownToMathQuill(lastWord)) {
          leftPos = lastSeparator;
          break;
        }
      }
    }
    if (leftPos > rightPos) {
      leftPos = rightPos;
    }
    if (rightPos - leftPos > 1000) {
      mqProblemSpan.innerHTML = "<span style ='color:red'><b></b></span>"
    }
    startingCharacterSectionUnderMathQuillEdit = '';
    if (calculatorInput.value[leftPos] === '\n' || calculatorInput.value[leftPos] === ' ' ||
      calculatorInput.value[leftPos] === '\t') {
      startingCharacterSectionUnderMathQuillEdit = calculatorInput.value[leftPos];
    }
    this.chopStrings(leftPos, rightPos);
  }

  computeFlags(
    /**@type {boolean} */
    forceShowAll,
  ) {
    let currentButtonPanel = document.getElementById(this.idButtonContainer);
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
      this.addLatexCommand("\\frac{}{}", "/");
      this.addKeySequence(["/"], "(\u2022)/(\u2022)", "font-size:10px"); // \u2022 = bullet.
      this.addLatexCommand("\\sqrt{}", "\u221A");
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
      this.addLatexCommand("\\binom", "binom", "font-size : 7px;");
    }
    if (noOptions || includeAll) {
      this.addLatexCommand("\\circ", "\u25CB");
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
      this.addLatexCommand("{}^{\\circ}", "\u00B0");
    }
    if (this.flagButtons.newtonsMethod.selected || includeAll) {
      this.addLatexCommand(["NewtonsMethod ", "(", ",", ",", ")"], "Newton", "font-size: 6px", false);
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
    this.computeFlags(forceShowAll);
    this.addButtons(forceShowAll, forceShowNone);
    let currentButtonPanel = document.getElementById(this.idButtonContainer);
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

/**@returns {Boolean} */
function isForRealProblem(problem) {
  let isForReal = false;
  if (problem !== null && problem !== undefined) {
    isForReal = problem.flagForReal;
  }
  return isForReal;
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
};