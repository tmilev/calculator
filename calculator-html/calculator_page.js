"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const drawing = require('./three-d').drawing;
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require('./miscellaneous_frontend');
const miscellaneous = require('./miscellaneous');
const BufferCalculator = require('./buffer').BufferCalculator;
const PanelExpandable = require('./panels').PanelExpandable;
const mathjax = require('./mathjax-calculator-setup');
const processMonitoring = require('./process_monitoring');
const initializeButtons = require('./initialize_buttons');

function Calculator() {
  this.parsedComputation = {};
  this.panelIdPairs = [];
  this.inputBoxNames = [];
  this.inputBoxToSliderUpdaters = {};
  this.canvases = null;
  this.examples = null;
  this.submissionCalculatorCounter = 0;
  this.lastSubmittedInput = "";
  this.numberOfCalculatorPanels = 0;
  processMonitoring.monitor.ownerCalculator = this;
}

function createSelectionNoFocus(field, start, end) {
  if (field.createTextRange) {
    var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
    calculatorMQfield.focus();
  } else if (field.setSelectionRange) {
    field.focus();
    field.setSelectionRange(start, end);
    calculatorMQfield.focus();
  } else if (typeof field.selectionStart != 'undefined') {
    field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
    calculatorMQfield.focus();
  }
}

Calculator.prototype.updateCalculatorSliderEventHandler = function (inputBox) {
  event.preventDefault();
  var sliderName = this.inputBoxToSliderUpdaters[inputBox.name];
  var theSliders = document.getElementsByName(sliderName);
  for (var counterSlider = 0; counterSlider < theSliders.length; counterSlider ++) {
    var currentSlider = theSliders[counterSlider];
    currentSlider.value = inputBox.value;
  }
  this.updateSliderToInputBox(inputBox.name, sliderName);
}

Calculator.prototype.addListenersToInputBoxes = function() {
  //var theString=" updating: box names, slider names: ";
  for (var i = 0; i < this.inputBoxNames.length; i ++) {
    var theBoxes = document.getElementsByName(this.inputBoxNames[i]);
    for (var j = 0; j < theBoxes.length; j ++) {
      theBoxes[j].addEventListener("input", this.updateCalculatorSliderEventHandler.bind(this, theBoxes[j]));
    }
  }
}

Calculator.prototype.updateSliderToInputBox =  function(boxName, sliderName) {
  var theBoxes = document.getElementsByName(boxName);
  var theSliders = document.getElementsByName(sliderName);
  var sliderValue = theSliders[0].value;
  for (var i = 0; i < theBoxes.length; i ++) {
    theBoxes[i].value = sliderValue;
  }
  var plodtId = drawing.plotUpdaters[sliderName];
  if (plodtId !== undefined) {
    var theCanvas = drawing.canvases[plodtId];
    if (theCanvas !== undefined) {
      if (theCanvas.canvasResetFunction !== null) {
        theCanvas.canvasResetFunction();
      }
    }
  }
}

function createSelection(field, start, end) {
  if (field.createTextRange) {
    var selRange = field.createTextRange();
    selRange.collapse(true);
    selRange.moveStart('character', start);
    selRange.moveEnd('character', end);
    selRange.select();
    field.focus();
  } else if (field.setSelectionRange) {
    field.focus();
    field.setSelectionRange(start, end);
  } else if (field.selectionStart !== undefined) {
    field.selectionStart = start;
    field.selectionEnd = end;
    field.focus();
  }
}

Calculator.prototype.processOneFunctionAtom = function(handlers, isComposite) {
  var resultString = "";
  for (var counterHandlers = 0; counterHandlers < handlers.length; counterHandlers ++) {
    resultString += "<br>";
    var currentDescription = handlers[counterHandlers].description;
    var currentExample = handlers[counterHandlers].example;
    resultString += `<calculatorAtom>${handlers[counterHandlers].atom}</calculatorAtom>`;
    if (handlers[counterHandlers].composite === "true") {
      resultString += "<calculatorCompositeAtom>(composite)</calculatorCompositeAtom>";
    }
    resultString += ` (${counterHandlers + 1} out of ${handlers.length})`;
    var currentId = "example_";
    if (isComposite) {
      currentId += "t_";
    } else {
      currentId += "f_";
    }
    var encodedAtom = encodeURIComponent(handlers[counterHandlers].atom);
    currentId += `${encodedAtom}_${counterHandlers}_${handlers.length}`;
    resultString += `<a href = '#' class = 'linkInfo' onclick = "window.calculator.miscellaneousFrontend.switchMenu('${currentId}')">info</a>`;
    resultString += `<calculatorExampleInfo id = "${currentId}" class = "hiddenClass">${currentDescription}`;
    resultString += `<br><b>Example:</b><br>${currentExample}</calculatorExampleInfo>`;

    var theLink = this.getComputationLink(currentExample);
    resultString += `<a href = '#${theLink}' class = "linkInfo"> Example</a>`;
    //resultString += currentExample;
    //console.log(handlers[counterHandlers]);
  }
  return resultString;
}

Calculator.prototype.processExamples = function(inputJSONtext) {
  try {
    this.examples = miscellaneous.jsonUnescapeParse(inputJSONtext);
    var examplesString = "";
    var atomsSorted = Object.keys(this.examples).slice().sort();
    var numHandlers = 0;
    for (var i = 0; i < atomsSorted.length; i ++) {
      var atom = atomsSorted[i];
      var currentExamples = this.examples[atom];
      examplesString += this.processOneFunctionAtom(currentExamples.regular, false);
      examplesString += this.processOneFunctionAtom(currentExamples.composite, true);
      numHandlers += this.examples[atom].regular.length + this.examples[atom].composite.length;
    }
    var resultString = `${atomsSorted.length} built-in atoms, ${numHandlers} handlers. `;
    resultString += examplesString;
    document.getElementById(ids.domElements.calculatorExamples).innerHTML = resultString;
  } catch (e) {
    console.log(`Bad json: ${e}\n Input JSON follows.`);
    console.log(inputJSONtext);
  }
}

Calculator.prototype.submitCalculatorInputOnEnter = function (event) {
  if (event.keyCode !== 13 || !event.shiftKey) {
    return;
  }
  this.submitComputation();
  event.preventDefault();
}

Calculator.prototype.toggleExamples = function(theButton) {
  var theExamples = document.getElementById(ids.domElements.calculatorExamples);
  var theURL = "";
  theURL += pathnames.urls.calculatorAPI;
  theURL += `?${pathnames.urlFields.request}=${pathnames.urlFields.requests.calculatorExamplesJSON}`;
  if (theExamples.innerHTML.length < 300) {
    submitRequests.submitGET({
      url: theURL,
      callback: this.processExamples.bind(this),
      progress: "spanProgressCalculatorExamples"
    });
    theButton.innerHTML = "&#9660;";
  } else {
    miscellaneousFrontend.switchMenu(ids.domElements.calculatorExamples);
    if (! theExamples.classList.contains("hiddenClass")) {
      theButton.innerHTML = "&#9660;";
    } else {
      theButton.innerHTML = "&#9656;";
    }
  }
}

Calculator.prototype.submitComputation = function() {
  processMonitoring.monitor.clearTimeout();
  var thePage = window.calculator.mainPage;
  var calculatorInput = document.getElementById(ids.domElements.inputMain).value;
  if (calculatorInput === this.lastSubmittedInput) {
    return;
  }
  this.lastSubmittedInput = calculatorInput;
  // submitComputationPartTwo is called by a callback in the function below:
  thePage.storage.variables.calculator.input.setAndStore(this.lastSubmittedInput);
}

/**@returns {String} */
Calculator.prototype.getComputationLink = function(input) {
  var theURL = {
    currentPage: "calculator",
    calculatorInput: input,
  };
  var thePage = window.calculator.mainPage;
  var stringifiedHash = thePage.storage.getPercentEncodedURL(theURL);
  return stringifiedHash;
}

Calculator.prototype.writeErrorsAndCrashes = function(
  /**@type {BufferCalculator} */
  buffer,
  inputParsed,
) {
  if (
    inputParsed.error !== undefined &&
    inputParsed.error !== null &&
    inputParsed.error !== ""
  ) {
    buffer.write("<b style = 'color:red'>Error.</b>");
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

Calculator.prototype.writeResult = function(
  /**@type {BufferCalculator} */
  buffer,
  inputParsed,
  panelIdPairs,
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
  buffer.write(`<table class = "tableCalculatorOutput"><tr><th>Input</th><th>Output</th></tr>`);
  if (typeof inputParsed.result.input === "string") {
    inputParsed.result.input = [inputParsed.result.input];
  }
  if (typeof inputParsed.result.output === "string") {
    inputParsed.result.output = [inputParsed.result.output];
  }
  var numEntries = Math.max(inputParsed.result.input.length, inputParsed.result.output.length);
  for (var i = 0; i < numEntries; i ++) {
    this.numberOfCalculatorPanels ++;
    var inputPanelId = `calculatorInputPanel${this.numberOfCalculatorPanels}`;
    var outputPanelId = `calculatorOutputPanel${this.numberOfCalculatorPanels}`;
    panelIdPairs.push([inputPanelId, outputPanelId]);
    buffer.write(`<tr>`);
    buffer.write(`<td class = "cellCalculatorInput"> <div id = "${inputPanelId}"></div></td>`);
    buffer.write(`<td class = "cellCalculatorResult"><div id = "${outputPanelId}"></div></td>`);
    buffer.write(`</tr>`);
  }
  buffer.write(`</table>`);
  buffer.write(`</td><td><div class = "containerComments">`);
  if (inputParsed.performance !== undefined) {
    buffer.write(inputParsed.performance);
    buffer.write("<br>");
  }
  if (inputParsed.comments !== undefined ) {
    buffer.write(inputParsed.comments);
  }
  buffer.write(`</div></td>`);
  var mainPage = window.calculator.mainPage;
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

Calculator.prototype.afterWriteOutput = function() {
  for (var i = 0; i < this.panelIdPairs.length; i ++) {
    var currentInput = this.parsedComputation.result.input[i];
    var currentOutput = this.parsedComputation.result.output[i];
    var inputPanelId = this.panelIdPairs[i][0];
    var outputPanelId = this.panelIdPairs[i][1];
    if (currentInput.length > 200) {
      var inputPanel = new PanelExpandable(inputPanelId, true);
      inputPanel.setPanelContent(currentInput);
      inputPanel.doToggleContent();
      inputPanel.matchPanelStatus();
    } else {
      document.getElementById(inputPanelId).innerHTML = currentInput;
    }
    if (currentOutput.length > 150) {
      var outputPanel = new PanelExpandable(outputPanelId, true);
      outputPanel.setPanelContent(currentOutput);
      outputPanel.doToggleContent();
      outputPanel.matchPanelStatus();
    } else {
      document.getElementById(outputPanelId).innerHTML = currentOutput;
    }
  }
  var spanVerification = document.getElementById(ids.domElements.spanCalculatorMainOutput);
  var incomingScripts = spanVerification.getElementsByTagName('script');
  var thePage = window.calculator.mainPage;
  var oldScripts = thePage.pages.calculator.scriptIds;
  thePage.removeScripts(oldScripts);
  this.inputBoxNames = [];
  this.inputBoxToSliderUpdaters = {};
  this.canvases = {};
  thePage.pages.calculator.sciptIds = [];
  for (var i = 0; i < incomingScripts.length; i ++) {
    var newId = `calculatorMainPageId_${i}`;
    thePage.pages.calculator.sciptIds.push(newId);
    thePage.injectScript(newId, incomingScripts[i].innerHTML);
  }
  this.addListenersToInputBoxes();
  mathjax.typeSetSoft(ids.domElements.spanCalculatorMainOutput);
}

Calculator.prototype.defaultOnLoadInjectScriptsAndProcessLaTeX = function(input, output) {
  this.parsedComputation = null;
  var inputHtml = null;
  this.panelIdPairs = [];
  try {
    this.parsedComputation = miscellaneous.jsonUnescapeParse(input);
    var buffer = new BufferCalculator();
    var progReportTimer = document.getElementById(ids.domElements.monitoring.progressTimer);
    progReportTimer.innerHTML = "";
    this.writeResult(buffer, this.parsedComputation, this.panelIdPairs);
    inputHtml = buffer.toString();
  } catch (e) {
    inputHtml = input + "<br>" + e;
    console.log("Error processing calculator output: " + e);
  }
  var spanVerification = document.getElementById(ids.domElements.spanCalculatorMainOutput);
  spanVerification.innerHTML = inputHtml;
  this.afterWriteOutput();
}

Calculator.prototype.submitComputationPartTwo = function(input) {
  //<- this function is called by a callback trigerred when calling
  //thePage.storage.variables.calculator.input.setAndStore(...)
  var thePage = window.calculator.mainPage;
  var urlCopy = Object.assign({}, thePage.storage.urlObject);
  urlCopy.inputFocus = true;
  var stringifiedHash = thePage.storage.getPercentEncodedURL(urlCopy);
  document.getElementById("spanComputationLink").innerHTML = `<a href = '#${stringifiedHash}'>Link to your input</a>`;
  setTimeout(() => {
    initializeButtons.calculatorPanel.mQHelpCalculator();
  }, 0);
  var url = pathnames.urls.calculatorAPI;
  var parameters = this.getQueryStringSubmitStringAsMainInput(input, pathnames.urlFields.calculatorCompute);
  submitRequests.submitPOST ({
    url: url,
    parameters:  parameters,
    callback: this.defaultOnLoadInjectScriptsAndProcessLaTeX.bind(this),
    progress: ids.domElements.spanProgressCalculatorInput,
  });
}

Calculator.prototype.getQueryStringSubmitStringAsMainInput = function(theString, requestType) {
  var inputParams = '';
  var thePage = window.calculator.mainPage;
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

Calculator.prototype.submitStringAsMainInput= function (theString, idOutput, requestType, onLoadFunction, idStatus) {
  var inputParams = this.getQueryStringSubmitStringAsMainInput(theString, requestType);
  this.submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus);
}

var calculator = new Calculator();

module.exports = {
  calculator,
};
