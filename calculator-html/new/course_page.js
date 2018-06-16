"use strict";

function ProblemWeight(inputIdURLed, inputWeight) {
  this.idURLed = inputIdURLed;
  this.idButton = `modifyPoints${this.idURLed}`;
  this.idTextarea = `points${this.idURLed}`;
  this.idModifyReport = `report${this.idURLed}`;
  this.weight = inputWeight;
}

ProblemWeight.prototype.toHTML = function() { 
  //console.log("DEBUG: problem:  " + this.idURLed);
  var result = "";
  result += "<span class = 'panelProblemWeights' style = 'opacity: 1; max-height: 200px;'>";
  result += `Pts: <textarea class = 'textareaStudentPoints' rows = '1' cols = '2' id = '${this.idTextarea}'>`;
  if (this.weight !== undefined && this.weight !== null) {
    result += this.weight;
  }
  result += "</textarea>";
  result += `<button id = '${this.idButton}' onclick = "modifyWeight('${this.idURLed}')" >Modify</button><br>`;
  result += `<span id = '${this.idModifyReport}'></span>`;
  result += "</span>";
  return result;
}

function toStringProblemWeight(problemData) {
  var result = "";
  var color = "brown";
  if (problemData.correctlyAnswered !== undefined && problemData.correctlyAnswered !== NaN) {
    if (
      problemData.correctlyAnswered >= problemData.totalQuestions && 
      problemData.totalQuestions !== undefined &&
      problemData.totalQuestions !== 0
    ) {
      color = "green";
    } 
    var numCorrectlyAnswered = problemData.correctlyAnswered;
    var totalQuestions = problemData.totalQuestions;
    if (totalQuestions === 0) {
      totalQuestions = "?";
    }
    result += `${numCorrectlyAnswered} out of ${totalQuestions}`;
    //console.log(`DEBUG: Problem weight: ` + JSON.stringify(problemData.weight));
    //console.log(`DEBUG: correctly answered: ` + problemData.correctlyAnswered);
    //console.log(`DEBUG: total: ` + problemData.totalQuestions);
    if (problemData.weight !== undefined) {
      var problemWeightConverted = parseInt(problemData.weight);
      //console.log("DEBUG: prob weight converted: " + problemWeightConverted + " correctly answered: " + problemData.correctlyAnswered)
      var points = ((0.0 + problemData.correctlyAnswered * problemWeightConverted) / problemData.totalQuestions);
      points = Number(points.toFixed(2));
      result += ` = ${points} pts`;
      if (problemData.correctlyAnswered < problemData.totalQuestions) {
        color = "red";
      } 
    } else {
      result += ` = ? pts`;
    }
  }
  return `<b style = "color:${color}">${result}</b>`;
}

ProblemWeight.prototype.callbackModifyWeight = function(input, output) {
  //console.log("DEBUG: got to mod weight callback. This id: " + this.idURLed);
  document.getElementById(this.idModifyReport).innerHTML = input;
}
var theProblemWeightCollection = {};

function modifyWeight(id) {
  if (!(id in theProblemWeightCollection)) {
    theProblemWeightCollection[id] = new ProblemWeight(id);
  }
  var theProblemWeight = theProblemWeightCollection[id];
  var problemWeightTextareaId = `points${id}`;
  var incomingPoints = document.getElementById(problemWeightTextareaId).value;
  var modifyObject = {};
  //console.log("DEBUG: id: " + id);
  var idDecoded = decodeURIComponent(id);
  var problemModifyWeightReport = `report${id}`;
  modifyObject[idDecoded] = {
    weight: incomingPoints
  };
  //console.log("DEBUG: about to fire up: " + JSON.stringify(modifyObject));
  var theURL = `${pathnames.calculatorAPI}?request =setProblemData&mainInput =${encodeURIComponent(JSON.stringify(modifyObject))}`;
  submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    callback: theProblemWeight.callbackModifyWeight.bind(theProblemWeight)
  });
}

function toStringProblemWeightCell(problemData) {
  var result = "";
  if (problemData.problem === "") {
    return "<td></td>";
  }
  //console.log("DEBUG: problemData.problem:  " + problemData.problem);
  var pointsString = `<button class = 'accordionLike' onclick = 'toggleProblemWeights()'> ${toStringProblemWeight(problemData)}</button>`;
  var theProblemWeight = new ProblemWeight(encodeURIComponent(problemData.problem));
  result += `<td>${pointsString}<br> ${theProblemWeight.toHTML()}</td>`;
  return result;
}

function convertStringToLaTeXFileName(input) {
  var result = encodeURIComponent(input.split(" ").join("-")).split("%").join("-");
  if (result.length === 0)
    return "undefined";
  if (result[0] === "-") {
    result = "L" + result;
  }
  return result;
}

var previousProblem = null;
function getHTMLProblems(theProblemContainer) {
  var result = "";
  result += "<div class = \"bodySubsection\">";
  result += "<table class = \"topicList\"><colgroup><col><col><col><col><col></colgroup>";
  for (var counterSubSection = 0; counterSubSection < theProblemContainer["children"].length; counterSubSection ++) {
    var currentProblemData = theProblemContainer["children"][counterSubSection];
    //console.log("DEBUG: current problem data: " + JSON.stringify(currentProblemData.title));
    if (currentProblemData.problem !== "") {
      var currentProblem = thePage.getProblem(currentProblemData.problem);
      currentProblem.previousProblem = previousProblem;
      if (
        currentProblem.previousProblem !== null && 
        currentProblem.previousProblem !== undefined && 
        currentProblem.previousProblem !== ""
      ) {
        thePage.getProblem(currentProblem.previousProblem).nextProblem = currentProblem.fileName;
      }
      previousProblem = currentProblem.fileName;
    }
    result += "<tr>";
    result += `<td>${currentProblemData.problemNumberString} ${currentProblemData.title}</td>`;
    result += "<td>";
    result += `<a class ='videoLink' href='${currentProblemData.video}' target = '_blank'>Video</a>`;
    result += `<a class ='slidesLink' href='${currentProblemData.slidesProjector}' target = '_blank'>Printable slides</a>`;
    result += `<a class ='slidesLink' href='${currentProblemData.slidesPrintable}' target = '_blank'>Slides</a>`;
    if (currentProblemData.linkSlidesLaTeX !== "" && currentProblemData.linkSlidesLaTeX !== undefined) {
      result += `<a class ='slidesLink' href='${currentProblemData.linkSlidesLaTeX}' target = '_blank' download='${convertStringToLaTeXFileName(currentProblemData.title)}.tex'>.tex</a>`;
    }
    result += "</td>";
    result += "<td>";
    if (currentProblemData.problem !== "") {
      if (thePage.user.flagLoggedIn) {
        result += `<a class = "problemLinkQuiz" href = "${currentProblem.getURL(true)}" `; 
        result += `onclick = "selectCurrentProblem('${currentProblemData.problem}', 'scoredQuizJSON');">Quiz</a>`;
      }
      result += `<a class = "problemLinkPractice" href = "${currentProblem.getURL(false)}" `;
      result += `onclick = "selectCurrentProblem('${currentProblemData.problem}', 'exerciseJSON');">Practice</a>`;
    }
    result += "</td>";
    result += toStringProblemWeightCell(currentProblemData);
    result += "<td></td>";
    result += "</tr>";
  }
  result += "</table>";
  result += "</div>";
  return result;
}

var problemWeightsVisible = false;

function initializeProblemWeights() {
  var theWeights = document.getElementsByClassName('panelProblemWeights');
  for (var i = 0; i < theWeights.length; i ++) { 
    //theScores[i].style.transition ='opacity 1s linear';
    theWeights[i].style.maxHeight = '0px';
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

function getHTMLSubSection(theSubSection) {
  //console.log("DEBUG: current subsection: " + JSON.stringify(theSubSection.title) + "; type: " + JSON.stringify(theSubSection.type));
  var result = "";
  result += `<div class = \"headSubsection\">${theSubSection.problemNumberString} ${theSubSection.title}</div>`;
  result += getHTMLProblems(theSubSection)
  return result;  
}

function isProblemContainer(section) {
  if (section["children"].length !== undefined) {
    if (section["children"].length > 0) {
      if (section["children"][0].type === "problem") {
        return true;
      }
    }
  }
  return false;
}

function getHTMLSection(theSection) {
  var result = "";
  //console.log("DEBUG: current section: " + JSON.stringify(theSection.title) + "; type: " + JSON.stringify(theSection.type));
  if (theSection.type === "section"){
    result += `<div class =\"headSection\">${theSection.problemNumberString} ${theSection.title}</div>`;    
  }
  result += "<div class =\"bodySection\">";
  if (isProblemContainer(theSection)) {
    result += getHTMLProblems(theSection);
  } else if (theSection.type === "section") {
    for (var counterSection = 0; counterSection < theSection["children"].length; counterSection ++) {
      var currentSubSection = theSection["children"][counterSection];
      result += getHTMLSubSection(currentSubSection);
    }
  } else {
    result += getHTMLSubSection(theSection);
  }
  result += "</div>";
  return result;  
}

function getHTMLChapter(theChapter) {
  var result = "";
  result += `<div class =\"headChapter\">${theChapter.problemNumberString} ${theChapter.title}</div>`;
  result += "<div class =\"bodyChapter\">";
  if (isProblemContainer(theChapter)) {
    result += getHTMLProblems(theChapter);
  } else {
    for (var counterSection = 0; counterSection < theChapter["children"].length; counterSection ++) {
      var currentSection = theChapter["children"][counterSection];
      result += getHTMLSection(currentSection);
    }
  }
  result += "</div>";
  return result;
}

function getHTMLfromTopics(theTopics) {
  var result = "";
  for (var counterChapter = 0; counterChapter < theTopics["children"].length; counterChapter ++) {
    var currentChapter = theTopics["children"][counterChapter];
    result += getHTMLChapter(currentChapter);
  }
  return result;
}

function afterLoadTopics(incomingTopics, result) {
  //console.log("DEBUG: topic list cookie @ afterLoadTopics: " + getCookie("topicList"));
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0) {
    return;
  }
  previousProblem = null;
  var stringHTMLContent = "";
  try {
    thePage.theTopics = JSON.parse(incomingTopics);
    stringHTMLContent += getHTMLfromTopics(thePage.theTopics);
    writeEditCoursePagePanel();
  } catch (e) {
    stringHTMLContent = "<b style='color:red'>Data error</b>. " + e;
  }
  //stringHTMLContent += "<hr>DEBUG: incoming topics JSON: " + incomingTopics;
  topicsElements[0].innerHTML = stringHTMLContent;
  initializeProblemWeights();
  previousProblem = null;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, topicsElements[0]]);
}

function writeEditCoursePagePanel(){
  var thePanel = "";
  thePanel += getEditPanel(thePage.currentCourse.fileName);
  thePanel += getEditPanel(thePage.currentCourse.topicList);
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
  var incomingPageWithPanel = "";
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
    url: `${pathnames.calculatorAPI}?request =${topicList}`,
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
    url: `${pathnames.calculatorAPI}?request =${topicRequest}`,
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
