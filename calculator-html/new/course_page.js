"use strict";

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
function getHTMLSubSection(theSubSection) {
  var result = "";
  result += `<div class = \"headSubsection\">${theSubSection.problemNumberString} ${theSubSection.title}</div>`;
  result += "<div class = \"bodySubsection\">";
  result += "<table class = \"topicList\"><colgroup><col><col><col><col><col></colgroup>";
  for (var counterSubSection = 0; counterSubSection < theSubSection["children"].length; counterSubSection ++) {
    var currentProblemData = theSubSection["children"][counterSubSection];
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
    result += `<a class='videoLink' href='${currentProblemData.video}' target = '_blank'>Video</a>`;
    result += `<a class='slidesLink' href='${currentProblemData.slidesProjector}' target = '_blank'>Printable slides</a>`;
    result += `<a class='slidesLink' href='${currentProblemData.slidesPrintable}' target = '_blank'>Slides</a>`;
    if (currentProblemData.linkSlidesLaTeX !== "" && currentProblemData.linkSlidesLaTeX !== undefined) {
      result += `<a class='slidesLink' href='${currentProblemData.linkSlidesLaTeX}' target = '_blank' download='${convertStringToLaTeXFileName(currentProblemData.title)}.tex'>.tex</a>`;
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
    result += "<td></td>";
    result += "<td></td>";
    result += "</tr>";
  }
  result += "</table>";
  result += "</div>";
  return result;  
}

function getHTMLSection(theSection) {
  var result = "";
  result += `<div class=\"headSection\">${theSection.problemNumberString} ${theSection.title}</div>`;
  result += "<div class=\"bodySection\">";
  for (var counterSection = 0; counterSection < theSection["children"].length; counterSection ++) {
    var currentSection = theSection["children"][counterSection];
    result += getHTMLSubSection(currentSection);
  }
  result += "</div>";
  return result;  
}

function getHTMLChapter(theChapter) {
  var result = "";
  result += `<div class=\"headChapter\">${theChapter.problemNumberString} ${theChapter.title}</div>`;
  result += "<div class=\"bodyChapter\">";
  for (var counterSection = 0; counterSection < theChapter["children"].length; counterSection ++) {
    var currentSection = theChapter["children"][counterSection];
    result += getHTMLSection(currentSection);
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
  } catch (e) {
    stringHTMLContent = "<b style='color:red'>Data error</b>. " + e;
  }
  stringHTMLContent += "<hr>" + incomingTopics;
  topicsElements[0].innerHTML = stringHTMLContent;
  previousProblem = null;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, topicsElements[0]]);
}

function afterLoadCoursePage(incomingPage, result) {
  document.getElementById("divCurrentCourse").innerHTML = incomingPage;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentCourse")]);
  //MathJax.Hub.Process();
  var theTopics = document.getElementsByTagName("topicList");
  var submitAnswers = "topicListJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    submitAnswers = "topicListJSON";
  }
  if (theTopics.length  === 0) {
    return;
  }
  //console.log("DEBUG: topic list cookie @ afterLoadCoursePage: " + getCookie("topicList"));
  submitGET({
    url: `${pathnames.calculatorAPI}?request=${submitAnswers}`,
    callback: afterLoadTopics,
    progress: "spanProgressReportGeneral"
  });
}

function selectCurrentCoursePage() {
  //console.log("DEBUG: topic list cookie @ selectCurrentCoursePage: " + getCookie("topicList"));
  var submitAnswers = "templateJSONNoLogin";
  if (thePage.user.flagLoggedIn) {
    submitAnswers = "templateJSON";
  }
  submitGET({
    url: `${pathnames.calculatorAPI}?request=${submitAnswers}`,
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
