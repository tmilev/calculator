"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements');
const editPage = require('./edit_page');
const problemPage = require('./problem_page');

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
  var thePage = window.calculator.mainPage;
  var thePanel = document.getElementById(deadlineId);
  var theProblem = thePage.problems[panelId];
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
  var thePage = window.calculator.mainPage;
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
    var currentProblem = thePage.problems[theButtons[i].name];
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
  var theCourseDiv = document.getElementById(ids.domElements.divCurrentCourseBody); 
  theCourseDiv.innerHTML = incomingPage;
  var titleElements = theCourseDiv.getElementsByTagName('title');
  if (titleElements !== null && titleElements !== undefined) {
    if (titleElements.length > 0) {
      document.getElementsByTagName('title')[0].text = titleElements[0].text;
    }
  }
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentCourse")]);
  //MathJax.Hub.Process();
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
    url: `${pathnames.urls.calculatorAPI}?request=${topicList}`,
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
  modifyDeadlines,
  selectCurrentCoursePage,
  toggleDeadline,
  toggleProblemWeights,
}