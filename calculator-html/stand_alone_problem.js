"use strict";
const submitRequests = require("./submit_requests");
const problemPage = require("./problem_page");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");

function StandAloneProblem() {
  /**@type{problemPage.Problem} */
  this.problem = null;
  /**@type{string} */
  this.containerId = "";
  /**@type{HTMLElement} */ 
  this.containerElement = null;
  /**@type{HTMLElement} */ 
  this.problemElement = null;
  /**@type{HTMLElement} */ 
  this.navigationElement = null;
  /**@type{HTMLElement} */
  this.progressReportGeneral = null;
}

StandAloneProblem.prototype.updateProblemCallback = function(input) {
  var theProblem = null;
  try {
    theProblem = JSON.parse(input);
  } catch (e) {
    this.problemElement.innerHTML = `Error parsing: ${e}. Failed to parse: ${input}`;
    return;
  }
  if (theProblem.crashReport !== undefined && theProblem.crashReport !== null) {
    this.problemElement.innerHTML = theProblem.crashReport;
    return;
  }
  this.problem.initializeProblemContent(theProblem);
}

StandAloneProblem.prototype.ensureDivsExist = function(input) {
  for (var label in input) {
    var desiredId = input[label];
    this[label] = document.getElementById(desiredId);
    if (this[label] === null) {
      var newElement = document.createElement("DIV"); 
      newElement.setAttribute("id", desiredId);
      this[label] = newElement;
      this.containerElement.appendChild(newElement);
    }
  }
}

StandAloneProblem.prototype.initAndRun = function (input) {
  if (input.id === null || input.id === undefined) {
    console.log("No output component id supplied.");
    return;
  }
  window.calculator.mainPage.flagProblemPageOnly = true;
  if (input.hardCodedServerAddress !== "" && input.hardCodedServerAddress !== null && input.hardCodedServerAddress !== undefined) {
    window.calculator.hardCodedServerAddress = input.hardCodedServerAddress;
  }
  this.containerId = input.id;
  this.containerElement = document.getElementById(this.containerId); 
  if (this.containerElement === null) {
    console.log(`Failed to find the supplied id. ${this.containerId}`);
    return;
  }
  this.ensureDivsExist({
    navigationElement: ids.domElements.divProblemInfoBar,
    problemElement: ids.domElements.problemPageContentContainer,
    progressReportGeneral: ids.domElements.spanProgressReportGeneral,
  });
  var courseHome = "/coursetemplates/Curriculum-300-Calculus-I.html";
  var topicList = "/topiclists/Curriculum-300-Calculus-I.txt";
  var problemFileName = "/problems/Find-function-inverse-fractional-linear-1.html";
  this.problem = problemPage.allProblems.CreateOrUpdateProblem({
    id: problemFileName,
  }); 
  var theURL = `${pathnames.urls.calculatorAPI}?${this.problem.getCalculatorURLRequestInput(false, problemFileName, courseHome, topicList)}`;
  submitRequests.submitGET({
    url: theURL,
    callback: this.updateProblemCallback.bind(this),
  });  
}

var standAloneProblem = new StandAloneProblem();

module.exports = {
  standAloneProblem,
};