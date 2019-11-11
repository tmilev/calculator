"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements');
const editPage = require('./edit_page');
const problemPage = require('./problem_page');
const mathjax = require('./mathjax-calculator-setup');

function callbackModifyDeadlines(incomingId, input, output) {
  document.getElementById(`deadlines${incomingId}`).innerHTML = input;
}

function modifyDeadlines(incomingId) {
  var thePage = window.calculator.mainPage;
  var nameDatePicker = `datePicker${incomingId}`;
  var theDates = document.getElementsByName(nameDatePicker);
  var jsonToSubmit = {};
  var idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (var counterDates = 0; counterDates < theDates.length; counterDates ++) {
    var currentSection = thePage.user.sectionsTaught[counterDates];
    jsonToSubmit[idDecoded].deadlines[currentSection] = theDates[counterDates].value;
  }
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemData}&`;
  theURL += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(jsonToSubmit))}`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: callbackModifyDeadlines.bind(null, incomingId)
  });
}

var problemWeightsVisible = false;

function toggleDeadline(deadlineId, panelId, button) {
  var thePanel = document.getElementById(deadlineId);
  var theProblem = problemPage.allProblems.getProblemById(panelId);
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
  var theWeights = document.getElementsByClassName('panelProblemWeights');
  var theButtons = document.getElementsByClassName('accordionLikeProblemWeight');
  for (var i = 0; i < theWeights.length; i ++) { 
    if (!problemWeightsVisible) { 
      theWeights[i].style.opacity = '1';
      theWeights[i].style.maxHeight = '200px';
    } else { 
      theWeights[i].style.opacity = '0';
      theWeights[i].style.maxHeight = '0';
    }
  }
  for (var i = 0; i < theButtons.length; i ++) {
    var currentProblem = problemPage.allProblems.getProblemById(theButtons[i].name);
    if (!problemWeightsVisible) { 
      theButtons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9660;`;
    } else {
      theButtons[i].innerHTML = `${currentProblem.toStringProblemWeight()} &#9666;`;
    }
  }
  problemWeightsVisible = !problemWeightsVisible;
}

function afterLoadCoursePage(incomingPage, result) {
  var thePage = window.calculator.mainPage;
  var courseBody = document.getElementById(ids.domElements.divCurrentCourseBody); 
  var coursePage = document.getElementById(ids.domElements.divCurrentCourse);
  courseBody.innerHTML = incomingPage;
  var titleElements = courseBody.getElementsByTagName('title');
  if (titleElements !== null && titleElements !== undefined) {
    if (titleElements.length > 0) {
      document.getElementsByTagName('title')[0].text = titleElements[0].text;
    }
  }
  //mathjax.typeSetHard(coursePage);
  mathjax.typeSetSoft(coursePage);
  var theTopics = document.getElementsByTagName("topicList");
  var topicList = "topicListJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicList = "topicListJSON";
  }
  problemPage.writeEditCoursePagePanel();
  if (theTopics.length  === 0) {
    return;
  }
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicList}`,
    callback: problemPage.afterLoadTopics,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

var lastLoadedCourse = {
  courseHome: null,
  topicList: null,
};

function selectCurrentCoursePage() {
  var thePage = window.calculator.mainPage;
  var storageVariables = thePage.storage.variables;
  var incomingCourse = storageVariables.currentCourse.courseHome.getValue();
  var incomingTopicList = storageVariables.currentCourse.topicList.getValue();
  if (incomingCourse === null || incomingCourse === "" || incomingCourse === undefined) {
    var courseBody = document.getElementById(ids.domElements.divCurrentCourseBody); 
    var temporarySelectCourseId = "buttonTemporarySelectCourse";
    var courseBodyHTML = `<button id = '${temporarySelectCourseId}' `;
    courseBodyHTML += `class = "buttonSelectPage buttonSlowTransition buttonFlash" style = "width:150px" `;
    courseBodyHTML += `onclick = "window.calculator.mainPage.selectPage('selectCourse')">Please select course</button>`;
    courseBody.innerHTML = courseBodyHTML;
    setTimeout(() => {
      document.getElementById(temporarySelectCourseId).classList.remove("buttonFlash");
    }, 100);
    return;
  }
  if (lastLoadedCourse.courseHome === incomingCourse && lastLoadedCourse.topicList === incomingTopicList) {
    return;
  }
  lastLoadedCourse.courseHome = incomingCourse;
  lastLoadedCourse.topicList = incomingTopicList;
  var topicRequest = "templateJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicRequest = "templateJSON";
  }
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${topicRequest}`,
    callback: afterLoadCoursePage,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports =  {
  lastLoadedCourse,
  modifyDeadlines,
  selectCurrentCoursePage,
  toggleDeadline,
  toggleProblemWeights,
};