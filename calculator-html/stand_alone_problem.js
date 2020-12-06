"use strict";
const submitRequests = require("./submit_requests");
const problemPage = require("./problem_page");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const editPage = require("./edit_page");
const coursePage = require("./course_page");

class StandAloneProblem {
  constructor() {
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
    /**@type{HTMLElement} */
    this.spanButtonNextEdit = null;
    /**@type{HTMLElement} */
    this.spanButtonPreviousEdit = null;
    /**@type{HTMLElement} */
    this.lineBreakBeforeProblem = null;
    /**@type{string} */
    this.courseHome = "";
    /**@type{string} */
    this.topicList = "";
    /**@type{string} */
    this.problemFileName = "";
  }

  updateProblemCallback(input) {
    var theProblem = null;
    try {
      theProblem = JSON.parse(input);
    } catch (e) {
      this.problemElement.innerHTML = `Error parsing: ${e}.Failed to parse: ${input} `;
      return;
    }
    if (theProblem.crashReport !== undefined && theProblem.crashReport !== null) {
      this.problemElement.innerHTML = theProblem.crashReport;
      return;
    }
    this.problem.initializeProblemContent(theProblem);
  }

  ensureSpansExist(input) {
    return this.ensureTagsExist(input, "span");
  }

  ensureDivsExist(input) {
    return this.ensureTagsExist(input, "div");
  }

  ensureButtonsExist(input) {
    return this.ensureTagsExist(input, "button");
  }

  ensureTextAreasExist(input) {
    return this.ensureTagsExist(input, "textarea");
  }

  ensureTagsExist(input, tag) {
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

  initCommon(input) {
    console.log("Initializing stand-alone calculator runtime.");
    if (input.id === null || input.id === undefined) {
      console.log("No output component id supplied.");
      return;
    }
    var thePage = window.calculator.mainPage;
    thePage.flagProblemPageOnly = true;
    if (
      input.hardCodedServerAddress !== "" &&
      input.hardCodedServerAddress !== null &&
      input.hardCodedServerAddress !== undefined
    ) {
      window.calculator.hardCodedServerAddress = input.hardCodedServerAddress;
    }
    this.containerId = input.id;
    this.containerElement = document.getElementById(this.containerId);
    if (this.containerElement === null) {
      console.log(`Failed to find the supplied id. ${this.containerId} `);
      return;
    }
    // tag defaults
    this.courseHome = "/coursetemplates/Curriculum-300-Calculus-I.html";
    this.topicList = "topiclists/classrooms_demo.txt";
    this.topicBundles = "topiclists/topicBundles.txt";
    this.problemFileName = "problems/Find-function-inverse-fractional-linear-1.html";
    var fileNameCookies = thePage.storage.variables.currentCourse.problemFileName.loadMe();
    if (
      fileNameCookies !== "" &&
      fileNameCookies !== undefined &&
      fileNameCookies !== null
    ) {
      this.problemFileName = fileNameCookies;
    }
    // overrides from input
    var tags = [
      "courseHome",
      "topicList",
      "topicBundles",
      "problemFileName",
    ];
    for (var i = 0; i < tags.length; i++) {
      var label = tags[i];
      if (
        input[label] !== undefined &&
        input[label] !== "" &&
        input[label] !== null
      ) {
        this[label] = input[label];
      }
    }
  }

  initAndEdit(input) {
    this.initCommon(input);
    this.ensureSpansExist({
      spanButtonPreviousEdit: ids.domElements.spanButtonPreviousEdit,
    });
    this.ensureButtonsExist({
      saveEdit: ids.domElements.buttonSaveEdit,
    });
    this.ensureSpansExist({
      spanButtonNextEdit: ids.domElements.spanButtonNextEdit,
    });
    this.ensureTextAreasExist({
      textAreaEditPage: ids.domElements.textAreaTopicListEntry,
    });
    this.ensureSpansExist({
      spanButtonNextEdit: ids.domElements.spanSubmitEditPageReport,
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
    var thePage = window.calculator.mainPage;

    thePage.storage.variables.editor.currentlyEditedPage.setAndStore(encodeURIComponent(this.problemFileName));
    thePage.storage.variables.currentCourse.topicList.setAndStore(this.topicList);
    coursePage.loadTopicList(problemPage.processLoadedTopicsWriteToEditPage);
  }

  initAndRun(input) {
    this.initCommon(input);
    this.ensureDivsExist({
      navigationElement: ids.domElements.divProblemInfoBar,
      problemElement: ids.domElements.problemPageContentContainer,
    });
    this.ensureSpansExist({
      progressReportGeneral: ids.domElements.spanProgressReportGeneral,
    });
    this.problem = problemPage.allProblems.CreateOrUpdateProblem({
      id: encodeURIComponent(this.problemFileName),
    });
    var theURL = `${pathnames.urls.calculatorAPI}?${this.problem.getCalculatorURLRequestInput(false, this.problemFileName, this.courseHome, this.topicList)}`;
    submitRequests.submitGET({
      url: theURL,
      callback: this.updateProblemCallback.bind(this),
    });
  }

}


var standAloneProblem = new StandAloneProblem();

module.exports = {
  standAloneProblem,
};