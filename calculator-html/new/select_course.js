"use strict";

function afterLoadSelectCoursePage(incomingPage, result){
  
  document.getElementById("divSelectCourse").innerHTML = incomingPage;
}

function selectSelectCoursePage(){
  submitGET({
    url: "cgi-bin/calculator?request=selectCourseJSON",
    callback: afterLoadSelectCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
