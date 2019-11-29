"use strict";
const submitRequests = require("./submit_requests");
const problemPage = require("./problem_page");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const editPage = require("./edit_page");

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
  this.editPageElement = null;
  /**@type{HTMLElement} */
  this.progressReportGeneral = null;
  /**@type{HTMLElement} */
  this.textAreaEditPage = null;
  /**@type{HTMLElement} */
  this.editorElement = null;
  /**@type{HTMLElement} */
  this.saveEdit = null;
  /**@type{HTMLElement} */
  this.spanErrorsEditPage = null;
  /**@type{string} */
  this.courseHome = "";
  /**@type{string} */
  this.topicList = "";
  /**@type{string} */
  this.problemFileName = "";
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

StandAloneProblem.prototype.ensureSpansExist = function(input) {
  return this.ensureTagsExist(input, "span");
}

StandAloneProblem.prototype.ensureDivsExist = function(input) {
  return this.ensureTagsExist(input, "div");
}

StandAloneProblem.prototype.ensureButtonsExist = function(input) {
  return this.ensureTagsExist(input, "button");
}

StandAloneProblem.prototype.ensureTextAreasExist = function(input) {
  return this.ensureTagsExist(input, "textarea");
}

StandAloneProblem.prototype.ensureTagsExist = function(input, tag) {
  for (var label in input) {
    var desiredId = input[label];
    this[label] = document.getElementById(desiredId);
    if (this[label] === null) {
      var newElement = document.createElement(tag); 
      newElement.setAttribute("id", desiredId);
      this[label] = newElement;
      this.containerElement.appendChild(newElement);
    }
  }
}

StandAloneProblem.prototype.initCommon = function(input) {
  console.log("Initializing stand-alone calculator runtime.");
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
  this.courseHome      = "/coursetemplates/Curriculum-300-Calculus-I.html";
  this.topicList       = "/topiclists/Curriculum-300-Calculus-I.txt";
  this.problemFileName = "/problems/Find-function-inverse-fractional-linear-1.html";
}

StandAloneProblem.prototype.initAndEdit = function (input) {
  this.initCommon(input);
  this.ensureButtonsExist({
    saveEdit: ids.domElements.buttonSaveEdit,
  });
  this.ensureTextAreasExist({
    textAreaEditPage: ids.domElements.textAreaTopicListEntry,
  });
  this.ensureSpansExist({
    spanErrorsEditPage: ids.domElements.spanErrorsEditPage,
  });
  this.ensureDivsExist({
    editorElement: ids.domElements.divEditorAce,
  });
  this.ensureSpansExist({
    progressReportGeneral: ids.domElements.spanProgressReportGeneral,
  });
  editPage.selectEditPage(this.problemFileName);
}

StandAloneProblem.prototype.initAndRun = function (input) {
  this.initCommon(input);
  this.ensureDivsExist({
    navigationElement: ids.domElements.divProblemInfoBar,
    problemElement: ids.domElements.problemPageContentContainer,
    editPageElement: ids.domElements.pages.editPage.div,
  });
  this.ensureSpansExist({
    progressReportGeneral: ids.domElements.spanProgressReportGeneral,
  });
  this.problem = problemPage.allProblems.CreateOrUpdateProblem({
    id: this.problemFileName,
  }); 
  var theURL = `${pathnames.urls.calculatorAPI}?${this.problem.getCalculatorURLRequestInput(false, this.problemFileName, this.courseHome, this.topicList)}`;
  submitRequests.submitGET({
    url: theURL,
    callback: this.updateProblemCallback.bind(this),
  });  
}

var standAloneProblem = new StandAloneProblem();

module.exports = {
  standAloneProblem,
};