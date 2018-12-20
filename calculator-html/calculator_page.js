"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const drawing = require('./three-d').drawing;
const ids = require('./ids_dom_elements');
const hideHtmlWithTags = require('./hide_html_with_tags');

function Calculator() {

  this.inputBoxNames = [];
  this.inputBoxToSliderUpdaters = {};
  this.canvases = null;
  this.examples = null;
  this.submissionCalculatorCounter = 0;
  this.lastSubmittedInput = "";
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

Calculator.prototype.exampleClick = function(theLink) {
  var theAtom = decodeURIComponent(theLink.atom);
  var theIndex = theLink.index;
  var isComposite = theLink.composite;
  var theHandler = null;
  //console.log(theLink);
  if (isComposite) {
    theHandler = this.examples[theAtom].composite[theIndex];
  } else {
    theHandler = this.examples[theAtom].regular[theIndex];
  }

  //console.log(theHandler);
  var mainInput = document.getElementById(ids.domElements.inputMain);
  mainInput.value = theHandler.example;
  this.submitComputation();
}

function processOneFunctionAtom(handlers, isComposite) {
  var resultString = "";
  for (var counterHandlers = 0; counterHandlers < handlers.length; counterHandlers ++) {
    resultString += "<br>";
    var currentDescription = handlers[counterHandlers].description;
    var currentExample = handlers[counterHandlers].example;
    resultString += `<calculatorAtom>${handlers[counterHandlers].atom}</calculatorAtom>`;
    if (handlers[counterHandlers].composite === "true")
        resultString += "<calculatorCompositeAtom>(composite)</calculatorCompositeAtom>";
    resultString += ` (${counterHandlers + 1} out of ${handlers.length})`;
    var currentId = "example_";
    if (isComposite) {
      currentId += "t_";
    } else {
      currentId += "f_";
    }
    var encodedAtom = encodeURIComponent(handlers[counterHandlers].atom);
    currentId += `${encodedAtom}_${counterHandlers}_${handlers.length}`;
    resultString += `<a href = '#' class = 'linkInfo' onclick = "window.calculator.hideHtmlWithTags.switchMenu('${currentId}')">info</a>`;
    resultString += `<calculatorExampleInfo id = "${currentId}" class = "hiddenClass">${currentDescription}<br><b>Example:</b><br>${currentExample}</calculatorExampleInfo>`;
    resultString += `<a href = "#" class = "linkInfo" onclick = "this.composite =${isComposite}; this.index =${counterHandlers}; this.atom='${encodedAtom}'; window.calculator.calculator.exampleClick(this);"> Example</a>`;
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
      examplesString += processOneFunctionAtom(currentExamples.regular, false);
      examplesString += processOneFunctionAtom(currentExamples.composite, true);
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
    hideHtmlWithTags.switchMenu(ids.domElements.calculatorExamples);
    if (! theExamples.classList.contains("hiddenClass")) {
      theButton.innerHTML = "&#9660;";
    } else {
      theButton.innerHTML = "&#9656;";
    }
  }
}

Calculator.prototype.calculatorLinkClickHandler = function(theAnchor) {
  window.calculator.mainPage.storage.loadSettings(theAnchor.href.split('#')[1]); 
  this.submitComputation();
}

Calculator.prototype.submitComputation = function() {
  var result = "";
  var thePage = window.calculator.mainPage;
  var calculatorInput = document.getElementById(ids.domElements.inputMain).value;
  if (calculatorInput === this.lastSubmittedInput) {
    return;
  }
  this.lastSubmittedInput = calculatorInput;


  var calculatorInputEncoded = encodeURIComponent(calculatorInput);
  var theJSON = {
    calculatorInput : calculatorInputEncoded,
    currentPage: thePage.pages.calculator.name,
  };
  this.submissionCalculatorCounter ++;
  var theId = `submitCalculatorLink${this.submissionCalculatorCounter}`;
  result += `<a href = '#${JSON.stringify(theJSON)}' id = "${theId}">Link to your input</a>`;
  document.getElementById("spanComputationLink").innerHTML = result;
  var theAnchor = document.getElementById(theId); 
  theAnchor.addEventListener('click', this.calculatorLinkClickHandler.bind(this, theAnchor));
  var url = pathnames.urls.calculatorAPI;
  var parameters = this.getQueryStringSubmitStringAsMainInput(this.lastSubmittedInput, pathnames.urlFields.calculatorCompute);
  submitRequests.submitPOST ({
    url: url,
    parameters:  parameters,
    result: ids.domElements.spanCalculatorMainOutput,
    callback: thePage.defaultOnLoadInjectScriptsAndProcessLaTeX.bind(thePage),
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
