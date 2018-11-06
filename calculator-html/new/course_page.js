"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');

function callbackModifyDeadlines(incomingId, input, output) {
  document.getElementById(`deadlines${incomingId}`).innerHTML = input;
}

function modifyDeadlines(incomingId) {
  var theDates = document.getElementsByName(incomingId);
  var jsonToSubmit = {};
  var idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (var counterDates = 0; counterDates < theDates.length; counterDates ++) {
    jsonToSubmit[idDecoded].deadlines[thePage.user.sectionsTaught[counterDates]] = theDates[counterDates].value;
  }
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?request=setProblemData&`;
  theURL += `mainInput=${encodeURIComponent(JSON.stringify(jsonToSubmit))}`;
  submitRequests.submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    callback: callbackModifyDeadlines.bind(null, incomingId)
  });
}

function convertStringToLaTeXFileName(input) {
  var result = encodeURIComponent(input.split(" ").join("-")).split("%").join("-");
  if (result.length === 0) {
    return "undefined";
  }
  if (result[0] === "-") {
    result = "L" + result;
  }
  return result;
}



var problemWeightsVisible = false;

function initializeProblemWeightsAndDeadlines() {
  var theWeights = document.getElementsByClassName('panelProblemWeights');
  for (var i = 0; i < theWeights.length; i ++) { 
    //theScores[i].style.transition ='opacity 1s linear';
    theWeights[i].style.maxHeight = '0px';
  }
  var theDeadlines = document.getElementsByClassName('panelDeadlines');
  for (var i = 0; i < theDeadlines.length; i ++) { 
    //theScores[i].style.transition ='opacity 1s linear';
    theDeadlines[i].style.maxHeight = '0px';
  }
}

function toggleDeadline(deadlineId, panelId, button) { 
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

function writeEditCoursePagePanel() {
  var thePanel = "";
  var thePage = window.storage.mainPage;
  thePanel += getEditPanel(thePage.storage.currentCourse.courseHome.getValue());
  thePanel += getEditPanel(thePage.storage.currentCourse.topicList.getValue());
  if (
    thePage.theTopics.topicBundleFile !== undefined && 
    thePage.theTopics.topicBundleFile !== null &&
    thePage.theTopics.topicBundleFile !== ""
  ) {
    for (var counterTopicBundle = 0; counterTopicBundle < thePage.theTopics.topicBundleFile.length; counterTopicBundle ++) {
      thePanel += getEditPanel(thePage.theTopics.topicBundleFile[counterTopicBundle]);
    }
  }
  document.getElementById("divCurrentCourseEditPanel").innerHTML = thePanel;
}

function afterLoadCoursePage(incomingPage, result) {
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
  writeEditCoursePagePanel();
  if (theTopics.length  === 0) {
    return;
  }
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?request=${topicList}`,
    callback: afterLoadTopics,
    progress: "spanProgressReportGeneral"
  });
}

function selectCurrentCoursePage() {
  var topicRequest = "templateJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicRequest = "templateJSON";
  }
  var urlObject = { 
    currentPage: thePage.storage.currentPage.getValue()
  };
  location.href = `${pathnames.urls.app}#${encodeURIComponent(JSON.stringify(urlObject))}`;
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?request=${topicRequest}`,
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}

module.exports =  {
  selectCurrentCoursePage
}