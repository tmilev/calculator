"use strict";

function afterLoadTopics(incomingTopics, result){
  var theTopics = document.getElementsByTagName("topicList");
  if (theTopics.length === 0){
    return;
  }
  theTopics[0].innerHTML = incomingTopics;
}

function afterLoadCoursePage(incomingPage, result){
  document.getElementById("divCurrentCourse").innerHTML = incomingPage;
  var theTopics = document.getElementsByTagName("topicList");
  if (theTopics.length  === 0)
    return;
  submitGET({
    url: "cgi-bin/calculator?request=topicListJSON",
    callback: afterLoadTopics,
    progress: "spanProgressReportGeneral"
  });

}

function selectCurrentCoursePage(){
  submitGET({
    url: "cgi-bin/calculator?request=templateNoLoginJSON",
    callback: afterLoadCoursePage,
    progress: "spanProgressReportGeneral"
  });
}
