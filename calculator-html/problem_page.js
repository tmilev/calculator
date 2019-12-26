"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const editPage = require("./edit_page");
const initializeButtons = require("./initialize_buttons");
const InputPanelData = initializeButtons.InputPanelData;
const mathjax = require("./mathjax-calculator-setup");
const miscellaneous = require("./miscellaneous");
const miscellaneousFrontend = require("./miscellaneous_frontend");

function ProblemCollection() {
  /** @type{Object<string,Problem>} */
  this.topicProblems = {};
  /** @type{Object<string,Problem>} */
  this.allProblems = {};
  /** @type{string} */
  this.previousProblemId = null;
  /** @type{string} */
  this.nextProblemId = null;
  /** @type{Object<string, boolean>} */
  this.theChapterIds = {};
  this.theTopics = {};
}

/** @returns{Problem} */
ProblemCollection.prototype.getProblemById = function(
  /**@type{string} */
  label,
) {
  if (!(label in this.allProblems)) {
    throw (`Error: problem label ${label} not found. `);
  }
  return this.allProblems[label];
}

ProblemCollection.prototype.resetTopicProblems = function() {
  this.theChapterIds = {};
  this.topicProblems = {};
}

/** @returns{Problem} */
ProblemCollection.prototype.CreateOrUpdateProblem = function(problemData) {
  // theProblemId is percent encoded, safe to embed in html.
  var theProblemId = encodeURIComponent(problemData.id);
  var currentProblem = this.getProblemByIdOrRegisterEmpty(theProblemId, "");
  currentProblem.initializeInfo(problemData, null);
  return currentProblem;
}

ProblemCollection.prototype.getProblemByIdOrRegisterEmpty = function(
  label, fileName
) {
  if (!(label in this.allProblems)) {
    var incoming = new Problem();
    incoming.initializeBasic({
      id: label,
      fileName: fileName
    }); 
    this.allProblems[label] = incoming;
  }
  return this.allProblems[label];
}

function selectCurrentProblem(problemIdURLed, exerciseType) {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.currentCourse.currentProblemId.setAndStore(problemIdURLed);
  thePage.storage.variables.currentCourse.problemFileName.setAndStore(decodeURIComponent(problemIdURLed));
  thePage.storage.variables.currentCourse.exerciseType.setAndStore(exerciseType);
  var theProblem = thePage.getCurrentProblem();
  theProblem.flagForReal = false;
  if (exerciseType === pathnames.urlFields.scoredQuizJSON) {
    theProblem.flagForReal = true;
  }
  thePage.pages.problemPage.flagLoaded = false;
  thePage.selectPage(thePage.pages.problemPage.name);
}

function Problem() {
  /**@type{string}*/
  this.nextProblemId = "";
  /**@type{string} */
  this.previousProblemId = "";
}

Problem.prototype.setRandomSeed = function(input) {
  this.randomSeed = input;  
}

Problem.prototype.setRandomSeedFromEnvironment = function() {
  var currentCourse = window.calculator.mainPage.storage.variables.currentCourse; 
  this.flagForReal = (currentCourse.exerciseType.getValue() !== pathnames.urlFields.exerciseJSON);
  var incomingRandomSeed = currentCourse.randomSeed.getValue();
  if (incomingRandomSeed === "" || incomingRandomSeed === null || incomingRandomSeed === undefined) {
    if (!this.flagForReal) {
      return;
    }
  } 
  this.setRandomSeed(incomingRandomSeed);  
  currentCourse.randomSeed.value = "";
} 

Problem.prototype.initializeBasic = function(problemData) {
  /**ProblemId is percent encoded, safe to embed in html. */
  this.problemId = encodeURIComponent(problemData.id);
  /**@type {string}  
   * This id is for problem navigation only, does not include the entire panel. 
  */
  this.idNavigationProblemNotEntirePanel = `navigationPanel${this.problemId}`;
  /**@type {InputPanelData[]} */
  this.answers = [];  
  this.title = problemData.title;
  this.fileName = problemData.fileName;
  if (this.fileName === undefined || this.fileName === null) {
    this.fileName = "";
  }
  if (window.calculator.mainPage.flagProblemPageOnly) {
    this.flagForReal = false;       
  } else {
    this.setRandomSeedFromEnvironment();
  }
}

Problem.prototype.initializeInfo = function(problemData, inputParentIdURLed) {
  this.initializeBasic(problemData);
  this.decodedProblem = "";
  this.commentsProblem = "";
  this.parentIdURLed = inputParentIdURLed;
  
  this.problemLabel = "";
  this.previousProblemId = "";
  this.nextProblemId = "";
  this.scriptIds = null;
  this.type = problemData.type;
  this.problemNumberString = problemData.problemNumberString;
  this.idButtonPoints = `modifyPoints${this.problemId}`;
  this.idTextareaPoints = `points${this.problemId}`;
  this.idModifyReportDeadline = `deadlines${this.problemId}`;
  this.idDeadlinePanel = `deadlinesPanel${this.problemId}`;
  this.idDeadlineContainer = `${ids.stringResources.prefixDeadlineContainer}${this.problemId}`;
  this.idModifyReportPoints = `report${this.problemId}`;
  this.correctlyAnswered = problemData.correctlyAnswered;
  this.totalQuestions = problemData.totalQuestions;
  this.deadlines = [];
  this.deadlineString = null;
  this.deadline = null;
  this.weight = problemData.weight;
  this.links = {
    /**@type{HTMLElement[]} */
    video: [],
    /**@type{HTMLElement[]} */
    slides: [],
    /**@type{HTMLElement[]} */
    homework: [],
  };
  /**@type{HTMLElement[]} */
  this.badProblemExplanation = [];
  if (problemData.deadlines !== undefined) {
    this.deadlines = problemData.deadlines;
  } 
  if (problemData.deadline !== undefined) {
    this.deadlineString = problemData.deadline;
  }
  if (this.fileName !== "") {
    this.previousProblemId = allProblems.previousProblemId;
    if (
      this.previousProblemId !== null && 
      this.previousProblemId !== undefined && 
      this.previousProblemId !== ""
    ) {
      var previousProblem = allProblems.allProblems[this.previousProblemId]; 
      previousProblem.nextProblemId = this.problemId;
    }
    allProblems.previousProblemId = this.problemId;
  }
  this.problemNumberString = problemData.problemNumberString;
  this.video = problemData.video;
  this.querySlides = problemData.querySlides;
  if (this.querySlides === undefined || this.querySlides === null) {
    this.querySlides = "";
  }
  this.queryHomework = problemData.queryHomework;
  if (this.queryHomework === undefined || this.queryHomework === null) {
    this.queryHomework = "";
  }
  if (this.type === "Chapter") {
    allProblems.theChapterIds[this.problemId] = true;
  }
  this.childrenIds = [];
  if (Array.isArray(problemData.children)) {
    for (var counterChildren = 0; counterChildren < problemData.children.length; counterChildren ++) {
      var currentChild = allProblems.CreateOrUpdateProblem(problemData.children[counterChildren]);
      this.childrenIds.push(currentChild.problemId);
    }
  }
}

/**@returns{HTMLElement[]} */
Problem.prototype.computeBadProblemExplanation = function() {
  var userHasInstructorRights = true;
  var pageLastKnownGoodProblemName = "";
  var thePage = window.calculator.mainPage;
  if (!thePage.flagProblemPageOnly) {
    userHasInstructorRights = thePage.user.hasInstructorRights();
  }
  this.badProblemExplanation = [];
  if (!this.decodedProblem.includes(pathnames.urlFields.problem.failedToLoadProblem)) {
    thePage.lastKnownGoodProblemFileName = this.fileName;
    return;
  }
  pageLastKnownGoodProblemName = thePage.lastKnownGoodProblemFileName;
  var badProblemExplanationPartOne = document.createElement("div");
  badProblemExplanationPartOne.innerHTML += "It appears your problem failed to load.<br>";
  if (this.lastKnownGoodProblemFileName !== "" && userHasInstructorRights) {
    badProblemExplanationPartOne.innerHTML += "Perhaps you may like to clone the last good known problem.<br>";
  }
  this.badProblemExplanation.push(badProblemExplanationPartOne);
  miscellaneousFrontend.appendHtmlToArray(this.badProblemExplanation, editPage.getClonePanel(pageLastKnownGoodProblemName, this.fileName));
  var epilogueElement = document.createElement("hr");
  this.badProblemExplanation.push(epilogueElement);
}

Problem.prototype.initializeProblemContent = function(problemData) {
  this.initializeBasic(problemData)
  this.decodedProblem = decodeURIComponent(problemData[pathnames.urlFields.problem.content]);
  this.commentsProblem = problemData["commentsProblem"];
  if (this.commentsProblem === undefined) {
    this.commentsProblem = "";
  }
  this.computeBadProblemExplanation();
  var answerVectors = problemData["answers"];
  if (answerVectors === undefined) {
    this.writeToHTML(ids.domElements.problemPageContentContainer);
    return;    
  }
  this.flagForReal = problemData["forReal"];
  this.setRandomSeed(problemData.randomSeed);
  for (var counterAnswers = 0;  counterAnswers < answerVectors.length; counterAnswers ++) {
    var currentVector = answerVectors[counterAnswers];
    this.answers[counterAnswers] = new InputPanelData({
      problemId:            this.problemId,
      properties:           currentVector.properties,
      previousAnswers:      currentVector.previousAnswers,
      answerHighlight:      currentVector.answerHighlight,
      answerPanelId:        currentVector.answerPanelId,
      MQpanelButtonOptions: currentVector.MQpanelButtonOptions,
      idMQSpan:             currentVector.idMQSpan,
      idMQSpanLocation:     currentVector.idMQSpanLocation,
      idPureLatex:          currentVector.idPureLatex,
      idButtonContainer:    currentVector.idButtonContainer,
      idButtonSubmit:       currentVector.idButtonSubmit,
      idButtonInterpret:    currentVector.idButtonInterpret,
      idButtonAnswer:       currentVector.idButtonAnswer,
      idButtonSolution:     currentVector.idButtonSolution,
      idVerificationSpan:   currentVector.idVerificationSpan,
      flagAnswerPanel:      true,
      flagCalculatorPanel:  false,
    });
  }
  this.writeToHTML(ids.domElements.problemPageContentContainer);
  this.scriptIds = [];
  for (var scriptLabel in problemData.scripts) {
    var newLabel = encodeURIComponent(this.problemId + "_" + scriptLabel);
    this.scriptIds.push(newLabel); 
    var scriptContent = decodeURIComponent(problemData.scripts[scriptLabel]);
    window.calculator.mainPage.scriptInjector.injectScript(newLabel, scriptContent);
  }
}

function getCalculatorURLRequestFileCourseTopicsFromStorage() {
  var thePage = window.calculator.mainPage;
  var currentCourse = thePage.storage.variables.currentCourse; 
  var exerciseType = currentCourse.exerciseType.getValue();
  if (exerciseType === "" || exerciseType === null || exerciseType === undefined) {
    exerciseType = pathnames.urlFields.exerciseJSON;
    currentCourse.exerciseType.setAndStore(exerciseType);
  }
  var fileName = currentCourse.fileName.getValue();
  var topicList = currentCourse.topicList.getValue();
  var courseHome = currentCourse.courseHome.getValue();
  var environmentRandomSeed = currentCourse.randomSeed.getValue();
  var result = "";
  result += `${pathnames.urls.calculatorAPI}?`;
  result += `${pathnames.urlFields.request}=${exerciseType}&fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}&`;
  if (
    environmentRandomSeed !== null && 
    environmentRandomSeed !== "" && 
    environmentRandomSeed !== undefined
  ) {
    result += `randomSeed=${environmentRandomSeed}&`;
  }
  currentCourse.randomSeed.value = "";
  return result;
}

Problem.prototype.getAppAnchorRequestFileCourseTopics = function(
  /**@type {boolean} */
  isScoredQuiz, 
  /**@type {boolean} */
  includeRandomSeed,
) {
  var thePage = window.calculator.mainPage;
  var theExerciseType = pathnames.urlFields.exerciseJSON;
  if (isScoredQuiz) {
    theExerciseType = pathnames.urlFields.scoredQuizJSON;
  }
  var requestJSON = {
    currentPage: thePage.pages.problemPage.name,
    exerciseType: theExerciseType,
    fileName: this.fileName,
    currentProblemId: this.problemId,
    courseHome: thePage.storage.variables.currentCourse.courseHome.getValue(),
    topicList: thePage.storage.variables.currentCourse.topicList.getValue(),
  };
  if (includeRandomSeed) {
    if (
      this.randomSeed !== "" && 
      this.randomSeed !== null && 
      this.randomSeed !== undefined
    ) {
      requestJSON["randomSeed"] = this.randomSeed;
    }
  }
  var thePage = window.calculator.mainPage;
  var stringifiedHash = thePage.storage.getCleanedUpURL(requestJSON);
  return stringifiedHash;
}

Problem.prototype.getCalculatorURLFileCourseTopics = function() {
  var thePage = window.calculator.mainPage;
  return this.getCalculatorURLInput(
    this.fileName, 
    thePage.storage.variables.currentCourse.courseHome.getValue(), 
    thePage.storage.variables.currentCourse.topicList.getValue(),
  );
}

Problem.prototype.getCalculatorURLInput = function(
  inputFileName, inputCourseHome, inputTopicList
) {
  var result = "";
  result += `fileName=${inputFileName}&`;
  result += `courseHome=${inputCourseHome}&`;
  result += `topicList=${inputTopicList}`;
  return result;
}

Problem.prototype.getCalculatorURLRequestPartOne = function(isScoredQuiz) {
  var result = "";
  if (isScoredQuiz === undefined) {
    isScoredQuiz = this.flagForReal;
  }
  result += `${pathnames.urlFields.request}=`;
  if (isScoredQuiz) {
    result += pathnames.urlFields.scoredQuizJSON;
  } else {
    result += pathnames.urlFields.exerciseJSON;
    if (this.randomSeed !== null && this.randomSeed !== "" && this.randomSeed !== undefined) {
      result += `&${pathnames.urlFields.randomSeed}=${this.randomSeed}`;
    }
  }
  return result;
}

Problem.prototype.getCalculatorURLRequestInput = function(
  isScoredQuiz, inputFileName, inputCourseHome, inputTopicList
) {
  var result = "";
  result += this.getCalculatorURLRequestPartOne(isScoredQuiz) + "&";
  result += this.getCalculatorURLInput(
    inputFileName, 
    inputCourseHome, 
    inputTopicList,
  );
  return result;
}

Problem.prototype.getCalculatorURLRequestFileCourseTopics = function(isScoredQuiz) {
  var result = "";
  result += this.getCalculatorURLRequestPartOne(isScoredQuiz);
  result += `&${this.getCalculatorURLFileCourseTopics()}&`;
  return result;
}

/**@returns {HTMLElement} */
Problem.prototype.getProblemNavigation = function() {
  var result = document.createElement("DIV");
  result.id = this.idNavigationProblemNotEntirePanel;
  result.className = 'problemNavigation';
  if (!window.calculator.mainPage.flagProblemPageOnly) {
    var children = this.getProblemNavigationContent();
    for (var i = 0; i < children.length; i ++) {
      result.append(children[i]);
    }
  }
  return result;
}

function ProblemNavigationHints() {
  this.linkType = "problemLinkPractice";
  this.defaultRequest = pathnames.urlFields.exerciseJSON;
  this.isScoredQuiz = false;
}

/**@returns {HTMLElement} */
Problem.prototype.getNextProblemButton = function(
  /** @type{ProblemNavigationHints} */
  hints
) {
  if (
    this.nextProblemId === null || 
    this.nextProblemId === "" || 
    this.nextProblemId === undefined
  ) {
    return document.createTextNode("");
  }
  var nextProblem = allProblems.getProblemById(this.nextProblemId); 
  var nextURL = nextProblem.getAppAnchorRequestFileCourseTopics(hints.isScoredQuiz, false);
  var nextProblemTag = document.createElement("a");
  nextProblemTag.className = hints.linkType;
  nextProblemTag.href = `#${nextURL}`;
  nextProblemTag.addEventListener(
    "click", 
    selectCurrentProblem.bind(
      null, 
      this.nextProblemId, 
      hints.defaultRequest,
    )
  );
  nextProblemTag.innerHTML = "&#8594;";
  return nextProblemTag;
}

/** @returns {HTMLElement} */
Problem.prototype.getPreviousProblemButton = function(
  /** @type{ProblemNavigationHints} */
  hints
) {
  if (
    this.previousProblemId === null || 
    this.previousProblemId === "" || 
    this.previousProblemId === undefined
  ) {
    return document.createTextNode("");
  }
  var previousProblem = allProblems.getProblemById(this.previousProblemId); 
  var previousURL = previousProblem.getAppAnchorRequestFileCourseTopics(hints.isScoredQuiz, false);
  var previousLink = document.createElement("a");
  previousLink.className = hints.linkType;
  previousLink.href = `#${previousURL}`;
  previousLink.addEventListener(
    "click", window.calculator.problemPage.selectCurrentProblem.bind(
      null, 
      this.previousProblemId, 
      hints.defaultRequest,
    )
  );
  previousLink.innerHTML = "&#8592;";
  return previousLink;
}

/** @returns{ProblemNavigationHints} */
Problem.prototype.getProblemNavigationHints = function() {
  var thePage = window.calculator.mainPage;
  var result = new ProblemNavigationHints();
  if (this.flagForReal && thePage.user.flagLoggedIn) {
    result.defaultRequest = pathnames.urlFields.scoredQuizJSON;
    result.linkType = "problemLinkQuiz";
    result.isScoredQuiz = true;
  }
  return result;
}

/**@returns {HTMLElement[]} */
Problem.prototype.getProblemNavigationContent = function() {
  var thePage = window.calculator.mainPage;
  var result = [];
  var hints = this.getProblemNavigationHints();
  result.push(this.getPreviousProblemButton(hints));
  if (this.flagForReal && thePage.user.flagLoggedIn) {
    var practiceURL = this.getAppAnchorRequestFileCourseTopics(false, false);
    var practiceTag = document.createElement("a");
    practiceTag.className = "problemLinkPractice";
    practiceTag.href = `#${practiceURL}`;
    practiceTag.addEventListener("click", window.calculator.problemPage.selectCurrentProblem.bind(null, this.problemId, "exerciseJSON"));
    practiceTag.innerHTML = "Practice";
    result.push(practiceTag);
  } else {
    var selectedPracticeTag = document.createElement("span");
    selectedPracticeTag.className = "problemLinkSelectedPractice";
    selectedPracticeTag.style.color = "green";
    selectedPracticeTag.innerHTML = "Practice";
    result.push(selectedPracticeTag)
  }
  if (!this.flagForReal && thePage.user.flagLoggedIn) { 
    var quizURL = this.getAppAnchorRequestFileCourseTopics(true, false);
    var quizTag = document.createElement("a");
    quizTag.className = "problemLinkQuiz";
    quizTag.href = `#${quizURL}`;
    quizTag.addEventListener("click", window.calculator.problemPage.selectCurrentProblem.bind(null, this.problemId, "scoredQuizJSON"));
    quizTag.innerHTML = "Quiz";
    result.push(quizTag);
  } else {
    if (this.flagForReal) {
      var quizTag = document.createElement("span");
      quizTag.className = "problemLinkSelectedQuiz";
      quizTag.style.color = "brown";
      quizTag.innerHTML = "Quiz";
      result.push(quizTag);
    }
  }
  result.push(this.getNextProblemButton(hints));
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    var scoresTag = document.createElement("b");
    scoresTag.style.color = "green";
    scoresTag.innerHTML = "Scores not recorded.";
    result.push(scoresTag);
    result.push(document.createTextNode(" "));
    var randomSeedElement = document.createElement("SPAN");
    randomSeedElement.id = ids.domElements.spanProblemLinkWithRandomSeed;
    var randomSeedAnchor = document.createElement("a");
    randomSeedAnchor.href = `#${this.getAppAnchorRequestFileCourseTopics(false, true)}`;
    randomSeedAnchor.innerHTML = this.randomSeed;
    randomSeedElement.appendChild(randomSeedAnchor);
    result.push(randomSeedElement);
  } else {
    var scoresTag = document.createElement("b");
    scoresTag.style.color = "brown";
    scoresTag.innerHTML = "Scores are recorded. ";
    result.push(scoresTag);
  }
  return result;
}

Problem.prototype.getEditPanel = function() {
  return editPage.getEditPanel(decodeURIComponent(this.problemId));
}

Problem.prototype.onePanelComputeHtmlElements = function(/**@type {InputPanelData} */ thePanel) {
  if (thePanel.answerHighlight !== undefined && thePanel.answerHighlight !== "") {
    thePanel.htmlAnswerHighlight = `<answerCalculatorHighlight>${thePanel.answerHighlight}</answerCalculatorHighlight>`;
  } else {
    thePanel.htmlAnswerHighlight = "";
  }
  if (this.flagForReal === true) {
    thePanel.htmlButtonAnswer = "";
    thePanel.htmlButtonSolution = "";
  } else {
    if (thePanel.idButtonAnswer !== null && thePanel.idButtonAnswer !== undefined) {
      thePanel.htmlButtonAnswer = `<button class = "buttonAnswer" id = "${thePanel.idButtonAnswer}">Answer</button>`;
    } else {
      thePanel.htmlButtonAnswer = "Answer not available. ";
    }
    if (thePanel.idButtonSolution !== null && thePanel.idButtonSolution !== undefined) {
      thePanel.htmlButtonSolution = `<button class = "buttonSolution" id = "${thePanel.idButtonSolution}">Solution</button>`;
    } else {
      thePanel.htmlButtonSolution = "";
    }
  } 
  thePanel.htmlButtonSubmit = `<button class = "buttonSubmit" id = "${thePanel.idButtonSubmit}">Submit</button>`;
  thePanel.htmlButtonInterpret = `<button class = "buttonPreview" id = "${thePanel.idButtonInterpret}">Interpret</button>`;
  thePanel.htmlPureLatex = `<button class = "accordion">details</button>`;
  thePanel.htmlPureLatex += `<textarea class = "calculatorAnswer" id = "${thePanel.idPureLatex}"></textarea>`;
  thePanel.htmlButtonContainer = `<div class = "mqButtonPanel" id = "${thePanel.idButtonContainer}" `;
  thePanel.htmlButtonContainer += `buttons = "${thePanel.MQpanelButtonOptions}"></div>`;
  thePanel.htmlMQFieldEnclosure = `<div class = "calculatorMQfieldEnclosure"><span id = "${thePanel.idMQSpan}"></span></div>`;

  thePanel.htmlVerificationSpan = `<span id = "${thePanel.idVerificationSpan}">`;

  thePanel.flagGenerateQuestionAndAnswerField = true;
  if (this.htmlAnswerHighlight === undefined || this.htmlAnswerHighlight === null || this.htmlAnswerHighlight === "") {
    if (document.getElementById(thePanel.idMQSpanLocation) !== null) {
      thePanel.flagGenerateQuestionAndAnswerField = false;
    }
  }

  if (thePanel.previousAnswers !== undefined && thePanel.previousAnswers !== "") {
    thePanel.htmlVerificationSpan += thePanel.previousAnswers;
  } else {
    if (thePanel.layoutVertical) {
      thePanel.htmlVerificationSpan += `<b style = "color:brown">Waiting for answer [unlimited tries]</b>`;
    } else {
      thePanel.htmlVerificationSpan += ``;
    }
  }
  thePanel.htmlVerificationSpan += "</span>";
  if (this.flagForReal !== true && thePanel.idSpanSolution !== undefined && thePanel.idSpanSolution !== null) {
    thePanel.htmlSolution = `<span id = "${thePanel.idSpanSolution}"></span>`;
  }
}

Problem.prototype.onePanelQuestionAndAnswerField = function(/**@type {InputPanelData} */ thePanel) {
  var result = "";
  result += "<table><tr>";
  if (thePanel.htmlAnswerHighlight !== "") {
    result += `<td>${thePanel.htmlAnswerHighlight}</td>`;
  }
  result += `<td class = "tableCellMQfield">`;
  result += thePanel.htmlMQFieldEnclosure;
  result += `</td>`;
  result += "</tr></table>";
  return result;
}

Problem.prototype.onePanelButtonsVerticalLayout = function(/**@type {InputPanelData} */ thePanel) {
  var result = "";
  result += "<table>";
  result += `<tr><td>${thePanel.htmlButtonSubmit}</td></tr>`;
  result += `<tr><td>${thePanel.htmlButtonInterpret}</td></tr>`;
  if (this.flagForReal !== true) {
    if (thePanel.htmlButtonAnswer !== "") {
      result += `<tr><td>${thePanel.htmlButtonAnswer}</td></tr>`;
    }
    if (thePanel.htmlButtonSolution !== "") {
      result += `<tr><td>${thePanel.htmlButtonSolution}</td></tr>`;
    }
  }
  result += "</table>";
  return result;
}

Problem.prototype.onePanelButtonsHorizontalLayout = function(/**@type {InputPanelData} */ thePanel) {
  var result = "";
  result += "<table><tr>";
  result += `<td>${thePanel.htmlButtonSubmit}</td>`;
  result += `<td>${thePanel.htmlButtonInterpret}</td>`;
  if (this.flagForReal !== true) {
    if (thePanel.htmlButtonAnswer !== "") {
      result += `<td>${thePanel.htmlButtonAnswer}</td>`;
    }
    if (thePanel.htmlButtonSolution !== "") {
      result += `<td>${thePanel.htmlButtonSolution}</td>`;
    }
  }
  result += "</tr></table>";
  return result;
}

Problem.prototype.onePanel = function(/**@type {InputPanelData} */ thePanel) {
  var latexChangeHandler = thePanel.editLaTeX;
  var latexChangeHandlerBound = latexChangeHandler.bind(thePanel);
  thePanel.layoutVertical = true;
  if (thePanel.properties !== undefined) {
    if (thePanel.properties.layout === "horizontal") {
      thePanel.layoutVertical = false;
    }
  }
  this.onePanelComputeHtmlElements(thePanel);
  if (!thePanel.flagGenerateQuestionAndAnswerField) {
    var mqSpan = document.getElementById(thePanel.idMQSpanLocation); 
    mqSpan.innerHTML = thePanel.htmlMQFieldEnclosure;
  }
  var panelContent = "";
  if (thePanel.layoutVertical === true) { 
    panelContent += "<table><tr>";
    if (thePanel.flagGenerateQuestionAndAnswerField) {
      panelContent += `<td>${this.onePanelQuestionAndAnswerField(thePanel)}</td>`;
    }
    panelContent += `<td>${thePanel.htmlButtonContainer}</td>`;
    panelContent += `<td>${this.onePanelButtonsVerticalLayout(thePanel)}</td>`;
    panelContent += `<td>${thePanel.htmlPureLatex}</td>`;
    panelContent += "</tr></table>";
    panelContent += thePanel.htmlVerificationSpan;
    panelContent += thePanel.htmlSolution;
  } else {
    panelContent += "<table><tr>";
    if (thePanel.flagGenerateQuestionAndAnswerField) {
      panelContent += `<td>${this.onePanelQuestionAndAnswerField(thePanel)}</td>`;
    }
    panelContent += `<td>${this.onePanelButtonsHorizontalLayout(thePanel)}</td>`;
    panelContent += `<td>${thePanel.htmlButtonContainer}</td>`;
    panelContent += `<td>${thePanel.htmlPureLatex}</td>`;
    panelContent += "</tr></table>";
    panelContent += `${thePanel.htmlVerificationSpan}${thePanel.htmlSolution}`;
  }
  var panelSpan = document.getElementById(thePanel.answerPanelId);
  panelSpan.innerHTML = panelContent;
  var theElement = document.getElementById(thePanel.idPureLatex);
  theElement.addEventListener('keyup', latexChangeHandlerBound);
  var interpretHandler = thePanel.submitPreview;
  var interpretHandlerBound = interpretHandler.bind(thePanel);
  theElement = document.getElementById(thePanel.idButtonInterpret);
  theElement.addEventListener('click', interpretHandlerBound);
  document.getElementById(thePanel.idButtonInterpret).addEventListener(
    'click', 
    thePanel.submitPreview.bind(thePanel)
  );
  document.getElementById(thePanel.idButtonSubmit).addEventListener(
    'click', 
    thePanel.submitAnswers.bind(thePanel)
  );
  if (this.flagForReal !== true && thePanel.htmlButtonAnswer !== "") {
    var buttonAnswer = document.getElementById(thePanel.idButtonAnswer); 
    buttonAnswer.addEventListener(
      'click', 
      thePanel.submitGiveUp.bind(thePanel)
    );
  }
  if (
    thePanel.idButtonSolution !== undefined && 
    thePanel.idButtonSolution !== null &&
    thePanel.idButtonSolution !== ""
  ) {
    var theSolutionButton = document.getElementById(thePanel.idButtonSolution);
    if (theSolutionButton !== null) {
      theSolutionButton.addEventListener(
        'click', thePanel.showSolution.bind(thePanel)
      );
    }
  }
  thePanel.initialize();
}

function ProblemNavigation() {
  /**@type{Problem} */ 
  this.currentProblem = null;

}

ProblemNavigation.prototype.setCurrentProblemAndUpdate = function(
  /**@type{Problem} */ 
  inputProblem,
) {
  this.currentProblem = inputProblem;
  this.writeToHTML();
}

ProblemNavigation.prototype.writeToHTML = function() {
  if (this.currentProblem === null) {
    return;
  }

  var problemTitle = document.createElement("DIV");
  problemTitle.className = "problemTitle";
  
  var problemTitleContainer = document.createElement("DIV");
  problemTitleContainer.className = "problemTitleContainer";
  problemTitle.appendChild(problemTitleContainer);
  if (
    this.currentProblem.problemLabel !== undefined && 
    this.currentProblem.problemLabel !== "" && 
    this.currentProblem.problemLabel !== null
  ) {
    problemTitleContainer.appendChild(document.createTextNode(this.currentProblem.problemLabel));
  }
  problemTitleContainer.appendChild(document.createTextNode(this.currentProblem.title));

  var infoBar = document.getElementById(ids.domElements.divProblemInfoBar);
  infoBar.innerHTML = "";
  if (!window.calculator.mainPage.flagProblemPageOnly) {
    infoBar.appendChild(this.currentProblem.getProblemNavigation());
  }

  if (
    this.currentProblem.links !== undefined && 
    this.currentProblem.links !== null
  ) {
    if (this.currentProblem.links.slides !== null) {
      miscellaneousFrontend.appendHtml(problemTitle, this.currentProblem.links.slides);
    }
    if (this.currentProblem.links.video !== null) {
      miscellaneousFrontend.appendHtml(problemTitle, this.currentProblem.links.video);
    }
  }
  //topPart += "<br>"
  problemTitle.appendChild(this.currentProblem.getEditPanel());
  infoBar.appendChild(problemTitle);
  mathjax.typeSetSoft(ids.domElements.divProblemInfoBar);
}

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  outputElement.innerHTML = "";
  problemNavigation.setCurrentProblemAndUpdate(this);
  var contentArray = [];
  miscellaneousFrontend.appendHtmlToArray(contentArray, this.badProblemExplanation);
  var problemBody = document.createElement("span");
  problemBody.innerHTML = this.decodedProblem + this.commentsProblem 
  contentArray.push(problemBody);
  miscellaneousFrontend.appendHtml(outputElement, contentArray);
  for (var counterAnswers = 0;  counterAnswers < this.answers.length; counterAnswers ++) {
    this.onePanel(this.answers[counterAnswers]);
  }
  initializeButtons.initializeAccordionButtons();
  mathjax.typeSetSoft(ids.domElements.problemPageContentContainer);
}

Problem.prototype.toStringDeadline = function() {
  var thePage = window.calculator.mainPage;
  if (thePage.user.hasInstructorRights() && !thePage.studentView()) {
    return "Deadlines";
  }
  if (thePage.user.hasInstructorRights() && thePage.studentView()) {
    var sectionIndex = thePage.storage.variables.currentSectionComputed.getValue();
    this.deadlineString = this.deadlines[sectionIndex];
  }
  if (this.deadlineString === "" || this.deadlineString === null || this.deadlineString === undefined) {
    if (this.parentIdURLed !== null && this.parentIdURLed !== undefined && this.parentIdURLed !== "") {
      var parentProblem = allProblems.getProblemById(this.parentIdURLed);
      var inheritedResult = parentProblem.toStringDeadline();
      if (inheritedResult !== "") {
        if (!inheritedResult.endsWith("[inherited]")) {
          inheritedResult += " [inherited]";
        }
      }
      return inheritedResult;
    }
    return "";
  }
  this.deadline = new Date(this.deadlineString);
  var remainingInHours = (this.deadline - (new Date())) / 1000 / 60 / 60;
  remainingInHours += 24;
  var resultString = "";
  if (this.isSolvedForSure()) {
    resultString += `<b style = 'color:green'>${this.deadline.toLocaleDateString()}</b>`;
    return resultString;
  }
  if (remainingInHours < 48 && remainingInHours >= 0) {
    resultString += `<b style = 'color:red'>${this.deadline.toLocaleDateString()}`;
    resultString += `, ~${remainingInHours.toFixed(1)} hrs left</b>`;
    return resultString;
  }
  if (remainingInHours < 0) {
    resultString += `<b style = 'color:red'>${this.deadline.toLocaleDateString()}</b>`;
    resultString += ` <b>[passed]</b>`;
    return resultString;
  }
  resultString += `<b style>${this.deadline.toLocaleDateString()}</b>`
  return resultString;
//  this.deadlines[];
}

Problem.prototype.toStringDeadlineContainer = function() {
  var result = "";
  result += `<span id = '${this.idDeadlineContainer}' class = '${ids.domElements.classSpanDeadlineContainer}'>${this.toStringDeadlinePanel()}</span>`;
  return result;
}

Problem.prototype.toStringDeadlinePanel = function() {
  var thePage = window.calculator.mainPage;
  if (!thePage.user.hasInstructorRights() || thePage.studentView()) {
    return this.toStringDeadline();
  }
  if (this.type === "Problem" && this.fileName === "") {
    return "";
  }
  var result = "";
  result += `<button class = "accordionLike" `;
  result += `onclick = "window.calculator.coursePage.toggleDeadline('${this.idDeadlinePanel}', '${this.problemId}', this);">`;
  result += `${this.toStringDeadline()} &#9666;</button>`;
  result += `<span class = "panelDeadlines" id = "${this.idDeadlinePanel}">`;
  result += "<table>";
  result += "<tr><th>Grp.</th><th>Deadline</th></tr>";
  for (var counterGroup = 0; counterGroup < thePage.user.sectionsTaught.length; counterGroup ++) {
    result += `<tr><td>${thePage.user.sectionsTaught[counterGroup]}</td>`;
    result += `<td><input class = "datePicker" name = "datePicker${this.problemId}" `;
    if (this.deadlines[counterGroup] !== "" && this.deadlines[counterGroup] !== undefined) {
      result += `value = "${this.deadlines[counterGroup]}"`;
    }
    result += `></input></td></tr>`;
  } 
  result += "</table>";
  //console.log("Problem data problem: " + JSON.stringify(this.fileName));
  //console.log("Problem data title: " + JSON.stringify(this.title));
  result += `<button onclick = "window.calculator.coursePage.modifyDeadlines('${this.problemId}')">Set</button>`;
  result += `<span id = '${this.idModifyReportDeadline}'></span>`;
  result += `</span>`;
  return result;
}

Problem.prototype.toHTMLWeights = function() { 
  var result = "";
  result += "<span class = 'panelProblemWeights' style = 'opacity: 1; max-height: 200px;'>";
  result += `Pts: <textarea class = 'textareaStudentPoints' rows = '1' cols = '2' id = '${this.idTextareaPoints}'>`;
  if (this.weight !== undefined && this.weight !== null) {
    result += this.weight;
  }
  result += "</textarea>";
  result += `<button id = '${this.idButtonPoints}' onclick = "window.calculator.problemPage.modifyWeight('${this.problemId}')">Modify</button><br>`;
  result += `<span id = '${this.idModifyReportPoints}'></span>`;
  result += "</span>";
  return result;
}

Problem.prototype.modifyWeight = function() {
  var problemWeightTextareaId = `points${this.problemId}`;
  var incomingPoints = document.getElementById(problemWeightTextareaId).value;
  var modifyObject = {};
  var idDecoded = decodeURIComponent(this.problemId);
  //var problemModifyWeightReport = `report${id}`;
  modifyObject[idDecoded] = {
    weight: incomingPoints
  };
  var theURL = ""; 
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemData}&`;
  theURL += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(modifyObject))}`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    result: this.idModifyReportPoints,
  });
}

function modifyWeight(id) {
  var theProblemWeight = allProblems.allProblems[id];
  theProblemWeight.modifyWeight();
}

/**@returns{HTMLElement[]} */
Problem.prototype.getProblemWeightContent = function(inputRow) {
  var thePage = window.calculator.mainPage;
  if (this.type !== "Problem" || this.fileName === "") {
    return [];
  }
  var content = document.createElement("span");
  if (!thePage.user.hasInstructorRights() || thePage.studentView()) {
    content.innerHTML = this.toStringProblemWeight();
    return [content];
  }
  var pointsString = ""; 
  pointsString += `<button class = 'accordionLikeProblemWeight' onclick = "window.calculator.coursePage.toggleProblemWeights()" `;
  pointsString += `name = "${this.problemId}">${this.toStringProblemWeight()} &#9666;</button>`;
  var problemWeightString = this.toHTMLWeights();
  content.innerHTML =`${pointsString}<br> ${problemWeightString}`;
  return [content];
}

Problem.prototype.isSolvedForSure = function () {
  if (this.correctlyAnswered === undefined || this.correctlyAnswered === NaN) {
    return false;
  }
  if (
    this.correctlyAnswered >= this.totalQuestions && 
    this.totalQuestions !== undefined &&
    this.totalQuestions !== 0
  ) {
    return true;
  } 
  return false;
}

Problem.prototype.toStringProblemWeight = function() {
  var result = "";
  var color = "brown";
  if (this.correctlyAnswered !== undefined && this.correctlyAnswered !== NaN) {
    if (this.isSolvedForSure()) {
      color = "green";
    }
    var numCorrectlyAnswered = this.correctlyAnswered;
    var totalQuestions = this.totalQuestions;
    if (totalQuestions === 0) {
      totalQuestions = "?";
    }
    result += `${numCorrectlyAnswered} out of ${totalQuestions}`;
    if (this.weight !== undefined && this.totalQuestions !== 0) {
      var problemWeightConverted = parseInt(this.weight);
      var points = ((0.0 + this.correctlyAnswered * problemWeightConverted) / this.totalQuestions);
      points = Number(points.toFixed(2));
      result += ` = ${points} pts`;
      if (this.correctlyAnswered < this.totalQuestions) {
        color = "red";
      } 
    } else {
      result += ` = ? pts`;
    }
  }
  return `<b style = "color:${color}">${result}</b>`;
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

var linkSpecs = {
  slidesProjector: { 
    request: pathnames.urlFields.requests.slidesFromSource,
    name: "Slides",
    extension: ".pdf",
    options: "layout=projector&",
  }, 
  slidesPrintable: {
    request: pathnames.urlFields.requests.slidesFromSource,
    name: "Printable",
    extension: ".pdf",
    options: "layout=printable&",
  }, 
  slidesTex: {
    request: pathnames.urlFields.requests.sourceSlides,
    name: ".tex",
    extension: ".tex",
    options: "",
    download: true,
    adminView: true,
  }, 
  homeworkWithAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    extension: ".pdf",
    name: "HW",
    options: "answerKey=true&",
  }, 
  homeworkNoAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    name: "HW+answ.",
    extension: ".pdf",
    options: "answerKey=false&",
  }, 
  homeworkTex: {
    request: pathnames.urlFields.requests.sourceHomework,
    name: ".tex",
    extension: ".pdf",
    options: "",
    download: true,
    adminView: true,
  },
};

/** @returns{HTMLElement} */
Problem.prototype.getLinkFromSpec = function(
  /**@type {{request: string, name: string, extension: string, options: string, download: boolean}} */
  linkSpec, 
  /**@type {string} */
  query
) {
  if (linkSpec.adminView === true) {
    var studentView = window.calculator.mainPage.storage.variables.flagStudentView.getValue();
    if (studentView !== false && studentView !== "false") {
      return null;
    }
  }
  var href = "";
  href += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${linkSpec.request}&`;
  href += `${query}&${linkSpec.options}`;
  var result = document.createElement("a");
  result.className = "slidesLink";
  result.href = href;
  var extension = linkSpec.extension;
  if (extension === undefined || extension === null) {
    extension = "";
  }
  result.download = `${convertStringToLaTeXFileName(this.title)}${extension}`;
  result.target = "_blank";
  result.innerHTML = linkSpec.name;
  return result;
}

var linkSlides = [
  "slidesProjector",
  "slidesPrintable",
  "slidesTex",
];

var linkHomework = [
  "homeworkWithAnswers",
  "homeworkNoAnswers",
  "homeworkTex",
];

/**@returns {HTMLElement[]} */
Problem.prototype.getProblemMaterialLinks = function() {
  var result = [];
  this.links.slides = [];
  this.links.video = [];
  this.links.homework = [];
  if (this.video !== "" && this.video !== undefined && this.video !== null) {
    var videoLink = document.createElement("a");
    videoLink.classList = "videoLink";
    videoLink.href = this.video;
    videoLink.target = "_blank";
    videoLink.innerHTML = "Video";
    this.links.video.push(videoLink);
  }
  if (this.querySlides !== "" && this.querySlides !== null && this.querySlides !== undefined) {
    for (var counter in linkSlides) {
      this.links.slides.push(this.getLinkFromSpec(linkSpecs[linkSlides[counter]], this.querySlides));
    }
  } 
  miscellaneousFrontend.appendHtmlToArray(result, this.links.slides);
  if (this.queryHomework !== "" && this.queryHomework !== null && this.queryHomework !== undefined) {
    for (var counter in linkHomework) {
      miscellaneousFrontend.appendHtmlToArray(this.links.homework, this.getLinkFromSpec(linkSpecs[linkHomework[counter]], this.queryHomework));
    }
  }
  miscellaneousFrontend.appendHtmlToArray(result, this.links.homework);
  return result;
}

Problem.prototype.getHTMLOneProblemTr = function (
  outputRow
) {
  var thePage = window.calculator.mainPage;
  var nextCell = outputRow.insertCell(- 1);
  nextCell.className = "topicListTitle";
  nextCell.innerHTML = `${this.problemNumberString} ${this.title}`;
  nextCell = outputRow.insertCell(- 1);
  nextCell.className = "topicListMaterials";
  var materialLinks = this.getProblemMaterialLinks();
  miscellaneousFrontend.appendHtml(nextCell, materialLinks);
  nextCell = outputRow.insertCell(- 1);
  nextCell.className = "topicListPracticeQuiz";
  if (this.fileName !== "") {
    if (thePage.user.flagLoggedIn) {
      var nextElement = document.createElement("a");
      nextElement.className = "problemLinkQuiz";
      nextElement.href = `#${this.getAppAnchorRequestFileCourseTopics(true)}`;
      nextElement.addEventListener(
        "click", 
        window.calculator.problemPage.selectCurrentProblem.bind(
          null, 
          this.problemId, 
          "scoredQuizJSON",
        ),
      );
      nextElement.innerHTML = "Quiz";
      nextCell.appendChild(nextElement);
    }
    var nextElement = document.createElement("a");
    nextElement.className = "problemLinkPractice";
    nextElement.href = `#${this.getAppAnchorRequestFileCourseTopics(false)}`;
    nextElement.innerHTML = "Practice";
    nextElement.addEventListener("click", window.calculator.problemPage.selectCurrentProblem.bind(null, this.problemId, "exerciseJSON"));
    nextCell.appendChild(nextElement);
  }
  nextCell = outputRow.insertCell(- 1);
  nextCell.className = "topicListProblemWeight";
  var weightContent = this.getProblemWeightContent();
  miscellaneousFrontend.appendHtml(nextCell, weightContent)
  nextCell = outputRow.insertCell(- 1);
  nextCell.className = "topicListDeadlines";
  nextCell.innerHTML = this.toStringDeadlineContainer();
}

/**@returns{HTMLElement[]} */
Problem.prototype.getHTMLProblems = function () {
  var nextElement = document.createElement("div");
  nextElement.className = "bodySubsection";
  var table = document.createElement("table");
  nextElement.appendChild(table);
  table.className = "topicList";  
  for (var counterSubSection = 0; counterSubSection < this.childrenIds.length; counterSubSection ++) {
    var currentProblem = allProblems.getProblemById(this.childrenIds[counterSubSection]);
    var row = table.insertRow(- 1);
    currentProblem.getHTMLOneProblemTr(row);
  }
  return [nextElement];
}

/**@returns{HTMLElement[]} */
Problem.prototype.getHTMLSubSection = function() {
  var result = [];
  var nextElement = document.createElement("div");
  nextElement.className = "headSubsection";
  nextElement.innerHTML = `${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}`;
  result.push(nextElement);
  miscellaneousFrontend.appendHtmlToArray(result, this.getHTMLProblems());
  return result;  
}

Problem.prototype.isProblemContainer = function() {
  if (this.childrenIds.length !== undefined) {
    if (this.childrenIds.length > 0) {
      var currentProblem = allProblems.getProblemById(this.childrenIds[0]);
      if (currentProblem.type === "Problem") {
        return true;
      }
    }
  }
  return false;
}

/** @returns{HTMLElement[]} */
Problem.prototype.getHTMLSection = function() {
  var result = [];
  if (this.type === "Section") {
    var sectionElement = document.createElement("div");
    sectionElement.className = "headSection";
    sectionElement.innerHTML = `${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}`; 
    result.push(sectionElement);
  }
  var nextElement = document.createElement("div");
  nextElement.className = "bodySection";
  result.push(nextElement);
  if (this.type === "Topic") {
    miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
  } else if (this.isProblemContainer()) {
    miscellaneousFrontend.appendHtml(nextElement, this.getHTMLProblems());
  } else if (this.type === "Section") {
    for (var counterSection = 0; counterSection < this.childrenIds.length; counterSection ++) {
      var currentSubSection = allProblems.getProblemById(this.childrenIds[counterSection]);
      miscellaneousFrontend.appendHtml(nextElement, currentSubSection.getHTMLSubSection());
    }
  } else {
    miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
  }
  return result;  
}

/**@returns{HTMLElement[]} */
Problem.prototype.toHTMLChapter =  function() {
  var result = [];
  var headChapterElement = document.createElement("div");
  headChapterElement.className = "headChapter";
  headChapterElement.innerHTML = `${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}`
  result.push(headChapterElement);
  var bodyChapterElement = document.createElement("div");
  bodyChapterElement.className = "bodyChapter";

  if (this.isProblemContainer()) {
    var incomingProblems = this.getHTMLProblems();
    for (var i = 0; i < incomingProblems.length; i ++) {
      bodyChapterElement.appendChild(incomingProblems[i]);
    }
  } else {
    for (var counterSection = 0; counterSection < this.childrenIds.length; counterSection ++) {
      var currentSection = allProblems.getProblemById(this.childrenIds[counterSection]);
      miscellaneousFrontend.appendHtml(bodyChapterElement, currentSection.getHTMLSection());
    }
  }
  result.push(bodyChapterElement); 
  return result;
}

/**@returns{HTMLElement[]} */
function getHTMLfromTopics() {
  var result = [];
  for (var label in allProblems.theChapterIds) {
    var currentProblem = allProblems.getProblemById(label); 
    result.push(currentProblem.toHTMLChapter());
  }
  if (allProblems.theTopics.comments !== undefined && allProblems.theTopics.comments !== null) {
    var comments = document.createElement("span");
    comments.innerHTML = "<hr>" + allProblems.theTopics.comments;
    result.push(comments);
  }
  
  return result;
}

function initializeDatePickers() {
  var thePickers = document.getElementsByClassName("datePicker");
  for (var counterPicker = 0; counterPicker < thePickers.length; counterPicker ++) {
    $(thePickers[counterPicker]).datepicker();
  }
}

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

function writeEditCoursePagePanel() {
  var thePage = window.calculator.mainPage;
  var panel = document.getElementById(ids.domElements.courseEditPanel);
  panel.innerHTML = "";
  var courseHome = thePage.storage.variables.currentCourse.courseHome.getValue()
  panel.appendChild(editPage.getEditPanel(courseHome));
  var topicList = thePage.storage.variables.currentCourse.topicList.getValue();
  panel.appendChild(editPage.getEditPanel(topicList));
  if (
    allProblems.theTopics.topicBundleFile !== undefined && 
    allProblems.theTopics.topicBundleFile !== null &&
    allProblems.theTopics.topicBundleFile !== ""
  ) {
    for (
      var counterTopicBundle = 0; 
      counterTopicBundle < allProblems.theTopics.topicBundleFile.length; 
      counterTopicBundle ++
    ) {
      var nextToEdit = allProblems.theTopics.topicBundleFile[counterTopicBundle];
      panel.appendChild(editPage.getEditPanel(nextToEdit));
    }
  }
}

function processLoadedTopics(incomingTopics, result) {
  allProblems.previousProblemId = null;
  allProblems.resetTopicProblems();
  allProblems.theTopics = miscellaneous.jsonUnescapeParse(incomingTopics);
  for (var counterChapter = 0; counterChapter < allProblems.theTopics["children"].length; counterChapter ++) {
    var currentChapter = allProblems.theTopics["children"][counterChapter];
    allProblems.CreateOrUpdateProblem(currentChapter);
  }
}

function processLoadedTopicsWriteToEditPage(incomingTopics, result) {
  processLoadedTopics(incomingTopics, result);  
  editPage.selectEditPage(null);
}

function processLoadedTopicsWriteToCoursePage(incomingTopics, result) {
  processLoadedTopics(incomingTopics, result);  
  writeTopicsToCoursePage();
}

function writeTopicsToCoursePage() {
  var thePage = window.calculator.mainPage;
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0) {
    return;
  }
  writeEditCoursePagePanel();
  var htmlContentElements = getHTMLfromTopics();
  miscellaneousFrontend.appendHtml(topicsElements[0], htmlContentElements);
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  //mathjax.typeSetHard(topicsElements[0]);
  if (thePage.pages.problemPage.flagLoaded) {
    problemNavigation.writeToHTML();
  }
  mathjax.typeSetSoft(topicsElements[0]);

}

function updateProblemPageCallback(input, outputComponent) {
  var thePage = window.calculator.mainPage;
  if (typeof outputComponent === "string" || outputComponent === undefined || outputComponent === null) {
    outputComponent = document.getElementById(outputComponent);
  }
  if (outputComponent === null || outputComponent === undefined) {
    outputComponent = document.getElementById(ids.domElements.problemPageContentContainer);
  }
  var theProblem = null;
  try {
    theProblem = miscellaneous.jsonUnescapeParse(input);
  } catch (e) {
    outputComponent.innerHTML = `Error parsing: ${e}. Failed to parse: ${input}`;
    thePage.cleanUpLoginSpan(outputComponent);
    return;
  }
  if (theProblem.crashReport !== undefined && theProblem.crashReport !== null) {
    outputComponent.innerHTML = theProblem.crashReport;
    return;
  }
  /**@type {Problem} */
  var currentProblem = thePage.getCurrentProblem();
  if (currentProblem === null || currentProblem === undefined) {
    var problemId = thePage.storage.variables.currentCourse.currentProblemId.getValue();
    allProblems.registerEmptyProblem(problemId)
    currentProblem = thePage.getCurrentProblem();
  }
  currentProblem.initializeProblemContent(theProblem);
}

function updateProblemPage() {
  setTimeout(() => { 
    window.calculator.coursePage.selectCurrentCoursePage();
  }, 0);
  var thePage = window.calculator.mainPage;
  // thePage.pages.problemPage.flagLoaded is modified by the following 
  // functions: selectCurrentProblem, logout, callbackClone,
  // the present function updateProblemPage
  /**@type {Problem} */
  var theProblem = thePage.getCurrentProblem();
  if (thePage.pages.problemPage.flagLoaded) {
    if (theProblem !== undefined && theProblem !== null) {
      var problemNavigation = document.getElementById(theProblem.idNavigationProblemNotEntirePanel);
      if (problemNavigation !== null) {
        problemNavigation.innerHTML = "";
        var updatedContent = theProblem.getProblemNavigationContent();        
        for (var i = 0; i < updatedContent.length; i ++) {
          problemNavigation.appendChild(updatedContent[i]);
        } 
      }
    }
    return;
  }
  var theURL;
  if (theProblem !== undefined && theProblem !== null) { 
    theURL = `${pathnames.urls.calculatorAPI}?${theProblem.getCalculatorURLRequestFileCourseTopics(theProblem.flagForReal)}`;
  } else {
    var fileName = thePage.storage.variables.currentCourse.fileName.getValue();
    if (fileName === "" || fileName === undefined || fileName === null) {
      var courseBody = document.getElementById(ids.domElements.problemPageContentContainer); 
      var temporarySelectProblem = "buttonTemporarySelectProblem";
      var selectProblemHtml = "";
      selectProblemHtml += `Problems are selected from the <button id = '${temporarySelectProblem}'`;
      selectProblemHtml += `class = "buttonSelectPage buttonSlowTransition buttonFlash" style = "width:150px"`; 
      selectProblemHtml += `onclick = "window.calculator.mainPage.selectPage('currentCourse')">Current course</button>`;
      selectProblemHtml += "<br>To select a problem, click Practice or Quiz within the course page. ";
      courseBody.innerHTML = selectProblemHtml;
      setTimeout(() => {
        document.getElementById(temporarySelectProblem).classList.remove("buttonFlash");
      }, 100);
      return; 
    } 
    theURL = getCalculatorURLRequestFileCourseTopicsFromStorage();
  }
  thePage.pages.problemPage.flagLoaded = true;
  //console.log("Current course: " + JSON.stringify(thePage.storage.variables.currentCourse));
  submitRequests.submitGET({
    url: theURL,
    callback: updateProblemPageCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

var problemNavigation = new ProblemNavigation();
var allProblems = new ProblemCollection();

module.exports = {
  Problem,
  allProblems,
  problemNavigation,
  updateProblemPage,
  processLoadedTopicsWriteToCoursePage,
  processLoadedTopicsWriteToEditPage,
  writeEditCoursePagePanel,
  selectCurrentProblem,
  modifyWeight,
};