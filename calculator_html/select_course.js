"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const editPage = require("./edit_page");
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require("./miscellaneous_frontend");

function selectCourse(courseIndex) {
  const page = window.calculator.mainPage;
  const course = page.courses[courseIndex];
  page.storage.variables.currentCourse.courseHome.setAndStore(course.courseHome);
  page.storage.variables.currentCourse.topicList.setAndStore(course.topicList);
  page.selectPage(page.pages.currentCourse.name);
}

function afterLoadSelectCoursePage(incomingPage, result) {
  result = [];
  const page = window.calculator.mainPage;
  page.courses = JSON.parse(incomingPage)["courses"];
  pageSetup.needsLoad = false;

  let userHasProblemEditRights = page.user.hasProblemEditRights();
  if (userHasProblemEditRights) {
    let problemInfoBar = document.createElement("div");
    problemInfoBar.className = "problemInfoBar";
    let editPanel = editPage.getEditPanel(
      "/coursesavailable/default.txt",
      page.hasInstructorRightsNotViewingAsStudent(),
      false,
    );
    problemInfoBar.appendChild(editPanel);
    result.push(problemInfoBar);
  }
  let courseContainer = document.createElement("div");
  courseContainer.style.textAlign = "center";
  courseContainer.style.width = "100%";
  for (let i = 0; i < page.courses.length; i++) {
    let currentCourse = page.courses[i];
    let isRoughDraft = false;
    if (currentCourse.roughDraft === "true" || currentCourse.roughDraft === true) {
      isRoughDraft = true;
      if (!page.serverIsOnLocalHost() && !userHasProblemEditRights) {
        continue;
      }
    }
    let editButton = document.createElement("button");
    editButton.className = "courseButton";
    editButton.addEventListener("click", selectCourse.bind(null, i));
    let editButtonInternal = "";
    editButtonInternal += `${currentCourse.title}`;
    if (isRoughDraft) {
      editButtonInternal += "<b style = 'color:red; font-size: x-small'>rough draft</b>";
    }
    editButton.innerHTML = editButtonInternal;
    courseContainer.appendChild(editButton);
    if (i != page.courses.length - 1) {
      courseContainer.appendChild(document.createElement("br"));
    }
  }
  result.push(courseContainer);
  let divSelectcourse = document.getElementById("divSelectCourse");
  divSelectcourse.innerHTML = "";
  miscellaneousFrontend.appendHtml(divSelectcourse, result);
}

const pageSetup = {
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