"use strict";

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
      resultString += "<problemInfoBar>";
      resultString += getEditPanel("/coursesavailable/default.txt");
      resultString += "</problemInfoBar>";
    }
    resultString += "<div style='text-align:center; width:100%'>";
    for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++) {
      var currentCourse = thePage.theCourses[counterCourses];
      resultString += `<a href="#" onclick="selectCourse(${counterCourses})" class = "courseLink">${currentCourse.title}</a>`;
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

function selectSelectCoursePage() {
  submitGET({
    url: `${pathnames.calculatorAPI}?request=selectCourseJSON`,
    callback: afterLoadSelectCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
