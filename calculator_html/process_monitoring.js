"use strict";
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;

class ProgressReport {
  constructor() {
    this.content = "";
    this.element = document.createElement("div");
    this.element.style.transition = "1s all";
  }

  setProgressContent(input) {
    const changed = (this.content !== input);
    this.content = input;
    if (!changed) {
      return;
    }
    miscellaneous.writeHTML(this.element, this.content);
    this.element.style.backgroundColor = "palegreen";
    setTimeout(() => {
      this.element.style.backgroundColor = "";
    }, 0);
  }
}

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
    /** @type{ProgressReport[]} */
    this.allProgressReports = [];
    this.progressReportElement = document.getElementById(
      ids.domElements.pages.calculator.monitoring.progressOutput
    );
  }

  pauseButton() {
    return document.getElementById(
      ids.domElements.pages.calculator.monitoring.buttonPauseToggle
    );
  }

  start(
    /** @type {String} */
    workerId,
  ) {
    storage.variables.calculator.workerId.setAndStore(
      workerId, true, false
    );
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
    const api = pathnames.urls.calculatorAPI;
    const urlFields = pathnames.urlFields;
    sURL += `${api}?${urlFields.request}=${urlFields.requests.indicator}`;
    sURL += `&${urlFields.requests.workerId}=${this.currentWorkerId}`;
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
    this.ownerCalculator.getOutputElement().textContent = "";
    const progressStrings = input[pathnames.urlFields.result.progressReports];
    if (progressStrings === undefined) {
      progressStrings = [];
    }
    for (
      let i = this.allProgressReports.length;
      i < progressStrings.length;
      i++
    ) {
      this.allProgressReports.push(new ProgressReport());
    }
    this.progressReportElement.textContent = "";
    const computationLimits = input["computationLimits"];
    if (computationLimits !== undefined && computationLimits !== null) {
      const computationLimitElement = document.createElement("div");
      miscellaneous.writeHTML(computationLimitElement, computationLimits);
      this.progressReportElement.appendChild(computationLimitElement);
    }
    for (let i = 0; i < progressStrings.length; i++) {
      const progress = this.allProgressReports[i];
      progress.setProgressContent(progressStrings[i]);
      this.progressReportElement.appendChild(
        progress.element
      );
    }
    this.allProgressReports.length = progressStrings.length;
    const stackTrace = input["stackTrace"];
    if (stackTrace !== null && stackTrace !== undefined) {
      const stackTraceElement = document.createElement("div");
      miscellaneous.writeHTML(stackTraceElement, stackTrace);
      this.progressReportElement.appendChild(stackTraceElement);
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
    miscellaneous.writeHTML(progressReportTimer, finalContent); pathnames
  }

  togglePause() {
    if (this.isFinished) {
      return;
    }
    let pauseURL = "";
    pauseURL += `${pathnames.urls.calculatorAPI}?`;
    const urlFields = pathnames.urlFields;
    if (!this.isPaused) {
      pauseURL += `${urlFields.request}=${urlFields.requests.pause}&`;
    } else {
      pauseURL += `${urlFields.request}=${urlFields.requests.unpause}&`;
    }
    pauseURL += `${urlFields.requests.workerId}=${this.currentWorkerId}&`;
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