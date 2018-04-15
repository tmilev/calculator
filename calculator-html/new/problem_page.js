"use strict";

function selectCurrentProblem(problem, exerciseType){
  thePage.currentCourse.fileName = problem;
  thePage.currentCourse.request = exerciseType;
  thePage.storeSettingsToCookies();
  thePage.flagCurrentProblemLoaded = false;
  thePage.selectPage("currentProblem");
}

function updateProblemPageCallback(input, outputComponent){
  document.getElementById("divCurrentProblemContentContainer").innerHTML = input;
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