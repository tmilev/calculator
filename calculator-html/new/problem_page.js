"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements');
const editPage = require('./edit_page');
const initializeButtons = require('./initialize_buttons');

function selectCurrentProblem(problemIdURLed, exerciseType) {
  var thePage = window.calculator.mainPage;
  thePage.storage.currentCourse.currentProblemId.setAndStore(problemIdURLed);
  thePage.storage.currentCourse.fileName.setAndStore(decodeURIComponent(problemIdURLed));
  thePage.storage.currentCourse.exerciseType.setAndStore(exerciseType);
  var theProblem = thePage.getCurrentProblem();
  theProblem.flagForReal = false;
  if (exerciseType === pathnames.urlFields.scoredQuizJSON) {
    theProblem.flagForReal = true;
  }
  //thePage.storage.currentCourse.request = exerciseType;
  thePage.pages.problemPage.flagLoaded = false;
  thePage.selectPage(thePage.pages.problemPage.name);
}

function Problem(problemData, inputParentIdURLed) {
  var thePage = window.calculator.mainPage;
  this.idURLed = encodeURIComponent(problemData.id);
  //if (inputParentIdURLed === undefined) {
  //  console.log("DEBUG: bad parent id");
  //}
  this.parentIdURLed = inputParentIdURLed;
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
  this.idDeadlineContainer = `${ids.stringResources.prefixDeadlineContainer}${this.idURLed}`;
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
    var currentChild = new Problem(problemData.children[counterChildren], this.idURLed);
    this.childrenIds.push(currentChild.idURLed);
  }
}

function getCalculatorURLRequestFileCourseTopicsFromStorage() {
  var thePage = window.calculator.mainPage;
  var currentCourse = thePage.storage.currentCourse; 
  var exerciseType = currentCourse.exerciseType.getValue();
  if (exerciseType === "" || exerciseType === null || exerciseType === undefined) {
    exerciseType = pathnames.urlFields.exerciseJSON;
    currentCourse.exerciseType.setAndStore(exerciseType);
  }
  var fileName = currentCourse.fileName.getValue();
  var topicList = currentCourse.topicList.getValue();
  var courseHome = currentCourse.courseHome.getValue();
  var result = "";
  result += `${pathnames.urls.calculatorAPI}?`;
  result += `request=${exerciseType}&fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}`;
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
    courseHome: thePage.storage.currentCourse.courseHome.getValue(),
    topicList: thePage.storage.currentCourse.topicList.getValue()
  };
  return encodeURIComponent(JSON.stringify(requestJSON));
}

Problem.prototype.getCalculatorURLFileCourseTopics = function() {
  var thePage = window.calculator.mainPage;
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
  result += `${pathnames.urlFields.request}=`;
  if (isScoredQuiz) {
    result += pathnames.urlFields.scoredQuizJSON;
  } else {
    result += pathnames.urlFields.exerciseJSON;
  }
  result += `&${this.getCalculatorURLFileCourseTopics()}`;
  return result;
}

Problem.prototype.getURLRandomSeedRequestFileCourseTopics = function(isScoredQuiz) {
  var result = this.getAppAnchorRequestFileCourseTopics(isScoredQuiz);
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
  var thePage = window.calculator.mainPage;
  var result = "";
  result += "<span class='problemNavigation'>";
  var linkType = "problemLinkPractice";
  var defaultRequest = pathnames.urlFields.exerciseJSON;
  if (this.flagForReal && thePage.user.flagLoggedIn) {
    defaultRequest = pathnames.urlFields.scoredQuizJSON;
    linkType = "problemLinkQuiz"
  }
  if (this.previousProblemId !== null && this.previousProblemId !== "") {
    var previousURL = thePage.problems[this.previousProblemId].getAppAnchorRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${previousURL}' `;
    result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.previousProblemId}', '${defaultRequest}')">&#8592;</a>`;
  }

  if (this.flagForReal && thePage.user.flagLoggedIn) {
    result += `<a class = 'problemLinkPractice' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.idURLed}' ,'exerciseJSON')">Practice</a>`;
  } else {
    result += "<span class = 'problemLinkSelectedPractice' style='color:green'>Practice</span>";
  }
  if (!this.flagForReal && thePage.user.flagLoggedIn) { 
    result += `<a class = 'problemLinkQuiz' href = '#${this.getAppAnchorRequestFileCourseTopics()}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.idURLed}' ,'scoredQuizJSON')">Quiz</a>`;
  } else {
    if (this.flagForReal) {
      result += "<span class = 'problemLinkSelectedQuiz' style='color:brown'>Quiz</span>";
    }
  }
  if (this.nextProblemId !== null && this.nextProblemId !== "") {
    var nextURL = thePage.problems[this.nextProblemId].getAppAnchorRequestFileCourseTopics();
    result += `<a class = '${linkType}' href = '#${nextURL}' onclick = "window.calculator.problemPage.selectCurrentProblem('${this.nextProblemId}', '${defaultRequest}')">&#8594;</a>`;
  }
  if (this.flagForReal !== true && this.flagForReal !== "true") {
    result += `<b style = 'color:green'>Scores not recorded. </b>`;
  } else {
    result += `<b style = 'color:brown'>Scores are recorded. </b>`;
  }
  result += "</span>";
  return result;
}

Problem.prototype.getEditPanel = function() {
  return editPage.getEditPanel(decodeURIComponent(this.idURLed));
}

function toggleClonePanel(button) {
  var thePanel = button.nextElementSibling; 
  if (thePanel.style.maxHeight === '200px') {
    thePanel.style.opacity = '0';
    thePanel.style.maxHeight = '0';
    button.innerHTML = `Clone panel &#9666;`;
  } else {
    thePanel.style.opacity = '1';
    thePanel.style.maxHeight = '200px';
    button.innerHTML = `Clone panel &#9660;`;
  }

}

Problem.prototype.writeToHTML = function(outputElement) {
  if (typeof outputElement === "string") {
    outputElement = document.getElementById(outputElement);
  }
  var topPart = "";
  topPart += "<div class = 'problemInfoBar'>";
  topPart += this.getProblemNavigation();
  topPart += `<span class = "problemTitle">${this.problemLabel} ${this.title}</span>`;
  //topPart += "<br>"
  topPart += this.getEditPanel();
  topPart += "</div>";
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
      'click', currentAnswerPanel.submitAnswers.bind(currentAnswerPanel)
    );
    if (!this.flagForReal) {
      document.getElementById(currentAnswerPanel.idButtonAnswer).addEventListener(
        'click', currentAnswerPanel.submitGiveUp.bind(currentAnswerPanel)
      );
    }
    if (
      currentAnswerPanel.idButtonSolution !== undefined && 
      currentAnswerPanel.idButtonSolution !== null &&
      currentAnswerPanel.idButtonSolution !== ""
    ) {
      var theSolutionButton = document.getElementById(currentAnswerPanel.idButtonSolution);
      if (theSolutionButton !== null) {
        theSolutionButton.addEventListener(
          'click', currentAnswerPanel.showSolution.bind(currentAnswerPanel)
        );
      }
    }
    
    //theElement.addEventListener('onchange', latexChangeHandler);
    //theElement.attributes.onkeyup = latexChangeHandler;
    //console.log(theElement);
    //console.log(latexChangeHandler);
    currentAnswerPanel.initialize();

  }

  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divProblemPageContentContainer")]);
  initializeButtons.initializeAccordionButtons();
}

Problem.prototype.toStringDeadline = function() {
  var thePage = window.calculator.mainPage;
  if (thePage.user.hasInstructorRights() && !thePage.studentView()) {
    return "Deadlines";
  }
  if (thePage.user.hasInstructorRights() && thePage.studentView()) {
    var sectionIndex = thePage.storage.currentSectionComputed.getValue();
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
  result += `onclick = "window.calculator.coursePage.toggleDeadline('${this.idDeadlinePanel}', '${this.idURLed}', this);">`;
  result += `${this.toStringDeadline()} &#9666;</button>`;
  result += `<span class = "panelDeadlines" id = "${this.idDeadlinePanel}">`;
  result += "<table>";
  result += "<tr><th>Grp.</th><th>Deadline</th></tr>";
  for (var counterGroup = 0; counterGroup < thePage.user.sectionsTaught.length; counterGroup ++) {
    result += `<tr><td>${thePage.user.sectionsTaught[counterGroup]}</td>`;
    result += `<td><input class = "datePicker" name = "${this.idURLed}" `;
    if (this.deadlines[counterGroup] !== "" && this.deadlines[counterGroup] !== undefined) {
      result += `value = "${this.deadlines[counterGroup]}"`;
    }
    result += `></input></td></tr>`;
  } 
  result += "</table>";
  //console.log("Problem data problem: " + JSON.stringify(this.fileName));
  //console.log("Problem data title: " + JSON.stringify(this.title));
  result += `<button onclick = "modifyDeadlines('${this.idURLed}')">Set</button>`;
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
  result += `<button id = '${this.idButtonPoints}' onclick = "modifyWeight('${this.idURLed}')">Modify</button><br>`;
  result += `<span id = '${this.idModifyReportPoints}'></span>`;
  result += "</span>";
  return result;
}

Problem.prototype.callbackModifyWeight = function(input, output) {
  document.getElementById(this.idModifyReportPoints).innerHTML = input;
}

Problem.prototype.modifyWeight = function() {
  var problemWeightTextareaId = `points${this.idURLed}`;
  var incomingPoints = document.getElementById(problemWeightTextareaId).value;
  var modifyObject = {};
  var idDecoded = decodeURIComponent(this.idURLed);
  //var problemModifyWeightReport = `report${id}`;
  modifyObject[idDecoded] = {
    weight: incomingPoints
  };
  var theURL = ""; 
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.request}=${pathnames.setProblemData}&`;
  theURL += `${pathnames.mainInput}=${encodeURIComponent(JSON.stringify(modifyObject))}`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: this.callbackModifyWeight.bind(this)
  });
}

function modifyWeight(id) {
  var thePage = window.calculator.mainPage;
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
  pointsString += `name = "${this.idURLed}">${this.toStringProblemWeight()} &#9666;</button>`;
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

Problem.prototype.getHTMLOneProblemTr = function () {
  var thePage = window.calculator.mainPage;
  var result = "";
  result += "<tr>";
  result += `<td>${this.problemNumberString} ${this.title}</td>`;
  result += "<td>";
  result += `<a class ='videoLink' href='${this.video}' target = '_blank'>Video</a>`;
  result += `<a class ='slidesLink' href='${this.slidesProjector}' target = '_blank'>Printable slides</a>`;
  result += `<a class ='slidesLink' href='${this.slidesPrintable}' target = '_blank'>Slides</a>`;
  if (this.linkSlidesLaTeX !== "" && this.linkSlidesLaTeX !== undefined) {
    result += `<a class ='slidesLink' href='${this.linkSlidesLaTeX}' target = '_blank' `;
    result += `download ='${convertStringToLaTeXFileName(this.title)}.tex'>.tex</a>`;
  }
  result += "</td>";
  result += "<td>";
  if (this.fileName !== "") {
    if (thePage.user.flagLoggedIn) {
      result += `<a class = "problemLinkQuiz" href = "#${this.getAppAnchorRequestFileCourseTopics(true)}" `; 
      result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.idURLed}', 'scoredQuizJSON');">Quiz</a>`;
    }
    result += `<a class = "problemLinkPractice" href = "#${this.getAppAnchorRequestFileCourseTopics(false)}" `;
    result += `onclick = "window.calculator.problemPage.selectCurrentProblem('${this.idURLed}', 'exerciseJSON');">Practice</a>`;
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
  result += "<div class = \"bodySubsection\">";
  result += "<table class = \"topicList\"><colgroup><col><col><col><col><col></colgroup>";
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
  result += `<div class = \"headSubsection\">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;
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
    result += `<div class =\"headSection\">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;    
  }
  result += "<div class =\"bodySection\">";
  if (this.isProblemContainer()) {
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
  result += `<div class =\"headChapter\">${this.problemNumberString} ${this.title} ${this.toStringDeadlineContainer()}</div>`;
  result += "<div class =\"bodyChapter\">";
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
  for (var counterPicker = 0; counterPicker < thePickers.length; counterPicker ++ ) {
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
  thePanel += editPage.getEditPanel(thePage.storage.currentCourse.courseHome.getValue());
  thePanel += editPage.getEditPanel(thePage.storage.currentCourse.topicList.getValue());
  if (
    thePage.theTopics.topicBundleFile !== undefined && 
    thePage.theTopics.topicBundleFile !== null &&
    thePage.theTopics.topicBundleFile !== ""
  ) {
    for (var counterTopicBundle = 0; counterTopicBundle < thePage.theTopics.topicBundleFile.length; counterTopicBundle ++) {
      thePanel += editPage.getEditPanel(thePage.theTopics.topicBundleFile[counterTopicBundle]);
    }
  }
  document.getElementById("divCurrentCourseEditPanel").innerHTML = thePanel;
}

function afterLoadTopics(incomingTopics, result) {
  var thePage = window.calculator.mainPage;
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0) {
    return;
  }
  thePage.previousProblemId = null;
  var stringHTMLContent = "";
  //try {
    thePage.resetProblems();
    thePage.theTopics = JSON.parse(incomingTopics);
    for (var counterChapter = 0; counterChapter < thePage.theTopics["children"].length; counterChapter ++) {
      var currentChapter = thePage.theTopics["children"][counterChapter];
      new Problem(currentChapter, null);
    }
    stringHTMLContent += getHTMLfromTopics();
    writeEditCoursePagePanel();
  //} catch (e) {
  //  stringHTMLContent = "<b style ='color:red'>Data error</b>. " + e;
  //}
  topicsElements[0].innerHTML = stringHTMLContent;
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  thePage.previousProblemId = null;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, topicsElements[0]]);
}

function updateProblemPageCallback(input, outputComponent) {
  var thePage = window.calculator.mainPage;
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
    thePage.problems[thePage.storage.currentCourse.currentProblemId.getValue()] = new Problem(theProblem, null);
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
    var currentVector = answerVectors[counterAnswers];
    currentProblem.answers[counterAnswers] = new initializeButtons.InputPanelData({
      problemId:           currentProblem.idURLed,
      idMQSpan:            currentVector.answerMQspanId,
      idPureLatex:         currentVector.answerIdPureLatex,
      idButtonContainer:   currentVector.preferredButtonContainer,
      idButtonSubmit:      currentVector.idButtonSubmit,
      idButtonInterpret:   currentVector.idButtonInterpret,
      idButtonAnswer:      currentVector.idButtonAnswer,
      idButtonSolution:    currentVector.idButtonSolution,
      idVerificationSpan:  currentVector.idVerificationSpan,
      flagAnswerPanel:     true,
      flagCalculatorPanel: false,
    });
  }
  currentProblem.writeToHTML("divProblemPageContentContainer");

  currentProblem.scriptIds = [];
  for (var scriptLabel in theProblem.scripts) {
    var newLabel = encodeURIComponent(currentProblem.idURLed + scriptLabel);
    currentProblem.scriptIds.push(newLabel); 
    thePage.injectScript(newLabel, decodeURIComponent(theProblem.scripts[scriptLabel]));
  }
}

function updateProblemPage() {
  var thePage = window.calculator.mainPage;
  if (thePage.pages.problemPage.flagLoaded) {
    return;
  }
  thePage.pages.problemPage.flagLoaded = true;
  var theProblem = thePage.getCurrentProblem();
  var theURL;
  if (theProblem !== undefined && theProblem !== null) {
    theURL = `${pathnames.urls.calculatorAPI}?${theProblem.getCalculatorURLRequestFileCourseTopics()}`;
  } else {
    theURL = getCalculatorURLRequestFileCourseTopicsFromStorage();
  }
  //console.log("Current course: " + JSON.stringify(thePage.storage.currentCourse));
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
}