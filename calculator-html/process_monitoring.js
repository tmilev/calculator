"use strict";
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const BufferCalculator = require("./buffer").BufferCalculator;
const miscellaneous = require("./miscellaneous");

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
  this.clearTimeout();
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
  this.clearTimeout();
  this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
}

Monitor.prototype.clearTimeout = function() {
  clearTimeout(this.currentTimeOutHandler);
}

Monitor.prototype.callbackPauseRequest = function(input, output) {
  var progressReportContent = "";
  if (input === "") {
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
    return;
  }
  var indicatorButton = document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest);
  this.ownerCalculator.parsedComputation = miscellaneous.jsonUnescapeParse(input);
  var status = this.ownerCalculator.parsedComputation.status;
  var doUpdateCalculatorPage = false;
  if (status === undefined || status === null) {
    if (this.ownerCalculator.parsedComputation.error !== null && this.ownerCalculator.parsedComputation.error !== undefined) {
      status = "error";
      progressReportContent += `<b>Error.</b> ${this.ownerCalculator.parsedComputation.error}`;
      progressReportContent += `<br>`;
    }
  }
  if (status === "error") {
    this.isFinished = true;
    this.isPaused = false;
    indicatorButton.innerHTML = "finished";
  } else if (status === "finished" || status === "crash") {
    this.isFinished = true;
    this.isPaused = false;
    indicatorButton.innerHTML = "finished";
    doUpdateCalculatorPage = true;
  } else if (status === "paused") {
    this.isPaused = true;
    indicatorButton.innerHTML = "Continue";
  } else {
    if (status === "noReport") {
      progressReportContent += "No report on last ping. "; 
    } else if (status === "unpaused") {
      progressReportContent += "Recently unpaused.";       
    } else {
      doUpdateCalculatorPage = true;
    }
    this.isPaused = false;
    indicatorButton.innerHTML = "Pause";
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
  }
  progressReportContent += `Refreshing every ${this.timeIncrement} second(s). `;
  progressReportContent += `Client time: ~${Math.floor(this.timeOutOldCounter)} second(s)<br>`;
  var progReportTimer = document.getElementById(ids.domElements.monitoring.progressTimer);
  progReportTimer.innerHTML = progressReportContent;

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
  pauseURL += `${pathnames.urls.calculatorAPI}?`;
  if (!this.isPaused) {
    pauseURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.pause}&`;
  } else {
    pauseURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.unpause}&`;
  }
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