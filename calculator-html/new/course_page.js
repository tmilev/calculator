"use strict";

function getHTMLSubSection(theSubSection){
  var result = "";
  result += "<div class = \"headSubsection\">" + theSubSection.title + "</div>";
  result += "<div class = \"bodySubsection\">";
  result += "<table class = \"topicList\"><colgroup><col><col><col><col><col></colgroup>";
  for (var counterSubSection = 0; counterSubSection < theSubSection["children"].length; counterSubSection ++){
    var currentProblem = theSubSection["children"][counterSubSection];
    result += "<tr>";
    result += `<td>${currentProblem.title}</td>`;
    result += "<td></td>";
    result += "<td></td>";
    result += "<td></td>";
    result += "<td></td>";
    result += "</tr>";
  }
  result += "</table>";
  result += "</div>";
  return result;  
}

function getHTMLSection(theSection){
  var result = "";
  result += "<div class=\"headSection\">" + theSection.title + "</div>";
  result += "<div class=\"bodySection\">";
  for (var counterSection = 0; counterSection < theSection["children"].length; counterSection++){
    var currentSection = theSection["children"][counterSection];
    result += getHTMLSubSection(currentSection);
  }
  result += "</div>";
  return result;  
}

function getHTMLChapter(theChapter){
  var result = "";
  result += "<div class=\"headChapter\">" + theChapter.title + "</div>";
  result += "<div class=\"bodyChapter\">";
  for (var counterSection = 0; counterSection < theChapter["children"].length; counterSection++){
    var currentSection = theChapter["children"][counterSection];
    result += getHTMLSection(currentSection);
  }
  result += "</div>";
  return result;
}

function getHTMLfromTopics(theTopics){
  var result = "";
  for (var counterChapter = 0; counterChapter < theTopics["children"].length; counterChapter++){
    var currentChapter = theTopics["children"][counterChapter];
    result += getHTMLChapter(currentChapter);
  }
  return result;
}

function afterLoadTopics(incomingTopics, result){
  var topicsElements = document.getElementsByTagName("topicList");
  if (topicsElements.length === 0){
    return;
  }
  var stringHTMLContent = "";
  try {
    thePage.theTopics = JSON.parse(incomingTopics);
    stringHTMLContent += getHTMLfromTopics(thePage.theTopics);
  } catch (e) {
    stringHTMLContent = "<b style='color:red'>Data error</b>. " + e;
  }
  stringHTMLContent += "<hr>" + incomingTopics;
  topicsElements[0].innerHTML = stringHTMLContent;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, topicsElements[0]]);
}

function afterLoadCoursePage(incomingPage, result){
  document.getElementById("divCurrentCourse").innerHTML = incomingPage;
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById("divCurrentCourse")]);
  //MathJax.Hub.Process();
  var theTopics = document.getElementsByTagName("topicList");
  if (theTopics.length  === 0)
    return;
  submitGET({
    url: `${thePage.calculator}?request=topicListJSON`,
    callback: afterLoadTopics,
    progress: "spanProgressReportGeneral"
  });

}

function selectCurrentCoursePage(){
  submitGET({
    url: `${thePage.calculator}?request=templateNoLoginJSON`,
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
