"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements');
const editPage = require('./edit_page');
const initializeButtons = require('./initialize_buttons');
const InputPanelData = initializeButtons.InputPanelData;
const mathjax = require('./mathjax-calculator-setup');

function selectCurrentProblem(problemIdURLed, exerciseType) {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.currentCourse.currentProblemId.setAndStore(problemIdURLed);
  thePage.storage.variables.currentCourse.fileName.setAndStore(decodeURIComponent(problemIdURLed));
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

}

Problem.prototype.initializeBasic = function(problemData) {
  this.problemId = encodeURIComponent(problemData.id);
  /**@type {InputPanelData[]} */
  this.answers = [];  
  this.fileName = problemData.fileName;
  if (this.fileName === null || this.fileName === undefined) {
    this.fileName = "";
  }
  this.title = problemData.title;
}

Problem.prototype.initializeInfo = function(problemData, inputParentIdURLed) {
  var thePage = window.calculator.mainPage;
  this.initializeBasic(problemData);
  this.decodedProblem = "";
  this.commentsProblem = "";
  this.parentIdURLed = inputParentIdURLed;
  this.randomSeed = null;
  this.problemLabel = "";
  this.flagForReal = true;
  this.previousProblemId = null;
  this.nextProblemId = null;
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
    video: "",
    slides: "",
    homework: "",
  };
  this.badProblemString = "";
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
      var previousProblem = thePage.problems[this.previousProblemId]; 
      previousProblem.nextProblemId = this.problemId;
    }
    thePage.previousProblemId = this.problemId;
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

  thePage.problems[this.problemId] = this;
  if (this.type === "chapter") {
    thePage.theChapterIds[this.problemId] = true;
  }
  this.childrenIds = [];
  if (Array.isArray(problemData.children)) {
    for (var counterChildren = 0; counterChildren < problemData.children.length; counterChildren ++) {
      var currentChild = new Problem();
      currentChild.initializeInfo(problemData.children[counterChildren], this.problemId);
      this.childrenIds.push(currentChild.problemId);
    }
  }
}

Problem.prototype.computeBadProblemString = function() {
  var thePage = window.calculator.mainPage;
  if (!this.decodedProblem.includes(pathnames.urlFields.problem.failedToLoadProblem)) {
    thePage.lastKnownGoodProblemFileName = this.fileName;
    this.badProblemString = "";
    return;
  }
  this.badProblemString = "";
  this.badProblemString += "It appears your problem failed to load.<br>";

  if (this.lastKnownGoodProblemFileName !== "" && thePage.user.hasInstructorRights()) {
    this.badProblemString += "Perhaps you may like to clone the last good known problem.<br>";
  }
  this.badProblemString += editPage.getClonePanel(thePage.lastKnownGoodProblemFileName, this.fileName);
  this.badProblemString += "<hr>";
}

Problem.prototype.initializeProblemContent = function(problemData) {
  this.initializeBasic(problemData)
  var thePage = window.calculator.mainPage;
  this.decodedProblem = decodeURIComponent(problemData["problemContent"]);
  this.commentsProblem = problemData["commentsProblem"];
  if (this.commentsProblem === undefined) {
    this.commentsProblem = "";
  }
  this.computeBadProblemString();
  var answerVectors = problemData["answers"];
  if (answerVectors === undefined) {
    this.writeToHTML(ids.domElements.problemPageContentContainer);
    return;    
  }
  this.flagForReal = problemData["forReal"];
  this.randomSeed = problemData.randomSeed;
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
  var problemLinkWithRandomSeed = document.getElementById(ids.domElements.spanProblemLinkWithRandomSeed);
  if (problemLinkWithRandomSeed !== null) {
    problemLinkWithRandomSeed.innerHTML = `<a href = '#${this.getAppAnchorRequestFileCourseTopics()}'>#${this.randomSeed}</a>`;
  }
  this.scriptIds = [];
  for (var scriptLabel in problemData.scripts) {
    var newLabel = encodeURIComponent(this.problemId + scriptLabel);
    this.scriptIds.push(newLabel); 
    thePage.injectScript(newLabel, decodeURIComponent(problemData.scripts[scriptLabel]));
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
  var randomSeed = currentCourse.randomSeed.getValue();
  var result = "";
  result += `${pathnames.urls.calculatorAPI}?`;
  result += `${pathnames.urlFields.request}=${exerciseType}&fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}&`;
  if (randomSeed !== null && randomSeed !== "" && randomSeed !== undefined) {
    result += `randomSeed=${randomSeed}&`;
  }
  return result;
}

Problem.prototype.getAppAnchorRequestFileCourseTopics = function(isScoredQuiz) {
  var thePage = window.calculator.mainPage;
  var theExerciseType = pathnames.urlFields.exerciseJSON;
  if (isScoredQuiz) {
    theExerciseType = pathnames.urlFields.scoredQuizJSON;
  }
  var requestJSON = {
    currentPage: thePage.pages.problemPage.name,
    exerciseType: theExerciseType,
    fileName: this.fileName,
    courseHome: thePage.storage.variables.currentCourse.courseHome.getValue(),
    topicList: thePage.storage.variables.currentCourse.topicList.getValue(),
  };
  if (this.randomSeed !== "" && this.randomSeed !== null && this.randomSeed !== undefined) {
    requestJSON["randomSeed"] = this.randomSeed;
  }
  var thePage = window.calculator.mainPage;
  var stringifiedHash = thePage.storage.getCleanedUpURL(requestJSON);
  return stringifiedHash;
}

Problem.prototype.getCalculatorURLFileCourseTopics = function() {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += `fileName=${this.fileName}&`;
  result += `courseHome=${thePage.storage.variables.currentCourse.courseHome.getValue()}&`;
  result += `topicList=${thePage.storage.variables.currentCourse.topicList.getValue()}&`;
  return result;
}

Problem.prototype.getCalculatorURLRequestFileCourseTopics = function(isScoredQuiz) {
  var result = "";
  if (isScoredQuiz === undefined) {
    isScoredQuiz = this.flagForReal;
  }
  result += `${pathnames.urlFields.request}=`;
  if (isScoredQuiz) {
    result += pathnames.urlFields.scoredQuizJSON;
  } else {
    result += pathnames.urlFields.exerciseJSON;
  }
  result += `&${this.getCalculatorURLFileCourseTopics()}`;
  return result;
}

Problem.prototype.getProblemNavigation = function() {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += `<div class = 'problemNavigation'>`;
  var linkType = "problemLinkPractice";
  var defaultRequest = pathnames.urlFields.exerciseJSON;
  if (this.flagForReal && thePage.user.flagLoggedIn) {
    defaultRequest = pathnames.urlFields.scoredQuizJSON;
    linkType = "problemLinkQuiz"
  }
  if (this.previousProblemId !== null && this.previousProblemId !== "" && this.previousProblemId !== undefined) {
    var previousProblem = thePage.problems[this.previousProblemId]; 
    var previousURL = previousProblem.getAppAnchorRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${previousURL}' `;
    result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.previousProblemId}', '${defaultRequest}')">&#8592;</a>`;
  }

  if (this.flagForReal && thePage.user.flagLoggedIn) {
    result += `<a class = 'problemLinkPractice' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.problemId}' ,'exerciseJSON')">Practice</a>`;
  } else {
    result += "<span class = 'problemLinkSelectedPractice' style='color:green'>Practice</span>";
  }
  if (!this.flagForReal && thePage.user.flagLoggedIn) { 
    result += `<a class = 'problemLinkQuiz' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.problemId}' ,'scoredQuizJSON')">Quiz</a>`;
  } else {
    if (this.flagForReal) {
      result += "<span class = 'problemLinkSelectedQuiz' style='color:brown'>Quiz</span>";
    }
  }
  if (this.nextProblemId !== null && this.nextProblemId !== "" && this.nextProblemId !== undefined) {
    var nextProblem = thePage.problems[this.nextProblemId]; 
    var nextURL = nextProblem.getAppAnchorRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${nextURL}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.nextProblemId}', '${defaultRequest}')">&#8594;</a>`;
  }
  
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    result += `<b style = 'color:green'>Scores not recorded. </b> <span id = "${ids.domElements.spanProblemLinkWithRandomSeed}"></span>`;
  } else {
    result += `<b style = 'color:brown'>Scores are recorded. </b>`;
  }
  result += "</div>";
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

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  var topPart = "";
  topPart += "<div class = 'problemInfoBar'>";
  topPart += this.getProblemNavigation();
  topPart += `<div class = "problemTitle"><div class = "problemTitleContainer">`;
  if (this.problemLabel !== undefined && this.problemLabel !== "" && this.problemLabel !== null) {
    topPart += `${this.problemLabel} `; 
  }
  topPart += `${this.title}</div></div>`;
  if (this.links !== undefined && this.links !== null) {
    topPart += this.links.slides;
    topPart += this.links.video;
  }
  //topPart += "<br>"
  topPart += this.getEditPanel();
  topPart += "</div>";
  topPart += "<br>";
  outputElement.innerHTML = topPart + this.badProblemString + this.decodedProblem + this.commentsProblem;
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
      var parentProblem = thePage.problems[this.parentIdURLed];
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
  if (this.type === "problem" && this.fileName === "") {
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

Problem.prototype.callbackModifyWeight = function(input, output) {
  document.getElementById(this.idModifyReportPoints).innerHTML = input;
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
    callback: this.callbackModifyWeight.bind(this)
  });
}

function modifyWeight(id) {
  var thePage = window.calculator.mainPage;
  /**@type {Problem} */
  var theProblemWeight = thePage.problems[id];
  theProblemWeight.modifyWeight();
}

Problem.prototype.toStringProblemWeightCell = function() {
  var thePage = window.calculator.mainPage;
  var result = "";
  if (this.type !== "problem" || this.fileName === "") {
    return "<td></td>";
  }
  if (!thePage.user.hasInstructorRights() || thePage.studentView()) {
    return `<td>${this.toStringProblemWeight()}</td>`;
  }
  var pointsString = ""; 
  pointsString += `<button class = 'accordionLikeProblemWeight' onclick = "window.calculator.coursePage.toggleProblemWeights()" `;
  pointsString += `name = "${this.problemId}">${this.toStringProblemWeight()} &#9666;</button>`;
  var problemWeightString = this.toHTMLWeights();
  result += `<td>${pointsString}<br> ${problemWeightString}</td>`;
  return result;
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
    options: "layout=projector&",
  }, 
  slidesPrintable: {
    request: pathnames.urlFields.requests.slidesFromSource,
    name: "Printable",
    options: "layout=printable&",
  }, 
  slidesTex: {
    request: pathnames.urlFields.requests.sourceSlides,
    name: ".tex",
    options: "",
    download: true,
    adminView: true,
  }, 
  homeworkWithAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    name: "HW",
    options: "answerKey=true&",
  }, 
  homeworkNoAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    name: "HW+answ.",
    options: "answerKey=false&",
  }, 
  homeworkTex: {
    request: pathnames.urlFields.requests.sourceHomework,
    name: ".tex",
    options: "",
    download: true,
    adminView: true,
  },
};

Problem.prototype.getLinkFromSpec = function(
  /**@type {{request: string, name: string, options: string, download: boolean}} */
  linkSpec, 
  /**@type {string} */
  query
) {
  if (linkSpec.adminView === true) {
    var studentView = window.calculator.mainPage.storage.variables.flagStudentView.getValue();
    if (studentView !== false && studentView !== "false") {
      return "";
    }
  }
  var href  = "";
  href += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${linkSpec.request}&`;
  href += `${query}&${linkSpec.options}`;
  var result = "";
  result += `<a class = 'slidesLink' href = '${href}' `;
  if (linkSpec.download === true) {
    result += `download = '${convertStringToLaTeXFileName(this.title)}.tex'`; 
  }
  result += ``
  result += `target = '_blank'>${linkSpec.name}</a>`;
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

Problem.prototype.getHTMLOneProblemTr = function () {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += "<tr>";
  result += `<td>${this.problemNumberString} ${this.title}</td>`;
  result += "<td>";
  if (this.video !== "" && this.video !== undefined && this.video !== null) {
    result += `<a class = 'videoLink' href = '${this.video}' target = '_blank'>Video</a>`;
  }
  this.links.slides = "";
  this.links.video = "";
  this.links.homework = "";
  if (this.querySlides !== "" && this.querySlides !== null && this.querySlides !== undefined) {
    for (var counter in linkSlides) {
      this.links.slides += this.getLinkFromSpec(linkSpecs[linkSlides[counter]], this.querySlides);
    }
  } 
  result += this.links.slides;
  if (this.queryHomework !== "" && this.queryHomework !== null && this.queryHomework !== undefined) {
    for (var counter in linkHomework) {
      this.links.homework += this.getLinkFromSpec(linkSpecs[linkHomework[counter]], this.queryHomework);
    }
  }
  result += this.links.homework;
  result += "</td>";
  result += "<td>";
  if (this.fileName !== "") {
    if (thePage.user.flagLoggedIn) {
      result += `<a class = "problemLinkQuiz" href = '#${this.getAppAnchorRequestFileCourseTopics(true)}' `; 
      result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.problemId}', 'scoredQuizJSON');">Quiz</a>`;
    }
    result += `<a class = "problemLinkPractice" href = '#${this.getAppAnchorRequestFileCourseTopics(false)}' `;
    result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.problemId}', 'exerciseJSON');">Practice</a>`;
  }
  result += "</td>";

  result += this.toStringProblemWeightCell();
  result += `<td>${this.toStringDeadlineContainer()}</td>`;
  result += "</tr>";
  return result;
}

Problem.prototype.getHTMLProblems = function () {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += `<div class = "bodySubsection">`;
  result += `<table class = "topicList"><colgroup><col><col><col><col><col></colgroup>`;
  for (var counterSubSection = 0; counterSubSection < this.childrenIds.length; counterSubSection ++) {
    var currentProblem = thePage.problems[this.childrenIds[counterSubSection]];
    result += currentProblem.getHTMLOneProblemTr();
  }
  result += "</table>";
  result += "</div>";
  return result;
}

Problem.prototype.getHTMLSubSection = function() {
  var result = "";
  result += `<div class = "headSubsection">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;
  result += this.getHTMLProblems();
  return result;  
}

Problem.prototype.isProblemContainer = function() {
  var thePage = window.calculator.mainPage;
  if (this.childrenIds.length !== undefined) {
    if (this.childrenIds.length > 0) {
      if (thePage.problems[this.childrenIds[0]].type === "problem") {
        return true;
      }
    }
  }
  return false;
}

Problem.prototype.getHTMLSection = function() {
  var thePage = window.calculator.mainPage;
  var result = "";
  if (this.type === "section") {
    result += `<div class = "headSection">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;    
  }
  result += `<div class = "bodySection">`;
  if (this.type === "subSection") {
    result += this.getHTMLSubSection();
  } else if (this.isProblemContainer()) {
    result += this.getHTMLProblems();
  } else if (this.type === "section") {
    for (var counterSection = 0; counterSection < this.childrenIds.length; counterSection ++) {
      var currentSubSection = thePage.problems[this.childrenIds[counterSection]];
      result += currentSubSection.getHTMLSubSection();
    }
  } else {
    result += this.getHTMLSubSection();
  }
  result += "</div>";
  return result;  
}

Problem.prototype.toHTMLChapter =  function() {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += `<div class = "headChapter">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;
  result += `<div class = "bodyChapter">`;
  if (this.isProblemContainer()) {
    result += this.getHTMLProblems();
  } else {
    for (var counterSection = 0; counterSection < this.childrenIds.length; counterSection ++) {
      var currentSection = thePage.problems[this.childrenIds[counterSection]];
      result += currentSection.getHTMLSection();
    }
  }
  result += "</div>";
  return result;
}

function getHTMLfromTopics() {
  var thePage = window.calculator.mainPage;
  var result = "";
  for (var label in thePage.theChapterIds) {
    result += thePage.problems[label].toHTMLChapter();
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
  var thePanel = "";
  var thePage = window.calculator.mainPage;
  thePanel += editPage.getEditPanel(thePage.storage.variables.currentCourse.courseHome.getValue());
  thePanel += editPage.getEditPanel(thePage.storage.variables.currentCourse.topicList.getValue());
  if (
    thePage.theTopics.topicBundleFile !== undefined && 
    thePage.theTopics.topicBundleFile !== null &&
    thePage.theTopics.topicBundleFile !== ""
  ) {
    for (var counterTopicBundle = 0; counterTopicBundle < thePage.theTopics.topicBundleFile.length; counterTopicBundle ++) {
      thePanel += editPage.getEditPanel(thePage.theTopics.topicBundleFile[counterTopicBundle]);
    }
  }
  document.getElementById(ids.domElements.courseEditPanel).innerHTML = thePanel;
}

function afterLoadTopics(incomingTopics, result) {
  var thePage = window.calculator.mainPage;
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0) {
    return;
  }
  thePage.previousProblemId = null;
  var stringHTMLContent = "";
  thePage.resetProblems();
  thePage.theTopics = JSON.parse(incomingTopics);
  for (var counterChapter = 0; counterChapter < thePage.theTopics["children"].length; counterChapter ++) {
    var currentChapter = thePage.theTopics["children"][counterChapter];
    var incomingProblem = new Problem();
    incomingProblem.initializeInfo(currentChapter, null);
  }
  stringHTMLContent += getHTMLfromTopics();
  writeEditCoursePagePanel();
  topicsElements[0].innerHTML = stringHTMLContent;
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  thePage.previousProblemId = null;
  //mathjax.typeSetHard(topicsElements[0]);
  mathjax.typeSetSoft(topicsElements[0]);
  //MathJax.Hub.queue.pending = 0;
  //MathJax.Hub.Typeset(ids.domElements.divCurrentCourseBody);
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
    theProblem = JSON.parse(input);
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
    thePage.problems[thePage.storage.variables.currentCourse.currentProblemId.getValue()] = new Problem();
    currentProblem = thePage.getCurrentProblem();
  }
  currentProblem.initializeProblemContent(theProblem);
}

function updateProblemPage() {
  var thePage = window.calculator.mainPage;
  if (thePage.pages.problemPage.flagLoaded) {
    return;
  }
  var theProblem = thePage.getCurrentProblem();
  var theURL;
  if (theProblem !== undefined && theProblem !== null) { 
    theURL = `${pathnames.urls.calculatorAPI}?${theProblem.getCalculatorURLRequestFileCourseTopics()}`;
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

module.exports = {
  updateProblemPage,
  afterLoadTopics,
  writeEditCoursePagePanel,
  selectCurrentProblem,
  modifyWeight,
};