"use strict";
const pathnames = require('./pathnames');
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');
const BufferCalculator = require('./buffer').BufferCalculator;

function Monitor() {
  this.isPaused = false;
  this.isFinished = true;
  this.timeIncrement = 2;
  this.timeOutOldCounter = 0;
  this.timeOutCounter = 0;
  /**@type{string} */
  this.currentWorkerId = ""; 
  this.currentTimeOutHandler = null;
  this.ownerCalculator = null;
}

Monitor.prototype.start = function(
  /**@type{String} */
  workerId
) {
  this.isFinished = false;
  this.isPaused = false;
  this.timeOutCounter = 0;
  this.timeOutOldCounter = 0;
  this.currentWorkerId = workerId;
  document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).innerHTML = "Pause";
  this.progressReport();
}

Monitor.prototype.progressReport = function() { 
  // Process monitor is started by 
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
  sURL += `&${pathnames.urlFields.requests.workerId}=${this.currentWorkerId}`;
  submitRequests.submitGET({
    url: sURL,
    progress: ids.domElements.spanProgressCalculatorInput,
    callback: this.callbackPauseRequest.bind(this), 
  });
  clearTimeout(this.currentTimeOutHandler);
  this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
}

Monitor.prototype.clearTimeout = function() {
  clearTimeout(this.currentTimeOutHandler);
}

Monitor.prototype.callbackPauseRequest = function(input, output) {
  this.clearTimeout();
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
  this.ownerCalculator.parsedComputation = JSON.parse(input);
  var status = this.ownerCalculator.parsedComputation.status;
  var doUpdateCalculatorPage = false;
  if (status === "finished" || status === "crash") {
    this.isFinished = true;
    this.isPaused = false;
    indicatorButton.innerHTML = "Finished";
    doUpdateCalculatorPage = true;
  } else if (status === "paused") {
    this.isPaused = true;
    indicatorButton.innerHTML = "Continue";
  } else {
    if (status === "noReport") {
      progReportTimer.innerHTML += "No report on last ping."; 
    } else if (status === "unpaused") {
      progReportTimer.innerHTML += "Recently unpaused.";       
    } else {
      doUpdateCalculatorPage = true;
    }
    this.isPaused = false;
    indicatorButton.innerHTML = "Pause";
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
  }  
  if (doUpdateCalculatorPage) {
    this.ownerCalculator.panelIdPairs = [];
    var buffer = new BufferCalculator();
    this.ownerCalculator.writeResult(buffer, this.ownerCalculator.parsedComputation, this.ownerCalculator.panelIdPairs);
    var resultComponent = document.getElementById(ids.domElements.spanCalculatorMainOutput);
    resultComponent.innerHTML = buffer.toString();
    this.ownerCalculator.afterWriteOutput();
  }
}

Monitor.prototype.togglePause = function() {
  if (this.isFinished) {
    return;
  }
  var pauseURL = "";
  pauseURL += `${pathnames.urls.calculatorAPI}?`
  pauseURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.pause}&`;
  pauseURL += `${pathnames.urlFields.requests.workerId}=${this.currentWorkerId}&`;
  submitRequests.submitGET({
    url: pauseURL,
    callback: this.callbackPauseRequest.bind(this),
    progress: ids.domElements.spanProgressCalculatorInput
  });
}

var monitor = new Monitor();

module.exports = {
  monitor
};