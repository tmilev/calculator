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
  for (var counterAnswers = 0;  counterAnswers < answerVectors.length; counterAnswers ++){
    thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers] = new InputPanelData({
      fileName: thePage.currentCourse.fileName,
      idMQSpan: answerVectors[counterAnswers].answerMQspanId,
      idPureLatex: answerVectors[counterAnswers].answerIdPureLatex,
      idButtonContainer: answerVectors[counterAnswers].preferredButtonContainer,
      idButtonSubmit: answerVectors[counterAnswers].idButtonSubmit,
      idButtonInterpret: answerVectors[counterAnswers].idButtonInterpret,
      idButtonAnswer: answerVectors[counterAnswers].idButtonAnswer,
      idVerificationSpan: answerVectors[counterAnswers].idVerificationSpan,
      flagAnswerPanel: true,
      flagCalculatorPanel: false,
    });
    var currentAnswerPanel = thePage.currentProblem[thePage.currentCourse.fileName].answers[counterAnswers];
    var latexChangeHandler = currentAnswerPanel.editLaTeX;
    var latexChangeHandlerBound = latexChangeHandler.bind(currentAnswerPanel);
    var theElement = document.getElementById(answerVectors[counterAnswers].answerIdPureLatex);
    theElement.addEventListener('keyup', latexChangeHandlerBound);

    var interpretHandler = currentAnswerPanel.submitPreview;
    var interpretHandlerBound = interpretHandler.bind(currentAnswerPanel);
    theElement = document.getElementById(currentAnswerPanel.idButtonInterpret);
    theElement.addEventListener('click', interpretHandlerBound);
    document.getElementById(currentAnswerPanel.idButtonInterpret).addEventListener(
      'click', currentAnswerPanel.submitPreview.bind(currentAnswerPanel)
    );
    document.getElementById(currentAnswerPanel.idButtonSubmit).addEventListener(
      'click', currentAnswerPanel.submitAnswer.bind(currentAnswerPanel)
    );
    document.getElementById(currentAnswerPanel.idButtonAnswer).addEventListener(
      'click', currentAnswerPanel.submitGiveUp.bind(currentAnswerPanel)
    );
    
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