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
const equationEditor = require("./equation_editor/src/equation_editor");
const startCalculatorWebAssemblyWorker = require("./web_assembly_worker");
const splitter = require("./splitter_two_elements");

function writeHTML(
  /** @type {HTMLElement} */ element,
  /** @type {string} */ htmlContent,
) {
  panels.writeHTML(element, htmlContent);
}

class ExampleBuiltIn {
  constructor(
    /** @type {string} */
    name,
    /** @type {string} */
    example,
  ) {
    this.name = name;
    this.example = example;
  }

  /** @return {HTMLElement} */
  toHTML() {
    let result = document.createElement("div");
    let anchor = document.createElement("a");
    anchor.textContent = this.name;
    anchor.href = pathnames.addresses.calculatorComputationWithoutAppURL(
      this.example,
    );
    result.appendChild(anchor);
    return result;
  }
}

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
    /** @type {HTMLElement|null} */
    this.panel = null;
    /** @type {HTMLElement|null} */
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
    /** @type {boolean} */
    this.cacheable = input["cacheable"]
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

  /** @return {HTMLElement} */
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
    if (!this.cacheable) {
      let element = document.createElement("b");
      element.textContent = "Non-cacheable";
      element.style.color = "purple";
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

  /** @return {HTMLElement} */
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


class Calculator {
  constructor() {
    this.parsedComputation = {};
    /** @type {panels.PanelExpandableData[]} */
    this.panels = [];
    /** @type {HTMLElement|null} */
    this.outputElement = null;
    this.handlerDocumentation = null;
    this.examples = null;
    this.submissionCalculatorCounter = 0;
    this.lastSubmittedInput = "";
    this.numberOfCalculatorPanels = 0;
    this.flagExamplesWantedShown = false;
    processMonitoring.monitor.ownerCalculator = this;
    /** @type {boolean} */
    this.flagTypeset = false;
    /** @type {boolean} */
    this.initialized = false;
    this.editor = new calculatorPageEditor.CalculatorEquationEditor((event) => {
      this.submitCalculatorInputOnEnter(event);
    });
    this.splitterInputOutput = new splitter.Splitter(
      ids.domElements.pages.calculator.divCalculatorSplitter,
      ids.domElements.pages.calculator.divCalculatorMainInputOutput,
      ids.domElements.pages.calculator.divCalculatorMainInput,
      ids.domElements.pages.calculator.divCalculatorMainOutput,
      storage.storage.variables.calculator.splitterInputOutput,
      true,
    );
    this.splitterExamples = new splitter.Splitter(
      ids.domElements.pages.calculator.divExamplesSplitter,
      ids.domElements.pages.calculator.divCalculatorPage,
      ids.domElements.pages.calculator.divCalculatorMainInputOutput,
      ids.domElements.pages.calculator.examplesContainer,
      storage.storage.variables.calculator.splitterExamples,
      false,
    );
  }

  /** @return {HTMLElement} */
  processOneFunctionAtom(handlers) {
    if (handlers === undefined || handlers === null) {
      throw new Error("Bad handlers");
    }
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

  /** @return {HTMLElement} */
  processOneBuiltInExample(
    /** @type {string} */ name,
    /** @type {string} */ example,
  ) {
    let element = new ExampleBuiltIn(name, example);
    return element.toHTML();
  }

  doProcessExamples(inputJSONtext) {
    let examplesMessage = miscellaneousFrontend.jsonUnescapeParse(inputJSONtext);
    this.handlerDocumentation = examplesMessage["calculatorHandlerDocumentation"];
    this.examples = examplesMessage["calculatorExamples"];
    let atomsSorted = Object.keys(this.handlerDocumentation).slice().sort();
    let examplesSorted = Object.keys(this.examples).slice().sort();
    let numHandlers = 0;
    let allElements = [];
    for (let i = 0; i < examplesSorted.length; i++) {
      let name = examplesSorted[i];
      let example = this.examples[name];
      let element = this.processOneBuiltInExample(name, example);
      allElements.push(element);
    }
    for (let i = 0; i < atomsSorted.length; i++) {
      let atom = atomsSorted[i];
      let currentExamples = this.handlerDocumentation[atom];
      allElements.push(this.processOneFunctionAtom(currentExamples.regular));
      allElements.push(this.processOneFunctionAtom(currentExamples.composite));
      numHandlers += this.handlerDocumentation[atom].regular.length + this.handlerDocumentation[atom].composite.length;
    }
    let handlerReport = document.createElement("span");
    handlerReport.textContent = `${atomsSorted.length} built-in atoms, ${numHandlers} handlers. `;
    let output = document.getElementById(ids.domElements.pages.calculator.examples);
    output.textContent = "";
    miscellaneousFrontend.writeHtmlFromCommentsAndErrors(examplesMessage, output);
    output.appendChild(handlerReport);
    for (let i = 0; i < allElements.length; i++) {
      output.appendChild(allElements[i]);
    }
  }

  processExamples(inputJSONtext) {
    try {
      this.doProcessExamples(inputJSONtext)

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
    this.submitComputationAndStore();
    event.preventDefault();
  }

  formatExamplesPanel() {
    let examples = document.getElementById(
      ids.domElements.pages.calculator.examples
    );
    let examplesContainer = document.getElementById(
      ids.domElements.pages.calculator.examplesContainer
    );
    let calculatorElement = document.getElementById(
      ids.domElements.divCalculatorMainInputOutput
    );
    const button = document.getElementById(
      ids.domElements.pages.calculator.buttonCalculatorExamples
    );
    if (this.flagExamplesWantedShown) {
      writeHTML(button, "&#9660;")
      // calculatorElement.style.maxWidth = "85%";
      examples.classList.remove("hiddenClass");
      //examplesContainer.style.height = "100%";
      //examplesContainer.style.maxHeight = "100%";
    } else {
      calculatorElement.style.maxWidth = "98%";
      writeHTML(button, "&#9656;")
      examples.classList.add("hiddenClass");
      //examplesContainer.style.height = "5%";
      //examplesContainer.style.maxHeight = "5%";
    }
  }

  toggleEquationEditor() {
    let hideEditor = storage.storage.variables.calculator.hideEquationEditor.isTrue();
    hideEditor = !hideEditor;
    storage.storage.variables.calculator.hideEquationEditor.setAndStore(
      hideEditor
    );
    this.doToggleEditor(hideEditor)
  }

  doToggleEditor(/** @type {boolean} */ hideEditor) {
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

  downloadExamples() {
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
  }

  setExamples() {
    let examples = document.getElementById(
      ids.domElements.pages.calculator.examplesContainer,
    );
    if (
      examples.textContent.length < 300 &&
      this.flagExamplesWantedShown
    ) {
      this.downloadExamples();
    }
    this.formatExamplesPanel();
  }

  initialize() {
    if (this.initialized) {
      return;
    }
    this.initialized = true;
    this.editor.initialize();
    this.initializeToggleExamples();
    this.initializeToggleEditor();
    this.splitterInputOutput.initialize();
    this.splitterExamples.initialize();
    let buttonGo = document.getElementById(
      ids.domElements.pages.calculator.buttonGoCalculatorPage
    );
    if (buttonGo === null) {
      return;
    }
    buttonGo.addEventListener('click', () => {
      this.submitComputationAndStore();
    });
    let buttonLinkLatex = document.getElementById(
      ids.domElements.pages.calculator.buttonCopyLatexLink
    );
    buttonLinkLatex.addEventListener('click', () => {
      this.prepareLatexLink();
    });
    this.formatExamplesPanel();
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
      this.toggleExamples();
    });
  }

  toggleExamples() {
    this.flagExamplesWantedShown = !this.flagExamplesWantedShown;
    storage.storage.variables.calculator.examplesWantedShown.setAndStore(
      this.flagExamplesWantedShown, true, false,
    );
    this.setExamples();
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
    /** @type {string} */
    input
  ) {
    let result = [];
    result.push("\\href{");
    let toBeEscaped = "%\\_{}#";
    let toBeEscapedMap = {};
    for (let i = 0; i < toBeEscaped.length; i++) {
      toBeEscapedMap[toBeEscaped[i]] = true;
    }
    for (let i = 0; i < input.length; i++) {
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
  }

  submitComputationAndStore() {
    processMonitoring.monitor.clearTimeout();
    let element = document.getElementById(
      ids.domElements.pages.calculator.inputMain
    )
    let calculatorInput = element.innerText;
    // Replace all non-breaking spaces with breaking spaces.
    calculatorInput = calculatorInput.split("\u00A0").join(" ");
    if (calculatorInput === this.lastSubmittedInput) {
      return;
    }
    this.lastSubmittedInput = calculatorInput;
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

  /** Writes html input to an element on the page. */
  writeHTML(
    /** @type {HTMLElement} */
    element,
    /**@type {string} */
    htmlContent,
  ) {
    panels.writeHTML(element, htmlContent);
  }


  /** @return {HTMLElement} */
  writeErrorsCrashesComments(
    inputParsed,
  ) {
    let result = document.createElement("span");
    if (
      inputParsed.commentsGlobal !== "" &&
      inputParsed.commentsGlobal !== undefined
    ) {
      let comments = document.createElement("div");
      this.writeHTML(comments, inputParsed.commentsGlobal);
      result.appendChild(comments);
    }
    let commentsDebug = document.getElementById(
      ids.domElements.pages.calculator.divDebugComments
    );
    if (
      inputParsed.commentsDebug !== "" &&
      inputParsed.commentsDebug !== undefined
    ) {
      this.writeHTML(commentsDebug, inputParsed.commentsDebug);
    } else {
      commentsDebug.textContent = "";
    }
    if (
      inputParsed.result === undefined &&
      inputParsed.comments !== undefined
    ) {
      let comments = document.createElement("div");
      this.writeHTML(comments, inputParsed.comments);
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
      this.writeHTML(badInput, inputParsed.badInput);
      result.appendChild(badInput);
    }
    if (
      inputParsed.crashReport !== undefined &&
      inputParsed.crashReport !== null &&
      inputParsed.crashReport !== ""
    ) {
      let crash = document.createElement("div");
      this.writeHTML(crash, inputParsed.crashReport);
      result.appendChild(crash);
    }
    return result;
  }

  /** @return {HTMLElement} */
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
        this.writeHTML(comment, inputParsed.timeOutComments);
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
      writeHTML(resultHTML, inputParsed.resultHtml);
      result.appendChild(resultHTML);
    }
    if (inputParsed.result === undefined) {
      return result;
    }
    const inputOutputComments = this.constructInputOutputComments(inputParsed);
    result.appendChild(inputOutputComments);
    if (inputParsed.parsingLog !== undefined) {
      let element = document.createElement("div");
      writeHTML(element, inputParsed.parsingLog);
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
      writeHTML(element, inputParsed.syntaxErrors);
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
      writeHTML(debugComments, inputParsed.debug);
      inputOutputComments.appendChild(debugComments);
    }
    return inputOutputComments;
  }

  /** @return {HTMLElement} */
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
          true,
        ));
      }
    }
    return inputOutputTable;
  }

  /** @return {HTMLElement} */
  constructComments(
    inputParsed,
  ) {
    let commentsContainer = document.createElement("div");
    commentsContainer.className = "containerComments";
    let performance = inputParsed[pathnames.urlFields.result.performance];
    if (performance !== undefined) {
      let contentWithoutStyle = performance[pathnames.urlFields.result.comments];
      let content = `<div style='width: 400px'>${contentWithoutStyle}</div>`;
      let computationTime = performance[
        pathnames.urlFields.result.computationTime
      ];
      let label = `<b style='color:blue'>${computationTime}</b>`;
      let performanceDetails = document.createElement("div");
      commentsContainer.appendChild(performanceDetails);
      this.panels.push(new panels.PanelExpandableData(
        content,
        performanceDetails,
        0,
        true,
        label,
      ));
      let performancePerHandler = performance[
        pathnames.urlFields.result.performancePerHandler
      ];
      if (
        performancePerHandler !== undefined &&
        performancePerHandler !== null
      ) {
        let divPerformancePerHandler = document.createElement("div");
        divPerformancePerHandler.textContent = performancePerHandler[
          pathnames.urlFields.result.comments
        ];
        commentsContainer.appendChild(divPerformancePerHandler);
        let performanceVerySlow = performancePerHandler[
          pathnames.urlFields.result.performanceTurnOffVerySlowRules
        ];
        this.writeAnchor(
          performanceVerySlow,
          "Turn off very slow rules.",
          commentsContainer,
        );
        let performanceSlow = performancePerHandler[
          pathnames.urlFields.result.performanceTurnOffSlowRules
        ];
        this.writeAnchor(
          performanceSlow,
          "Turn off slow rules.",
          commentsContainer,
        );
      }
    }
    if (inputParsed.comments !== undefined) {
      let comments = document.createElement("div");
      writeHTML(comments, inputParsed.comments);
      commentsContainer.appendChild(comments);
    }
    return commentsContainer;
  }

  writeAnchor(
    /** @type {string} */ input,
    /** @type {string} */ label,
    /** @type {HTMLElement} */ outputContainer,
  ) {
    if (input === "" || input === null || input === undefined) {
      return;
    }
    let anchor = document.createElement("a");
    anchor.href = pathnames.addresses.calculatorComputationWithoutAppURL(input);
    anchor.textContent = label;
    outputContainer.appendChild(anchor);
  }

  typeset(
    /** @type {Function} */
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
    /** @type {equationEditor.EquationEditor} */
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
    /** @type {panels.PanelExpandable} */
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
      let next = panels.makePanelFromData(this.panels[i], true);
      if (next === null) {
        continue;
      }
      constructedPanels[next.containerId] = next;
    }
    this.flagTypeset = false;
    let elementWithScripts = null;
    try {
      elementWithScripts = dynamicJavascript.bootstrapAllScripts(
        this.getOutputElement(),
        this.parsedComputation.scripts,
      );
    } catch (e) {
      console.log(e);
    }
    this.typeset((
      /** @type {equationEditor.EquationEditor} */
      editor,
      /** @type {HTMLElement} */
      output,
    ) => {
      if (elementWithScripts === null) {
        return;
      }
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
      progressReportTimer.textContent = "";
      this.writeResultAndUpdateElement();
    } catch (e) {
      let inputHtml = input + "<br>" + e;
      console.log("Error processing calculator output: " + inputHtml);
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
