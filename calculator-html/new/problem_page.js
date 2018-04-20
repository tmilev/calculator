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
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentProblemContentContainer")]);
  thePage.currentProblem[thePage.currentCourse.fileName] = {};
  thePage.currentProblem[thePage.currentCourse.fileName].answers = [];
  var answerVectors = theProblem["answers"];
  for (var counterAnswer = 0;  counterAnswer < answerVectors.length; counterAnswer ++){
    thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswer] = new InputPanelData({
      fileName: thePage.currentCourse.fileName,
      idMQSpan: answerVectors.answerMQspanIds[counterAnswer],
      idPureLatex: answerVectors.answerIdsPureLatex[counterAnswers],
      idButtonContainer: "asdf"
    });
    theProblem["answers"];
  }
  initializeMathQuill(thePage.currentCourse.fileName);
}

function previewAnswers(answerData){
  
}

function initializeMathQuill(fileName){
  var theAnswers = thePage.currentProblem[fileName].answers;
  console.log(theAnswers);
  initializeButtonsCommon();
  for (var counterAnswers = 0; counterAnswers < theAnswers.length; counterAnswers ++) {
    theAnswers[counterAnswers].initialize();
  }
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