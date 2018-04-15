"use strict";

function selectCourse(courseIndex){
  thePage.currentCourse = thePage.theCourses[courseIndex];
  thePage.storeSettingsToCookies();
  thePage.selectPage("currentCourse");
}

function afterLoadSelectCoursePage(incomingPage, result){
  var resultString = "";
  try {
    thePage.theCourses = JSON.parse(incomingPage)["courses"];
    //resultString += JSON.stringify(thePage.theCourses);
    resultString += "<div style='text-align:center; width:100%'>";
    for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++){
      var currentCourse = thePage.theCourses[counterCourses];
      resultString += `<a href="#" onclick="selectCourse(${counterCourses})" class="courseLink">${currentCourse.title}</a>`;
      if (counterCourses != thePage.theCourses.length - 1){
        resultString += "<br>";
      }
    }
    resultString += "</div>";
    document.getElementById("divSelectCourse").innerHTML = resultString;
  } catch (e){
    document.getElementById("divSelectCourse").innerHTML = "<b style='color:red'>Error</b>" + e;
  }

}

function selectSelectCoursePage(){
  submitGET({
    url: `${pathnames.calculator}?request=selectCourseJSON`,
    callback: afterLoadSelectCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
