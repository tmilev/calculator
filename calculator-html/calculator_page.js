"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const drawing = require("./graphics").drawing;
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require("./miscellaneous_frontend");
const miscellaneous = require("./miscellaneous_frontend");
const BufferCalculator = require("./buffer").BufferCalculator;
const panels = require("./panels");
const processMonitoring = require("./process_monitoring");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const storage = require("./storage");
const autocomplete = require("./autocomplete");
const initializeButtons = require("./initialize_buttons");
const dynamicJavascript = require("./dynamic_javascript").dynamicJavascript;

class AtomHandler {
  constructor() {
    this.description = "";
    this.atom = "";
    this.example = "";
    this.composite = false;
    this.index = - 1;
    this.totalRules = 0;
    this.administrative = false;
    this.experimental = false;
  }

  fromObject(
    input,
    /**@type {number}*/ index,
    /**@type {number}*/ totalRules,
  ) {
    this.index = index;
    this.totalRules = totalRules;
    this.description = input.description;
    this.example = input.example;
    this.atom = input.atom;
    this.ruleName = input.ruleName;
    this.visible = true;
    this.administrative = false;
    this.experimental = false;
    if (input.administrative === "true" || input.administrative === true) {
      this.administrative = true;
    }
    if (input.experimental === "true" || input.experimental === true) {
      this.experimental = true;
    }
    if (input.visible === "false" || input.visible === false) {
      this.visible = false;
    }
    if (input.composite === "true" || input.composite === true) {
      this.composite = true;
    } else {
      this.composite = false;
    }
  }

  toString(
    /**@type {Calculator}*/
    calculator,
  ) {
    let resultString = "";
    resultString += `<calculatorAtom>${this.atom}</calculatorAtom>`;
    if (this.composite) {
      resultString += "<calculatorCompositeAtom>(composite)</calculatorCompositeAtom>";
    }
    resultString += ` (${this.index + 1} out of ${this.totalRules})`;
    let currentId = "example_";
    if (this.composite) {
      currentId += "t_";
    } else {
      currentId += "f_";
    }
    let encodedAtom = encodeURIComponent(this.atom);
    currentId += `${encodedAtom}_${this.index}_${this.totalRules}`;
    resultString += `<button class='accordionLikeIndividual' onclick = "window.calculator.miscellaneousFrontend.switchMenu('${currentId}')">info</button>`;
    resultString += `<calculatorExampleInfo id = "${currentId}" class = "hiddenClass">`;
    if (this.administrative) {
      resultString += "<b>(administrative)</b> ";
    }
    if (this.experimental) {
      resultString += "<b>(experimental)</b> ";
    }
    resultString += this.description;
    resultString += `<br><b>Example:</b>`;
    resultString += `<br>${this.example}</calculatorExampleInfo>`;
    let theLink = calculator.getComputationLink(this.example);
    resultString += `<a href = '#${theLink}' class = "linkInfo">Example</a>`;
    resultString += ` [${this.ruleName}]`;
    return resultString;
  }
}

class Calculator {
  constructor() {
    this.parsedComputation = {};
    /** @type {panels.PanelExpandableData[]}*/
    this.panels = [];
    this.examples = null;
    this.submissionCalculatorCounter = 0;
    this.lastSubmittedInput = "";
    this.numberOfCalculatorPanels = 0;
    processMonitoring.monitor.ownerCalculator = this;
    /**@type{boolean} */
    this.flagTypeset = false;
    /**@type{boolean} */
    this.initialized = false;
    /**@type{InputPanelData|null} */
    this.calculatorPanel = null;
  }

  processOneFunctionAtom(handlers) {
    let resultStrings = [];
    for (let i = 0; i < handlers.length; i++) {
      let handler = new AtomHandler();
      handler.fromObject(handlers[i], i, handlers.length);
      if (handler.visible) {
        resultStrings.push("<br>");
        resultStrings.push(handler.toString(this));
      }
    }
    return resultStrings.join("");
  }

  processExamples(inputJSONtext) {
    try {
      this.examples = miscellaneous.jsonUnescapeParse(inputJSONtext);
      let examplesString = "";
      let atomsSorted = Object.keys(this.examples).slice().sort();
      let numHandlers = 0;
      for (let i = 0; i < atomsSorted.length; i++) {
        let atom = atomsSorted[i];
        let currentExamples = this.examples[atom];
        examplesString += this.processOneFunctionAtom(currentExamples.regular);
        examplesString += this.processOneFunctionAtom(currentExamples.composite);
        numHandlers += this.examples[atom].regular.length + this.examples[atom].composite.length;
      }
      let resultString = `${atomsSorted.length} built-in atoms, ${numHandlers} handlers. `;
      resultString += examplesString;
      document.getElementById(ids.domElements.calculatorExamples).innerHTML = resultString;
      let calculatorElement = document.getElementById(ids.domElements.divCalculatorMainInputOutput);
      calculatorElement.style.maxWidth = "70%";
    } catch (e) {
      console.log(`Bad json: ${e}\n Input JSON follows.`);
      console.log(inputJSONtext);
    }
  }

  submitCalculatorInputOnEnter(event) {
    if (event.keyCode !== 13 || !event.shiftKey) {
      return;
    }
    this.submitComputation();
    event.preventDefault();
  }

  adjustCalculatorPageSize() {
    miscellaneousFrontend.switchMenu(ids.domElements.calculatorExamples);
    let element = document.getElementById(ids.domElements.calculatorExamples);
    let calculatorElement = document.getElementById(ids.domElements.divCalculatorMainInputOutput);
    if (element.classList.contains("hiddenClass")) {
      calculatorElement.style.maxWidth = "100%";
    } else {
      calculatorElement.style.maxWidth = "70%";
    }
  }

  toggleExamples(
    /**@type{HTMLElement} */
    button,
  ) {
    let theExamples = document.getElementById(ids.domElements.calculatorExamples);
    let theURL = "";
    theURL += pathnames.urls.calculatorAPI;
    theURL += `?${pathnames.urlFields.request}=${pathnames.urlFields.requests.calculatorExamplesJSON}`;
    if (theExamples.innerHTML.length < 300) {
      submitRequests.submitGET({
        url: theURL,
        callback: (input) => {
          this.processExamples(input);
        },
        progress: "spanProgressCalculatorExamples"
      });
      button.innerHTML = "&#9660;";
    } else {
      this.adjustCalculatorPageSize();
      if (!theExamples.classList.contains("hiddenClass")) {
        button.innerHTML = "&#9660;";
      } else {
        button.innerHTML = "&#9656;";
      }
    }
  }

  initialize() {
    if (this.initialized) {
      return;
    }
    this.initialized = true;
    this.calculatorPanel = new InputPanelData({
      idEquationEditorElement: "mainInputEditorField",
      idEditorComments: "mqPanelComments",
      problemId: "",
      idPureLatex: ids.domElements.pages.calculator.inputMain,
      idButtonContainer: 'mainInputEditorFieldButtons',
      flagCalculatorPanel: true,
      valueChangeHandler: () => {
        this.equationEditorChangeCallback();
      },
    });
    this.calculatorPanel.initialize();
    document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle).addEventListener(
      "click", () => {
        processMonitoring.monitor.togglePause();
      }
    )
    let inputMain = document.getElementById(ids.domElements.pages.calculator.inputMain);
    inputMain.addEventListener("keypress", (e) => {
      this.submitCalculatorInputOnEnter(e);
    });
    inputMain.addEventListener("keyup", (e) => {
      autocomplete.suggestWord();
      this.calculatorPanel.editorHelpCalculator();
    });
    inputMain.addEventListener("keydown", (e) => {
      autocomplete.suggestWord();
      this.calculatorPanel.editorHelpCalculator();
      autocomplete.arrowAction(e);
    });
    inputMain.addEventListener("mouseup", (e) => {
      autocomplete.suggestWord();
      this.calculatorPanel.editorHelpCalculator();
    });
    inputMain.addEventListener("input", (e) => {
      autocomplete.suggestWord();
      this.calculatorPanel.editorHelpCalculator();
    });
  }
  selectCalculatorPage() {
    this.initialize();
    this.submitComputation();
  }

  submitComputation() {
    processMonitoring.monitor.clearTimeout();
    let calculatorInput = document.getElementById(ids.domElements.pages.calculator.inputMain).value;
    this.typeset();
    if (calculatorInput === this.lastSubmittedInput) {
      return;
    }
    this.lastSubmittedInput = calculatorInput;
    // submitComputationPartTwo is called by a callback in the function below:
    storage.storage.variables.calculator.input.setAndStore(this.lastSubmittedInput);
  }

  /**@returns {String} */
  getComputationLink(input) {
    let theURL = {
      currentPage: "calculator",
      calculatorInput: input,
    };
    let thePage = window.calculator.mainPage;
    let stringifiedHash = thePage.storage.getPercentEncodedURL(theURL);
    return stringifiedHash;
  }

  writeErrorsAndCrashes(
    /**@type {BufferCalculator} */
    buffer,
    inputParsed,
  ) {
    if (
      inputParsed.error !== undefined &&
      inputParsed.error !== null &&
      inputParsed.error !== ""
    ) {
      buffer.write("<b style ='color:red'>Error.</b>");
      buffer.write(inputParsed.error);
    }
    if (
      inputParsed.crashReport !== undefined &&
      inputParsed.crashReport !== null &&
      inputParsed.crashReport !== ""
    ) {
      buffer.write(inputParsed.crashReport);
    }
  }

  writeResult(
    /**@type {BufferCalculator} */
    buffer,
    inputParsed,
    /** @type {panels.PanelExpandableData[]} */
    panelData,
  ) {
    if (inputParsed.commentsGlobal !== "" && inputParsed.commentsGlobal !== undefined) {
      buffer.write(inputParsed.commentsGlobal);
    }
    if (inputParsed.result === undefined && inputParsed.comments !== undefined) {
      buffer.write(inputParsed.comments);
    }
    this.writeErrorsAndCrashes(buffer, inputParsed);
    if (inputParsed.timeOut === true) {
      if (inputParsed.timeOutComments !== undefined) {
        buffer.write(inputParsed.timeOutComments);
      }
      processMonitoring.monitor.start(inputParsed.workerId);
      return;
    }
    if (inputParsed.result === undefined && inputParsed.resultHtml !== undefined) {
      buffer.write(inputParsed.resultHtml);
    }
    if (inputParsed.result === undefined) {
      return;
    }
    buffer.write(`<table><tr><td>`);
    if (inputParsed.syntaxErrors !== undefined) {
      buffer.write(inputParsed.syntaxErrors);
    }
    buffer.write(`<table class='tableCalculatorOutput'><tr><th>Input</th><th>Output</th></tr>`);
    if (typeof inputParsed.result.input === "string") {
      inputParsed.result.input = [inputParsed.result.input];
    }
    if (typeof inputParsed.result.output === "string") {
      inputParsed.result.output = [inputParsed.result.output];
    }
    let numEntries = Math.max(inputParsed.result.input.length, inputParsed.result.output.length);
    for (let i = 0; i < numEntries; i++) {
      this.numberOfCalculatorPanels++;
      let inputPanelId = `calculatorInputPanel${this.numberOfCalculatorPanels}`;
      let outputPanelId = `calculatorOutputPanel${this.numberOfCalculatorPanels}`;
      if (i < inputParsed.result.input.length) {
        panelData.push(new panels.PanelExpandableData(
          inputParsed.result.input[i], inputPanelId, 150,
        ));
      }
      if (i < inputParsed.result.output.length) {
        panelData.push(new panels.PanelExpandableData(
          inputParsed.result.output[i], outputPanelId, 150,
        ));
      }
      buffer.write(`<tr>`);
      buffer.write(`<td class='cellCalculatorInput'><div id='${inputPanelId}'></div></td>`);
      buffer.write(`<td class='cellCalculatorResult'><div id='${outputPanelId}'></div></td>`);
      buffer.write(`</tr>`);
    }
    buffer.write(`</table>`);
    buffer.write(`</td><td><div class="containerComments">`);
    buffer.write("<small>Double-click formulas: get LaTeX. Double-click back: hide. </small>");
    let performance = inputParsed[pathnames.urlFields.result.performance];
    if (performance !== undefined) {
      let content = performance[pathnames.urlFields.result.comments];
      let label = `<b style='color:blue'>${performance[pathnames.urlFields.result.computationTime]}</b>`;
      buffer.write(`<div id='${ids.domElements.divPerformance}'></div>`);
      panelData.push(new panels.PanelExpandableData(
        content,
        ids.domElements.divPerformance,
        0,
        true,
        label
      ));
      buffer.write("<br>");
    }
    if (inputParsed.comments !== undefined) {
      buffer.write(inputParsed.comments);
    }
    buffer.write(`</div></td>`);
    let mainPage = window.calculator.mainPage;
    if (mainPage.storage.variables.flagDebug.isTrue() && inputParsed.debug !== undefined) {
      buffer.write(`<td>`);
      buffer.write(inputParsed.debug);
      buffer.write(`</td>`);
    }
    buffer.write(`</tr></table>`);
    if (inputParsed.parsingLog !== undefined) {
      buffer.write(inputParsed.parsingLog);
    }
  }

  typeset() {
    if (storage.storage.variables.currentPage.getValue() !== "calculator") {
      return;
    }
    if (this.flagTypeset === true) {
      return;
    }
    dynamicJavascript.typeset(this.getOutputElement(), {
      "lineBreakWidth": 600,
    });
    this.flagTypeset = true;
  }

  afterWriteOutput() {
    for (let i = 0; i < this.panels.length; i++) {
      panels.makePanelFromData(this.panels[i]);
    }
    this.flagTypeset = false;
    this.typeset();
    dynamicJavascript.bootstrapAllScripts(this.getOutputElement());
  }

  getOutputElement() {
    return document.getElementById(ids.domElements.spanCalculatorMainOutput);
  }

  defaultOnLoadInjectScriptsAndProcessLaTeX(input, output) {
    let inputHtml = null;
    this.panels.length = 0;
    try {
      this.parsedComputation = miscellaneous.jsonUnescapeParse(input);
      let buffer = new BufferCalculator();
      let progReportTimer = document.getElementById(
        ids.domElements.pages.calculator.monitoring.progressTimer,
      );
      progReportTimer.innerHTML = "";
      this.writeResult(buffer, this.parsedComputation, this.panels);
      inputHtml = buffer.toString();
    } catch (e) {
      inputHtml = input + "<br>" + e;
      console.log("Error processing calculator output: " + e);
    }
    this.getOutputElement().innerHTML = inputHtml;
    this.afterWriteOutput();
  }

  helpCalculator() {
    if (!this.initialized) {
      return;
    }
    this.calculatorPanel.editorHelpCalculator();
  }

  submitComputationPartTwo(input) {
    //<- this function is called by a callback trigerred when calling
    //thePage.storage.variables.calculator.input.setAndStore(...)
    let thePage = window.calculator.mainPage;
    let urlCopy = Object.assign({}, thePage.storage.urlObject);
    urlCopy.inputFocus = true;
    let stringifiedHash = thePage.storage.getPercentEncodedURL(urlCopy);
    document.getElementById("spanComputationLink").innerHTML = `<a href = '#${stringifiedHash}'>Link to your input</a>`;
    setTimeout(() => {
      this.helpCalculator();
    }, 0);
    let url = pathnames.urls.calculatorAPI;
    let parameters = this.getQueryStringSubmitStringAsMainInput(input, pathnames.urlFields.calculatorCompute);
    submitRequests.submitPOST({
      url: url,
      parameters: parameters,
      callback: this.defaultOnLoadInjectScriptsAndProcessLaTeX.bind(this),
      progress: ids.domElements.pages.calculator.progress,
    });
  }

  getQueryStringSubmitStringAsMainInput(theString, requestType) {
    let inputParams = '';
    let thePage = window.calculator.mainPage;
    inputParams += `${pathnames.urlFields.request}=${requestType}&`;
    inputParams += `${pathnames.urlFields.requests.calculatorInput}=${encodeURIComponent(theString)}&`;
    if (thePage.storage.variables.flagDebug.isTrue()) {
      inputParams += `${pathnames.urlFields.debugFlag}=true&`;
    }
    if (thePage.storage.variables.calculator.monitoring.value === "false") {
      inputParams += `${pathnames.urlFields.requests.monitoring}=false&`;
    } else {
      inputParams += `${pathnames.urlFields.requests.monitoring}=true&`;
    }
    return inputParams;
  }

  submitStringAsMainInput(theString, idOutput, requestType, onLoadFunction, idStatus) {
    let inputParams = this.getQueryStringSubmitStringAsMainInput(theString, requestType);
    this.submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus);
  }

  equationEditorChangeCallback() {
    if (!this.calculatorPanel.flagCalculatorMQStringIsOK) {
      return;
    }
    let theBoxContent = this.equationEditor.rootNode.toLatex();
    if (this.calculatorLeftString === null || this.calculatorRightString === null) {
      this.editorHelpCalculator();
    }
    let theInserted = initializeButtons.processMathQuillLatex(theBoxContent);
    if (theInserted.length > 0 && startingCharacterSectionUnderMathQuillEdit.length > 0) {
      if (theInserted[0] !== ' ') {
        theInserted = ' ' + theInserted;
      }
    }
    latexBox.value = this.calculatorLeftString + theInserted + this.calculatorRightString;
  }
}

let calculator = new Calculator();

module.exports = {
  calculator,
};
