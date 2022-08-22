"use strict";
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const BufferCalculator = require("./buffer").BufferCalculator;
const miscellaneous = require("./miscellaneous_frontend");

class Monitor {
  constructor() {
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

  pauseButton() {
    return document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle);
  }

  start(
    /**@type{String} */
    workerId,
  ) {
    this.isFinished = false;
    this.isPaused = false;
    this.timeOutCounter = 0;
    this.timeOutOldCounter = 0;
    this.currentWorkerId = workerId;
    let pauseButton = this.pauseButton();
    pauseButton.innerHTML = "Pause";
    pauseButton.style.display = "";
    this.progressReport();
  }

  progressReport() {
    // Process monitor is started by
    if (this.isFinished) {
      return;
    }
    if (this.isPaused) {
      return;
    }
    this.timeOutOldCounter = this.timeOutCounter;
    this.timeOutCounter += this.timeIncrement;
    let sURL = "";
    sURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.indicator}`;
    sURL += `&${pathnames.urlFields.requests.workerId}=${this.currentWorkerId}`;
    submitRequests.submitGET({
      url: sURL,
      progress: ids.domElements.pages.calculator.progress,
      callback: this.callbackPauseRequest.bind(this),
    });
    this.clearTimeout();
    this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
  }

  clearTimeout() {
    clearTimeout(this.currentTimeOutHandler);
  }

  pauseButtonMarkFinished() {
    let pauseButton = this.pauseButton();
    pauseButton.innerHTML = "finished";
    pauseButton.style.display = "none";
  }

  callbackPauseRequest(input, output) {
    let progressReportContent = "";
    if (input === "") {
      this.clearTimeout();
      this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
      return;
    }
    let indicatorButton = this.pauseButton();
    this.ownerCalculator.parsedComputation = miscellaneous.jsonUnescapeParse(input);
    let status = this.ownerCalculator.parsedComputation.status;
    let doUpdateCalculatorPage = false;
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
      this.pauseButtonMarkFinished();
    } else if (status === "finished" || status === "crash") {
      this.isFinished = true;
      this.isPaused = false;
      this.pauseButtonMarkFinished();
      doUpdateCalculatorPage = true;
    } else if (status === "paused") {
      this.isPaused = true;
      indicatorButton.innerHTML = "Continue";
    } else {
      if (status === "noReport") {
        progressReportContent += "No report on last ping. ";
      } else if (status === "unpaused") {
        progressReportContent += "Recently unpaused. ";
      } else {
        doUpdateCalculatorPage = true;
      }
      this.isPaused = false;
      indicatorButton.innerHTML = "Pause";
      this.clearTimeout();
      this.currentTimeOutHandler = setTimeout(this.progressReport.bind(this), this.timeIncrement * 1000);
    }
    progressReportContent += `Refreshing every ${this.timeIncrement} second(s). `;
    progressReportContent += `Client time: ~${Math.floor(this.timeOutOldCounter)} second(s)<br>`;
    let progReportTimer = document.getElementById(ids.domElements.pages.calculator.monitoring.progressTimer);
    progReportTimer.innerHTML = progressReportContent;

    if (doUpdateCalculatorPage) {
      this.ownerCalculator.panels.length = 0;
      let buffer = new BufferCalculator();
      this.ownerCalculator.writeResult(buffer, this.ownerCalculator.parsedComputation, this.ownerCalculator.panels);
      let resultComponent = document.getElementById(
        ids.domElements.pages.calculator.divCalculatorMainOutput
      );
      resultComponent.innerHTML = buffer.toString();
      this.ownerCalculator.afterWriteOutput();
    }
  }

  togglePause() {
    if (this.isFinished) {
      return;
    }
    let pauseURL = "";
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
      progress: ids.domElements.pages.calculator.progress
    });
  }
}

let monitor = new Monitor();

module.exports = {
  monitor
};