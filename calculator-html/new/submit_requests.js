"use srict";

function recordProgressDone(progress, timeFinished) {
  if (progress === null || progress === undefined) {
    return;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  var theButton = progress.childNodes[0];
  var timeTotal = timeFinished - progress.getAttribute("timeStarted");
  theButton.innerHTML = `<b style ='color:green'>Received</b> ${timeTotal} ms`;
}

function recordProgressStarted(progress, address, isPost, timeStarted) {
  if (progress === null || progress === undefined) {
    return;
  }
  if (isPost === undefined || isPost === null) {
    isPost = false;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  progress.setAttribute("timeStarted", timeStarted);
  var theHTML = "";
  theHTML += `<button class = "buttonProgress" onclick = "if (this.nextSibling.nextSibling.style.display === 'none')
{this.nextSibling.nextSibling.style.display = '';} else {this.nextSibling.nextSibling.style.display = 'none';}">`;
  theHTML += `<b style ="color:orange">Sent</b></button>`;
  theHTML += `<br><span class ="spanProgressReport" style ="display:none">`;
  if (!isPost) {
    theHTML += `<a href='${address}' target ='_blank'>${address}</a></span>`;
  } else {
    theHTML += address;
  }
  progress.innerHTML = theHTML;
}

function recordResult(resultText, resultSpan) {
  if (resultSpan === null || resultSpan === undefined) {
    return;
  }
  if (typeof resultSpan === "string") {
    resultSpan = document.getElementById(resultSpan);
  }
  resultSpan.innerHTML = escape(resultText);
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
  if (theAddress === undefined || theAddress === null) {
    theAddress = `${globalVars.serverInfo.imappServer}${inputObject.service}`;
  }
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
    document.body.appendChild(spanOutput);
    spanOutput.innerHTML = `<span style ='color:red'> ERROR: span with id ${idOutput} MISSING! </span>`;
  }
  var https = new XMLHttpRequest();
  https.open("POST", pathnames.calculatorAPI, true);
  https.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  if (idStatus === undefined) {
    idStatus = idOutput;
  }
  console.log("DEBUG: id status: " + idStatus);
  timeOutCounter = 0;

  var postRequest = `POST ${pathnames.calculatorAPI}<br>message: ${inputParams}`;
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

function submitStringAsMainInput(theString, idOutput, requestType, onLoadFunction, idStatus) {
  var inputParams = '';
  inputParams += 'request=' + requestType;
  inputParams += '&mainInput=' + encodeURIComponent(theString);
  if (thePage.flagDebug === true) {
    inputParams += "&debugFlag=true";
  }
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
  var progReportTimer = document.getElementById("idProgressReportTimer");
  if (isPaused) {
    return;
  }
  progReportTimer.innerHTML = "<hr>Refreshing every " + timeIncrementInTenthsOfSecond / 10 +
  " second(s). Client time: ~" + Math.floor(timeOutCounter / 10) + " second(s)<br>";
  timeOutCounter += timeIncrementInTenthsOfSecond;
  var sURL  = `${pathnames.calculatorAPI}?request=indicator&mainInput=${currentWorkerNumber}`;
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
      progReport.innerHTML = newReportString + "<hr>";
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
  var pauseURL = `${pathnames.calculatorAPI}?request=pause&mainInput=${currentWorkerNumber}`;
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

function submitCalculatorInputOnEnter() { 
  if (event.keyCode !== 13 || !event.shiftKey) {
    return;
  }
  submitCalculatorComputation();
  event.preventDefault();
}

function submitCalculatorComputation() { 
  submitStringAsMainInput(
    document.getElementById('mainInputID').value,
    'calculatorOutput', 
    'compute', 
    defaultOnLoadInjectScriptsAndProcessLaTeX,
    'mainComputationStatus'
  );
}
