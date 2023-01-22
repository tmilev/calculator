"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const problemPage = require("./problem_page");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");

function modifyDeadlines(incomingId) {
  let page = window.calculator.mainPage;
  let nameDatePicker = `datePicker${incomingId}`;
  let theDates = document.getElementsByName(nameDatePicker);
  let jsonToSubmit = {};
  let idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (let counterDates = 0; counterDates < theDates.length; counterDates++) {
    let currentSection = page.user.sectionsTaught[counterDates];
    jsonToSubmit[idDecoded].deadlines[currentSection] = theDates[counterDates].value;
  }
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  url += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemDeadline}&`;
  url += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(jsonToSubmit))}`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    result: `deadlines${incomingId}`,
  });
}

let problemWeightsVisible = false;

function toggleDeadline(
  /** @type {HTMLElement} */
  deadline,
  panelId,
  /** @type {HTMLElement} */
  button,
) {
  let problem = problemPage.allProblems.getProblemById(panelId);
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

function toggleProblemWeights() {
  let weights = document.getElementsByClassName('panelProblemWeights');
  let buttons = document.getElementsByClassName('accordionLikeProblemWeight');
  for (let i = 0; i < weights.length; i++) {
    if (!problemWeightsVisible) {
      weights[i].style.opacity = '1';
      weights[i].style.maxHeight = '200px';
    } else {
      weights[i].style.opacity = '0';
      weights[i].style.maxHeight = '0';
    }
  }
  for (let i = 0; i < buttons.length; i++) {
    let currentProblem = problemPage.allProblems.getProblemById(buttons[i].name);
    if (!problemWeightsVisible) {
      buttons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9660;`;
    } else {
      buttons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9666;`;
    }
  }
  problemWeightsVisible = !problemWeightsVisible;
}

function afterLoadCoursePage(incoming, result) {
  let courseBody = document.getElementById(ids.domElements.divCurrentCourseBody);
  let coursePage = document.getElementById(ids.domElements.divCurrentCourse);
  courseBody.innerHTML = miscellaneous.jsonParseGetHtmlStandard(incoming);
  let titleElements = courseBody.getElementsByTagName('title');
  if (titleElements !== null && titleElements !== undefined) {
    if (titleElements.length > 0) {
      document.getElementsByTagName('title')[0].text = titleElements[0].text;
    }
  }
  typeset.typesetter.typesetSoft(coursePage, "");
  let topics = document.getElementsByTagName("topicList");
  problemPage.writeEditCoursePagePanel();
  if (topics.length === 0) {
    return;
  }
  loadTopicList(problemPage.processLoadedTopicsWriteToCoursePage);
}

function loadTopicList(callback) {
  let page = window.calculator.mainPage;
  let topicListRequest = "topicListJSONNoLogin";
  if (page.isLoggedIn()) {
    topicListRequest = "topicListJSON";
  }
  let topicName = page.storage.variables.currentCourse.topicList.getValue();
  let courseHome = page.storage.variables.currentCourse.courseHome.getValue();
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicListRequest}&`;
  url += `${pathnames.urlFields.problem.topicList}=${topicName}&`;
  url += `${pathnames.urlFields.problem.courseHome}=${courseHome}&`;
  submitRequests.submitGET({
    url: url,
    callback: callback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

let lastLoadedCourse = {
  courseHome: null,
  topicList: null,
};

function selectCurrentCoursePage() {
  let page = window.calculator.mainPage;
  let storageVariables = page.storage.variables;
  let incomingCourse = storageVariables.currentCourse.courseHome.getValue();
  let incomingTopicList = storageVariables.currentCourse.topicList.getValue();
  if (
    incomingCourse === null ||
    incomingCourse === "" ||
    incomingCourse === undefined
  ) {
    let courseBody = document.getElementById(ids.domElements.divCurrentCourseBody);
    let button = document.createElement("button");
    button.classList.add("buttonSelectPage", "buttonSlowTransition", "buttonFlash");
    button.style.width = "150px";
    button.textContent = "Please select course";
    button.addEventListener("click", () => { 
      page.selectPage('selectCourse');
    });
    setTimeout(() => {
      button.classList.remove("buttonFlash");
    }, 0);
    courseBody.textContent = "";
    courseBody.appendChild(button);
    return;
  }
  if (lastLoadedCourse.courseHome === incomingCourse && lastLoadedCourse.topicList === incomingTopicList) {
    return;
  }
  // The lastLoadedCourse variable may be reset on successful login. 
  lastLoadedCourse.courseHome = incomingCourse;
  lastLoadedCourse.topicList = incomingTopicList;
  let topicRequest = "templateJSONNoLogin";
  if (page.user.flagLoggedIn) {
    topicRequest = "templateJSON";
  }
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicRequest}&`;
  url += `${pathnames.urlFields.problem.courseHome}=${incomingCourse}&`;
  url += `${pathnames.urlFields.problem.topicList}=${incomingTopicList}`;
  submitRequests.submitGET({
    url: url,
    callback: afterLoadCoursePage,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports = {
  loadTopicList,
  lastLoadedCourse,
  modifyDeadlines,
  selectCurrentCoursePage,
  toggleDeadline,
  toggleProblemWeights,
};