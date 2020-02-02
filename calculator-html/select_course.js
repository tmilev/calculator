"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const editPage = require("./edit_page");
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require("./miscellaneous_frontend");

function selectCourse(courseIndex) {
  var thePage = window.calculator.mainPage;
  var theCourse = thePage.theCourses[courseIndex];
  thePage.storage.variables.currentCourse.courseHome.setAndStore(theCourse.courseHome);
  thePage.storage.variables.currentCourse.topicList.setAndStore(theCourse.topicList);
  thePage.selectPage(thePage.pages.currentCourse.name);
}

function afterLoadSelectCoursePage(incomingPage, result) {
  var result = [];
  var thePage = window.calculator.mainPage;
  thePage.theCourses = JSON.parse(incomingPage)["courses"];
  pageSetup.needsLoad = false;

  var userHasProblemEditRights = thePage.user.hasProblemEditRights();
  if (userHasProblemEditRights) {
    var problemInfoBar = document.createElement("div");
    problemInfoBar.className = "problemInfoBar";
    problemInfoBar.appendChild(editPage.getEditPanel("/coursesavailable/default.txt"));
    result.push(problemInfoBar);
  }
  var courseContainer = document.createElement("div");
  courseContainer.style.textAlign = "center";
  courseContainer.style.width = "100%";
  for (var counterCourses = 0; counterCourses < thePage.theCourses.length; counterCourses ++) {
    var currentCourse = thePage.theCourses[counterCourses];
    var isRoughDraft = false;
    if (currentCourse.roughDraft === "true" || currentCourse.roughDraft === true) {
      isRoughDraft = true;
      if (!thePage.serverIsOnLocalHost() && !userHasProblemEditRights) {
        continue;
      }
    }
    var editButton = document.createElement("button");
    editButton.className = "courseButton";
    editButton.addEventListener("click", selectCourse.bind(null, counterCourses));
    var editButtonInternal = "";
    editButtonInternal += `${currentCourse.title}`;
    if (isRoughDraft) {
      editButtonInternal += "<b style = 'color:red; font-size: x-small'>rough draft</b>";
    }
    editButton.innerHTML = editButtonInternal;
    courseContainer.appendChild(editButton);
    if (counterCourses != thePage.theCourses.length - 1) {
      courseContainer.appendChild(document.createElement("br"));
    }
  }
  result.push(courseContainer);
  var divSelectcourse = document.getElementById("divSelectCourse");
  divSelectcourse.innerHTML = "";
  miscellaneousFrontend.appendHtml(divSelectcourse, result);
}

var pageSetup = {
  needsLoad: true,
};

function selectCoursePage() {
  if (pageSetup.needsLoad !== true) {
    return;
  }
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.selectCourse}`,
    callback: afterLoadSelectCoursePage,
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

module.exports = {
  pageSetup,
  selectCoursePage,
  selectCourse,
};