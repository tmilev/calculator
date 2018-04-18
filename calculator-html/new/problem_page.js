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
  thePage.currentProblem[thePage.currentCourse.fileName].answers = theProblem["answers"];
  initializeMathQuill(thePage.currentCourse.fileName);
}

function previewAnswers(answerData){
  
}

function initializeMathQuill(fileName){
  var theAnswers = thePage.currentProblem[fileName].answers;
  for (var counterAnswers = 0; counterAnswers < theAnswers.answerMQspanIds.length; counterAnswers ++) {
    var currentMQSpanId = theAnswers.answerMQspanIds[counterAnswers];
    var currentSpanVariable = document.getElementById(theAnswers.answerIdsPureLatex[counterAnswers]);
    globalMQ.config({
      autoFunctionize: 'sin cos tan sec csc cot log ln'
    });
    var answerData = {
      fileName: fileName,
      counter: counterAnswers,
      latexId: theAnswers.answerIdsPureLatex[counterAnswers],
      mqSpanId: currentMQSpanId,
      mqObject: null,
      ignoreNextMathQuillUpdateEvent: false,
      mqEditFunction: function() { // useful event handlers
        if (this.ignoreNextMathQuillUpdateEvent) {
          return;
        }
        var currentIdLaTeX = this.latexId;
        document.getElementById(currentIdLaTeX).value = processMathQuillLatex(this.mqObject.latex()); // simple API
        previewAnswers(currentIdLaTeX, "verification" + currentIdLaTeX);
      }
    };
    var currentMQspan = document.getElementById(currentMQSpanId);
    var mqEditFunctionBound = answerData.mqEditFunction.bind(answerData);
    answerData.mqObject = globalMQ.MathField(currentMQspan, {
      spaceBehavesLikeTab: true, // configurable
      supSubsRequireOperand: true, // configurable
      autoSubscriptNumerals: true, // configurable
      handlers: {
        edit: mqEditFunctionBound
      }
    });
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