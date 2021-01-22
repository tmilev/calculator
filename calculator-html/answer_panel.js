const miscellaneous = require("./miscellaneous_frontend");
const submitRequests = require("./submit_requests");
const initializeButtons = require("./initialize_buttons");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const answerProcessing = require("./answer_processing").answerProcessing;
const typeset = require("./math_typeset");

class AnswerPanel {
  constructor(
    input,
  ) {
    this.input = input;
    /**@type{HTMLElement|null} */
    this.element = null;
    /**@type{boolean} */
    this.flagForReal = false;
    if (input["forReal"] === true) {
      this.flagForReal = true;
    }
    this.idPureLatex = input.idPureLatex;
    this.problemId = input.problemId;
    /**@type{boolean} */
    this.flagGenerateInterpretButton = true;
    if (input.generateInterpretButton === false) {
      this.flagGenerateInterpretButton = false;
    }
    /**@type{boolean} */
    this.layoutVertical = true;

    /**@type {InputPanelData|null} */
    this.panel = null;
    /**@type{HTMLTableElement|null} */
    this.table = null;
    /**@type{HTMLElement|null} */
    this.buttonContainerMath = null;
    /**@type{HTMLElement|null} */
    this.pureLatexElement = null;
    /**@type{HTMLElement|null} */
    this.verificationSpan = null;
    /**@type{HTMLElement|null} */
    this.solutionSpan = null;
    /**@type{HTMLElement|null} */
    this.buttonInterpret = null;
    /**@type{HTMLElement|null} */
    this.buttonSubmit = null;
    /**@type{HTMLElement|null} */
    this.buttonAnswer = null;
    /**@type{HTMLElement|null} */
    this.buttonSolution = null;
    /**@type{HTMLElement|null} */
    this.buttonDetails = null;
    /**@type{HTMLElement|null} */
    this.spanAnswerHighlight = null;
    /**@type{HTMLElement|null} */
    this.editorSpan = null;
    /**@type{HTMLElement|null} */
    this.editorEnclosure = null;
  }

  writeToElement(
    /**@type{HTMLElement} */
    element,
  ) {
    this.element = element;
    this.element.textContent = "";
    this.createTable();
    this.input.equationEditorContainer = this.editorSpan;
    this.input.buttonContainer = this.buttonContainerMath;
    this.input.pureLatexElement = this.pureLatexElement;
    this.input.valueChangeHandler = () => {
      this.callbackLatexChange();
    };
    this.panel = new InputPanelData(this.input);
    this.panel.initialize();
  }

  createTable() {
    this.layoutVertical = this.input.properties.layout !== "horizontal";
    this.onePanelComputeHtmlElements();
    this.table = document.createElement("table");
    if (this.layoutVertical === true) {
      /**@type{HTMLTableRowElement} */
      let row = this.table.insertRow();
      row.insertCell().appendChild(this.onePanelQuestionAndAnswerField());
      row.insertCell().appendChild(this.buttonContainerMath);
      row.insertCell().appendChild(this.onePanelButtonsVerticalLayout());
      row.insertCell().appendChild(this.pureLatexElement);
    } else {
      let row = this.table.insertRow();
      row.insertCell().appendChild(this.onePanelQuestionAndAnswerField());
      row.insertCell().appendChild(this.onePanelButtonsHorizontalLayout());
      row.insertCell().appendChild(this.buttonContainerMath);
      row.insertCell().appendChild(this.pureLatexElement);
    }
    this.element.appendChild(this.table);
    this.element.appendChild(this.verificationSpan);
    if (this.solutionSpan !== null) {
      this.element.appendChild(this.solutionSpan);
    }
    this.pureLatexElement.addEventListener('keyup', () => {
      this.editLaTeX();
    });

    this.buttonInterpret.addEventListener('click', () => {
      this.submitPreview();
    });
    this.buttonSubmit.addEventListener('click', () => {
      this.submitPreview();
    });
    if (this.flagForReal !== true && this.buttonAnswer !== null) {
      this.buttonAnswer.addEventListener('click', () => {
        this.submitGiveUp();
      });
    }
    if (
      this.buttonSolution !== null
    ) {
      this.buttonSolution.addEventListener('click', () => {
        this.showSolution();
      });
    }
  }

  onePanelComputeHtmlElements() {
    if (this.flagForReal === true) {
      this.buttonAnswer = null;
      this.htmlButtonSolution = null;
    } else {
      if (this.input.idButtonAnswer !== undefined && this.input.buttonAnswer !== null) {
        this.buttonAnswer = document.createElement("button");
        this.buttonAnswer.textContent = "Answer";
        this.buttonAnswer.className = "buttonAnswer";
      }
      if (this.input.idButtonSolution !== null && this.input.idButtonSolution !== undefined) {
        this.buttonSolution = document.createElement("button");
        this.buttonSolution.textContent = "Solution";
        this.buttonSolution.className = "buttonSolution";
      }
    }
    this.buttonSubmit = document.createElement("button");
    this.buttonSubmit.className = "buttonSubmit";
    this.buttonSubmit.textContent = "Submit";
    if (this.flagGenerateInterpretButton) {
      this.buttonInterpret = document.createElement("button");
      this.buttonInterpret.className = "buttonPreview";
      this.buttonInterpret.textContent = "Interpret";
    }
    this.buttonDetails = document.createElement("button");
    this.buttonDetails.className = "accordion";
    this.buttonDetails.textContent = "details";
    this.pureLatexElement = document.createElement("textarea");
    this.pureLatexElement.className = "calculatorAnswer";
    this.buttonContainerMath = document.createElement("div");
    this.buttonContainerMath.className = "mqButtonPanel";
    this.buttonContainerMath.setAttribute("buttons", this.input.mathQuillPanelOptions);
    this.editorEnclosure = document.createElement("div");
    this.editorEnclosure.className = "calculatorMQfieldEnclosure";
    this.editorSpan = document.createElement("span");
    this.spanAnswerHighlight = document.createElement("span");
    if (
      this.input.answerHighlight !== undefined &&
      this.input.answerHighlight !== null
    ) {
      this.spanAnswerHighlight.innerHTML = this.input.answerHighlight;
    }
    this.editorEnclosure.appendChild(this.editorSpan);
    this.verificationSpan = document.createElement("span");
    if (this.input.previousAnswers !== undefined && this.input.previousAnswers !== "") {
      this.verificationSpan.innerHTML = this.input.previousAnswers;
    } else {
      if (this.layoutVertical === true) {
        this.verificationSpan.innerHTML = `<b style = "color:brown">Waiting for answer [unlimited tries]</b>`;
      }
    }
    if (this.flagForReal !== true && this.input.idSpanSolution !== undefined && this.input.idSpanSolution !== null) {
      this.solutionSpan = document.createElement("span");
    }
  }

  /**@returns{HTMLElement} */
  onePanelQuestionAndAnswerField() {
    let result = document.createElement("table");
    let row = result.insertRow();
    row.insertCell().appendChild(this.spanAnswerHighlight);
    let cell = row.insertCell();
    cell.appendChild(this.editorEnclosure);
    cell.className = "tableCellMQfield";
    return result;
  }

  /**@returns{HTMLElement} */
  onePanelButtonsVerticalLayout() {
    let result = document.createElement("table");
    result.insertRow().insertCell().appendChild(this.buttonSubmit);
    if (this.flagGenerateInterpretButton) {
      result.insertRow().insertCell().appendChild(this.buttonInterpret);
    }
    if (this.flagForReal !== true) {
      if (this.buttonAnswer !== null) {
        result.insertRow().insertCell().appendChild(this.buttonAnswer);
      }
      if (this.buttonSolution !== null) {
        result.insertRow().insertCell().appendChild(this.buttonSolution);
      }
    }
    return result;
  }

  /**@returns{HTMLElement} */
  onePanelButtonsHorizontalLayout() {
    let result = document.createElement("table");
    let row = result.insertRow();
    row.insertCell().appendChild(this.buttonSubmit);
    if (this.flagGenerateInterpretButton) {
      row.insertCell().appendChild(this.buttonInterpret);
    }
    if (this.flagForReal !== true) {
      if (this.buttonAnswer !== null) {
        row.insertCell().appendChild(this.buttonAnswer);
      }
      if (this.buttonSolution !== null) {
        row.insertCell().appendChild(this.buttonSolution);
      }
    }
    return result;
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
    let timeComputation = inputParsed.timeComputation;
    if (timeComputation !== null && timeComputation !== undefined) {
      if (resultHtml !== "") {
        resultHtml += "<br>";
      }
      resultHtml += `<br><span style='font-size:x-small;float:right'>Computation time: ${timeComputation} ms</span>`;
    }
    resultHtml += answerProcessing.htmlUserFriendlyResultComparisonErrorsOnly(inputParsed);
    resultHtml += miscellaneous.htmlFromCommentsAndErrors(inputParsed);
    outputComponent.innerHTML = resultHtml;
    let spanVerification = this.verificationSpan;
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

  submitOrPreviewAnswers(
    /**@type{string} */
    requestQuery,
  ) {
    clearTimeout(this.timerForPreviewAnswers);
    let studentAnswer = this.panel.getPureLatexElement().value;
    let theURL = "";
    theURL += `${pathnames.urls.calculatorAPI}?${requestQuery}&`;
    theURL += `calculatorAnswer${this.idPureLatex}=${encodeURIComponent(studentAnswer)}&`;
    theURL += `${pathnames.urlFields.problem.fileName}=${this.problemId}&`;
    submitRequests.submitGET({
      url: theURL,
      progress: ids.domElements.spanProgressReportGeneral,
      callback: this.submitOrPreviewAnswersCallback.bind(this, this.verificationSpan),
    });
  }

  showSolution() {
    let theRequest = "";
    let thePage = window.calculator.mainPage;
    let currentProblem = thePage.getProblemById(this.problemId);
    if (!this.flagForReal) {
      if (thePage.isLoggedIn()) {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemSolution}&`;
      } else {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.problemSolutionNoLogin}&`;
      }
      if (currentProblem.randomSeed !== undefined) {
        theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
      }
    }
    this.submitOrPreviewAnswers(theRequest);
  }

  submitAnswers() {
    let theRequest = "";
    let thePage = window.calculator.mainPage;
    let currentProblem = thePage.getProblemById(this.problemId);
    if (thePage.isLoggedIn()) {
      if (this.flagForReal) {
        theRequest = `${pathnames.urlFields.request}=${pathnames.urlFields.submitAnswers}`;
      } else {
        theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExercise}&`;
        if (currentProblem.randomSeed !== undefined) {
          theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
        }
      }
    } else {
      theRequest += `${pathnames.urlFields.request}=${pathnames.urlFields.submitExerciseNoLogin}&`;
      if (currentProblem.randomSeed !== undefined) {
        theRequest += `${pathnames.urlFields.randomSeed}=${currentProblem.randomSeed}&`;
      }
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
      if (this.flagForReal) {
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

  callbackLatexChange() {
    this.pureLatexElement.value = initializeButtons.processMathQuillLatex(this.panel.equationEditor.rootNode.toLatex());
    this.submitPreviewWithTimeOut();
    return;
  }
}

module.exports = {
  AnswerPanel,
};