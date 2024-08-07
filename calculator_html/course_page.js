"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const problemPage = require("./problem_page");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;

class CoursePage {
  constructor() {
    this.mainPage = null;
  }
  initialize(mainPage) {
    this.mainPage = mainPage;
  }
  selectCurrentCoursePage() {
    let currentCourse = storage.variables.currentCourse;
    let incomingCourse = currentCourse.courseHome.getValue();
    let incomingTopicList = currentCourse.topicList.getValue();
    if (
      incomingCourse === null ||
      incomingCourse === "" ||
      incomingCourse === undefined
    ) {
      let courseBody = document.getElementById(
        ids.domElements.divCurrentCourseBody
      );
      let button = document.createElement("button");
      button.classList.add(
        "buttonSelectPage",
        "buttonSlowTransition",
        "buttonFlash"
      );
      button.style.width = "150px";
      button.textContent = "Please select course";
      button.addEventListener("click", () => {
        this.mainPage.selectPage('selectCourse');
      });
      setTimeout(() => {
        button.classList.remove("buttonFlash");
      }, 0);
      courseBody.textContent = "";
      courseBody.appendChild(button);
      return;
    }
    if (
      lastLoadedCourse.courseHome === incomingCourse &&
      lastLoadedCourse.topicList === incomingTopicList
    ) {
      return;
    }
    // The lastLoadedCourse variable may be reset on successful login. 
    lastLoadedCourse.courseHome = incomingCourse;
    lastLoadedCourse.topicList = incomingTopicList;
    let topicRequest = "templateJSONNoLogin";
    if (this.mainPage.user.flagLoggedIn) {
      topicRequest = "templateJSON";
    }
    let url = "";
    const api = pathnames.urls.calculatorAPI;
    url += `${api}?${pathnames.urlFields.request}=${topicRequest}&`;
    url += `${pathnames.urlFields.problem.courseHome}=${incomingCourse}&`;
    url += `${pathnames.urlFields.problem.topicList}=${incomingTopicList}`;
    submitRequests.submitGET({
      url: url,
      callback: afterLoadCoursePage,
      progress: ids.domElements.spanProgressReportGeneral
    });
  }
}

function modifyDeadlines(incomingId) {
  let nameDatePicker = `datePicker${incomingId}`;
  let dates = document.getElementsByName(nameDatePicker);
  let jsonToSubmit = {};
  let idDecoded = decodeURIComponent(incomingId);
  jsonToSubmit[idDecoded] = {
    deadlines: {}
  };

  for (let i = 0; i < dates.length; i++) {
    let currentSection = globalUser.sectionsTaught[i];
    jsonToSubmit[idDecoded].deadlines[currentSection] = dates[i].value;
  }
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  const setProblemDeadline = pathnames.urlFields.requests.setProblemDeadline;
  url += `${pathnames.urlFields.request}=${setProblemDeadline}&`;
  const mainInput = encodeURIComponent(JSON.stringify(jsonToSubmit));
  url += `${pathnames.urlFields.mainInput}=${mainInput}&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    result: `deadlines${incomingId}`,
  });
}

let problemWeightsVisible = false;

function toggleDeadline(
  /** @type {HTMLElement} */
  deadline,
  panelId,
  /** @type {HTMLElement} */
  button,
) {
  let problem = problemPage.allProblems.getProblemById(panelId);
  if (deadline.style.maxHeight === '200px') {
    deadline.style.opacity = '0';
    deadline.style.maxHeight = '0';
    button.textContent = "";
    button.appendChild(problem.toHTMLDeadlineElement());
    let arrowLeft = document.createTextNode(" \u25C2");
    button.appendChild(arrowLeft);
  } else {
    deadline.style.opacity = '1';
    deadline.style.maxHeight = '200px';
    button.textContent = "";
    button.appendChild(problem.toHTMLDeadlineElement())
    button.appendChild(document.createTextNode(" \u25BE"));
  }
}

function toggleProblemWeights() {
  let weights = document.getElementsByClassName('panelProblemWeights');
  let buttons = document.getElementsByClassName('accordionLikeProblemWeight');
  for (let i = 0; i < weights.length; i++) {
    if (!problemWeightsVisible) {
      weights[i].style.opacity = '1';
      weights[i].style.maxHeight = '200px';
    } else {
      weights[i].style.opacity = '0';
      weights[i].style.maxHeight = '0';
    }
  }
  for (let i = 0; i < buttons.length; i++) {
    let currentProblem = problemPage.allProblems.getProblemById(
      buttons[i].name
    );
    if (!problemWeightsVisible) {
      miscellaneous.writeHTML(
        buttons[i],
        `${currentProblem.toStringProblemWeight()} &#9660;`
      );
    } else {
      miscellaneous.writeHTML(
        buttons[i],
        `${currentProblem.toStringProblemWeight()} &#9666;`
      );
    }
  }
  problemWeightsVisible = !problemWeightsVisible;
}

function afterLoadCoursePage(incoming, result) {
  let courseBody = document.getElementById(
    ids.domElements.divCurrentCourseBody
  );
  let coursePage = document.getElementById(
    ids.domElements.divCurrentCourse
  );
  miscellaneous.writeHTML(
    courseBody,
    miscellaneous.jsonParseGetHtmlStandard(incoming),
  );
  let titleElements = courseBody.getElementsByTagName('title');
  if (titleElements !== null && titleElements !== undefined) {
    if (titleElements.length > 0) {
      document.getElementsByTagName('title')[0].text = titleElements[0].text;
    }
  }
  typeset.typesetter.typesetSoft(coursePage, "");
  problemPage.writeEditCoursePagePanel();
  if (problemPage.getTopicListElement() === null) {
    return;
  }
  loadTopicList(problemPage.processLoadedTopicsWriteToCoursePage);
}

function loadTopicList(callback) {
  let topicListRequest = "topicListJSONNoLogin";
  if (globalUser.isLoggedIn()) {
    topicListRequest = "topicListJSON";
  }
  let topicName = storage.variables.currentCourse.topicList.getValue();
  let courseHome = storage.variables.currentCourse.courseHome.getValue();
  let url = "";
  const api = pathnames.urls.calculatorAPI;
  url += `${api}?${pathnames.urlFields.request}=${topicListRequest}&`;
  url += `${pathnames.urlFields.problem.topicList}=${topicName}&`;
  url += `${pathnames.urlFields.problem.courseHome}=${courseHome}&`;
  submitRequests.submitGET({
    url: url,
    callback: callback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

let lastLoadedCourse = {
  courseHome: null,
  topicList: null,
};

function selectCurrentCoursePage() {
  coursePage.selectCurrentCoursePage();
}

const coursePage = new CoursePage();

module.exports = {
  coursePage,
  loadTopicList,
  lastLoadedCourse,
  modifyDeadlines,
  selectCurrentCoursePage,
  toggleDeadline,
  toggleProblemWeights,
};