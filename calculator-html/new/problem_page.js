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
  this.previousProblem = null;
  this.nextProblem = null;
  this.scriptIds = null;

//  thePage.pages.problemPage.problems[this.fileName] = this;
}

Problem.prototype.getURLFileCourseTopics = function() {
  return `fileName=${this.fileName}&currentCourse=${thePage.currentCourse.courseHome}&topicList=${thePage.currentCourse.topicList}&`;
}

Problem.prototype.getURLRequestFileCourseTopics = function(isScoredQuiz) {
  var result = "";
  result += "request=";
  if (isScoredQuiz){
    result += "scoredQuizJSON";
  } else {
    if (thePage.flagUserLoggedIn){
      result += "exerciseJSON";
    } else {
      result += "exerciseNoLoginJSON";
    }
  }
  result += `&${this.getURLFileCourseTopics()}`;
  return result;
}

Problem.prototype.getURLRandomSeedRequestFileCourseTopics = function(isScoredQuiz) {
  var result = this.getURLRequestFileCourseTopics(isScoredQuiz);
  if (
    !this.flagForReal && this.randomSeed !== undefined && 
    this.randomSeed !== "" && this.randomSeed !== null  
  ){
    result += `randomSeed=${this.randomSeed}&`;
  }
  return result;
}

Problem.prototype.getURL = function(isScoredQuiz) {
  var result = "";
  result += `#problemPage?${this.getURLRequestFileCourseTopics(isScoredQuiz)}`; 
  return result;
}

Problem.prototype.getProblemNavigation = function() {
  var result = "";
  result += "<problemNavigation>";
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    result += `<b style = 'color:green'>Scores not recorded. </b>`;
  } else {
    result += `<b style = 'color:brown'>Scores are recorded. </b>`;
  }
  var defaultRequest = 'exerciseJSON';
  if (this.flagForReal) {
    defaultRequest = 'scoredQuizJSON';
  }
  if (!thePage.flagUserLoggedIn) {
    defaultRequest = 'exerciseNoLoginJSON';
  }
  if (this.previousProblem !== null && this.previousProblem !== "") {
    result+= `<a class='problemLinkPractice' href='#${thePage.getProblem(this.previousProblem).getURLRequestFileCourseTopics()}' onclick = "selectCurrentProblem('${this.previousProblem}' ,'${defaultRequest}')">&#8592;</a>`;
  }

  if (this.flagForReal && thePage.flagUserLoggedIn) {
    result += `<a class='problemLinkPractice' href='${this.getURL()}' onclick = "selectCurrentProblem('${this.fileName}' ,'exerciseJSON')">Practice</a>`;
  } else {
    result += "<b style='color:green'>Practice</b>";
  }
  if (!this.flagForReal && thePage.flagUserLoggedIn) { 
    result += `<a class='problemLinkPractice' href='${this.getURL()}' onclick = "selectCurrentProblem('${this.fileName}' ,'scoredQuizJSON')">Quiz</a>`;
  } else {
    if (this.flagForReal) {
      result += "<b style='color:brown'>Quiz</b>";
    }
  }
  if (this.nextProblem !== null && this.nextProblem !== "") {
    result+= `<a class='problemLinkPractice' href='#${thePage.getProblem(this.nextProblem).getURLRequestFileCourseTopics()}' onclick = "selectCurrentProblem('${this.nextProblem}' ,'${defaultRequest}')">&#8594;</a>`;
  }

  result += "</problemNavigation>";
  return result;
}

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  var topPart = "";
  topPart += `<problemTitle>${this.problemLabel} ${this.title}</problemTitle>`;
  topPart += this.getProblemNavigation();
  topPart += "<br>";
  outputElement.innerHTML = topPart + this.decodedProblem;
  for (var counterAnswers = 0;  counterAnswers < this.answers.length; counterAnswers ++) {
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
    if (! this.flagForReal) {
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

function updateProblemPageCallback(input, outputComponent) {
  var theFileName = thePage.currentCourse.fileName;
  if (typeof outputComponent === "string") {
    outputComponent = document.getElementById(outputComponent);
  }
  if (outputComponent === null || outputComponent === undefined){
    outputComponent = document.getElementById("divProblemPageContentContainer");
  }
  if (outputComponent.innerHTML === undefined){
    outputComponent = document.getElementById("divProblemPageContentContainer");
  }
  var currentProblem = thePage.getProblem(theFileName);
  var theProblem = null;
  try {
    theProblem = JSON.parse(input);
  } catch (e) {
    outputComponent.innerHTML = `Error parsing: ${e}. Failed to parse${input}`;
    return;
  }
  currentProblem.decodedProblem = decodeURIComponent(theProblem["problem"]);
  //var theScripts = currentProblem.scripts.split ("</script>");
  //for (var counterScripts = 0; counterScripts < theScripts.length; counterScripts++){
  //  console.log(`Scripts: ${theScripts[counterScripts]}`);
  //}
  currentProblem.title = theProblem.title;
  currentProblem.problemLabel = theProblem["problemLabel"];
  currentProblem.flagForReal = theProblem["forReal"];
  currentProblem.randomSeed = theProblem.randomSeed;

  var answerVectors = theProblem["answers"];  
  for (var counterAnswers = 0;  counterAnswers < answerVectors.length; counterAnswers ++) {
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

  currentProblem.scriptIds = [];
  for (var scriptLabel in theProblem.scripts){
    var newLabel = encodeURIComponent(theFileName + scriptLabel);
    currentProblem.scriptIds.push(newLabel); 
    thePage.injectScript(newLabel, decodeURIComponent(theProblem.scripts[scriptLabel]));
  }
}

function updateProblemPage() {
  if (thePage.flagCurrentProblemLoaded) {
    return;
  }
  thePage.flagCurrentProblemLoaded = true;
  var theProblem = thePage.getCurrentProblem();
  var theURL = `${pathnames.calculatorAPI}?request=${thePage.currentCourse.request}&${theProblem.getURLFileCourseTopics()}`;
  submitGET({
    url: theURL,
    callback: updateProblemPageCallback,
    progress: "spanProgressReportGeneral"
  });
}