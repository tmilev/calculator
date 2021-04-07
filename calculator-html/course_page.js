"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const problemPage = require("./problem_page");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");

function modifyDeadlines(incomingId) {
  let thePage = window.calculator.mainPage;
  let nameDatePicker = `datePicker${incomingId}`;
  let theDates = document.getElementsByName(nameDatePicker);
  let jsonToSubmit = {};
  let idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (let counterDates = 0; counterDates < theDates.length; counterDates++) {
    let currentSection = thePage.user.sectionsTaught[counterDates];
    jsonToSubmit[idDecoded].deadlines[currentSection] = theDates[counterDates].value;
  }
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemDeadline}&`;
  theURL += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(jsonToSubmit))}`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    result: `deadlines${incomingId}`,
  });
}

let problemWeightsVisible = false;

function toggleDeadline(deadlineId, panelId, button) {
  let thePanel = document.getElementById(deadlineId);
  let theProblem = problemPage.allProblems.getProblemById(panelId);
  if (thePanel.style.maxHeight === '200px') {
    thePanel.style.opacity = '0';
    thePanel.style.maxHeight = '0';
    button.innerHTML = `${theProblem.toStringDeadline()} &#9666;`;
  } else {
    thePanel.style.opacity = '1';
    thePanel.style.maxHeight = '200px';
    button.innerHTML = `${theProblem.toStringDeadline()} &#9660;`;
  }
}

function toggleProblemWeights() {
  let theWeights = document.getElementsByClassName('panelProblemWeights');
  let theButtons = document.getElementsByClassName('accordionLikeProblemWeight');
  for (let i = 0; i < theWeights.length; i++) {
    if (!problemWeightsVisible) {
      theWeights[i].style.opacity = '1';
      theWeights[i].style.maxHeight = '200px';
    } else {
      theWeights[i].style.opacity = '0';
      theWeights[i].style.maxHeight = '0';
    }
  }
  for (let i = 0; i < theButtons.length; i++) {
    let currentProblem = problemPage.allProblems.getProblemById(theButtons[i].name);
    if (!problemWeightsVisible) {
      theButtons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9660;`;
    } else {
      theButtons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9666;`;
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
  let theTopics = document.getElementsByTagName("topicList");
  problemPage.writeEditCoursePagePanel();
  if (theTopics.length === 0) {
    return;
  }
  loadTopicList(problemPage.processLoadedTopicsWriteToCoursePage);
}

function loadTopicList(callback) {
  let thePage = window.calculator.mainPage;
  let topicListRequest = "topicListJSONNoLogin";
  if (thePage.isLoggedIn()) {
    topicListRequest = "topicListJSON";
  }
  let topicName = thePage.storage.variables.currentCourse.topicList.getValue();
  let courseHome = thePage.storage.variables.currentCourse.courseHome.getValue();
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicListRequest}&`;
  theURL += `${pathnames.urlFields.problem.topicList}=${topicName}&`;
  theURL += `${pathnames.urlFields.problem.courseHome}=${courseHome}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: callback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

let lastLoadedCourse = {
  courseHome: null,
  topicList: null,
};

function selectCurrentCoursePage() {
  let thePage = window.calculator.mainPage;
  let storageVariables = thePage.storage.variables;
  let incomingCourse = storageVariables.currentCourse.courseHome.getValue();
  let incomingTopicList = storageVariables.currentCourse.topicList.getValue();
  if (
    incomingCourse === null ||
    incomingCourse === "" ||
    incomingCourse === undefined
  ) {
    let courseBody = document.getElementById(ids.domElements.divCurrentCourseBody);
    let temporarySelectCourseId = "buttonTemporarySelectCourse";
    let courseBodyHTML = `<button id = '${temporarySelectCourseId}' `;
    courseBodyHTML += `class = "buttonSelectPage buttonSlowTransition buttonFlash" style = "width:150px" `;
    courseBodyHTML += `onclick = "window.calculator.mainPage.selectPage('selectCourse')">Please select course</button>`;
    courseBody.innerHTML = courseBodyHTML;
    setTimeout(() => {
      document.getElementById(temporarySelectCourseId).classList.remove("buttonFlash");
    }, 0);
    return;
  }
  if (lastLoadedCourse.courseHome === incomingCourse && lastLoadedCourse.topicList === incomingTopicList) {
    return;
  }
  // The lastLoadedCourse variable may be reset on successful login. 
  lastLoadedCourse.courseHome = incomingCourse;
  lastLoadedCourse.topicList = incomingTopicList;
  let topicRequest = "templateJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicRequest = "templateJSON";
  }
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicRequest}&`;
  theURL += `${pathnames.urlFields.problem.courseHome}=${incomingCourse}&`;
  theURL += `${pathnames.urlFields.problem.topicList}=${incomingTopicList}`;
  submitRequests.submitGET({
    url: theURL,
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