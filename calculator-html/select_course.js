"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const editPage = require('./edit_page');
const ids = require('./ids_dom_elements');

function selectCourse(courseIndex) {
  var thePage = window.calculator.mainPage;
  var theCourse = thePage.theCourses[courseIndex];
  thePage.storage.variables.currentCourse.courseHome.setAndStore(theCourse.courseHome);
  thePage.storage.variables.currentCourse.topicList.setAndStore(theCourse.topicList);
  thePage.selectPage(thePage.pages.currentCourse.name);
}

function afterLoadSelectCoursePage(incomingPage, result) {
  var resultString = "";
  var thePage = window.calculator.mainPage;
  thePage.theCourses = JSON.parse(incomingPage)["courses"];
  pageSetup.needsLoad = false;

  //resultString += JSON.stringify(thePage.theCourses);
  var userHasProblemEditRights = thePage.user.hasProblemEditRights();
  if (userHasProblemEditRights) {
    resultString += "<div class = 'problemInfoBar'>";
    resultString += editPage.getEditPanel("/coursesavailable/default.txt");
    resultString += "</div>";
  }
  resultString += "<div style = 'text-align:center; width:100%'>";
  for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++) {
    var currentCourse = thePage.theCourses[counterCourses];
    var isRoughDraft = false;
    if (currentCourse.roughDraft === "true" || currentCourse.roughDraft === true) {
      isRoughDraft = true;
      if (!thePage.serverIsOnLocalHost() && !userHasProblemEditRights) {
        continue;
      }
    }
    resultString += `<a href = "#" onclick = "window.calculator.selectCourse.selectCourse(${counterCourses})" class = "courseLink">`;
    resultString += `${currentCourse.title}`;
    if (isRoughDraft) {
      resultString += "<b style = 'color:red; font-size: x-small'>rough draft</b>";
    }
    resultString += `</a>`;
    if (counterCourses != thePage.theCourses.length - 1) {
      resultString += "<br>";
    }
  }
  resultString += "</div>";
  document.getElementById("divSelectCourse").innerHTML = resultString;
}

var pageSetup = {
  needsLoad: true
};

function selectCoursePage() {
  if (pageSetup.needsLoad !== true) {
    return;
  }
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.selectCourse}`,
    callback: afterLoadSelectCoursePage,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports = {
  pageSetup,
  selectCoursePage,
  selectCourse,
};