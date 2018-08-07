"use strict";

function selectCurrentProblem(problemIdURLed, exerciseType) {
  thePage.storage.currentCourse.currentProblemId.setAndStore(problemIdURLed);
  thePage.storage.currentCourse.fileName.setAndStore(decodeURIComponent(problemIdURLed));
  thePage.storage.currentCourse.exerciseType.setAndStore(exerciseType);
  var theProblem = thePage.getCurrentProblem();
  theProblem.flagForReal = false;
  if (exerciseType === "scoredQuizJSON") {
    theProblem.flagForReal = true;
  }
  //thePage.storage.currentCourse.request = exerciseType;
  thePage.pages.problemPage.flagLoaded = false;
  thePage.selectPage(thePage.pages.problemPage.name);
}

function Problem(problemData) {
  this.idURLed = encodeURIComponent(problemData.id);
  this.randomSeed = null;
  this.answers = [];
  this.problemLabel = "";
  this.flagForReal = true;
  this.previousProblemId = null;
  this.nextProblemId = null;
  this.scriptIds = null;
  this.type = problemData.type;
  this.fileName = problemData.fileName;
  if (this.fileName === null || this.fileName === undefined) {
    this.fileName = "";
  }
  this.title = problemData.title;
  this.problemNumberString = problemData.problemNumberString;
  this.idButtonPoints = `modifyPoints${this.idURLed}`;
  this.idTextareaPoints = `points${this.idURLed}`;
  this.idModifyReportDeadline = `deadlines${this.idURLed}`;
  this.idDeadlinePanel = `deadlinesPanel${this.idURLed}`;
  this.idDeadlineContainer = `${stringResources.prefixDeadlineContainer}${this.idURLed}`;
  this.idModifyReportPoints = `report${this.idURLed}`;
  this.correctlyAnswered = problemData.correctlyAnswered;
  this.totalQuestions = problemData.totalQuestions;
  this.deadlines = [];
  this.deadlineString = null;
  this.deadline = null;
  this.weight = problemData.weight;
  if (problemData.deadlines !== undefined) {
    this.deadlines = problemData.deadlines;
  } 
  if (problemData.deadline !== undefined) {
    this.deadlineString = problemData.deadline;
  }
  if (this.fileName !== "") {
    this.previousProblemId = thePage.previousProblemId;
    if (
      this.previousProblemId !== null && 
      this.previousProblemId !== undefined && 
      this.previousProblemId !== ""
    ) {
      thePage.problems[this.previousProblemId].nextProblemId = this.idURLed;
    }
    thePage.previousProblemId = this.idURLed;
  }
  this.problemNumberString = problemData.problemNumberString;
  this.video = problemData.video;
  this.slidesProjector = problemData.slidesProjector;
  this.slidesPrintable = problemData.slidesPrintable;
  this.linkSlidesLaTeX = problemData.linkSlidesLaTeX;

  thePage.problems[this.idURLed] = this;
  if (this.type === "chapter") {
    thePage.theChapterIds[this.idURLed] = true;
  }
  this.childrenIds = [];
  if (problemData.children === undefined) {
    return;
  }
  for (var counterChildren = 0; counterChildren < problemData.children.length; counterChildren ++) {
    var currentChild = new Problem(problemData.children[counterChildren]);
    this.childrenIds.push(currentChild.idURLed);
  }
}

function getCalculatorURLRequestFileCourseTopicsFromStorage() {
  var currentCourse = thePage.storage.currentCourse; 
  var exerciseType = currentCourse.exerciseType.getValue();
  var fileName = currentCourse.fileName.getValue();
  var topicList = currentCourse.topicList.getValue();
  var courseHome = currentCourse.courseHome.getValue();
  var result = "";
  result += `${pathnames.calculatorAPI}?`;
  result += `request=${exerciseType}&fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}`;
  return result;
}

Problem.prototype.getAppAnchorRequestFileCourseTopics = function(isScoredQuiz) {
  var theExerciseType = "exerciseJSON";
  if (isScoredQuiz) {
    theExerciseType = "scoredQuizJSON";
  }
  var requestJSON = {
    currentPage: thePage.pages.problemPage.name,
    exerciseType: theExerciseType,
    fileName: this.fileName,
    courseHome: thePage.storage.currentCourse.courseHome.getValue(),
    topicList: thePage.storage.currentCourse.topicList.getValue()
  };
  return encodeURIComponent(JSON.stringify(requestJSON));
}

Problem.prototype.getCalculatorURLFileCourseTopics = function() {
  var result = "";
  result += `fileName=${this.fileName}&`;
  result += `courseHome=${thePage.storage.currentCourse.courseHome.getValue()}&`;
  result += `topicList=${thePage.storage.currentCourse.topicList.getValue()}&`;
  return result;
}

Problem.prototype.getCalculatorURLRequestFileCourseTopics = function(isScoredQuiz) {
  var result = "";
  if (isScoredQuiz === undefined) {
    isScoredQuiz = this.flagForReal;
  }
  result += `${pathnames.request}=`;
  if (isScoredQuiz) {
    result += "scoredQuizJSON";
  } else {
    result += "exerciseJSON";
  }
  result += `&${this.getCalculatorURLFileCourseTopics()}`;
  return result;
}

Problem.prototype.getURLRandomSeedRequestFileCourseTopics = function(isScoredQuiz) {
  var result = this.getURLRequestFileCourseTopics(isScoredQuiz);
  if (
    !this.flagForReal && 
    this.randomSeed !== undefined && 
    this.randomSeed !== "" && 
    this.randomSeed !== null  
  ) {
    result += `randomSeed=${this.randomSeed}&`;
  }
  return result;
}

Problem.prototype.getProblemNavigation = function() {
  var result = "";
  result += "<problemNavigation>";
  var linkType = "problemLinkPractice";
  var defaultRequest = 'exerciseJSON';
  if (this.flagForReal && thePage.user.flagLoggedIn) {
    defaultRequest = 'scoredQuizJSON';
    linkType = "problemLinkQuiz"
  }
  if (this.previousProblemId !== null && this.previousProblemId !== "") {
    var previousURL = thePage.problems[this.previousProblemId].getURLRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${previousURL}'`;
    result += `onclick = "selectCurrentProblem('${this.previousProblemId}', '${defaultRequest}')">&#8592;</a>`;
  }

  if (this.flagForReal && thePage.user.flagLoggedIn) {
    result += `<a class = 'problemLinkPractice' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "selectCurrentProblem('${this.idURLed}' ,'exerciseJSON')">Practice</a>`;
  } else {
    result += "<span class = 'problemLinkSelectedPractice' style='color:green'>Practice</span>";
  }
  if (!this.flagForReal && thePage.user.flagLoggedIn) { 
    result += `<a class = 'problemLinkQuiz' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "selectCurrentProblem('${this.idURLed}' ,'scoredQuizJSON')">Quiz</a>`;
  } else {
    if (this.flagForReal) {
      result += "<span class = 'problemLinkSelectedQuiz' style='color:brown'>Quiz</span>";
    }
  }
  if (this.nextProblemId !== null && this.nextProblemId !== "") {
    var nextURL = thePage.problems[this.nextProblemId].getAppAnchorRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${nextURL}' onclick = "selectCurrentProblem('${this.nextProblemId}', '${defaultRequest}')">&#8594;</a>`;
  }
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    result += `<b style = 'color:green'>Scores not recorded. </b>`;
  } else {
    result += `<b style = 'color:brown'>Scores are recorded. </b>`;
  }
  result += "</problemNavigation>";
  return result;
}

Problem.prototype.getEditPanel = function() {
  return getEditPanel(decodeURIComponent(this.idURLed));
}

function getEditPanel(fileName) {
  if (!thePage.user.hasProblemEditRights() || thePage.studentView()) {
    return "";
  }
  if (fileName === "" || fileName === undefined || fileName === null) {
    return "";
  }
  var result = "";
  result += `<span class = 'spanFileInfo'>`;
  result += `<button class = "buttonSaveEdit" onclick = "selectEditPage('${fileName}')">Edit</button>${fileName}&nbsp;`;
//  result += `<button class = "accordionLike" onclick = "selectEditPage('${fileName}')">Clone&#9666;</button>`;
  result += ``;
  result += `</span>`;
  return result;
}

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  var topPart = "";
  topPart += "<problemInfoBar>";
  topPart += this.getProblemNavigation();
  topPart += `<problemTitle>${this.problemLabel} ${this.title}</problemTitle>`;
  //topPart += "<br>"
  topPart += this.getEditPanel();
  topPart += "</problemInfoBar>";
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
    if (!this.flagForReal) {
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
  if (typeof outputComponent === "string" || outputComponent === undefined || outputComponent === null) {
    outputComponent = document.getElementById(outputComponent);
  }
  if (outputComponent === null || outputComponent === undefined) {
    outputComponent = document.getElementById("divProblemPageContentContainer");
  }
  var theProblem = null;
  try {
    theProblem = JSON.parse(input);
  } catch (e) {
    outputComponent.innerHTML = `Error parsing: ${e}. Failed to parse: ${input}`;
    thePage.cleanUpLoginSpan(outputComponent);
    return;
  }
  var currentProblem = thePage.getCurrentProblem();
  if (currentProblem === null || currentProblem === undefined) {
    thePage.problems[thePage.storage.currentCourse.currentProblemId.getValue()] = new Problem(theProblem);
    currentProblem = thePage.getCurrentProblem();
  }
  currentProblem.decodedProblem = decodeURIComponent(theProblem["problemContent"]);
  //var theScripts = currentProblem.scripts.split ("</script>");
  //for (var counterScripts = 0; counterScripts < theScripts.length; counterScripts++) {
  //  console.log(`Scripts: ${theScripts[counterScripts]}`);
  //}
  var answerVectors = theProblem["answers"];
  if (answerVectors === undefined) {
    currentProblem.writeToHTML("divProblemPageContentContainer");
    return;    
  }
  currentProblem.problemLabel = theProblem["problemLabel"];
  currentProblem.flagForReal = theProblem["forReal"];
  currentProblem.randomSeed = theProblem.randomSeed;

  for (var counterAnswers = 0;  counterAnswers < answerVectors.length; counterAnswers ++) {
    currentProblem.answers[counterAnswers] = new InputPanelData({
      problemId: currentProblem.idURLed,
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
  for (var scriptLabel in theProblem.scripts) {
    var newLabel = encodeURIComponent(theFileName + scriptLabel);
    currentProblem.scriptIds.push(newLabel); 
    thePage.injectScript(newLabel, decodeURIComponent(theProblem.scripts[scriptLabel]));
  }
}

function updateProblemPage() {
  if (thePage.pages.problemPage.flagLoaded) {
    return;
  }
  thePage.pages.problemPage.flagLoaded = true;
  var theProblem = thePage.getCurrentProblem();
  var theURL;
  if (theProblem !== undefined && theProblem !== null) {
    theURL = `${pathnames.calculatorAPI}?${theProblem.getCalculatorURLRequestFileCourseTopics()}`;
  } else {
    theURL = getCalculatorURLRequestFileCourseTopicsFromStorage();
  }
  //console.log("Current course: " + JSON.stringify(thePage.storage.currentCourse));
  submitGET({
    url: theURL,
    callback: updateProblemPageCallback,
    progress: "spanProgressReportGeneral"
  });
}