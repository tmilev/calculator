"use strict";
const submitRequests = require('./submit_requests');

function selectCourse(courseIndex) {
  var theCourse = thePage.theCourses[courseIndex];
  thePage.storage.currentCourse.courseHome.setAndStore(theCourse.courseHome);
  thePage.storage.currentCourse.topicList.setAndStore(theCourse.topicList);
  thePage.selectPage(thePage.pages.currentCourse.name);
}

function afterLoadSelectCoursePage(incomingPage, result) {
  var resultString = "";
  try {
    thePage.theCourses = JSON.parse(incomingPage)["courses"];
    //resultString += JSON.stringify(thePage.theCourses);
    if (thePage.user.hasProblemEditRights()) {
      resultString += "<div class ='problemInfoBar'>";
      resultString += getEditPanel("/coursesavailable/default.txt");
      resultString += "</div>";
    }
    resultString += "<div style='text-align:center; width:100%'>";
    for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++) {
      var currentCourse = thePage.theCourses[counterCourses];
      resultString += `<a href="#" onclick="selectCourse(${counterCourses})" class = "courseLink">`;
      resultString += `${currentCourse.title}</a>`;
      if (counterCourses != thePage.theCourses.length - 1) {
        resultString += "<br>";
      }
    }
    resultString += "</div>";
    document.getElementById("divSelectCourse").innerHTML = resultString;
  } catch (e) {
    document.getElementById("divSelectCourse").innerHTML = "<b style='color:red'>Error</b>" + e;
  }
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