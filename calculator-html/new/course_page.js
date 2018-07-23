"use strict";

Problem.prototype.toStringDeadline = function() {
  if (!thePage.user.hasAdminRights()) {
    return "";
  }
  if (this.type === "problem" && this.problem === "") {
    return "";
  }
  var result = "";
  result += `<button class = "accordionLike" onclick = "toggleDeadline('${this.idDeadlinePanel}')">Deadline</button>`;
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
  console.log("Problem data problem: " + JSON.stringify(this.problem));
  console.log("Problem data title: " + JSON.stringify(this.title));
  result += `<button onclick = "modifyDeadlines('${this.idURLed}')">Set</button>`;
  result += `<span id = '${this.idModifyReportDeadline}'></span>`;
  result += `</span>`;
  return result;
}

Problem.prototype.toHTMLWeights = function() { 
  //console.log("DEBUG: problem:  " + this.idURLed);
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
  //console.log("DEBUG: got to mod weight callback. This id: " + this.idURLed);
  document.getElementById(this.idModifyReportPoints).innerHTML = input;
}

Problem.prototype.modifyWeight = function() {
  var problemWeightTextareaId = `points${this.idURLed}`;
  var incomingPoints = document.getElementById(problemWeightTextareaId).value;
  var modifyObject = {};
  console.log("DEBUG: id: " + this.idURLed);
  var idDecoded = decodeURIComponent(this.idURLed);
  //var problemModifyWeightReport = `report${id}`;
  modifyObject[idDecoded] = {
    weight: incomingPoints
  };
  //console.log("DEBUG: about to fire up: " + JSON.stringify(modifyObject));
  var theURL = `${pathnames.calculatorAPI}?request=setProblemData&mainInput=${encodeURIComponent(JSON.stringify(modifyObject))}`;
  submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    callback: this.callbackModifyWeight.bind(this)
  });
}

function modifyWeight(id) {
  var theProblemWeight = theProblemMetaDataCollection[id];
  theProblemWeight.modifyWeight();
}

Problem.prototype.toStringProblemWeightCell = function() {
  var result = "";
  if (this.type !== "problem") {
    return "<td></td>";
  }
  //console.log("DEBUG: problemData.problem:  " + problemData.problem);
  var pointsString = `<button class = 'accordionLike' onclick = 'toggleProblemWeights()'>${this.toStringProblemWeight()}</button>`;
  var problemWeightString = this.toHTMLWeights();
  result += `<td>${pointsString}<br> ${problemWeightString}</td>`;
  return result;
}

Problem.prototype.toStringProblemWeight = function() {
  var result = "";
  var color = "brown";
  if (this.correctlyAnswered !== undefined && this.correctlyAnswered !== NaN) {
    if (
      this.correctlyAnswered >= this.totalQuestions && 
      this.totalQuestions !== undefined &&
      this.totalQuestions !== 0
    ) {
      color = "green";
    } 
    var numCorrectlyAnswered = this.correctlyAnswered;
    var totalQuestions = this.totalQuestions;
    if (totalQuestions === 0) {
      totalQuestions = "?";
    }
    result += `${numCorrectlyAnswered} out of ${totalQuestions}`;
    //console.log(`DEBUG: Problem weight: ` + JSON.stringify(this.weight));
    //console.log(`DEBUG: correctly answered: ` + this.correctlyAnswered);
    //console.log(`DEBUG: total: ` + this.totalQuestions);
    if (this.weight !== undefined && this.totalQuestions !== 0) {
      var problemWeightConverted = parseInt(this.weight);
      //console.log("DEBUG: prob weight converted: " + problemWeightConverted + " correctly answered: " + this.correctlyAnswered)
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

function callbackModifyDeadlines(incomingId, input, output) {
  console.log(`DEBUG: ${incomingId}, input: ${input}`);
  document.getElementById(`deadlines${incomingId}`).innerHTML = input;
}

function modifyDeadlines(incomingId) {
  var theDates = document.getElementsByName(incomingId);
  var jsonToSubmit = {};
  var idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (var counterDates = 0; counterDates < theDates.length; counterDates ++) {
    jsonToSubmit[idDecoded].deadlines[thePage.user.sectionsTaught[counterDates]] = theDates[counterDates].value;
  }
  console.log("DEBUG: id: " + incomingId);
  //console.log("DEBUG: about to fire up: " + JSON.stringify(modifyObject));
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?request=setProblemData&`;
  theURL += `mainInput=${encodeURIComponent(JSON.stringify(jsonToSubmit))}`;
  submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    callback: callbackModifyDeadlines.bind(null, incomingId)
  });
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
  if (this.problem !== "") {
    if (thePage.user.flagLoggedIn) {
      result += `<a class = "problemLinkQuiz" href = "${this.getURL(true)}" `; 
      result += `onclick = "selectCurrentProblem('${this.idURLed}', 'scoredQuizJSON');">Quiz</a>`;
    }
    result += `<a class = "problemLinkPractice" href = "${this.getURL(false)}" `;
    result += `onclick = "selectCurrentProblem('${this.idURLed}', 'exerciseJSON');">Practice</a>`;
  }
  result += "</td>";

  result += this.toStringProblemWeightCell();
  result += `<td>${this.toStringDeadline()}</td>`;
  result += "</tr>";
  return result;
}

Problem.prototype.getHTMLProblems = function () {
  var result = "";
  result += "<div class = \"bodySubsection\">";
  result += "<table class = \"topicList\"><colgroup><col><col><col><col><col></colgroup>";
  for (var counterSubSection = 0; counterSubSection < this.childrenIds.length; counterSubSection ++) {
    var currentProblem = thePage.problems[this.childrenIds[counterSubSection]];
    console.log("DEBUG: current problem data deadline: " + JSON.stringify(currentProblem.deadline));
    result += currentProblem.getHTMLOneProblemTr();
  }
  result += "</table>";
  result += "</div>";
  return result;
}

var problemWeightsVisible = false;

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

function toggleDeadline(deadlineId) { 
  var thePanel = document.getElementById(deadlineId);
  if (thePanel.style.maxHeight === '200px') {
    thePanel.style.opacity = '0';
    thePanel.style.maxHeight = '0';
  } else {
    thePanel.style.opacity = '1';
    thePanel.style.maxHeight = '200px';
  }
}

function toggleProblemWeights() { 
  var theWeights = document.getElementsByClassName('panelProblemWeights');
  if (!problemWeightsVisible) { 
    for (var i = 0; i < theWeights.length; i ++) { 
      //theScores[i].style.transition ='opacity 1s linear';
      theWeights[i].style.opacity = '1';
      theWeights[i].style.maxHeight = '200px';
    }
  } else { 
    for (i = 0; i < theWeights.length; i ++) { 
      //theScores[i].style.transition ='opacity 1s linear';
      theWeights[i].style.opacity = '0';
      theWeights[i].style.maxHeight = '0';
    }
  }
  problemWeightsVisible = !problemWeightsVisible;
}

Problem.prototype.getHTMLSubSection = function() {
  //console.log("DEBUG: current subsection: " + JSON.stringify(this.title) + "; type: " + JSON.stringify(this.type));
  var result = "";
  result += `<div class = \"headSubsection\">${this.problemNumberString} ${this.title}</div>`;
  result += this.getHTMLProblems();
  return result;  
}

Problem.prototype.isProblemContainer = function() {
  if (this.childrenIds.length !== undefined) {
    if (this.childrenIds.length > 0) {
      if (this.childrenIds[0].type === "problem") {
        return true;
      }
    }
  }
  return false;
}

Problem.prototype.getHTMLSection = function() {
  var result = "";
  //console.log("DEBUG: current section: " + JSON.stringify(this.title) + "; type: " + JSON.stringify(this.type));
  if (this.type === "section") {
    result += `<div class =\"headSection\">${this.problemNumberString} ${this.title} ${this.toStringDeadline()}</div>`;    
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
  var result = "";
  result += `<div class =\"headChapter\">${this.problemNumberString} ${this.title}</div>`;
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

function afterLoadTopics(incomingTopics, result) {
  //console.log("DEBUG: topic list cookie @ afterLoadTopics: " + getCookie("topicList"));
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0) {
    return;
  }
  thePage.previousProblemId = null;
  var stringHTMLContent = "";
  console.log ("DEBUG: temporarily disabled error catching ");
  //try {
    thePage.theTopics = JSON.parse(incomingTopics);
    for (var counterChapter = 0; counterChapter < thePage.theTopics["children"].length; counterChapter ++) {
      var currentChapter = thePage.theTopics["children"][counterChapter];
      new Problem(currentChapter);
    }
    stringHTMLContent += getHTMLfromTopics();
    writeEditCoursePagePanel();
  //} catch (e) {
  //  stringHTMLContent = "<b style ='color:red'>Data error</b>. " + e;
  //}
  //stringHTMLContent += "<hr>DEBUG: incoming topics JSON: " + incomingTopics;
  topicsElements[0].innerHTML = stringHTMLContent;
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  thePage.previousProblemId = null;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, topicsElements[0]]);
}

function writeEditCoursePagePanel() {
  var thePanel = "";
  thePanel += getEditPanel(thePage.storage.currentCourse.courseHome.getValue());
  thePanel += getEditPanel(thePage.storage.currentCourse.topicList.getValue());
  if (
    thePage.theTopics.topicBundleFile !== undefined && 
    thePage.theTopics.topicBundleFile !== null &&
    thePage.theTopics.topicBundleFile !== ""
  ) {
    for (var counterTopicBundle = 0; counterTopicBundle < thePage.theTopics.topicBundleFile.length; counterTopicBundle ++) {
      thePanel += getEditPanel(thePage.theTopics.topicBundleFile[counterTopicBundle]);
    }
  }
  document.getElementById("divCurrentCourseEditPanel").innerHTML = thePanel;
}

function afterLoadCoursePage(incomingPage, result) {
  document.getElementById("divCurrentCourseBody").innerHTML = incomingPage;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentCourse")]);
  //MathJax.Hub.Process();
  var theTopics = document.getElementsByTagName("topicList");
  var topicList = "topicListJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicList = "topicListJSON";
  }
  writeEditCoursePagePanel();
  if (theTopics.length  === 0) {
    return;
  }
  //console.log("DEBUG: topic list cookie @ afterLoadCoursePage: " + getCookie("topicList"));
  submitGET({
    url: `${pathnames.calculatorAPI}?request=${topicList}`,
    callback: afterLoadTopics,
    progress: "spanProgressReportGeneral"
  });
}

function selectCurrentCoursePage() {
  //console.log("DEBUG: topic list cookie @ selectCurrentCoursePage: " + getCookie("topicList"));
  var topicRequest = "templateJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    topicRequest = "templateJSON";
  }
  submitGET({
    url: `${pathnames.calculatorAPI}?request=${topicRequest}`,
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
