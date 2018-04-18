"use strict";

function selectCurrentProblem(problem, exerciseType){
  thePage.currentCourse.fileName = problem;
  thePage.currentCourse.request = exerciseType;
  thePage.storeSettingsToCookies();
  thePage.flagCurrentProblemLoaded = false;
  thePage.selectPage("currentProblem");
}

function updateProblemPageCallback(input, outputComponent){
  var theProblem = JSON.parse(input);
  var decodedProblem = decodeURIComponent(theProblem["problem"]);
  var scripts = decodeURIComponent(theProblem["scripts"]);
  document.getElementById("divCurrentProblemContentContainer").innerHTML = decodedProblem + scripts;
  thePage.currentProblem = {};
  thePage.currentProblem.answers = theProblem["answers"];
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentProblemContentContainer")]);
  
}

function updateProblemPage(){
  if (thePage.flagCurrentProblemLoaded) {
    return;
  }
  thePage.flagCurrentProblemLoaded = true;
  var theURL = `${pathnames.calculator}?request=${thePage.currentCourse.request}&`;
  theURL += `fileName=${thePage.currentCourse.fileName}&topicList=${thePage.currentCourse.topicList}&`;
  theURL += `courseHome=${thePage.currentCourse.courseHome}&`;
  submitGET({
    url: theURL,
    callback: updateProblemPageCallback
  });
}