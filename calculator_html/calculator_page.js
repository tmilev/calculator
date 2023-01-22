"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require("./miscellaneous_frontend");
const panels = require("./panels");
const processMonitoring = require("./process_monitoring");
const storage = require("./storage");
const dynamicJavascript = require("./dynamic_javascript").dynamicJavascript;
const calculatorPageEditor = require("./calculator_page_editor");
const equationEditor = require("./equation_editor");
const startCalculatorWebAssemblyWorker = require("./web_assembly_worker");

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
    this.shown = false;
    /** @type{HTMLElement|null} */
    this.panel = null;
    /** @type{HTMLElement|null} */
    this.button = null;
  }

  fromObject(
    input,
    /** @type {number} */
    index,
    /** @type {number} */
    totalRules,
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

  toHTMLInfoButton() {
    this.button = document.createElement("button");
    this.button.textContent = 'info\u25C2';
    this.button.className = 'accordionLikeIndividual';
    this.button.addEventListener('click', () => {
      this.togglePanel();
    });
    return this.button;
  }

  togglePanel() {
    this.shown = !this.shown;
    if (this.shown) {
      this.panel.classList.remove("hiddenClass");
      this.button.textContent = "info\u25BE";
    } else {
      this.panel.classList.add("hiddenClass");
      this.button.textContent = 'info\u25C2';
    }
  }

  /** @return{HTMLElement} */
  toHTMLInfo() { 
    let result = document.createElement("div");
    let countElement = document.createElement("span");
    countElement.textContent = `(${this.index + 1} out of ${this.totalRules})`;
    result.appendChild(countElement);
    result.appendChild(document.createElement("br"));
    if (this.composite) {
      let extraInfo = document.createElement("span");
      extraInfo.className = "calculatorCompositeAtom";
      extraInfo.textContent = "(composite) ";
      result.appendChild(extraInfo);
    }
    if (this.administrative) {
      let element = document.createElement("b");
      element.textContent = "(administrative)";
      result.appendChild(element);
    }
    if (this.experimental) {
      let element = document.createElement("b");
      element.textContent = "(experimental)";
      result.appendChild(element);      
    }
    let infoElement = document.createElement("span");
    infoElement.textContent = this.description;
    infoElement.className = "calculatorExamplePanel";
    result.appendChild(infoElement);
    let example = document.createElement("b");
    example.textContent = "Example:";
    result.append(example);
    result.append(document.createElement("br"));
    let exampleElement = document.createElement("span");
    exampleElement.textContent = `${this.example}`;
    exampleElement.className = "calculatorExampleInfo";
    result.appendChild(exampleElement);
    let handlerLabel = document.createElement("b");
    handlerLabel.textContent = "Handler:";
    result.appendChild(handlerLabel);
    result.appendChild(document.createElement("br"));
    let handlerName = document.createTextNode(`${this.ruleName}`);
    result.appendChild(handlerName);
    result.className = "calculatorExamplePanel";
    result.classList.add("hiddenClass");
    return result;
  }
  
  /** @return{HTMLElement} */
  toHTML(
    /** @type {Calculator} */
    calculator,
  ) {
    let result = document.createElement("div");
    let anchor = document.createElement("a");
    anchor.className = "linkInfo";
    let link = calculator.getComputationLink(this.example);
    anchor.href = "#" + link;
    anchor.textContent = this.atom;
    this.panel = this.toHTMLInfo();
    result.appendChild(this.toHTMLInfoButton());
    result.appendChild(this.panel);
    result.appendChild(anchor);
    if (this.totalRules > 1) {
      let countElement = document.createElement("span");
      countElement.textContent = `(${this.index + 1})`;
      result.appendChild(countElement);
    }
    return result;
  }
}

class Splitter {
  constructor() {
    /** @type {HTMLElement} */
    this.element = document.getElementById(
      ids.domElements.pages.calculator.divCalculatorSplitter
    );
    this.parentElement = document.getElementById(
      ids.domElements.pages.calculator.divCalculatorMainInputOutput
    );
    this.resizing = false;
  }

  initialize() {
    this.element.addEventListener('mousedown', (mouseEvent) => {
      this.onMouseDown(mouseEvent);
    });    
    this.parentElement.addEventListener('mousemove', (mouseEvent) => {
      this.onMouseMove(mouseEvent);
    });
    this.parentElement.addEventListener('mouseup', (mouseEvent) => {
      this.onMouseUp(mouseEvent);
    });
  }

  getXY(mouseEvent) {
    return {
      x: mouseEvent.clientX,
      y: mouseEvent.clientY,
    }
  }

  onMouseDown(mouseEvent) {
    this.resizing = true;
    mouseEvent.stopPropagation();
  }

  onMouseUp(mouseEvent) {
    this.resizing = false;
    mouseEvent.stopPropagation();
  }

  onMouseMove(mouseEvent) {
    if (!this.resizing) {
      return;
    }
    let xy = this.getXY(mouseEvent);
    let y = xy.y;
    if (y < 100) {
      y = 100;
    }
    this.setHeight(y - 10);
    storage.storage.variables.calculator.splitterInputOutput.setAndStore(
      y - 10, false, false
    );
  }

  setHeight(y) {
    let mainOutput = document.getElementById(
      ids.domElements.pages.calculator.divCalculatorMainInput
    );
    mainOutput.style.height = y;
  }
}

class Calculator {
  constructor() {
    this.parsedComputation = {};
    /** @type {panels.PanelExpandableData[]} */
    this.panels = [];
    /** @type{HTMLElement|null} */
    this.outputElement = null;
    this.examples = null;
    this.submissionCalculatorCounter = 0;
    this.lastSubmittedInput = "";
    this.numberOfCalculatorPanels = 0;
    this.flagExamplesWantedShown = false;
    processMonitoring.monitor.ownerCalculator = this;
    /** @type{boolean} */
    this.flagTypeset = false;
    /** @type{boolean} */
    this.initialized = false;
    this.editor = new calculatorPageEditor.CalculatorEquationEditor((event) => {
      this.submitCalculatorInputOnEnter(event);
    });
    this.splitter = new Splitter();
  }

  /** @return{HTMLElement} */
  processOneFunctionAtom(handlers) {
    let result = document.createElement("div");
    for (let i = 0; i < handlers.length; i++) {
      let handler = new AtomHandler();
      handler.fromObject(handlers[i], i, handlers.length);
      if (handler.visible) {
        result.appendChild(handler.toHTML(this));
      }
    }
    return result;
  }

  processExamples(inputJSONtext) {
    try {
      this.examples = miscellaneousFrontend.jsonUnescapeParse(inputJSONtext);
      
      let atomsSorted = Object.keys(this.examples).slice().sort();
      let numHandlers = 0;
      let allElements = [];
      for (let i = 0; i < atomsSorted.length; i++) {
        let atom = atomsSorted[i];
        let currentExamples = this.examples[atom];
        allElements.push(this.processOneFunctionAtom(currentExamples.regular));
        allElements.push(this.processOneFunctionAtom(currentExamples.composite));
        numHandlers += this.examples[atom].regular.length + this.examples[atom].composite.length;
      }
      let handlerReport = document.createElement("span");
      handlerReport.textContent = `${atomsSorted.length} built-in atoms, ${numHandlers} handlers. `;
      let output = document.getElementById(ids.domElements.calculatorExamples);
      output.appendChild(handlerReport);
      for (let i = 0; i < allElements.length; i++) {
        output.appendChild(allElements[i]); 
      }      
    } catch (e) {
      console.log(`Bad json: ${e}\n Input JSON follows.`);
      console.log(inputJSONtext);
      throw e;
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
    let calculatorElement = document.getElementById(
      ids.domElements.divCalculatorMainInputOutput
    );
    if (element.classList.contains("hiddenClass")) {
      calculatorElement.style.maxWidth = "100%";
    } else {
      calculatorElement.style.maxWidth = "80%";
    }
  }

  changeSplitterInputOutput(
    /** @type{string} */
    value,
  ) {
    this.splitter.setHeight(value);    
  }

  toggleEquationEditor() {
    let hideEditor = storage.storage.variables.calculator.hideEquationEditor.isTrue();
    hideEditor = !hideEditor;
    storage.storage.variables.calculator.hideEquationEditor.setAndStore(
      hideEditor
    );
    this.doToggleEditor(hideEditor)
  }

  doToggleEditor(/** @type{boolean} */ hideEditor) {
    let editor = document.getElementById(
      ids.domElements.pages.calculator.divEquationEditor
    );
    let editorButtons = document.getElementById(
      ids.domElements.pages.calculator.divEquationEditorButtons
    );
    let button = document.getElementById(
      ids.domElements.pages.calculator.buttonToggleEquations
    );
    if (hideEditor) {
      editor.style.display = 'none';
      editorButtons.style.display = 'none';
      button.textContent = '\u25B8';
    } else {
      editor.style.display = '';
      editorButtons.style.display = '';
      button.textContent = '\u25BE';
    }
  }
 
  setExamples(
    /** @type{HTMLElement} */
    button,
  ) {
    let examples = document.getElementById(
      ids.domElements.calculatorExamples
    );
    if (
      examples.innerHTML.length < 300 &&
      this.flagExamplesWantedShown
    ) {
      let url = "";
      url += pathnames.urls.calculatorAPI;
      url += `?${pathnames.urlFields.request}=${pathnames.urlFields.requests.calculatorExamplesJSON}`;
      submitRequests.submitGET({
        url: url,
        callback: (input) => {
          this.processExamples(input);
        },
        progress: "spanProgressCalculatorExamples"
      });
    } else {
      this.adjustCalculatorPageSize();
    }
    if (this.flagExamplesWantedShown) {
      button.innerHTML = "&#9660;";
    } else {
      button.innerHTML = "&#9656;";
    }
  }

  initialize() {
    if (this.initialized) {
      return;
    }
    this.initialized = true;
    this.editor.initialize();
    this.initializeToggleExamples();
    this.initializeToggleEditor();
    this.splitter.initialize();
    let buttonGo = document.getElementById(
      ids.domElements.pages.calculator.buttonGoCalculatorPage
    );
    if (buttonGo === null) {
      return;
    }
    buttonGo.addEventListener('click', () => {
      this.submitComputation();
    });
    let buttonLinkLatex = document.getElementById(
      ids.domElements.pages.calculator.buttonCopyLatexLink
    );
    buttonLinkLatex.addEventListener('click', () => {
      this.prepareLatexLink();
    });
  }

  initializeToggleExamples() {
    let buttonExamples = document.getElementById(
      ids.domElements.pages.calculator.buttonCalculatorExamples
    );
    if (buttonExamples === null) {
      return;
    }
    this.flagExamplesWantedShown = storage.storage.variables.calculator.examplesWantedShown.isTrue();
    if (this.flagExamplesWantedShown) {
      this.setExamples(buttonExamples);
    }
    buttonExamples.addEventListener('click', () => {
      this.flagExamplesWantedShown = !this.flagExamplesWantedShown;
      storage.storage.variables.calculator.examplesWantedShown.setAndStore(
        this.flagExamplesWantedShown, true, false,
      );
      this.setExamples(buttonExamples);
    });
  }

  initializeToggleEditor() {
    let buttonToggleEditor = document.getElementById(
      ids.domElements.pages.calculator.buttonToggleEquations
    );
    if (buttonToggleEditor === null) {
      return;
    }
    buttonToggleEditor.addEventListener('click', () => {
      this.toggleEquationEditor();
    });
    let hideEditor = storage.storage.variables.calculator.hideEquationEditor.isTrue();
    this.doToggleEditor(hideEditor);
  }

  latexLink(
    /** @type{string} */
    input
  ) {
    let result = [];
    result.push("\\href{");
    let toBeEscaped = "%\\_{}#";
    let toBeEscapedMap = {};
    for (let i = 0; i < toBeEscaped.length; i++) {
      toBeEscapedMap[toBeEscaped[i]] = true;      
    }
    for (let i = 0; i < input.length; i++){
      let current = input[i];
      if (current in toBeEscapedMap) {
        result.push("\\" + current);
        continue;
      }
      result.push(current);
    }
    result.push("}{your computation}");
    return result.join("");
  }

  prepareLatexLink() {
    let anchor = document.getElementById(
      ids.domElements.pages.calculator.anchorComputationLink
    );
    let latexLink = this.latexLink(anchor.href);
    navigator.clipboard.writeText(latexLink);
    anchor.style.backgroundColor = "lightgreen";
    setTimeout(() => {
      anchor.style.backgroundColor = "";
    }, 1000);
  }

  selectCalculatorPage() {
    this.initialize();
    this.submitComputation();
  }

  submitComputation() {
    processMonitoring.monitor.clearTimeout();
    let calculatorInput = document.getElementById(
      ids.domElements.pages.calculator.inputMain
    ).value;
    if (calculatorInput === this.lastSubmittedInput) {
      return;
    }
    this.lastSubmittedInput = calculatorInput;
    // submitComputationPartTwo is called by a callback in the function below:
    storage.storage.variables.calculator.input.setAndStore(
      this.lastSubmittedInput
    );
  }

  /** @return {String} */
  getComputationLink(input) {
    let url = {
      currentPage: "calculator",
      calculatorInput: input,
    };
    let page = window.calculator.mainPage;
    let stringifiedHash = page.storage.getPercentEncodedURL(url);
    return stringifiedHash;
  }

  /** @return{HTMLElement} */
  writeErrorsCrashesComments(
    inputParsed,
  ) {
    let result = document.createElement("span");
    if (
      inputParsed.commentsGlobal !== "" &&
      inputParsed.commentsGlobal !== undefined
    ) {
      let comments = document.createElement("div");
      comments.innerHTML = inputParsed.commentsGlobal;
      result.appendChild(comments);
    }
    if (
      inputParsed.result === undefined &&
      inputParsed.comments !== undefined
    ) {
      let comments = document.createElement("div");
      comments.innerHTML = inputParsed.comments;
      result.appendChild(comments);
    }
    if (
      inputParsed.error !== undefined &&
      inputParsed.error !== null &&
      inputParsed.error !== ""
    ) {
      let element = document.createElement("b");
      element.style.color = "red";
      element.textContent = "Error. ";
      result.appendChild(element);
      result.appendChild(document.createTextNode(inputParsed.error));
    }
    if (
      inputParsed.badInput !== undefined &&
      inputParsed.badInput !== null &&
      inputParsed.badInput !== ""
    ) {
      let badInput = document.createElement("div");
      badInput.innerHTML = inputParsed.badInput;
      result.appendChild(badInput);
    }
    if (
      inputParsed.crashReport !== undefined &&
      inputParsed.crashReport !== null &&
      inputParsed.crashReport !== ""
    ) {
      let crash = document.createElement("div");
      crash.innerHTML = inputParsed.crashReport;
      result.appendChild(crash);
    }
    return result;
  }

  /** @return{HTMLElement} */
  writeResult(
    inputParsed,
  ) {
    let result = document.createElement("div");
    result.style.width = "100%";
    result.style.height = "100%";
    const crashesAndComments = this.writeErrorsCrashesComments(inputParsed);
    result.appendChild(crashesAndComments);
    if (inputParsed.timeOut === true) {
      if (inputParsed.timeOutComments !== undefined) {
        let comment = document.createElement("div");
        comment.innerHTML = inputParsed.timeOutComments;
        result.append(comment);
      }
      processMonitoring.monitor.start(inputParsed.workerId);
      return result;
    }
    if (
      inputParsed.result === undefined &&
      inputParsed.resultHtml !== undefined
    ) {
      // If both resultHTML and result are specified,
      // resultHTML must be a fallback, 
      // so we ignore it.
      let resultHTML = document.createElement("div");
      resultHTML.innerHTML = inputParsed.resultHtml;
      result.appendChild(resultHTML);
    }
    if (inputParsed.result === undefined) {
      return result;
    }
    const inputOutputComments = this.constructInputOutputComments(inputParsed);
    result.appendChild(inputOutputComments);
    if (inputParsed.parsingLog !== undefined) {
      let element = document.createElement("div");
      element.innerHTML = inputParsed.parsingLog;
      result.appendChild(element);
    }
    return result;
  }

  constructInputOutputComments(
    inputParsed,
  ) {
    let inputOutputComments = document.createElement("div");
    inputOutputComments.className = "calculatorInputOutputComments";
    let inputOutput = document.createElement("div");
    inputOutputComments.appendChild(inputOutput);
    inputOutput.className = "calculatorInputOutput";
    if (inputParsed.syntaxErrors !== undefined) {
      let element = document.createElement("div");
      element.innerHTML = inputParsed.syntaxErrors;
      inputOutput.appendChild(element);
    }
    let inputOutputTable = this.constructInputOutput(inputParsed);
    inputOutput.appendChild(inputOutputTable);
    let commentsContainer = this.constructComments(inputParsed);
    inputOutputComments.appendChild(commentsContainer);
    if (
      storage.storage.variables.flagDebug.isTrue() &&
      inputParsed.debug !== undefined
    ) {
      let debugComments = document.createElement("div");
      debugComments.innerHTML = inputParsed.debug;
      inputOutputComments.appendChild(debugComments);
    }
    return inputOutputComments;
  }

  /** @return{HTMLElement} */
  constructInputOutput(
    inputParsed,
  ) {
    let inputOutputTable = document.createElement("table");
    inputOutputTable.className = 'tableCalculatorOutput';
    let headerRow = inputOutputTable.insertRow();
    let inputLabelCell = document.createElement("th");
    inputLabelCell.textContent = "Input";
    headerRow.appendChild(inputLabelCell);
    let outputLabelCell = document.createElement("th");
    outputLabelCell.textContent = "Output";
    headerRow.appendChild(outputLabelCell);
    if (typeof inputParsed.result.input === "string") {
      inputParsed.result.input = [inputParsed.result.input];
    }
    if (typeof inputParsed.result.output === "string") {
      inputParsed.result.output = [inputParsed.result.output];
    }
    let totalEntries = Math.max(
      inputParsed.result.input.length,
      inputParsed.result.output.length,
    );
    for (let i = 0; i < totalEntries; i++) {
      this.numberOfCalculatorPanels++;
      let currentInput = inputParsed.result.input[i];
      let currentOutput = inputParsed.result.output[i];
      let currentRow = inputOutputTable.insertRow();
      let inputCell = currentRow.insertCell();
      let outputCell = currentRow.insertCell();
      inputCell.className = 'cellCalculatorInput';
      outputCell.className = 'cellCalculatorResult';
      let input = document.createElement("div");
      let output = document.createElement("div");
      inputCell.append(input);
      outputCell.append(output);
      if (i < inputParsed.result.input.length) {
        this.panels.push(new panels.PanelExpandableData(
          currentInput,
          input,
          150,
          false,
          "",
          true,
        ));
      }
      if (i < inputParsed.result.output.length) {
        this.panels.push(new panels.PanelExpandableData(
          currentOutput,
          output,
          150,
          false,
          "",
          true,
        ));
      }
    }
    return inputOutputTable;
  }

  /** @return{HTMLElement} */
  constructComments(
    inputParsed,
  ) {
    let commentsContainer = document.createElement("div");
    commentsContainer.className = "containerComments";
    let performance = inputParsed[pathnames.urlFields.result.performance];
    if (performance !== undefined) {
      let content = performance[pathnames.urlFields.result.comments];
      let label = `<b style='color:blue'>${performance[pathnames.urlFields.result.computationTime]}</b>`;
      let performanceDetails = document.createElement("div");
      commentsContainer.appendChild(performanceDetails);
      this.panels.push(new panels.PanelExpandableData(
        content,
        performanceDetails,
        0,
        true,
        label,
      ));
    }
    if (inputParsed.comments !== undefined) {
      let comments = document.createElement("div");
      comments.innerHTML = inputParsed.comments;
      commentsContainer.appendChild(comments);
    }
    return commentsContainer;
  }

  typeset(
    /**@type{Function} */
    typeSetCallback,
  ) {
    if (this.flagTypeset === true) {
      return;
    }
    dynamicJavascript.typeset(
      this.getOutputElement(),
      { "lineBreakWidth": 600, },
      typeSetCallback,
    );
    this.flagTypeset = true;
  }

  resizePanel(
    /** @type{equationEditor.EquationEditor} */
    editor,
    constructedPanels,
  ) {
    let parent = editor.container;
    for (let i = 0; i < 5; i++) {
      if (parent === null) {
        break;
      }
      if (parent.id in constructedPanels) {
        break;
      }
      parent = parent.parentElement;
    }
    if (parent === null) {
      return;
    }
    if (!(parent.id in constructedPanels)) {
      return;
    }
    /** @type{panels.PanelExpandable} */
    let panel = constructedPanels[parent.id];
    panel.computeOriginalDimensions();
    if (panel.originalWidth > 1500) {
      panel.originalWidth = 1000
    }
    if (panel.originalHeight > 1000) {
      panel.originalHeight = 1000
    }
    panel.matchPanelStatus();
  }

  afterWriteOutput() {
    let constructedPanels = {};
    for (let i = 0; i < this.panels.length; i++) {
      let next = panels.makePanelFromData(this.panels[i]);
      if (next === null) {
        continue;
      }
      constructedPanels[next.containerId] = next;
    }
    this.flagTypeset = false;
    let elementWithScripts = null;
    try {
      elementWithScripts = dynamicJavascript.bootstrapAllScripts(
        this.getOutputElement()
      );
    } catch (e) {
      console.log(e);
    }
    this.typeset((
      /** @type{equationEditor.EquationEditor} */
      editor,
      /** @type{HTMLElement} */
      output,
    ) => {
      if (elementWithScripts === null) {
        return;
      }
      elementWithScripts.bootstrapFormInputs(editor, output);
      this.resizePanel(editor, constructedPanels);
    });
  }

  getOutputElement() {
    return document.getElementById(
      ids.domElements.pages.calculator.divCalculatorMainOutput,
    );
  }

  writeResultAndUpdateElement() {
    this.panels = [];
    let result = this.writeResult(this.parsedComputation, this.panels);
    this.getOutputElement().textContent = '';
    this.getOutputElement().appendChild(result);
  }

  defaultOnLoadInjectScriptsAndProcessLaTeX(input, output) {
    this.panels.length = 0;
    try {
      this.parsedComputation = miscellaneousFrontend.jsonUnescapeParse(input);
      let progressReportTimer = document.getElementById(
        ids.domElements.pages.calculator.monitoring.progressTimer,
      );
      progressReportTimer.innerHTML = "";
      this.writeResultAndUpdateElement();
    } catch (e) {
      inputHtml = input + "<br>" + e;
      console.log("Error processing calculator output: " + e);
    }
    this.afterWriteOutput();
  }

  helpCalculator() {
    if (!this.initialized) {
      return;
    }
    this.editor.writeIntoEquationEditor();
  }

  submitComputationPartTwo(input) {
    if (storage.storage.variables.calculator.useWebAssembly.value === "true") {
      this.submitComputationToWebAssembly(input);
      return;
    }
    this.submitComputationToBackend(input);
  }

  submitComputationToWebAssembly(input) {
    let requestWithProgress = new submitRequests.RequestWithProgress(
      "",
      "",
      false,
      ids.domElements.pages.calculator.progress,
      null,
      {},
      `wasm:${encodeURIComponent(input)}`,
    );
    requestWithProgress.recordProgressStarted();
    startCalculatorWebAssemblyWorker.calculatorWorker.compute(input, (result) => {
      this.defaultOnLoadInjectScriptsAndProcessLaTeX(result);
      requestWithProgress.recordProgressDone();
    });
  }

  submitComputationToBackend(input) {
    //<- this function is called by a callback trigerred when calling
    // page.storage.variables.calculator.input.setAndStore(...)
    let page = window.calculator.mainPage;
    let urlCopy = Object.assign({}, page.storage.urlObject);
    urlCopy.inputFocus = true;
    let stringifiedHash = page.storage.getPercentEncodedURL(urlCopy);
    let anchor = document.getElementById(
      ids.domElements.pages.calculator.anchorComputationLink
    );
    anchor.href = `#${stringifiedHash}`;
    anchor.textContent = "Link to your input";
    let latexCopyButton = document.getElementById(
      ids.domElements.pages.calculator.buttonCopyLatexLink
    );
    latexCopyButton.style.visibility = "";
    setTimeout(() => {
      this.helpCalculator();
    }, 0);
    let url = pathnames.urls.calculatorAPI;
    let parameters = this.getQueryStringSubmitStringAsMainInput(
      input, pathnames.urlFields.calculatorCompute
    );
    submitRequests.submitPOST({
      url: url,
      parameters: parameters,
      callback: (input) => {
        this.defaultOnLoadInjectScriptsAndProcessLaTeX(input);
      },
      progress: ids.domElements.pages.calculator.progress,
    });
  }

  getQueryStringSubmitStringAsMainInput(inputString, requestType) {
    let inputParams = '';
    let page = window.calculator.mainPage;
    inputParams += `${pathnames.urlFields.request}=${requestType}&`;
    inputParams += `${pathnames.urlFields.requests.calculatorInput}=${encodeURIComponent(inputString)}&`;
    if (page.storage.variables.flagDebug.isTrue()) {
      inputParams += `${pathnames.urlFields.debugFlag}=true&`;
    }
    if (page.storage.variables.calculator.monitoring.value === "false") {
      inputParams += `${pathnames.urlFields.requests.monitoring}=false&`;
    } else {
      inputParams += `${pathnames.urlFields.requests.monitoring}=true&`;
    }
    return inputParams;
  }

  submitStringAsMainInput(
    inputString, idOutput, requestType, onLoadFunction, idStatus,
  ) {
    let inputParams = this.getQueryStringSubmitStringAsMainInput(
      inputString, requestType,
    );
    this.submitStringCalculatorArgument(
      inputParams, idOutput, onLoadFunction, idStatus,
    );
  }
}

let calculator = new Calculator();

module.exports = {
  calculator,
};
