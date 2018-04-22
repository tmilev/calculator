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
  for (var counterAnswers = 0;  counterAnswers < answerVectors.answerMQspanIds.length; counterAnswers ++){
    thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers] = new InputPanelData({
      fileName: thePage.currentCourse.fileName,
      idMQSpan: answerVectors.answerMQspanIds[counterAnswers],
      idPureLatex: answerVectors.answerIdsPureLatex[counterAnswers],
      idButtonContainer: answerVectors.preferredButtonContainers[counterAnswers],
      flagAnswerPanel: true,
      flagCalculatorPanel: false,
    });
    var latexChangeHandler = thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers].editLaTeX;
    var handlerBound = latexChangeHandler.bind(thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers]);
    var theElement = document.getElementById(answerVectors.answerIdsPureLatex[counterAnswers]);
    theElement.addEventListener('keyup', handlerBound);
    //theElement.addEventListener('onchange', latexChangeHandler);
    //theElement.attributes.onkeyup = latexChangeHandler;
    //console.log(theElement);
    //console.log(latexChangeHandler);
    thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers].initialize();
  }
  initializeAccordionButtons();
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