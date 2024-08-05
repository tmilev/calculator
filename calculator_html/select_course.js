"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const editPage = require("./edit_page");
const ids = require("./ids_dom_elements");
const miscellaneousFrontend = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;

class CourseSelector {
  constructor() {
    this.courses = {};
    this.mainPage = null;
    this.needsLoad = true;
  }

  initialize(mainPage) {
    this.mainPage = mainPage;
  }

  selectCourse(courseIndex) {
    const course = this.courses[courseIndex];
    storage.variables.currentCourse.courseHome.setAndStore(course.courseHome);
    storage.variables.currentCourse.topicList.setAndStore(course.topicList);
    this.mainPage.selectPage(this.mainPage.pages.currentCourse.name);
  }

  selectCoursePage() {
    if (this.needsLoad !== true) {
      return;
    }
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    const selectCourse = pathnames.urlFields.selectCourse;
    submitRequests.submitGET({
      url: `${api}?${request}=${selectCourse}`,
      callback: (input) => {
        this.afterLoadSelectCoursePage(input);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }


  afterLoadSelectCoursePage(incomingPage) {
    this.courses = JSON.parse(incomingPage)["courses"];
    this.needsLoad = false;
    this.updateSelectCoursePage();
  }

  updateSelectCoursePage() {
    const result = [];
    let userHasProblemEditRights = globalUser.hasProblemEditRights();
    if (userHasProblemEditRights) {
      let problemInfoBar = document.createElement("div");
      problemInfoBar.className = "problemInfoBar";
      let editPanel = editPage.getEditPanel(
        "/coursesavailable/default.txt",
        globalUser.hasInstructorRightsNotViewingAsStudent(),
        false,
      );
      problemInfoBar.appendChild(editPanel);
      result.push(problemInfoBar);
    }
    let courseContainer = document.createElement("div");
    courseContainer.style.textAlign = "center";
    courseContainer.style.width = "100%";
    for (let i = 0; i < this.courses.length; i++) {
      let currentCourse = this.courses[i];
      let isRoughDraft = false;
      if (
        currentCourse.roughDraft === "true" ||
        currentCourse.roughDraft === true
      ) {
        isRoughDraft = true;
        if (!this.mainPage.serverIsOnLocalHost() && !userHasProblemEditRights) {
          continue;
        }
      }
      let editButton = document.createElement("button");
      editButton.className = "courseButton";
      editButton.addEventListener("click", selectCourse.bind(null, i));
      let editButtonInternal = "";
      editButtonInternal += `${currentCourse.title}`;
      if (isRoughDraft) {
        editButtonInternal += "<b style = 'color:red; font-size: x-small'>" +
          "rough draft</b>";
      }
      miscellaneousFrontend.writeHTML(editButton, editButtonInternal);
      courseContainer.appendChild(editButton);
      if (i != this.courses.length - 1) {
        courseContainer.appendChild(document.createElement("br"));
      }
    }
    result.push(courseContainer);
    let divSelectcourse = document.getElementById("divSelectCourse");
    divSelectcourse.textContent = "";
    miscellaneousFrontend.appendHtml(divSelectcourse, result);
  }
}

function selectCourse(courseIndex) {
  courseSelector.selectCourse(courseIndex);
}


function selectCoursePage() {
  courseSelector.selectCoursePage();
}

const courseSelector = new CourseSelector();

module.exports = {
  courseSelector,
  selectCoursePage,
  selectCourse,
};