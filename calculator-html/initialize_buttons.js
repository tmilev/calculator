"use strict";
const submitRequests = require("./submit_requests");
const panels = require("./panels");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous");
const equation_editor = require("./equation_editor");
const EquationEditor = require("./equation_editor").EquationEditor;
const EquationEditorOptions = require("./equation_editor").EquationEditorOptions;

var calculatorLeftPosition = 0;
var calculatorRightPosition = 0;

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
var panelDataRegistry = {};

function processMathQuillLatex(theText) {
  for (var i = 0; i < theText.length; i++) {
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
    for (var i = 0; i < theText.length - 1; i++) {
      for (var j = 0; j < charsToSplit.length; j++) {
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

var MathQuillCommandButtonCollection = {};

class MathQuillCommandButton {
  constructor(inputCommand, inputLabel, inputAdditionalStyle, doWriteInsteadOfCmdInput, inputExtraDirection) {
    this.theCommand = inputCommand;
    this.theLabel = inputLabel;
    this.isComposite = false;
    this.extraDirection = inputExtraDirection;
    this.additionalStyle = inputAdditionalStyle;
    if (typeof (inputCommand) !== "string") {
      this.isComposite = true;
    }
    this.id = "";
    if (!this.isComposite) {
      this.id = this.theCommand;
    } else {
      this.id = "";
      for (var i = 0; i < inputCommand.length; i++) {
        this.id += inputCommand[i];
      }
    }
    if (doWriteInsteadOfCmdInput !== undefined) {
      this.doWriteInsteadOfCmd = doWriteInsteadOfCmdInput;
    } else {
      this.doWriteInsteadOfCmd = false;
    }
    this.idEscaped = "";
    for (var i = 0; i < this.id.length; i++) {
      this.idEscaped += this.id[i];
      if (this.id[i] === '\\') {
        this.idEscaped += "\\";
      }
    }
  }

  getButtonId(inputPanel) {
    return `${encodeURIComponent(inputPanel.problemId)}_${encodeURIComponent(inputPanel.idButtonContainer)}_${encodeURIComponent(this.id)}`;
  }

  getButton(inputPanel) {
    var resultString = "";
    resultString += "<button class = 'buttonMQ'";
    if (this.additionalStyle !== undefined) {
      resultString += ` style ='${this.additionalStyle}'`;
    }
    resultString += ` id ='${this.getButtonId(inputPanel)}'`
    resultString += `>${this.theLabel}</button>`;
    return resultString;
  }

  clickFunction(
    /**@type{InputPanelData} */
    inputPanel,
  ) {
    let editor = inputPanel.equationEditor;
    var doCMD = !this.doWriteInsteadOfCmd;
    if (!this.isComposite) {
      if (doCMD) {
        editor.writeLatexToFocused(this.theCommand);
      } else {
        editor.writeLatex(this.theCommand);
      }
    } else {
      for (var i = 0; i < this.theCommand.length; i++) {
        if (!doCMD) {
          if (
            this.theCommand[i] === '(' ||
            this.theCommand[i] === ')' ||
            this.theCommand[i] === ' '
          ) {
            doCMD = true;
          }
        }
        if (doCMD) {
          editor.writeLatexToFocused(this.theCommand[i]);
        } else {
          editor.writeLatex(this.theCommand[i]);
        }
      }
    }
    if (this.extraDirection !== undefined) {
      console.log("DEBUG: Extra direction not implemented");
    }
  }
}

function mathQuillCommandButton(inputCommand, inputLabel, additionalStyle, doWriteInsteadOfCmdInput, inputExtraDirection) {
  var commandObject = new MathQuillCommandButton(
    inputCommand, inputLabel, additionalStyle, doWriteInsteadOfCmdInput, inputExtraDirection
  );
  MathQuillCommandButtonCollection[commandObject.id] = commandObject;
}

mathQuillCommandButton("+", "+");
mathQuillCommandButton("-", "-");
mathQuillCommandButton("*", "*");
mathQuillCommandButton("/", "/");
mathQuillCommandButton("sqrt", "&#8730;");
mathQuillCommandButton("nthroot", "&#8731;");
mathQuillCommandButton("^", "^");
mathQuillCommandButton("(", "(");
mathQuillCommandButton(")", ")");
mathQuillCommandButton(",", ",");
mathQuillCommandButton("[", "[");
mathQuillCommandButton("]", "]");
mathQuillCommandButton("i", "i");
mathQuillCommandButton("x", "x");
mathQuillCommandButton("y", "y");
mathQuillCommandButton("=", "=");
mathQuillCommandButton(["log", "_"], "log_", "font-size:10px; ");
mathQuillCommandButton("_", "_");
mathQuillCommandButton("ln", "ln");
mathQuillCommandButton("e", "e");
mathQuillCommandButton("arcsin", "asin", "font-size:7px");
mathQuillCommandButton("arccos", "acos", "font-size:7px");
mathQuillCommandButton("arctan", "atan", "font-size:7px");


mathQuillCommandButton("sin", "sin", "font-size:10px; ");
mathQuillCommandButton("cos", "cos", "font-size:10px; ");
mathQuillCommandButton("tan", "tan", "font-size:10px; ");
mathQuillCommandButton("cot", "cot", "font-size:10px; ");
mathQuillCommandButton("sec", "sec", "font-size:10px; ");
mathQuillCommandButton("csc", "csc", "font-size:10px; ");

mathQuillCommandButton("emptyset", "&#8709;");
mathQuillCommandButton("alpha", "&alpha;");
mathQuillCommandButton("beta", "&beta;");
mathQuillCommandButton("gamma", "&gamma;");
mathQuillCommandButton("theta", "&theta;");
mathQuillCommandButton("pi", "&pi;");
mathQuillCommandButton("sum", "&#8721;");
mathQuillCommandButton("infty", "&#8734;");
mathQuillCommandButton(" DNE ", "DNE", "font-size: 7px");
mathQuillCommandButton("!", "!");
mathQuillCommandButton("binom", "binom", "font-size : 7px;");
mathQuillCommandButton("cup", "&#8746;");
mathQuillCommandButton(" or ", "or");
mathQuillCommandButton("in", "&#8712;");
mathQuillCommandButton(["^", "circ"], "&#176;");
mathQuillCommandButton("circ", "&#9675;");
mathQuillCommandButton(["NewtonsMethod ", "(", ",", ",", ")"], "Newton", "font-size: 6px", false);
mathQuillCommandButton("\\begin{pmatrix} \\\\ \\end{pmatrix}", "2x1", "font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} \\\\ \\\\ \\end{pmatrix}", "3x1", "font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} & \\\\ & \\end{pmatrix}", "2x2", "font-size : 7px;", true);
mathQuillCommandButton("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}", "3x3", "font-size : 7px;", true);

mathQuillCommandButton("\\mathbf{i}", "i", "font-weight: bold");
mathQuillCommandButton("\\mathbf{j}", "j", "font-weight: bold");
mathQuillCommandButton("\\mathbf{k}", "k", "font-weight: bold");

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
      var theProps = Object.getOwnPropertyNames(panelsCollapseStatus);
      for (var i = 0; i < theProps.length; i++) {
        var current = panelsCollapseStatus[theProps[i]];
        if (current.isCollapsed) {
          panels.toggleHeight(document.getElementById(current.button), theProps[i]);
        }
      }
    }
  }
  var acc = document.getElementsByClassName("accordion");
  for (i = 0; i < acc.length; i++) {
    acc[i].onclick = function () {
      if (this.firstLoad === undefined) {
        this.firstLoad = true;
        var theDeadlines = this.nextElementSibling.getElementsByClassName("modifyDeadlineInput");
        for (var j = 0; j < theDeadlines.length; j++) {
          $('#' + theDeadlines[j].id).datepicker();
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

function initializeCalculatorPage() {
  calculatorPanel.initialize();
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
    //to serve autocomplete:
    this.idMQSpan = input.idMQSpan;
    this.idMQSpanLocation = input.idMQSpanLocation;
    this.idMQcomments = input.idMQcomments;
    this.problemId = input.problemId;

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
    this.idExpandCollapseToggle = input.idButtonContainer + "_expand_collapse_toggle";
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
    //just in case we forget some entry above:
    Object.assign(this, input);
    panelDataRegistry[this.idButtonContainer] = this;
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
    if (this.idMQcomments === undefined) {
      this.idMQcomments = null;
    }
  }

  mQHelpCalculator() {
    this.getSemiColumnEnclosure();
    if (this.equationEditor === null) {
      return;
    }
    this.ignoreNextMathQuillUpdateEvent = true;
    if (this.flagCalculatorMQStringIsOK) {
      this.equationEditor.rootNode.toLatex(this.theLaTeXString);
    }
    this.ignoreNextMathQuillUpdateEvent = false;
  }

  submitOrPreviewAnswersCallback(outputComponent, input) {
    if (typeof outputComponent === "string") {
      outputComponent = document.getElementById(outputComponent);
    }
    var inputParsed = miscellaneous.jsonUnescapeParse(input);
    var resultHtml = "";
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
    var crashReport = inputParsed.crashReport;
    if (crashReport !== undefined && crashReport !== null && crashReport) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += crashReport;
    }
    var timeComputation = inputParsed.timeComputation;
    if (timeComputation !== null && timeComputation !== undefined) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += `<br><span style = 'font-size:x-small;float:right'>Response: ${timeComputation} ms</small>`;
    }
    outputComponent.innerHTML = resultHtml;
    var spanVerification = document.getElementById(this.idVerificationSpan);
    var scripts = spanVerification.getElementsByTagName('script');
    var theHead = document.getElementsByTagName('head')[0];
    for (var i = 0; i < this.numInsertedJavascriptChildren; i++) {
      theHead.removeChild(theHead.lastChild);
    }
    this.numInsertedJavascriptChildren = 0;
    for (var i = 0; i < scripts.length; i++) {
      var scriptChild = document.createElement('script');
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
    var studentAnswer = document.getElementById(this.idPureLatex).value;
    var theURL = "";
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
    var theRequest = "";
    var thePage = window.calculator.mainPage;
    var currentProblem = thePage.getProblemById(this.problemId);
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
    var theRequest = "";
    var thePage = window.calculator.mainPage;
    var currentProblem = thePage.getProblemById(this.problemId);
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
    var thePage = window.calculator.mainPage;
    var currentProblem = thePage.getProblemById(this.problemId);
    var theRequest = "";
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
    var thePage = window.calculator.mainPage;
    var theRequest = "";
    var currentProblem = thePage.getProblemById(this.problemId);
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

  submitPreviewWithTimeOut() { // useful event handlers
    clearTimeout(this.timerForPreviewAnswers);
    this.timerForPreviewAnswers = setTimeout(this.submitPreview.bind(this), 4000);
  }

  editLaTeX() { // useful event handlers
    this.ignoreNextMathQuillUpdateEvent = true;
    this.mqObject.latex(document.getElementById(this.idPureLatex).value + ' ');
    this.ignoreNextMathQuillUpdateEvent = false;
    this.submitPreviewWithTimeOut();
  }

  editMQFunction(
    /**@type{EquationEditor} */
    unused,
    /**@type{equation_editor.MathNode} */
    unusedNode,
  ) { // useful event handlers
    if (this.ignoreNextMathQuillUpdateEvent) {
      return;
    }
    if (this.flagAnswerPanel) {
      document.getElementById(this.idPureLatex).value = processMathQuillLatex(this.equationEditor.rootNode.toLatex());
      this.submitPreviewWithTimeOut();
      return;
    }
    if (this.flagCalculatorPanel) {
      if (!this.flagCalculatorMQStringIsOK) {
        return;
      }
      var theBoxContent = this.equationEditor.rootNode.toLatex();
      if (this.calculatorLeftString === null || this.calculatorRightString === null) {
        this.mQHelpCalculator();
      }
      var theInserted = processMathQuillLatex(theBoxContent);
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
    let currentMQspan = document.getElementById(this.idMQSpan);
    this.equationEditor = new EquationEditor(currentMQspan, new EquationEditorOptions({
      editHandler: (editor, node) => {
        this.editMQFunction(editor, node);
      },
    }));
    this.initializePartTwo(false);
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

  chopStrings() {
    var mqCommentsSpan = document.getElementById(this.idMQcomments);
    if (calculatorRightPosition - calculatorLeftPosition > 1000) {
      this.flagCalculatorMQStringIsOK = false;
      mqCommentsSpan.innerHTML = "<span style ='color:red'><b>Formula too big </b></span>";
      return;
    }
    this.flagCalculatorMQStringIsOK = true;
    mqCommentsSpan.innerHTML = "Equation assistant";
    document.getElementById(this.idMQSpan).style.visibility = "visible";
    var calculatorInput = document.getElementById(this.idPureLatex);
    this.theLaTeXString = calculatorInput.value.substring(calculatorLeftPosition, calculatorRightPosition + 1);
    this.calculatorLeftString = calculatorInput.value.substring(0, calculatorLeftPosition);
    this.calculatorRightString = calculatorInput.value.substring(calculatorRightPosition + 1);
  }

  getSemiColumnEnclosure() {
    var startPos = this.selectionEnd;
    var calculatorInput = document.getElementById(this.idPureLatex);
    for (; startPos > 0 && startPos < calculatorInput.value.length; startPos--) {
      if (isSeparatorCharacter(calculatorInput.value[startPos])) {
        break;
      }
    }
    if (startPos >= calculatorInput.value.length) {
      startPos = calculatorInput.value.length - 1;
    }
    var rightPos = startPos;
    var lastSeparator = startPos;
    var lastWord = '';
    var currentChar = 'a';
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
    var calculatorSeparatorCounts = {
      leftSeparators: 0,
      rightSeparators: 0
    };
    var leftPos = rightPos - 1;
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
    calculatorLeftPosition = leftPos;
    calculatorRightPosition = rightPos;
    startingCharacterSectionUnderMathQuillEdit = '';
    if (calculatorInput.value[leftPos] === '\n' || calculatorInput.value[leftPos] === ' ' ||
      calculatorInput.value[leftPos] === '\t') {
      startingCharacterSectionUnderMathQuillEdit = calculatorInput.value[leftPos];
    }
    this.chopStrings();
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

  initializePartTwo(forceShowAll) {
    this.computeFlags(forceShowAll);
    let buttonBindings = [];
    function addCommand(theCmd) {
      buttonBindings.push(MathQuillCommandButtonCollection[theCmd]);
    }
    let currentButtonPanel = document.getElementById(this.idButtonContainer);
    let noOptions = this.flagButtons.noPreference.selected;
    let includeAll = this.flagButtons.all.selected;
    if (this.flagButtons.algebra.selected || noOptions || includeAll) {
      addCommand("+");
      addCommand("-");
      addCommand("*");
      addCommand("/");

      addCommand("sqrt");
      addCommand("nthroot");
      addCommand("^");
      addCommand("(");
      addCommand(")");
    }
    if (this.flagButtons.trigonometry.selected || includeAll) {
      addCommand("sin");
      addCommand("cos");
      addCommand("tan");
      addCommand("cot");
      addCommand("sec");
      addCommand("csc");
    }
    if (this.flagButtons.inverseTrigonometry.selected || includeAll) {
      addCommand("arcsin");
      addCommand("arccos");
      addCommand("arctan");
    }
    if (this.flagButtons.comma.selected || includeAll) {
      addCommand(",");
    }
    if (this.flagButtons.brackets.selected || includeAll) {
      addCommand("[");
      addCommand("]");
    }
    if (this.flagButtons.complex.selected || includeAll) {
      addCommand("i");
    }
    if (this.flagButtons.variables.selected || includeAll) {
      addCommand("x");
      addCommand("y");
      addCommand("=");
    }
    if (this.flagButtons.logarithms.selected || noOptions || includeAll) {
      addCommand("log_");
      addCommand("_");
      addCommand("ln");
      addCommand("e");
    }
    if (this.flagButtons.infinity.selected || includeAll || noOptions) {
      addCommand("infty");
    }
    if (this.flagButtons.limits.selected || includeAll) {
      addCommand(" DNE ");
    }
    if (this.flagButtons.series.selected || noOptions || includeAll) {
      addCommand("binom");
      addCommand("!");
      addCommand("sum");
    }
    if (noOptions || includeAll) {
      addCommand("circ");
    }
    if (this.flagButtons.logical.selected || noOptions || includeAll) {
      addCommand(" or ");
    }
    if (this.flagButtons.setOperations.selected || noOptions || includeAll) {
      addCommand("cup");
      addCommand("in");
      addCommand("emptyset");
    }
    if (this.flagButtons.matrix.selected || includeAll) {
      addCommand("\\begin{pmatrix} \\\\ \\end{pmatrix}");
      addCommand("\\begin{pmatrix} \\\\ \\\\ \\end{pmatrix}");
      addCommand("\\begin{pmatrix} & \\\\ & \\end{pmatrix}");
      addCommand("\\begin{pmatrix} & & \\\\ & & \\\\ & & \\end{pmatrix}");
    }
    if (this.flagButtons.angles.selected || noOptions || includeAll) {
      addCommand("pi");
      addCommand("^circ");
      addCommand("alpha");
      addCommand("beta");
      addCommand("gamma");
      addCommand("theta");
    }
    if (this.flagButtons.newtonsMethod.selected || includeAll) {
      addCommand("NewtonsMethod (,,)");
    }
    let theContent = "<table>";
    for (let j = 0; j < buttonBindings.length; j++) {
      if (j % this.buttonsPerLine === 0) {
        if (j !== 0) {
          theContent += "</tr>";
        }
        theContent += "<tr>";
      }
      theContent += "<td>" + buttonBindings[j].getButton(this) + "</td>";
    }
    if (buttonBindings.length > 0) {
      theContent += "</tr>";
    }
    theContent += "</table>";
    theContent += `<button href = '#' id = '${this.idExpandCollapseToggle}' class = "buttonShowExpandMQPanel"><small>Show all</small></button>`;
    let oldHeight = window.getComputedStyle(currentButtonPanel).height;
    //console.log("oldHeight: " + oldHeight);
    currentButtonPanel.style.maxHeight = "";
    currentButtonPanel.style.height = "";
    currentButtonPanel.innerHTML = theContent;
    var toggleElement = document.getElementById(this.idExpandCollapseToggle);
    toggleElement.addEventListener(
      'click',
      clickExpandPanel.bind(
        null,
        this.idButtonContainer,
        !forceShowAll && !includeAll,
      )
    );
    for (var j = 0; j < buttonBindings.length; j++) {
      document.getElementById(buttonBindings[j].getButtonId(this)).addEventListener(
        'click', buttonBindings[j].clickFunction.bind(buttonBindings[j], this)
      );
    }
    if (oldHeight !== 0 && oldHeight !== "0px") {
      var newHeight = window.getComputedStyle(currentButtonPanel).height;
      currentButtonPanel.style.maxHeight = oldHeight;
      currentButtonPanel.style.height = oldHeight;
      setTimeout(function () {
        panels.modifyHeightForTimeout(currentButtonPanel, newHeight)
      }, 0);
    }
    return false;
  }

}


/**@returns {Boolean} */
function isForRealProblem(problem) {
  var isForReal = false;
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
  for (var i = 0; i < keyWordsKnownToMathQuill.length; i++) {
    if (keyWordsKnownToMathQuill[i].startsWith(input)) {
      return true;
    }
  }
  return false;
}

function isKeyWordEndKnownToMathQuill(input) {
  for (var i = 0; i < keyWordsKnownToMathQuill.length; i++) {
    if (keyWordsKnownToMathQuill[i].endsWith(input)) {
      return true;
    }
  }
  return false;
}

function clickExpandPanel(panelId, forceShowAll) {
  window.calculator.initializeButtons.panelDataRegistry[panelId].initializePartTwo(forceShowAll);
}

var calculatorPanel = new InputPanelData({
  idMQSpan: "mainInputMQfield",
  idMQcomments: "mqPanelComments",
  problemId: "",
  idPureLatex: ids.domElements.inputMain,
  idButtonContainer: 'mainInputMQfieldButtons',
  flagCalculatorPanel: true,
});

module.exports = {
  panelDataRegistry,
  initializeAccordionButtons,
  initializeButtons,
  initializeCalculatorPage,
  calculatorPanel,
  InputPanelData,
};