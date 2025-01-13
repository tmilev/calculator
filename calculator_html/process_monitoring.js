"use strict";
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");

class Monitor {
  constructor() {
    this.isPaused = false;
    this.isFinished = true;
    this.timeIncrement = 2;
    this.timeOutOldCounter = 0;
    this.timeOutCounter = 0;
    /** @type {string} */
    this.currentWorkerId = "";
    this.currentTimeOutHandler = null;
    this.ownerCalculator = null;
  }

  pauseButton() {
    return document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle);
  }

  start(
    /** @type {String} */
    workerId,
  ) {
    this.isFinished = false;
    this.isPaused = false;
    this.timeOutCounter = 0;
    this.timeOutOldCounter = 0;
    this.currentWorkerId = workerId;
    let pauseButton = this.pauseButton();
    pauseButton.textContent = "Pause";
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
      callback: (input) => {
        this.callbackPauseRequest(input);
      }
    });
    this.clearTimeout();
    this.currentTimeOutHandler = setTimeout(() => {
      this.progressReport();
    }, this.timeIncrement * 1000);
  }

  clearTimeout() {
    clearTimeout(this.currentTimeOutHandler);
  }

  pauseButtonMarkFinished() {
    let pauseButton = this.pauseButton();
    pauseButton.textContent = "finished";
    pauseButton.style.display = "none";
  }

  callbackPauseRequest(input) {
    let progressReportContent = "";
    if (input === "") {
      this.clearTimeout();
      this.currentTimeOutHandler = setTimeout(() => {
        this.callbackPauseRequest();
      }, this.timeIncrement * 1000);
      return;
    }
    let indicatorButton = this.pauseButton();
    const parsed = miscellaneous.jsonUnescapeParse(input);
    let status = parsed.status;
    if (status === undefined || status === null) {
      const error = parsed.error;
      if (error !== null && error !== undefined) {
        status = "error";
        progressReportContent += `<b>Error.</b> ${parsed.error}`;
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
      this.writeFinalResult(parsed);
    } else if (status === "paused") {
      this.isPaused = true;
      indicatorButton.textContent = "Continue";
    } else {
      if (status === "noReport") {
        progressReportContent += "No report on last ping. ";
      } else if (status === "unpaused") {
        progressReportContent += "Recently unpaused. ";
      }
      this.isPaused = false;
      indicatorButton.textContent = "Pause";
      this.clearTimeout();
      this.writeProgressReportUnfinishedComputation(parsed);
      this.currentTimeOutHandler = setTimeout(() => {
        this.progressReport();
      }, this.timeIncrement * 1000);
    }
    this.writeProgressReportOverView(progressReportContent);
  }

  writeProgressReportUnfinishedComputation(input) {
    const outputElement = this.ownerCalculator.getOutputElement();
    outputElement.textContent = "";
    let resultHTML = input[pathnames.urlFields.result.resultHtml];
    const computationLimits = input["computationLimits"];
    if (computationLimits !== undefined && computationLimits != null) {
      resultHTML += computationLimits;
    }
    const stackTrace = input["stackTrace"];
    if (stackTrace !== null && stackTrace !== undefined) {
      resultHTML += stackTrace;
    }
    if (resultHTML !== "" && resultHTML !== undefined && resultHTML !== null) {
      miscellaneous.writeHTML(outputElement, resultHTML);
    }
  }

  writeFinalResult(parsed) {
    this.ownerCalculator.parsedComputation = parsed;
    this.ownerCalculator.panels.length = 0;
    this.ownerCalculator.writeResultAndUpdateElement();
    this.ownerCalculator.afterWriteOutput();
  }

  writeProgressReportOverView(progressReportContent) {
    let finalContent = progressReportContent;
    finalContent += `Refreshing every ${this.timeIncrement} second(s). `;
    finalContent += `Client time: ~${Math.floor(this.timeOutOldCounter)} second(s)<br>`;
    let progressReportTimer = document.getElementById(
      ids.domElements.pages.calculator.monitoring.progressTimer,
    );
    miscellaneous.writeHTML(progressReportTimer, finalContent);
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