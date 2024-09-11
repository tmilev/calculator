"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const problemPage = require("./problem_page");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;
const datePicker = require("./date_picker").datePicker;

class CoursePage {
  constructor() {
    this.mainPage = null;
    this.problemWeightsVisible = false;
    /** @type {string|null} */
    this.lastCourseHome = null;
    /** @type {string|null} */
    this.lastTopicList = null;    
  }
  
  initialize(mainPage) {
    this.mainPage = mainPage;
  }

  selectCurrentCourse() {
    let currentCourse = storage.variables.currentCourse;
    let incomingCourse = currentCourse.courseHome.getValue();
    let incomingTopicList = currentCourse.topicList.getValue();
    if (
      incomingCourse === null ||
      incomingCourse === "" ||
      incomingCourse === undefined
    ) {
      let courseBody = document.getElementById(
        ids.domElements.divCurrentCourseBody
      );
      let button = document.createElement("button");
      button.classList.add(
        "buttonSelectPage",
        "buttonSlowTransition",
        "buttonFlash"
      );
      button.style.width = "150px";
      button.textContent = "Please select course";
      button.addEventListener("click", () => {
        this.mainPage.selectPage('selectCourse');
      });
      setTimeout(() => {
        button.classList.remove("buttonFlash");
      }, 0);
      courseBody.textContent = "";
      courseBody.appendChild(button);
      return;
    }
    if (
      this.lastCourseHome === incomingCourse &&
      this.lastTopicList === incomingTopicList
    ) {
      return;
    }
    // The this.lastCourseHome variable may be reset on successful login. 
    this.lastCourseHome = incomingCourse;
    let topicRequest = "templateJSONNoLogin";
    if (globalUser.flagLoggedIn) {
      topicRequest = "templateJSON";
    }
    let url = "";
    const api = pathnames.urls.calculatorAPI;
    url += `${api}?${pathnames.urlFields.request}=${topicRequest}&`;
    url += `${pathnames.urlFields.problem.courseHome}=${incomingCourse}&`;
    url += `${pathnames.urlFields.problem.topicList}=${incomingTopicList}`;
    submitRequests.submitGET({
      url: url,
      callback: (incoming) => {
        this.afterLoadCoursePage(incoming, incomingTopicList);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  processLoadedTopicsWriteToCoursePage(incomingTopics) {
    const startTime = new Date().getTime();
    problemPage.allTopics.processLoadedTopics(incomingTopics);
    this.writeTopicsToCoursePage();
    const writeTime = new Date().getTime() - startTime;
    if (writeTime > 1000) {
      console.log(
        `Html generation took ${writeTime} ms, this is too slow.`
      );
    }
  }

  writeTopicsToCoursePage() {
    let topicsElement = this.getTopicListElement();
    topicsElement.textContent = "";
    let htmlContentElements = problemPage.allTopics.getHTMLfromTopics();
    problemPage.writeEditCoursePagePanel();
    let extraComments = miscellaneous.htmlElementsFromCommentsAndErrors(
      problemPage.allTopics.topics
    );
    miscellaneous.appendHtml(topicsElement, extraComments);
    miscellaneous.appendHtml(topicsElement, htmlContentElements);
    this.initializeProblemWeightsAndDeadlines();
    this.initializeDatePickers();
    if (problemPage.allTopics.flagLoaded) {
      problemPage.problemNavigation.writeToHTML();
    }
    typeset.typesetter.typesetSoft(topicsElement, "");
  }

  /** @return {HTMLElement|null} */
  getTopicListElement() {
    let topics = document.getElementsByTagName("topicList");
    if (topics.length === 0) {
      topics = document.getElementsByClassName("topicList");
    }
    if (topics.length === 0) {
      return null;
    }
    return topics[0];
  }

  toggleDeadline(
    /** @type {HTMLElement} */
    deadline,
    panelId,
    /** @type {HTMLElement} */
    button,
  ) {
    let problem = problemPage.allTopics.getTopicElementById(panelId);
    if (deadline.style.maxHeight === '200px') {
      deadline.style.opacity = '0';
      deadline.style.maxHeight = '0';
      button.textContent = "";
      button.appendChild(problem.toHTMLDeadlineElement());
      let arrowLeft = document.createTextNode(" \u25C2");
      button.appendChild(arrowLeft);
    } else {
      deadline.style.opacity = '1';
      deadline.style.maxHeight = '200px';
      button.textContent = "";
      button.appendChild(problem.toHTMLDeadlineElement())
      button.appendChild(document.createTextNode(" \u25BE"));
    }
  }

  toggleProblemWeights() {
    let weights = document.getElementsByClassName('panelProblemWeights');
    let buttons = document.getElementsByClassName('accordionLikeProblemWeight');
    for (let i = 0; i < weights.length; i++) {
      if (!this.problemWeightsVisible) {
        weights[i].style.opacity = '1';
        weights[i].style.maxHeight = '200px';
      } else {
        weights[i].style.opacity = '0';
        weights[i].style.maxHeight = '0';
      }
    }
    for (let i = 0; i < buttons.length; i++) {
      let currentProblem = problemPage.allTopics.getTopicElementById(
        buttons[i].name
      );
      if (!this.problemWeightsVisible) {
        miscellaneous.writeHTML(
          buttons[i],
          `${currentProblem.toStringProblemWeight()} &#9660;`
        );
      } else {
        miscellaneous.writeHTML(
          buttons[i],
          `${currentProblem.toStringProblemWeight()} &#9666;`
        );
      }
    }
    this.problemWeightsVisible = !this.problemWeightsVisible;
  }
  
  modifyDeadlines(
    /** @type {string} */
    incomingId,
    /** @type {HTMLElement} */
    deadlinesPanel,
    /** @type {HTMLElement} */
    finalReport,
  ) {
    let dates = deadlinesPanel.getElementsByClassName("datePicker");
    let jsonToSubmit = {};
    jsonToSubmit[incomingId] = {
      deadlines: {}
    };
    for (let i = 0; i < dates.length; i++) {
      let currentSection = globalUser.sectionsTaught[i];
      jsonToSubmit[incomingId].deadlines[currentSection] = dates[i].value;
    }
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?`;
    const setProblemDeadline = pathnames.urlFields.requests.setProblemDeadline;
    url += `${pathnames.urlFields.request}=${setProblemDeadline}&`;
    const mainInput = encodeURIComponent(JSON.stringify(jsonToSubmit));
    url += `${pathnames.urlFields.mainInput}=${mainInput}&`;
    submitRequests.submitGET({
      url: url,
      progress: ids.domElements.spanProgressReportGeneral,
      result: finalReport,
    });
  } 

  loadTopicList(desiredTopicList) {
    let topicListRequest = "topicListJSONNoLogin";
    if (globalUser.isLoggedIn()) {
      topicListRequest = "topicListJSON";
    }
    let courseHome = storage.variables.currentCourse.courseHome.getValue();
    let url = "";
    const api = pathnames.urls.calculatorAPI;
    url += `${api}?${pathnames.urlFields.request}=${topicListRequest}&`;
    url += `${pathnames.urlFields.problem.topicList}=${desiredTopicList}&`;
    url += `${pathnames.urlFields.problem.courseHome}=${courseHome}&`;
    submitRequests.submitGET({
      url: url,
      callback: (incoming) => {
        this.processLoadedTopicsWriteToCoursePage(incoming);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  afterLoadCoursePage(incomingData, desiredTopicList) {
    let courseBody = document.getElementById(
      ids.domElements.divCurrentCourseBody
    );
    let coursePageDiv = document.getElementById(
      ids.domElements.divCurrentCourse
    );
    miscellaneous.writeHTML(
      courseBody,
      miscellaneous.jsonParseGetHtmlStandard(incomingData),
    );
    let titleElements = courseBody.getElementsByTagName('title');
    if (titleElements !== null && titleElements !== undefined) {
      if (titleElements.length > 0) {
        document.getElementsByTagName('title')[0].text = titleElements[0].text;
      }
    }
    typeset.typesetter.typesetSoft(coursePageDiv, "");
    problemPage.writeEditCoursePagePanel();
    const topicListElement = this.getTopicListElement(); 
    if (topicListElement === null) {
      return;
    }
    if (globalUser.loginSequenceInProgress) {
      miscellaneous.writeHTML(
        topicListElement,
        "<b style='color:purple'>Waiting for login ...</b>"
      );
      return;
    }
    miscellaneous.writeHTML(
      topicListElement,
      "<b style='color:orange'>Loading topic list ...</b>"
    );
    this.loadTopicList(desiredTopicList);
  }

   initializeDatePickers() {
    let pickers = document.getElementsByClassName("datePicker");
    for (let i = 0; i < pickers.length; i++) {
      datePicker.createDatePicker(pickers[i]);
    }
  }
 
  initializeProblemWeightsAndDeadlines() {
    let weights = document.getElementsByClassName('panelProblemWeights');
    for (let i = 0; i < weights.length; i++) {
      weights[i].style.maxHeight = '0px';
    }
    let deadlines = document.getElementsByClassName('panelDeadlines');
    for (let i = 0; i < deadlines.length; i++) {
      deadlines[i].style.maxHeight = '0px';
    }
  }
}

const coursePage = new CoursePage();

module.exports = {
  coursePage,
};