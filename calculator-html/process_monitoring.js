"use strict";
const pathnames = require('./pathnames');
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');

function Monitor() {
  this.isPaused = false;
  this.isFinished = true;
  this.timeIncrementInTenthsOfSecond = 20;
  this.timeOutCounter = 0;
  this.currentWorkerNumber = - 1; 
}

Monitor.prototype.start = function(inputWorkerNumber) {
  this.isFinished = false;
  this.isPaused = false;
  this.currentWorkerNumber = inputWorkerNumber;
}

Monitor.prototype.progressReport = function() { 
  if (isFinished) {
    return;
  }
  clearTimeout(this.timeoutID);
  var progReportTimer = document.getElementById("idProgressReportTimer");
  if (isPaused) {
    return;
  }
  progressReportContent = "";
  progressReportContent += `<hr>Refreshing every ${timeIncrementInTenthsOfSecond / 10} second(s). `;
  progressReportContent += `Client time: ~${Math.floor(timeOutCounter / 10)} second(s)<br>`;
  progReportTimer.innerHTML += progressReportContent;
  timeOutCounter += timeIncrementInTenthsOfSecond;
  var sURL = "";
  sURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.indicator}`;
  sURL += `&${pathnames.urlFields.requests.mainInput}=${currentWorkerNumber}`;
  submitRequests.submitGET({
    url: sURL,
    progress: ids.domElements.spanProgressCalculatorInput,
    result: ids.domElements.spanCalculatorMainOutput
  });
}

Monitor.prototype.callbackPauseRequest = function(input, output) {
  var isPaused = null;
  if (input === "paused") {
    isPaused = true;
    document.getElementById("idButtonSendTogglePauseRequest").innerHTML = "Continue";
  } else {
    isPaused = false;
    document.getElementById("idButtonSendTogglePauseRequest").innerHTML = "Pause";
  }
}

Monitor.prototype.togglePause = function() {
  if (isFinished) {
    return;
  }
  var pauseURL = "";
  pauseURL += `${pathnames.urls.calculatorAPI}?`
  pauseURL += `${pathnames.urls.request}=${pathnames.urlFields.requests.pause}&`;
  pauseURL += `${pathnames.urlFields.requests.mainInput}=${currentWorkerNumber}&`;
  submitRequests.submitGET({
    url: pauseURL,
    callback: callbackPauseRequest,
    progress: ids.domElements.spanProgressCalculatorInput
  });
}

var monitor = new Monitor();

module.exports = {
  monitor
}