"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const drawing = require('./three-d').drawing;
const ids = require('./ids_dom_elements');
const miscellaneousFrontend = require('./miscellaneous_frontend');
const miscellaneous = require('./miscellaneous');
const BufferCalculator = require('./buffer').BufferCalculator;
const PanelExpandable = require('./panels').PanelExpandable;

function Calculator() {

  this.inputBoxNames = [];
  this.inputBoxToSliderUpdaters = {};
  this.canvases = null;
  this.examples = null;
  this.submissionCalculatorCounter = 0;
  this.lastSubmittedInput = "";
  this.numberOfCalculatorPanels = 0;
}

var calculatorMQString;
var calculatorMQStringIsOK = true;
var ignoreNextMathQuillUpdateEvent = false;
var startingCharacterSectionUnderMathQuillEdit;

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
    resultString += `<calculatorExampleInfo id = "${currentId}" class = "hiddenClass">${currentDescription}<br><b>Example:</b><br>${currentExample}</calculatorExampleInfo>`;

    var theLink = this.getComputationLink(currentExample);
    resultString += `<a href = '#${theLink}' class = "linkInfo"> Example</a>`;
    //resultString += currentExample;
    //console.log(handlers[counterHandlers]);
  }
  return resultString;
}

Calculator.prototype.processExamples = function(inputJSONtext) {
  try {
    this.examples = JSON.parse(inputJSONtext);
    var examplesString = "";
    var atomsSorted = Object.keys(this.examples).slice().sort();
    var numHandlers = 0;
    for (var counterAtoms = 0; counterAtoms < atomsSorted.length; counterAtoms ++) {
      var atom = atomsSorted[counterAtoms];
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
  if (theExamples.innerHTML.length < 300) {
    submitRequests.submitGET({
      url: `${pathnames.urls.calculatorAPI}?request=calculatorExamplesJSON`,
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
  var thePage = window.calculator.mainPage;
  var calculatorInput = document.getElementById(ids.domElements.inputMain).value;
  if (calculatorInput === this.lastSubmittedInput) {
    return;
  }
  this.lastSubmittedInput = calculatorInput;
  //submitComputationPartTwo is called by a callback in the function below:
  thePage.storage.variables.calculator.input.setAndStore(this.lastSubmittedInput);
}

/**@returns {String} */
Calculator.prototype.getComputationLink = function(input) {
  var theURL = {
    currentPage: "calculator",
    calculatorInput: input,
  };
  var thePage = window.calculator.mainPage;
  var stringifiedHash = thePage.storage.getCleanedUpURL(theURL);
  return stringifiedHash;
}

Calculator.prototype.defaultOnLoadInjectScriptsAndProcessLaTeX = function(input, output) { 
  var inputParsed = null;
  var inputHtml = null;
  var commentsHtml = "";
  var performanceHtml = "";
  var logParsing = "";
  var numEntries = 0;
  var panelIdPairs = [];
  try {
    inputParsed = JSON.parse(input);
    inputHtml = inputParsed.resultHtml;
    commentsHtml = inputParsed.comments;
    performanceHtml = inputParsed.performance;
    logParsing = inputParsed.parsingLog;
    var buffer = new BufferCalculator();
    buffer.write(`<table><tr><td>`);
    buffer.write(`<table class = "tableCalculatorOutput"><tr><th>Input</th><th>Output</th></tr>`);
    numEntries = Math.max(inputParsed.result.input.length, inputParsed.result.output.length);

    for (var i = 0; i < numEntries; i ++) {
      this.numberOfCalculatorPanels ++;
      var inputPanelId = `calculatorInputPanel${this.numberOfCalculatorPanels}`;
      var outputPanelId = `calculatorOutputPanel${this.numberOfCalculatorPanels}`;
      panelIdPairs.push([inputPanelId, outputPanelId]);
      buffer.write("<tr>");
      buffer.write(`<td class = "cellCalculatorInput"> <span id = "${inputPanelId}"></span></td>`)
      buffer.write(`<td class = "cellCalculatorResult"><span id = "${outputPanelId}"></span></td>`);
      buffer.write("</tr>");    
    }
    buffer.write("</table>");  
    buffer.write(`</td><td>${performanceHtml} ${commentsHtml}</td></tr><table>${logParsing}`)
    inputHtml = buffer.toString();
  } catch (e) {
    inputHtml = input + "<br>" + e;
  }
  var spanVerification = document.getElementById(ids.domElements.spanCalculatorMainOutput);
  spanVerification.innerHTML = inputHtml;
  for (var i = 0; i < panelIdPairs.length; i ++) {
    var currentInput = inputParsed.result.input[i];
    var currentOutput = inputParsed.result.output[i];
    var inputPanelId = panelIdPairs[i][0];
    var outputPanelId = panelIdPairs[i][1];
    var inputPanel = new PanelExpandable(inputPanelId, true);
    var outputPanel = new PanelExpandable(outputPanelId, true);
    if (currentInput.length > 200) {
      inputPanel.setPanelContent(currentInput);
      inputPanel.doToggleContent();
      inputPanel.matchPanelStatus();
    } else {
      document.getElementById(inputPanelId).innerHTML = currentInput;
    }
    if (currentOutput.length > 150) {
      outputPanel.setPanelContent(currentOutput);
      outputPanel.doToggleContent();
      outputPanel.matchPanelStatus();
    } else {
      document.getElementById(outputPanelId).innerHTML = currentOutput;
    }
  }
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
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById(ids.domElements.spanCalculatorMainOutput)]);
  MathJax.Hub.Queue([this.addListenersToInputBoxes.bind(this)]);
//  alert(theString);
}

Calculator.prototype.submitComputationPartTwo = function(input) {
  //<- this function is called by a callback trigerred when calling 
  //thePage.storage.variables.calculator.input.setAndStore(...)
  var thePage = window.calculator.mainPage;
  var stringifiedHash = thePage.storage.getCleanedUpURL(thePage.storage.urlObject);
  document.getElementById("spanComputationLink").innerHTML = `<a href = '#${stringifiedHash}'>Link to your input</a>`;

  var url = pathnames.urls.calculatorAPI;
  var parameters = this.getQueryStringSubmitStringAsMainInput(input, pathnames.urlFields.calculatorCompute);
  submitRequests.submitPOST ({
    url: url,
    parameters:  parameters,
    callback: this.defaultOnLoadInjectScriptsAndProcessLaTeX.bind(this),
    progress: ids.domElements.spanProgressCalculatorInput
  });
}

Calculator.prototype.callbackToggleStatus = function (input, output) {
  var monitorResult = document.getElementById(ids.domElements.monitoring.spanStatus);
  var parsedInput = JSON.parse(input);
  var monitoring = parsedInput["monitoring"];
  if (monitoring === "true" || monitoring === true) {
    monitorResult.innerHTML = "Monitor <b style = 'color:red'>on</b>";
    document.getElementById(ids.domElements.switch.monitoring).checked = true;
    document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).style.display = "";
  } else {
    document.getElementById(ids.domElements.switch.monitoring).checked = false;
    monitorResult.innerHTML = "Monitor <b style = 'color:green'>off</b>";
    document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).style.display = "none";
  }
}

Calculator.prototype.callbackToggleMonitoring = function (input, output) {
  var url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}`;
  submitRequests.submitGET({
    url: url,
    result: ids.domElements.monitoring.spanStatus,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: this.callbackToggleStatus.bind(this)
  });
}

Calculator.prototype.toggleMonitoring = function () {
  var url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.toggleMonitoring}`;
  submitRequests.submitGET({
    url: url,
    callback: this.callbackToggleMonitoring.bind(this),
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

Calculator.prototype.getQueryStringSubmitStringAsMainInput = function(theString, requestType) {
  var inputParams = '';
  var thePage = window.calculator.mainPage;
  inputParams += `${pathnames.urlFields.request}=${requestType}&`;
  inputParams += `${pathnames.urlFields.mainInput}=${encodeURIComponent(theString)}&`;
  if (thePage.storage.variables.flagDebug.isTrue()) {
    inputParams += `${pathnames.urlFields.debugFlag}=true&`;
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
}
