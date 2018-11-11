"use srict";
const pathnames = require('./pathnames');
const miscellaneous = require('./miscellaneous');

function recordProgressDone(progress, timeFinished) {
  if (progress === null || progress === undefined || progress === "") {
    return;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  var panelInfo = getPanelInfoInitializeIfNeeded(progress);
  var messageComponent = document.getElementById(panelInfo.messageStatusId);
  var timeTotal = timeFinished - progress.getAttribute("timeStarted");
  messageComponent.innerHTML = `<b style ='color:green'>Received</b> ${timeTotal} ms`;
}

function setStatusToBeCalledThroughTimeout(panel, status, originalHeight) {
  if (status === "expanded") {
    panel.style.maxHeight = originalHeight;
    panel.style.height = originalHeight;
    //panel.style.opacity = "1";
  } else {
    panel.style.maxHeight = "0px";
    panel.style.height = "0px";
    //panel.style.opacity = "0";
  }
}

function doToggleContent(progressId) {
  var progress = document.getElementById(progressId);
  var panelInfo = getPanelInfoInitializeIfNeeded(progress);
  var panel = document.getElementById(panelInfo.panelId);
  if (panelInfo.panelStatus === "collapsed") {
    document.getElementById(panelInfo.expandedMarkerId).innerHTML = "&#9662;";    
    panelInfo.panelStatus = "expanded";
  } else {
    document.getElementById(panelInfo.expandedMarkerId).innerHTML = "&#9666;";    
    panelInfo.panelStatus = "collapsed";
  }
  progress.setAttribute("panelStatus", panelInfo.panelStatus);
  setTimeout(setStatusToBeCalledThroughTimeout.bind(null, panel, panelInfo.panelStatus, panelInfo.originalHeight), 0);
}

function convertStringToHtml(input) {
  var result = "";
  for (var counter = 0; counter < input.length; counter ++) {
    if (input[counter] === '&') {
      result += '&amp;';
      continue;
    }
    result += input[counter];
  }
  return result;
}

var numberOfButtonToggleProgressReport = 0;

/** @typedef {{buttonId: string, panelId: string, expandedMarkerId: string, messageStatusId: string, panelStatus: string, originalHeight: string}} PanelInfo */
/** @returns {PanelInfo} */
function getPanelInfoInitializeIfNeeded(progressDOMComponent) {
  var result = {
    buttonId: null, 
    panelId: null,
    expandedMarkerId: null,
    messageStatusId: null,
    panelStatus: null,
    originalHeight: "0px",
  }
  if (progressDOMComponent === null || progressDOMComponent === undefined) {
    return result;
  }
  if (typeof progressDOMComponent === "string") {
    progressDOMComponent = document.getElementById(progressDOMComponent);
  }
  var isGood = true;
  for (var label in result) {
    var incoming = progressDOMComponent.getAttribute(label);
    result[label] = incoming;
    if (incoming === null || incoming === undefined || incoming === undefined) {
      isGood = false;
    }
  }
  if (!isGood) {
    numberOfButtonToggleProgressReport ++;
    for (var label in result) {
      result[label] = `progressReport${label}${numberOfButtonToggleProgressReport}`;
    }
    result.panelStatus = "collapsed";
    initializeProgressPanel(progressDOMComponent, result);
  }
  return result;
}

function initializeProgressPanel(progressDOMComponent, /**@type {PanelInfo} */result) {
  for (var label in result) {
    progressDOMComponent.setAttribute(label, result[label]);
  }  
  var spanContainer = document.createElement("span");
  var spanContent = "";
  spanContent += `<button  id = "${result.buttonId}" `; 
  spanContent += `class = "buttonProgress accordionLikeIndividual" `;
  spanContent += `onclick = "window.calculator.submitRequests.doToggleContent('${progressDOMComponent.id}');">`;
  spanContent += `<span id = "${result.messageStatusId}"></span> <span id = "${result.expandedMarkerId}">&#9666;</span>`;
  spanContent += "</button>";
  spanContent += `<div id = "${result.panelId}" class = "spanProgressReport panelExpandable"></div>`;
  spanContainer.innerHTML = spanContent;
  progressDOMComponent.appendChild(spanContainer);
}

function recordProgressStarted(progress, address, isPost, timeStarted) {
  if (progress === "" || progress === null || progress === undefined) {
    return;
  }
  if (isPost === undefined || isPost === null) {
    isPost = false;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  var panelInfo = getPanelInfoInitializeIfNeeded(progress);
  progress.setAttribute("timeStarted", timeStarted);

  var messageStatus = document.getElementById(panelInfo.messageStatusId);
  var panel = document.getElementById(panelInfo.panelId);
  messageStatus.innerHTML = `<b style ="color:orange">Sent</b>`
  var addressSpreadOut = address.split("&").join(" &");
  addressSpreadOut = addressSpreadOut.split("=").join("= ");
  addressSpreadOut = addressSpreadOut.split("?").join("? ");
  var addressPassSplit = addressSpreadOut.split("password=");
  if (addressPassSplit.length > 1) {
    var indexAmpersand = addressPassSplit[1].search("&");
    addressPassSplit[1] = addressPassSplit[1].substr(indexAmpersand);
    addressPassSplit[1] = "***" + addressPassSplit[1];
    addressSpreadOut = addressPassSplit.join("password= ");
  }
  var content = "";
  if (!isPost) {
    content += `<a href='${address}' target ='_blank' class = 'linkProgressReport'>${convertStringToHtml(addressSpreadOut)}</a>`;
  } else {
    content += addressSpreadOut;
  }
  panel.innerHTML = content;
  panel.style.maxHeight = "";
  panel.style.height = "";
  var originalHeight = window.getComputedStyle(panel).height;
  progress.setAttribute("originalHeight", originalHeight);
  if (panelInfo.panelStatus === "collapsed") {
    panel.style.maxHeight = "0px";
    panel.style.height = "0px";
  } else {
    panel.style.maxHeight = originalHeight;
    panel.style.height = originalHeight;
  }
}

function recordResult(resultText, resultSpan) {
  if (resultSpan === null || resultSpan === undefined) {
    return;
  }
  if (typeof resultSpan === "string") {
    resultSpan = document.getElementById(resultSpan);
  }
  resultSpan.innerHTML = resultText;
}

/**
 * Fires up a get requests.
 * Expected input: an object with the following fields filled in.
 *
 * inputObject.url: url of the address to get. If omitted, service must be specified.
 *
 * inputObject.service: if url is missing, this is appended to the default server name and used
 *   in place of inputObject.url. If inputObject.url is specified, this argument is ignored.
 *
 * inputObject.callback: function to callback. The function will be passed on
 *   as arguments the received result.
 *   The result may in addition be displayed in the component inputObject.result, should
 *   this object be provided.
 *   The function will be called only if the get operation was successful.
 *
 * inputObject.progress: id or handle of an object to display the progress of the operation.
 *   Indended for developer use.
 *   Will create a button whose label shows progress of the operation and
 *   clicking which will show/hide the address.
 *   Pass null or undefined if you don't want any progress report.
 *
 * inputObject.result: id or handle of an object to dump the html-escaped
 *   but otherwise non-processed final result.
 *   Pass null or undefined if you don't want to show the result.
 */
function submitGET(inputObject) {
  var theAddress = inputObject.url;
  var progress = inputObject.progress;
  var result = inputObject.result;
  var callback = inputObject.callback;
  var xhr = new XMLHttpRequest();
  recordProgressStarted(progress, theAddress, false, (new Date()).getTime());
  xhr.open('GET', theAddress, true);
  xhr.setRequestHeader('Accept', 'text/html');
  xhr.onload = function () {
    recordProgressDone(progress, (new Date()).getTime());
    recordResult(xhr.responseText, result);
    if (callback !== undefined && callback !== null) {
      callback(xhr.responseText, result);
    }
  };
  xhr.send();
}

function submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus) {
  var spanOutput = document.getElementById(idOutput);
  if (spanOutput === null) {
    spanOutput = document.createElement('span');
    spanOutput.innerHTML = `<span style ='color:red'> ERROR: span with id ${idOutput} MISSING! </span>`;
    document.body.appendChild(spanOutput);
  }
  var https = new XMLHttpRequest();
  https.open("POST", pathnames.urls.calculatorAPI, true);
  https.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  if (idStatus === undefined) {
    idStatus = idOutput;
  }
  timeOutCounter = 0;

  var postRequest = `POST ${pathnames.urls.calculatorAPI}<br>message: ${miscellaneous.shortenString(inputParams, 200)}`;
  recordProgressStarted(idStatus, postRequest, true, (new Date()).getTime());

  https.onload = function() { 
    recordProgressDone(idStatus, (new Date()).getTime());
    spanOutput.innerHTML = https.responseText;
    if (onLoadFunction !== undefined && onLoadFunction !== null && onLoadFunction !== 0) {
      onLoadFunction(idOutput);
    }
  }
  ////////////////////////////////////////////
  https.send(inputParams);
  ////////////////////////////////////////////
}

function getQueryStringSubmitStringAsMainInput(theString, requestType) {
  var inputParams = '';
  var thePage = window.calculator.mainPage;
  inputParams += `${pathnames.urlFields.request}=${requestType}&`;
  inputParams += `${pathnames.urlFields.mainInput}=${encodeURIComponent(theString)}&`;
  if (thePage.flagDebug === true) {
    inputParams += `${pathnames.urlFields.debugFlag}=true&`;
  }
  return inputParams;
}

function submitStringAsMainInput(theString, idOutput, requestType, onLoadFunction, idStatus) {
  var inputParams = getQueryStringSubmitStringAsMainInput(theString, requestType);
  submitStringCalculatorArgument(inputParams, idOutput, onLoadFunction, idStatus);
}

var isPaused = false;
var isFinished = false;
var timeIncrementInTenthsOfSecond = 20; //measured in tenths of a second\n";
var timeOutCounter = 0; //measured in tenths of a second\n";
var currentWorkerNumber = - 1;
function progressReport() { 
  if (isFinished) {
    return;
  }
  clearTimeout(this.timeoutID);
  var progReport = document.getElementById("idProgressReport");
  var requestStatus = document.getElementById("idProgressReportRequestStatus");
  var progressReportTimer = document.getElementById("idProgressReportTimer");
  if (isPaused) {
    return;
  }
  var progressReportContent = "";
  progressReportContent += `<hr>Refreshing every ${timeIncrementInTenthsOfSecond / 10}`;
  progressReportContent += ` second(s). Client time: ~ ${Math.floor(timeOutCounter / 10)} second(s)<br>`;
  progressReportTimer.innerHTML = progressReportContent;
  timeOutCounter += timeIncrementInTenthsOfSecond;
  var sURL  = `${pathnames.urls.calculatorAPI}?request=indicator&mainInput=${currentWorkerNumber}`;
  var https = new XMLHttpRequest();
  https.open("GET", sURL, true);
  https.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  https.onload = function() {
    newReportString = https.responseText;
    if (https.responseText === "finished") { 
      isFinished = true;
      requestStatus.innerHTML = "<span style ='color:green'><b>Computation finished.</b></span>";
      return;
    }
    if (https.responseText !== "") { 
      progReport.innerHTML = `${newReportString}<hr>`;
      requestStatus.innerHTML = '';
    } else {
      requestStatus.innerHTML = "<span style ='color:red'><b>Empty response</b></span>";
    }
  }
  ////////////////////////////////////////////
  requestStatus.innerHTML = "<span style ='color:orange'><b>Request sent</b></span>";
  https.send(null);
  this.timeoutID = window.setTimeout("progressReport()", timeIncrementInTenthsOfSecond * 100);
}

function SendTogglePauseRequest() { 
  if (isFinished) {
    return;
  }
  var requestStatus = document.getElementById("idProgressReportRequestStatus");
  var pauseRequest = new XMLHttpRequest();
  var pauseURL = `${pathnames.urls.calculatorAPI}?request=pause&mainInput=${currentWorkerNumber}`;
  pauseRequest.open("GET", pauseURL, true);
  pauseRequest.onload = function() {
    if (pauseRequest.status !== 200) {
      return;
    }
    requestStatus.innerHTML = pauseRequest.responseText;
    if (pauseRequest.responseText === "paused") {
      isPaused = true;
    }
    if (pauseRequest.responseText === "unpaused") {
      isPaused = false;
    }
    if (isPaused) {
      document.getElementById("idButtonSendTogglePauseRequest").innerHTML = "Continue";
    } else {
      document.getElementById("idButtonSendTogglePauseRequest").innerHTML = "Pause";
    }
    document.getElementById("idProgressReportRequestStatus").innerHTML = pauseRequest.responseText;
    if (!isPaused) {
      progressReport();
    }
  };
  pauseRequest.send(null);
}

var submissionCalculatorCounter = 0;
function calculatorLinkClickHandler() {
  window.calculator.mainPage.loadSettings(this.href.split('#')[1]); 
  submitCalculatorComputation();
}

function submitCalculatorComputation() {
  var result = "";
  var thePage = window.calculator.mainPage;
  var calculatorInput = document.getElementById("mainInputID").value;
  if (calculatorInput === thePage.calculatorInputLastSubmitted) {
    return;
  }
  thePage.calculatorInputLastSubmitted = calculatorInput;
  var calculatorInputEncoded = encodeURIComponent(calculatorInput);
  var theJSON = {
    calculatorRequest: "calculatorOutput",
    calculatorInput : calculatorInputEncoded,
    currentPage: thePage.pages.calculator.name
  };
  var theId = `submitCalculatorLink${submissionCalculatorCounter}`;
  result += `<a href = '#${encodeURIComponent(JSON.stringify(theJSON))}' id = "${theId}">Link to your input</a>`;
  document.getElementById("spanComputationLink").innerHTML = result;
  theAnchor = document.getElementById(theId); 
  theAnchor.addEventListener('click', calculatorLinkClickHandler.bind(theAnchor));
  submitStringAsMainInput(
    document.getElementById("mainInputID").value,
    "calculatorOutput", 
    "compute", 
    thePage.defaultOnLoadInjectScriptsAndProcessLaTeX.bind(thePage),
    "mainComputationStatus"
  );
}

module.exports = {
  submitCalculatorComputation,
  submitGET,
  doToggleContent,
}