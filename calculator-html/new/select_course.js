"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const editPage = require('./edit_page');

function selectCourse(courseIndex) {
  var thePage = window.calculator.mainPage;
  var theCourse = thePage.theCourses[courseIndex];
  thePage.storage.currentCourse.courseHome.setAndStore(theCourse.courseHome);
  thePage.storage.currentCourse.topicList.setAndStore(theCourse.topicList);
  thePage.selectPage(thePage.pages.currentCourse.name);
}

function afterLoadSelectCoursePage(incomingPage, result) {
  var resultString = "";
  var thePage = window.calculator.mainPage;
  thePage.theCourses = JSON.parse(incomingPage)["courses"];
  //resultString += JSON.stringify(thePage.theCourses);
  if (thePage.user.hasProblemEditRights()) {
    resultString += "<div class ='problemInfoBar'>";
    resultString += editPage.getEditPanel("/coursesavailable/default.txt");
    resultString += "</div>";
  }
  resultString += "<div style='text-align:center; width:100%'>";
  for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++) {
    var currentCourse = thePage.theCourses[counterCourses];
    resultString += `<a href="#" onclick="window.calculator.selectCourse.selectCourse(${counterCourses})" class = "courseLink">`;
    resultString += `${currentCourse.title}</a>`;
    if (counterCourses != thePage.theCourses.length - 1) {
      resultString += "<br>";
    }
  }
  resultString += "</div>";
  document.getElementById("divSelectCourse").innerHTML = resultString;
}

function selectCoursePage() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?request=selectCourseJSON`,
    callback: afterLoadSelectCoursePage,
    progress: "spanProgressReportGeneral"
  });
}

module.exports = {
  selectCoursePage,
  selectCourse,
}