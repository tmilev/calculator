"use strict";

function selectCurrentProblem(problem, exerciseType){
  thePage.currentCourse.fileName = problem;
  thePage.currentCourse.request = exerciseType;
  thePage.storeSettingsToCookies();
  thePage.flagCurrentProblemLoaded = false;
  thePage.selectPage("problemPage");
}

function Problem (inputFileName){
  this.fileName = inputFileName;
  this.randomSeed = null;
  this.answers = [];
  this.title = "";
  this.problemLabel = "";
  this.flagForReal = true;

//  thePage.pages.problemPage.problems[this.fileName] = this;
}

Problem.prototype.getURLFileCourseTopics = function() {
  return `fileName=${this.fileName}&currentCourse=${thePage.currentCourse.courseHome}&topicList=${thePage.currentCourse.topicList}&`;
}

Problem.prototype.getURL = function() {
  var result = `${pathnames.app}#${this.getURLFileCourseTopics()}`;
  if (this.flagForReal){
    return result;
  }
  result += `randomSeed=${this.randomSeed}&`;
  return result;
}

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  var topPart = "";
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    topPart = `<b style = 'color:green'>Scores not recorded.</b> ${this.problemLabel} ${this.title}`;
  } else {
    topPart = `<b style = 'color:brown'>Scores are recorded.</b> ${this.problemLabel} ${this.title}`;
  }
  if (!this.flagForReal){
    topPart += `&nbsp; <a class='problemLinkPractice' href='${this.getURL()}'>#${this.randomSeed}</a>`;
  }
  topPart += "<br>";
  outputElement.innerHTML = topPart + this.decodedProblem + this.scripts;
  for (var counterAnswers = 0;  counterAnswers < this.answers.length; counterAnswers ++){
    var currentAnswerPanel = this.answers[counterAnswers];
    var latexChangeHandler = currentAnswerPanel.editLaTeX;
    var latexChangeHandlerBound = latexChangeHandler.bind(currentAnswerPanel);
    var theElement = document.getElementById(currentAnswerPanel.idPureLatex);
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
    if (! this.flagForReal){
      document.getElementById(currentAnswerPanel.idButtonAnswer).addEventListener(
        'click', currentAnswerPanel.submitGiveUp.bind(currentAnswerPanel)
      );
    }
    
    //theElement.addEventListener('onchange', latexChangeHandler);
    //theElement.attributes.onkeyup = latexChangeHandler;
    //console.log(theElement);
    //console.log(latexChangeHandler);
    currentAnswerPanel.initialize();

  }

  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divProblemPageContentContainer")]);
  initializeAccordionButtons();

}

function updateProblemPageCallback(input, outputComponent){
  thePage.pages.problemPage.problems[thePage.currentCourse.fileName] = new Problem(thePage.currentCourse.fileName);
  var currentProblem = thePage.pages.problemPage.problems[thePage.currentCourse.fileName];
  var theProblem = JSON.parse(input);

  currentProblem.decodedProblem = decodeURIComponent(theProblem["problem"]);
  currentProblem.scripts = decodeURIComponent(theProblem["scripts"]);
  currentProblem.title = theProblem.title;
  currentProblem.problemLabel = theProblem["problemLabel"];
  currentProblem.flagForReal = theProblem["forReal"];
  currentProblem.randomSeed = theProblem.randomSeed;

  var answerVectors = theProblem["answers"];  
  for (var counterAnswers = 0;  counterAnswers < answerVectors.length; counterAnswers ++){
    currentProblem.answers[counterAnswers] = new InputPanelData({
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
  }
  currentProblem.writeToHTML("divProblemPageContentContainer");
}

function updateProblemPage(){
  if (thePage.flagCurrentProblemLoaded) {
    return;
  }
  thePage.flagCurrentProblemLoaded = true;
  var theURL = `${pathnames.calculatorAPI}?request=${thePage.currentCourse.request}&`;
  theURL += `fileName=${thePage.currentCourse.fileName}&topicList=${thePage.currentCourse.topicList}&`;
  theURL += `courseHome=${thePage.currentCourse.courseHome}&`;
  submitGET({
    url: theURL,
    callback: updateProblemPageCallback
  });
}