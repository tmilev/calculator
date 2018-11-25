"use strict";
const pathnames = require('./pathnames');
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');

function Monitor() {
  this.isPaused = false;
  this.isFinished = true;
  this.timeIncrement = 2;
  this.timeOutOldCounter = 0;
  this.timeOutCounter = 0;
  this.currentWorkerNumber = - 1; 
  this.currentTimeOutHandler = null;
}

Monitor.prototype.start = function(inputWorkerNumber) {
  this.isFinished = false;
  this.isPaused = false;
  this.timeOutCounter = 0;
  this.timeOutOldCounter = 0;
  this.currentWorkerNumber = Number(inputWorkerNumber);
  document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).innerHTML = "Pause";
  this.progressReport();
}

Monitor.prototype.progressReport = function() { 
  if (this.isFinished) {
    return;
  }
  clearTimeout(this.timeoutID);
  if (this.isPaused) {
    return;
  }
  this.timeOutOldCounter = this.timeOutCounter;
  this.timeOutCounter += this.timeIncrement;
  var sURL = "";
  sURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.indicator}`;
  sURL += `&${pathnames.urlFields.requests.mainInput}=${this.currentWorkerNumber}`;
  submitRequests.submitGET({
    url: sURL,
    progress: ids.domElements.spanProgressCalculatorInput,
    callback: this.callbackPauseRequest.bind(this), 
  });
  clearTimeout(this.currentTimeOutHandler);
  this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
}

Monitor.prototype.callbackPauseRequest = function(input, output) {
  clearTimeout(this.currentTimeOutHandler);
  var progressReportContent = "";
  progressReportContent += `Refreshing every ${this.timeIncrement} second(s). `;
  progressReportContent += `Client time: ~${Math.floor(this.timeOutOldCounter)} second(s)<br>`;
  var progReportTimer = document.getElementById(ids.domElements.monitoring.progressTimer);
  progReportTimer.innerHTML = progressReportContent;
  if (input === "") {
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
    return;
  }
  var indicatorButton = document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest);
  var resultComponent = document.getElementById(ids.domElements.spanCalculatorMainOutput);
  var resultJSON = JSON.parse(input);
  var status = resultJSON.status;
  var resultContent = resultJSON.data;
  if (resultContent !== null && resultContent !== undefined) {
    resultComponent.innerHTML = resultContent;
  }
  if (status === "finished") {
    this.isFinished = true;
    this.isPaused = false;
    indicatorButton.innerHTML = "Finished";
  } else if (status === "paused") {
    this.isPaused = true;
    indicatorButton.innerHTML = "Continue";
  } else {
    this.isPaused = false;
    indicatorButton.innerHTML = "Pause";
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
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