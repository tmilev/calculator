"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const editPage = require("./edit_page");
const initializeButtons = require("./initialize_buttons");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");
const miscellaneousFrontend = require("./miscellaneous_frontend");
const datePicker = require("./date_picker").datePicker;
const storage = require("./storage");
const AnswerPanel = require("./answer_panel").AnswerPanel;
const dynamicJavascript = require("./dynamic_javascript").dynamicJavascript;

class ProblemCollection {
  constructor() {
    /** @type{Object<string,Problem>} */
    this.topicProblems = {};
    /** @type{Object<string,Problem>} */
    this.allProblems = {};
    /** @type{string} */
    this.previousProblemId = null;
    /** @type{string} */
    this.nextProblemId = null;
    /** @type{Object<string, boolean>} */
    this.theChapterIds = {};
    this.topics = {};
  }

  /** @return{Problem} */
  getProblemById(
    /** @type{string} */
    label,
  ) {
    if (!(label in this.allProblems)) {
      throw (`Error: problem label ${label} not found. `);
    }
    return this.allProblems[label];
  }

  resetTopicProblems() {
    this.theChapterIds = {};
    this.topicProblems = {};
  }

  /** @return{Problem} */
  createOrUpdateProblem(
    problemData,
  ) {
    if (problemData.id.includes("%")) {
      console.log("Unexpected percent sign in problem id.");
    }
    // theProblemId is percent encoded, safe to embed in html.
    let theProblemId = encodeURIComponent(problemData.id);
    let element = document.getElementById(ids.domElements.problemPageContentContainer);
    let currentProblem = this.getProblemByIdOrRegisterEmpty(theProblemId, element);
    currentProblem.initializeInfo(problemData, null);
    return currentProblem;
  }

  normalizeFileName(
    problemFileName,
  ) {
    problemFileName = decodeURIComponent(problemFileName);
    return encodeURIComponent(problemFileName);
  }

  getProblemByIdOrNull(
    problemFileName,
  ) {
    let label = this.normalizeFileName(problemFileName);
    if (label in this.allProblems) {
      return this.allProblems[label];
    }
    return null;
  }

  /** @return{Problem} */
  getProblemByIdOrRegisterEmpty(
    problemFileName,
    /** @type{HTMLElement} */
    outputElement,
  ) {
    // normalize the file name:
    let problem = this.getProblemByIdOrNull(problemFileName);
    if (problem !== null) {
      return problem;
    }
    let incoming = new Problem(outputElement);
    let label = this.normalizeFileName(problemFileName);
    incoming.initializeBasic({
      id: label,
      fileName: problemFileName
    });
    this.allProblems[label] = incoming;
    return incoming;
  }

  processLoadedTopicsWriteToCoursePage(incomingTopics) {
    const startTime = new Date().getTime();
    this.processLoadedTopics(incomingTopics);
    writeTopicsToCoursePage();
    const writeTime = new Date().getTime() - startTime;
    if (writeTime > 1000) {
      console.log(`Less than a second to generate html expected, needed ${writeTime} ms instead.`);
    }
  }

  processLoadedTopics(incomingTopics) {
    const startTime = new Date().getTime();
    this.previousProblemId = null;
    this.resetTopicProblems();
    this.topics = miscellaneous.jsonUnescapeParse(incomingTopics);
    if (!Array.isArray(this.topics["children"])) {
      return;
    }
    let totalChildren = 0;
    for (let i = 0; i < this.topics["children"].length; i++) {
      const currentChapter = this.topics["children"][i];
      const problem = this.createOrUpdateProblem(currentChapter);
      totalChildren += problem.totalChildren;
    }
    const finalTime = new Date().getTime() - startTime;
    const timePerChild = finalTime / totalChildren;
    if (finalTime > 1000) {
      let logMessage = `Final time of processLoadedTopics: `;
      logMessage += `${finalTime} ms for ${totalChildren} children `;
      logMessage += `at ${timePerChild.toFixed()} ms per child.`;
      console.log(logMessage);
    }
  }

  updateProblemPage() {
    let page = window.calculator.mainPage;
    window.calculator.coursePage.selectCurrentCoursePage();
    // page.pages.problemPage.flagLoaded is modified by the following
    // functions: selectCurrentProblem, logout, callbackClone,
    // the present function updateProblemPage
    /** @type {Problem} */
    let problem = getCurrentProblem();
    if (page.pages.problemPage.flagLoaded) {
      if (problem !== undefined && problem !== null) {
        let problemNavigation = document.getElementById(problem.idNavigationProblemNotEntirePanel);
        if (problemNavigation !== null) {
          problemNavigation.innerHTML = "";
          let updatedContent = problem.getProblemNavigationContent();
          for (let i = 0; i < updatedContent.length; i++) {
            problemNavigation.appendChild(updatedContent[i]);
          }
        }
      }
      return;
    }
    let url;
    if (problem !== undefined && problem !== null) {
      let forReal = problem.flagForReal;
      url = `${pathnames.urls.calculatorAPI}?${problem.getCalculatorURLRequestFileCourseTopics(forReal)}`;
    } else {
      let problemFileName = storage.storage.variables.currentCourse.problemFileName.getValue();
      if (
        problemFileName === "" ||
        problemFileName === undefined ||
        problemFileName === null
      ) {
        this.displaySelectCourse();
        return;
      }
      url = getCalculatorURLRequestFileCourseTopicsFromStorage();
    }
    page.pages.problemPage.flagLoaded = true;
    submitRequests.submitGET({
      url: url,
      callback: (input, outputComponent) => {
        problem.writeProblemPage(input, outputComponent);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  displaySelectCourse() {
    let courseBody = document.getElementById(
      ids.domElements.problemPageContentContainer,
    );
    courseBody.textContent = "";
    let text = document.createTextNode("Problems are selected from the ");
    courseBody.appendChild(text);
    let button = document.createElement("button");
    button.textContent = 'Current course';
    button.classList.add(
      "buttonSelectPage", "buttonSlowTransition", "buttonFlash"
    );
    button.style.width = "150px";
    const page = window.calculator.mainPage;
    button.addEventListener("click", () => {
      page.selectPage('currentCourse');
    });
    courseBody.appendChild(button);
    courseBody.appendChild(document.createElement("br"));
    courseBody.appendChild(document.createTextNode(
      "To select a problem, click Practice or Quiz within the course page."
    ));
    setTimeout(() => {
      button.classList.remove("buttonFlash");
    }, 100);
  }
}

function selectCurrentProblem(problemIdURLed, exerciseType) {
  let page = window.calculator.mainPage;
  page.storage.variables.currentCourse.problemFileName.setAndStore(decodeURIComponent(problemIdURLed));
  page.storage.variables.currentCourse.exerciseType.setAndStore(exerciseType);
  let problem = getCurrentProblem();
  problem.flagForReal = false;
  if (
    exerciseType === pathnames.urlFields.scoredQuizJSON &&
    !page.user.flagDatabaseInactiveEveryoneIsAdmin
  ) {
    problem.flagForReal = true;
  }
  page.pages.problemPage.flagLoaded = false;
  // This will force a page reload.
  page.currentPage = "";  
  page.selectPage(page.pages.problemPage.name);
}

class Problem {
  constructor(
    /** @type{HTMLElement}*/
    outputElement,
  ) {
    /** @type{string} */
    this.nextProblemId = "";
    /** @type{string} */
    this.previousProblemId = "";
    /** @type{number} */
    this.totalChildren = 0;
    /** @type{HTMLElement} */
    this.outputElement = outputElement;
    /** @type{string} */
    this.title = "";
  }

  setRandomSeed(input) {
    if (input === undefined) {
      input = null;
    }
    this.randomSeed = input;
  }

  setRandomSeedFromEnvironment() {
    let currentCourse = window.calculator.mainPage.storage.variables.currentCourse;
    this.flagForReal = (currentCourse.exerciseType.getValue() !== pathnames.urlFields.exerciseJSON);
    let incomingRandomSeed = currentCourse.randomSeed.getValue();
    if (
      incomingRandomSeed === "" ||
      incomingRandomSeed === null ||
      incomingRandomSeed === undefined
    ) {
      if (!this.flagForReal) {
        return;
      }
    }
    this.setRandomSeed(incomingRandomSeed);
    currentCourse.randomSeed.value = "";
  }

  initializeBasic(problemData) {
    /** ProblemId is percent encoded, safe to embed in html. */
    this.problemId = encodeURIComponent(problemData.id);
    /** 
     * @type {string}
     * This id is for problem navigation only, 
     * does not include the entire panel.
     */
    this.idNavigationProblemNotEntirePanel = `navigationPanel${this.problemId}`;
    /** @type {AnswerPanel[]} */
    this.answerPanels = [];
    this.title = problemData.title;
    this.fileName = problemData.fileName;
    if (this.fileName === undefined || this.fileName === null) {
      this.fileName = "";
    }
    if (window.calculator.mainPage.flagProblemPageOnly) {
      this.flagForReal = false;
    } else {
      this.setRandomSeedFromEnvironment();
    }
  }

  writeProblemPage(input, outputComponent) {
    let problemData = null;
    this.outputElement.textContent = "";
    try {
      problemData = miscellaneous.jsonUnescapeParse(input);
    } catch (e) {
      this.outputElement.textContent = `Error parsing: ${e}. Failed to parse: ${input}`;
      return;
    }
    if (problemData.crashReport !== undefined && problemData.crashReport !== null) {
      let html = miscellaneousFrontend.htmlFromCommentsAndErrors(problemData);
      this.outputElement.innerHTML = html;
      return;
    }
    this.initializeProblemContent(problemData);
  }

  /** @return{number} Number of children processed. */
  initializeInfo(problemData, inputParentIdURLed) {
    this.initializeBasic(problemData);
    this.decodedProblem = "";
    this.commentsProblem = "";
    this.parentIdURLed = inputParentIdURLed;

    this.problemLabel = "";
    this.previousProblemId = "";
    this.nextProblemId = "";
    this.scriptIds = null;
    this.type = problemData.type;
    this.problemNumberString = problemData.problemNumberString;
    this.idTextareaPoints = `points${this.problemId}`;
    this.idModifyReportDeadline = `deadlines${this.problemId}`;
    this.idDeadlinePanel = `deadlinesPanel${this.problemId}`;
    this.idDeadlineContainer = `${ids.stringResources.prefixDeadlineContainer}${this.problemId}`;
    this.correctlyAnswered = problemData.correctlyAnswered;
    this.totalQuestions = problemData.totalQuestions;
    this.deadlines = [];
    this.deadlineString = null;
    this.deadline = null;
    this.weight = problemData.weight;
    this.links = {
      /** @type{HTMLElement[]} */
      video: [],
      /** @type{HTMLElement[]} */
      slides: [],
      /** @type{HTMLElement[]} */
      homework: [],
    };
    /** @type{HTMLElement[]} */
    this.badProblemExplanation = [];
    if (problemData.deadlines !== undefined) {
      this.deadlines = problemData.deadlines;
    }
    if (problemData.deadline !== undefined) {
      this.deadlineString = problemData.deadline;
    }
    if (this.fileName !== "") {
      this.previousProblemId = allProblems.previousProblemId;
      if (
        this.previousProblemId !== null &&
        this.previousProblemId !== undefined &&
        this.previousProblemId !== ""
      ) {
        let previousProblem = allProblems.allProblems[this.previousProblemId];
        previousProblem.nextProblemId = this.problemId;
      }
      allProblems.previousProblemId = this.problemId;
    }
    this.problemNumberString = problemData.problemNumberString;
    this.video = problemData.video;
    this.querySlides = problemData.querySlides;
    if (this.querySlides === undefined || this.querySlides === null) {
      this.querySlides = "";
    }
    this.queryHomework = problemData.queryHomework;
    if (this.queryHomework === undefined || this.queryHomework === null) {
      this.queryHomework = "";
    }
    if (this.type === "Chapter") {
      allProblems.theChapterIds[this.problemId] = true;
    }
    // const secondPartTime = new Date().getTime();
    this.childrenIds = [];
    if (!Array.isArray(problemData.children)) {
      return;
    }
    for (let i = 0; i < problemData.children.length; i++) {
      const currentChild = allProblems.createOrUpdateProblem(problemData.children[i]);
      this.totalChildren += currentChild.totalChildren + 1;
      this.childrenIds.push(currentChild.problemId);
    }
  }

  /** @return{HTMLElement[]} */
  computeBadProblemExplanation() {
    let userHasInstructorRights = true;
    let pageLastKnownGoodProblemName = "";
    let page = window.calculator.mainPage;
    if (!page.flagProblemPageOnly) {
      userHasInstructorRights = page.user.hasInstructorRights();
    }
    this.badProblemExplanation = [];
    if (!this.decodedProblem.includes(pathnames.urlFields.problem.failedToLoadProblem)) {
      page.lastKnownGoodProblemFileName = this.fileName;
      return;
    }
    pageLastKnownGoodProblemName = page.lastKnownGoodProblemFileName;
    let badProblemExplanationPartOne = document.createElement("div");
    badProblemExplanationPartOne.innerHTML += "It appears your problem failed to load.<br>";
    if (this.lastKnownGoodProblemFileName !== "" && userHasInstructorRights) {
      badProblemExplanationPartOne.innerHTML += "Perhaps you may like to clone the last good known problem.<br>";
    }
    this.badProblemExplanation.push(badProblemExplanationPartOne);
    miscellaneousFrontend.appendHtmlToArray(
      this.badProblemExplanation,
      editPage.getClonePanel(
        pageLastKnownGoodProblemName,
        this.fileName
      ),
    );
    let epilogueElement = document.createElement("hr");
    this.badProblemExplanation.push(epilogueElement);
  }

  initializeProblemContent(problemData) {
    if (pathnames.standardResponses.isNotLoggedInResponse(problemData)) {
      this.title = "";
      this.decodedProblem = "Not logged in. Please select practice mode or login.";
      this.flagForReal = true;
      this.answerPanels = [];
    } else {
      this.initializeBasic(problemData);
      let content = problemData[pathnames.urlFields.problem.content];
      try {
        this.decodedProblem = decodeURIComponent(content);
      } catch (e) {
        this.decodedProblem = "Failed to decode problem content. Content follows.<br>" + content;
      }
    }
    this.commentsProblem = problemData["commentsProblem"];
    if (this.commentsProblem === undefined) {
      this.commentsProblem = "";
    }
    this.commentsProblem += miscellaneousFrontend.htmlFromCommentsAndErrors(problemData);
    this.computeBadProblemExplanation();
    let answerVectors = problemData["answers"];
    if (answerVectors === undefined) {
      this.writeToHTML();
      return;
    }
    this.flagForReal = problemData["forReal"];
    this.setRandomSeed(problemData.randomSeed);
    for (let i = 0; i < answerVectors.length; i++) {
      let currentVector = answerVectors[i];
      this.answerPanels[i] = new AnswerPanel({
        problemId: this.problemId,
        forReal: this.flagForReal,
        properties: currentVector.properties,
        previousAnswers: currentVector.previousAnswers,
        answerHighlight: currentVector.answerHighlight,
        answerPanelId: currentVector.answerPanelId,
        mathQuillPanelOptions: currentVector.mathQuillPanelOptions,
        idEquationEditorElement: currentVector.idEquationEditorElement,
        idPureLatex: currentVector.idPureLatex,
        idButtonContainer: currentVector.idButtonContainer,
        idButtonSubmit: currentVector.idButtonSubmit,
        idButtonInterpret: currentVector.idButtonInterpret,
        idButtonAnswer: currentVector.idButtonAnswer,
        idButtonSolution: currentVector.idButtonSolution,
        idVerificationSpan: currentVector.idVerificationSpan,
        flagAnswerPanel: true,
        flagCalculatorPanel: false,
        randomSeed: problemData.randomSeed,
      });
    }
    this.writeToHTML();
    this.scriptIds = [];
  }

  getAppAnchorRequestFileCourseTopics(
    /** @type {boolean} */
    isScoredQuiz,
    /** @type {boolean} */
    includeRandomSeed,
  ) {
    let page = window.calculator.mainPage;
    let exerciseType = pathnames.urlFields.exerciseJSON;
    if (isScoredQuiz && !page.user.flagDatabaseInactiveEveryoneIsAdmin) {
      exerciseType = pathnames.urlFields.scoredQuizJSON;
    }
    let requestJSON = {
      currentPage: page.pages.problemPage.name,
      exerciseType: exerciseType,
      problemFileName: this.fileName,
      courseHome: page.storage.variables.currentCourse.courseHome.getValue(),
      topicList: page.storage.variables.currentCourse.topicList.getValue(),
    };
    if (includeRandomSeed) {
      if (
        this.randomSeed !== "" &&
        this.randomSeed !== null &&
        this.randomSeed !== undefined
      ) {
        requestJSON["randomSeed"] = this.randomSeed;
      }
    }
    let stringifiedHash = page.storage.getPercentEncodedURL(requestJSON);
    return stringifiedHash;
  }

  getCalculatorURLFileCourseTopics() {
    let page = window.calculator.mainPage;
    return this.getCalculatorURLInput(
      this.fileName,
      page.storage.variables.currentCourse.courseHome.getValue(),
      page.storage.variables.currentCourse.topicList.getValue(),
    );
  }

  getCalculatorURLInput(
    inputFileName, inputCourseHome, inputTopicList,
  ) {
    let result = "";
    result += `fileName=${inputFileName}&`;
    result += `courseHome=${inputCourseHome}&`;
    result += `topicList=${inputTopicList}`;
    return result;
  }

  getCalculatorURLRequestPartOne(
    /** @type{boolean} */
    isScoredQuiz,
  ) {
    let page = window.calculator.mainPage;
    let result = "";
    if (isScoredQuiz === undefined) {
      isScoredQuiz = this.flagForReal;
    }
    result += `${pathnames.urlFields.request}=`;
    if (isScoredQuiz && !page.user.flagDatabaseInactiveEveryoneIsAdmin) {
      result += pathnames.urlFields.scoredQuizJSON;
    } else {
      result += pathnames.urlFields.exerciseJSON;
      if (this.randomSeed !== null && this.randomSeed !== "" && this.randomSeed !== undefined) {
        result += `&${pathnames.urlFields.randomSeed}=${this.randomSeed}`;
      }
    }
    return result;
  }

  getCalculatorURLRequestInput(
    isScoredQuiz, inputFileName, inputCourseHome, inputTopicList
  ) {
    let result = "";
    result += this.getCalculatorURLRequestPartOne(isScoredQuiz) + "&";
    result += this.getCalculatorURLInput(
      inputFileName,
      inputCourseHome,
      inputTopicList,
    );
    return result;
  }

  getCalculatorURLRequestFileCourseTopics(isScoredQuiz) {
    let result = "";
    result += this.getCalculatorURLRequestPartOne(isScoredQuiz);
    result += `&${this.getCalculatorURLFileCourseTopics()}&`;
    return result;
  }

  /** @return {HTMLElement} */
  getProblemNavigation() {
    let result = document.createElement("div");
    result.id = this.idNavigationProblemNotEntirePanel;
    result.className = 'problemNavigation';
    if (!window.calculator.mainPage.flagProblemPageOnly) {
      let children = this.getProblemNavigationContent();
      for (let i = 0; i < children.length; i++) {
        result.append(children[i]);
      }
    }
    return result;
  }

  /** @return {HTMLElement} */
  getNextProblemButton(
    /** @type{ProblemNavigationHints} */
    hints
  ) {
    if (
      this.nextProblemId === null ||
      this.nextProblemId === "" ||
      this.nextProblemId === undefined
    ) {
      return document.createTextNode("");
    }
    let nextProblem = allProblems.getProblemById(this.nextProblemId);
    let nextURL = nextProblem.getAppAnchorRequestFileCourseTopics(hints.isScoredQuiz, false);
    let nextProblemTag = document.createElement("a");
    nextProblemTag.className = hints.linkType;
    nextProblemTag.href = `#${nextURL}`;
    nextProblemTag.addEventListener(
      "click",
      () => {
        selectCurrentProblem(this.nextProblemId, hints.defaultRequest);
      }
    );
    nextProblemTag.innerHTML = "&#8594;";
    return nextProblemTag;
  }

  /** @return{HTMLElement} */
  getPreviousProblemButton(
    /** @type{ProblemNavigationHints} */
    hints
  ) {
    let previousLink = document.createElement("a");
    let enabled = true;
    if (
      this.previousProblemId === null ||
      this.previousProblemId === "" ||
      this.previousProblemId === undefined
    ) {
      enabled = false;
    }
    previousLink.className = hints.linkType;
    previousLink.innerHTML = "&#8592;";
    if (enabled) {
      let previousProblem = allProblems.getProblemById(this.previousProblemId);
      let previousURL = previousProblem.getAppAnchorRequestFileCourseTopics(hints.isScoredQuiz, false);
      previousLink.href = `#${previousURL}`;
      previousLink.addEventListener(
        "click",
        () => {
          selectCurrentProblem(this.previousProblemId, hints.defaultRequest);
        }
      );
    } else {
      previousLink.style.visibility = "hidden";
    }
    return previousLink;
  }

  /** @return{ProblemNavigationHints} */
  getProblemNavigationHints() {
    let page = window.calculator.mainPage;
    let result = new ProblemNavigationHints();
    if (this.flagForReal && page.user.flagLoggedIn && !page.user.flagDatabaseInactiveEveryoneIsAdmin) {
      result.defaultRequest = pathnames.urlFields.scoredQuizJSON;
      result.linkType = "problemLinkQuiz";
      result.isScoredQuiz = true;
    }
    return result;
  }

  /** @return {HTMLElement[]} */
  getProblemNavigationContent() {
    let page = window.calculator.mainPage;
    let result = [];
    let hints = this.getProblemNavigationHints();
    result.push(this.getPreviousProblemButton(hints));
    if (this.flagForReal) {
      let practiceURL = this.getAppAnchorRequestFileCourseTopics(false, false);
      let practiceTag = document.createElement("a");
      practiceTag.className = "problemLinkPractice";
      practiceTag.href = "#" + practiceURL;
      practiceTag.addEventListener("click", () => {
        selectCurrentProblem(this.problemId, "exerciseJSON");
      });
      practiceTag.innerHTML = "Practice";
      result.push(practiceTag);
    } else {
      let selectedPracticeTag = document.createElement("span");
      selectedPracticeTag.className = "problemLinkSelectedPractice";
      selectedPracticeTag.style.color = "green";
      selectedPracticeTag.innerHTML = "Practice";
      result.push(selectedPracticeTag)
    }
    if (
      !this.flagForReal &&
      page.user.flagLoggedIn &&
      !page.user.flagDatabaseInactiveEveryoneIsAdmin
    ) {
      let quizURL = this.getAppAnchorRequestFileCourseTopics(true, false);
      let quizTag = document.createElement("a");
      quizTag.className = "problemLinkQuiz";
      quizTag.href = "#" + quizURL;
      quizTag.addEventListener(
        "click",
        () => {
          selectCurrentProblem(this.problemId, "scoredQuizJSON");
        }
      );
      quizTag.innerHTML = "Quiz";
      result.push(quizTag);
    } else {
      if (this.flagForReal) {
        let quizTag = document.createElement("span");
        quizTag.className = "problemLinkSelectedQuiz";
        quizTag.style.color = "brown";
        quizTag.innerHTML = "Quiz";
        result.push(quizTag);
      }
    }
    result.push(this.getNextProblemButton(hints));
    if (this.flagForReal !== true && this.flagForReal !== "true") {
      let scoresTag = document.createElement("b");
      scoresTag.style.color = "green";
      scoresTag.innerHTML = "Scores not recorded.";
      result.push(scoresTag);
      result.push(document.createTextNode(" "));
      let randomSeedElement = document.createElement("span");
      randomSeedElement.id = ids.domElements.spanProblemLinkWithRandomSeed;
      let randomSeedAnchor = document.createElement("a");
      randomSeedAnchor.href = "#" + this.getAppAnchorRequestFileCourseTopics(false, true);
      randomSeedAnchor.innerHTML = this.randomSeed;
      randomSeedElement.appendChild(randomSeedAnchor);
      result.push(randomSeedElement);
    } else {
      let scoresTag = document.createElement("b");
      scoresTag.style.color = "brown";
      scoresTag.innerHTML = "Scores are recorded. ";
      result.push(scoresTag);
    }
    return result;
  }

  getEditPanel() {
    return editPage.getEditPanel(
      decodeURIComponent(this.problemId),
      this.hasInstructorRightsNotViewingAsStudent(),
      true,
    );
  }

  /** @return{HTMLElement[]} */
  getHTMLProblems() {
    let nextElement = document.createElement("div");
    nextElement.className = "bodySubsection";
    let table = document.createElement("table");
    nextElement.appendChild(table);
    table.className = "topicList";
    for (let i = 0; i < this.childrenIds.length; i++) {
      let currentProblem = allProblems.getProblemById(this.childrenIds[i]);
      let row = table.insertRow(- 1);
      currentProblem.getHTMLOneProblemTr(row);
    }
    return [nextElement];
  }

  toHTMLProblemNumber() {
    return document.createTextNode(this.problemNumberString + " ");
  }

  toHTMLTitle() {
    return document.createTextNode(this.title + " ");
  }

  /** @return{HTMLElement[]} */
  getHTMLSubSection() {
    let result = [];
    let nextElement = document.createElement("div");
    nextElement.className = "headSubsection";
    nextElement.textContent = "";
    nextElement.appendChild(this.toHTMLProblemNumber());
    nextElement.appendChild(this.toHTMLTitle());
    nextElement.appendChild(this.toHTMLDeadline());
    result.push(nextElement);
    miscellaneousFrontend.appendHtmlToArray(result, this.getHTMLProblems());
    return result;
  }

  isProblemContainer() {
    if (this.childrenIds.length !== undefined) {
      if (this.childrenIds.length > 0) {
        let currentProblem = allProblems.getProblemById(this.childrenIds[0]);
        if (currentProblem.type === "Problem") {
          return true;
        }
      }
    }
    return false;
  }

  /** @return{HTMLElement[]} */
  getHTMLSection() {
    let result = [];
    if (this.type === "Section") {
      let sectionElement = document.createElement("div");
      sectionElement.className = "headSection";
      sectionElement.appendChild(this.toHTMLProblemNumber());
      sectionElement.appendChild(this.toHTMLTitle());
      sectionElement.appendChild(this.toHTMLDeadline());
      result.push(sectionElement);
    }
    let nextElement = document.createElement("div");
    nextElement.className = "bodySection";
    result.push(nextElement);
    if (this.type === "Topic") {
      miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
    } else if (this.isProblemContainer()) {
      miscellaneousFrontend.appendHtml(nextElement, this.getHTMLProblems());
    } else if (this.type === "Section") {
      for (let i = 0; i < this.childrenIds.length; i++) {
        let currentSubSection = allProblems.getProblemById(this.childrenIds[i]);
        miscellaneousFrontend.appendHtml(nextElement, currentSubSection.getHTMLSubSection());
      }
    } else {
      miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
    }
    return result;
  }

  /** @return{HTMLElement[]} */
  toHTMLChapter() {
    let result = [];
    let headChapterElement = document.createElement("div");
    headChapterElement.className = "headChapter";
    headChapterElement.appendChild(this.toHTMLProblemNumber());
    headChapterElement.appendChild(this.toHTMLTitle());
    headChapterElement.appendChild(this.toHTMLDeadline());
    result.push(headChapterElement);
    let bodyChapterElement = document.createElement("div");
    bodyChapterElement.className = "bodyChapter";

    if (this.isProblemContainer()) {
      let incomingProblems = this.getHTMLProblems();
      for (let i = 0; i < incomingProblems.length; i++) {
        bodyChapterElement.appendChild(incomingProblems[i]);
      }
    } else {
      for (let i = 0; i < this.childrenIds.length; i++) {
        let currentSection = allProblems.getProblemById(this.childrenIds[i]);
        miscellaneousFrontend.appendHtml(bodyChapterElement, currentSection.getHTMLSection());
      }
    }
    result.push(bodyChapterElement);
    return result;
  }

  /** @return{HTMLElement[]} */
  getProblemMaterialLinks() {
    let result = [];
    this.links.slides = [];
    this.links.video = [];
    this.links.homework = [];
    if (
      this.video !== "" &&
      this.video !== undefined &&
      this.video !== null
    ) {
      let videoLink = document.createElement("a");
      videoLink.classList = "videoLink";
      videoLink.href = this.video;
      videoLink.target = "_blank";
      videoLink.innerHTML = "Video";
      this.links.video.push(videoLink);
    }
    if (
      this.querySlides !== "" &&
      this.querySlides !== null &&
      this.querySlides !== undefined
    ) {
      for (let counter in linkSlides) {
        this.links.slides.push(this.getLinkFromSpec(
          linkSpecs[linkSlides[counter]],
          this.querySlides,
        ));
      }
    }
    miscellaneousFrontend.appendHtmlToArray(result, this.links.slides);
    if (
      this.queryHomework !== "" &&
      this.queryHomework !== null &&
      this.queryHomework !== undefined
    ) {
      for (let counter in linkHomework) {
        miscellaneousFrontend.appendHtmlToArray(
          this.links.homework,
          this.getLinkFromSpec(
            linkSpecs[linkHomework[counter]],
            this.queryHomework
          ),
        );
      }
    }
    miscellaneousFrontend.appendHtmlToArray(result, this.links.homework);
    return result;
  }

  getHTMLOneProblemTr(
    outputRow,
  ) {
    let page = window.calculator.mainPage;
    let nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListTitle";
    nextCell.innerHTML = this.problemNumberString + " " + this.title;
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListMaterials";
    let materialLinks = this.getProblemMaterialLinks();
    miscellaneousFrontend.appendHtml(nextCell, materialLinks);
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListPracticeQuiz";
    if (this.fileName !== "") {
      if (page.user.flagLoggedIn) {
        let nextElement = document.createElement("a");
        nextElement.className = "problemLinkQuiz";
        nextElement.href = "#" + this.getAppAnchorRequestFileCourseTopics(true);
        nextElement.addEventListener(
          "click",
          () => {
            selectCurrentProblem(this.problemId, "scoredQuizJSON");
          }
        );
        nextElement.innerHTML = "Quiz";
        nextCell.appendChild(nextElement);
      }
      let nextElement = document.createElement("a");
      nextElement.className = "problemLinkPractice";
      nextElement.href = "#" + this.getAppAnchorRequestFileCourseTopics(false);
      nextElement.innerHTML = "Practice";
      nextElement.addEventListener(
        "click",
        () => {
          selectCurrentProblem(this.problemId, "exerciseJSON");
        }
      );
      nextCell.appendChild(nextElement);
    }
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListProblemWeight";
    let weightContent = this.getProblemWeightContent();
    miscellaneousFrontend.appendHtml(nextCell, weightContent)
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListDeadlines";
    nextCell.appendChild(this.toHTMLDeadline());
  }

  writeToHTML() {
    this.outputElement.textContent = "";
    problemNavigation.setCurrentProblemAndUpdate(this);
    let contentArray = [];
    miscellaneousFrontend.appendHtmlToArray(contentArray, this.badProblemExplanation);
    let problemBody = document.createElement("span");
    problemBody.innerHTML = this.decodedProblem + this.commentsProblem
    contentArray.push(problemBody);
    miscellaneousFrontend.appendHtml(this.outputElement, contentArray);
    for (let i = 0; i < this.answerPanels.length; i++) {
      let answer = this.answerPanels[i];
      let answerElement = document.getElementById(answer.input.answerPanelId);
      answer.writeToElement(answerElement, this.outputElement);
    }
    initializeButtons.initializeAccordionButtons();
    let elementWithScripts = null;
    try {
      elementWithScripts = dynamicJavascript.bootstrapAllScripts(this.outputElement);
    } catch (e) {
      console.log(e);
    }
    dynamicJavascript.typeset(
      this.outputElement, {}, (editor, element) => {
        if (elementWithScripts === null) {
          return;
        }
        elementWithScripts.bootstrapFormInputs(editor, element);
      });
  }

  hasInstructorRightsNotViewingAsStudent() {
    let page = window.calculator.mainPage;
    return page.hasInstructorRightsNotViewingAsStudent();
  }

  /** @return{HTMLElement} */
  toHTMLDeadlineElement() {
    let page = window.calculator.mainPage;
    let result = document.createElement("span");
    if (page.hasInstructorRightsNotViewingAsStudent()) {
      result.textContent = "Deadlines";
      return result;
    }
    if (page.user.hasInstructorRights() && page.studentView()) {
      let sectionIndex = page.storage.variables.currentSectionComputed.getValue();
      this.deadlineString = this.deadlines[sectionIndex];
    }
    if (
      this.deadlineString === "" ||
      this.deadlineString === null ||
      this.deadlineString === undefined
    ) {
      if (
        this.parentIdURLed !== null &&
        this.parentIdURLed !== undefined &&
        this.parentIdURLed !== ""
      ) {
        let parentProblem = allProblems.getProblemById(this.parentIdURLed);
        let inheritedResult = parentProblem.toHTMLDeadlineElement();
        if (inheritedResult.textContent !== "") {
          if (!inheritedResult.textContent.contains("[inherited]")) {
            inheritedResult.appendChild(document.createTextNode(" [inherited]"));
          }
        }
        return inheritedResult;
      }
      return result;
    }
    this.deadline = new Date(this.deadlineString);
    let remainingInHours = (this.deadline - (new Date())) / 1000 / 60 / 60;
    remainingInHours += 24;
    if (this.isSolvedForSure()) {
      let deadline = document.createElement("b");
      deadline.textContent = this.deadline.toLocaleDateString()
      deadline.style.color = "green";
      result.appendChild(deadline);
      return result;
    }
    if (remainingInHours < 48 && remainingInHours >= 0) {
      let deadline = document.createElement("b");
      deadline.style.color = "red";
      deadline.textContent = `${this.deadline.toLocaleDateString()}, ~${remainingInHours.toFixed(1)} hrs left`;
      result.appendChild(deadline)
      return result;
    }
    if (remainingInHours < 0) {
      let deadline = document.createElement("b");
      deadline.style.color = "red";
      deadline.textContent = this.deadline.toLocaleDateString();
      result.appendChild(deadline);
      let passed = document.createElement("b");
      passed.textContent = "[passed]";
      result.appendChild(passed);
      return result;
    }
    let deadline = document.createElement("b");
    result.appendChild(deadline);
    return result;
  }

  /** @return{HTMLElement} */
  toHTMLDeadline() {
    let result = document.createElement("span");
    result.className = ids.domElements.classSpanDeadlineContainer;
    result.appendChild(this.toHTMLDeadlinePanel());
    return result;
  }

  /** @return{HTMLElement} */
  toHTMLDeadlinePanel() {
    let page = window.calculator.mainPage;
    let result = document.createElement("span");
    result.textContent = "";
    if (!page.user.hasInstructorRights() || page.studentView()) {
      result.appendChild(this.toHTMLDeadlineElement());
      return result;
    }
    if (this.type === "Problem" && this.fileName === "") {
      return result;
    }
    let button = document.createElement("button");
    button.className = "accordionLike";
    button.textContent = "";
    button.appendChild(this.toHTMLDeadlineElement());
    button.appendChild(document.createTextNode(" \u25C2"))
    result.append(button);
    let panelDeadlines = document.createElement("span");
    panelDeadlines.className = "panelDeadlines";
    button.addEventListener("click", () => {
      window.calculator.coursePage.toggleDeadline(
        panelDeadlines, this.problemId, button,
      );
    });
    result.append(panelDeadlines);
    let table = document.createElement("table");
    let headerRow = table.insertRow();
    let group = document.createElement("th");
    group.textContent = "Grp.";
    let deadline = document.createElement("th");
    deadline.textContent = "Deadline";
    headerRow.appendChild(group);
    headerRow.appendChild(deadline);
    for (let i = 0; i < page.user.sectionsTaught.length; i++) {
      let row = table.insertRow();
      let cell = row.insertCell();
      cell.textContent = page.user.sectionsTaught[i];
      let inputCell = row.insertCell();
      let input = document.createElement("input");
      input.type = "date";
      input.className = "datePicker";
      input.name = `datePicker${this.problemId}`;
      if (this.deadlines[i] !== "" && this.deadlines[i] !== undefined) {
        let deadline = this.deadlines[i];
        input.value = deadline;
      }
      inputCell.appendChild(input);
    }
    panelDeadlines.appendChild(table);
    let buttonSet = document.createElement("button");
    buttonSet.textContent = "Set";
    buttonSet.addEventListener("click", () => {
      window.calculator.coursePage.modifyDeadlines(this.problemId);
    });
    panelDeadlines.appendChild(buttonSet);
    let report = document.createElement("span");
    report.id = this.idModifyReportDeadline;
    panelDeadlines.appendChild(report);
    return result;
  }

  /** @return{HTMLElement} */
  toHTMLWeights() {
    let result = document.createElement("span");
    result.className = "panelProblemWeights";
    result.style.opacity = 1;
    result.style.maxHeight = "200px";
    result.appendChild(document.createTextNode("Pts: "));
    let textArea = document.createElement("textarea");
    textArea.className = "textareaStudentPoints";
    textArea.rows = 1;
    textArea.cols = 2;
    if (this.weight !== undefined && this.weight !== null) {
      textArea.textContent = this.weight;
    }
    result.appendChild(textArea);
    let button = document.createElement("button");
    let reportSpan = document.createElement("span");
    button.addEventListener("click", () => { 
      this.modifyWeight(textArea, reportSpan);
    });
    button.textContent = "Modify";
    result.appendChild(button);
    result.appendChild(document.createElement("br"));
    result.appendChild(reportSpan);
    return result;
  }

  modifyWeight(
    /** @type {HTMLTextAreaElement} */
    textArea,
    /** @type {HTMLElement} */
    reportSpan,
  ) {
    let incomingPoints = textArea.value;
    let modifyObject = {};
    let idDecoded = decodeURIComponent(this.problemId);
    modifyObject[idDecoded] = {
      weight: incomingPoints
    };
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?`;
    url += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemWeight}&`;
    url += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(modifyObject))}`;
    submitRequests.submitGET({
      url: url,
      progress: ids.domElements.spanProgressReportGeneral,
      result: reportSpan,
    });
  }

  /** @return{HTMLElement[]} */
  getProblemWeightContent() {
    let page = window.calculator.mainPage;
    if (this.type !== "Problem" || this.fileName === "") {
      return [];
    }
    let content = document.createElement("span");
    if (!page.user.hasInstructorRights() || page.studentView()) {
      content.innerHTML = this.toStringProblemWeight();
      return [content];
    }

    let button = document.createElement("button");
    button.className = "accordionLikeProblemWeight";
    button.innerHTML = `${this.toStringProblemWeight()} &#9666`;
    button.name = this.problemId;
    button.addEventListener("click", () => { 
      window.calculator.coursePage.toggleProblemWeights();
    });
    content.appendChild(button);
    content.appendChild(document.createElement("br"));
    content.appendChild(this.toHTMLWeights());
    return [content];
  }

  isSolvedForSure() {
    if (this.correctlyAnswered === undefined || this.correctlyAnswered === NaN) {
      return false;
    }
    if (
      this.correctlyAnswered >= this.totalQuestions &&
      this.totalQuestions !== undefined &&
      this.totalQuestions !== 0
    ) {
      return true;
    }
    return false;
  }

  toStringProblemWeight() {
    let result = "";
    let color = "brown";
    if (this.correctlyAnswered !== undefined && this.correctlyAnswered !== NaN) {
      if (this.isSolvedForSure()) {
        color = "green";
      }
      let numCorrectlyAnswered = this.correctlyAnswered;
      let totalQuestions = this.totalQuestions;
      if (totalQuestions === 0) {
        totalQuestions = "?";
      }
      result += `${numCorrectlyAnswered} out of ${totalQuestions}`;
      if (this.weight !== undefined && this.totalQuestions !== 0) {
        let problemWeightConverted = parseInt(this.weight);
        let points = ((0.0 + this.correctlyAnswered * problemWeightConverted) / this.totalQuestions);
        points = Number(points.toFixed(2));
        result += ` = ${points} pts`;
        if (this.correctlyAnswered < this.totalQuestions) {
          color = "red";
        }
      } else {
        result += ` = ? pts`;
      }
    }
    let page = window.calculator.mainPage;
    if (page.user.hasInstructorRights() && !page.studentView() && result === "") {
      result = "modify";
    }
    return `<b style = "color:${color}">${result}</b>`;
  }

  /** @return{HTMLElement} */
  getLinkFromSpec(
    /** @type {{request: string, name: string, extension: string, options: string, download: boolean}} */
    linkSpec,
    /** @type {string} */
    query
  ) {
    if (linkSpec.adminView === true) {
      let studentView = window.calculator.mainPage.storage.variables.flagStudentView.getValue();
      if (studentView !== false && studentView !== "false") {
        return null;
      }
    }
    let href = "";
    href += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${linkSpec.request}&`;
    href += `${query}&${linkSpec.options}`;
    let result = document.createElement("a");
    result.className = "slidesLink";
    result.href = href;
    let extension = linkSpec.extension;
    if (extension === undefined || extension === null) {
      extension = "";
    }
    result.download = `${convertStringToLaTeXFileName(this.title)}${extension}`;
    result.target = "_blank";
    result.innerHTML = linkSpec.name;
    return result;
  }
}

function getCalculatorURLRequestFileCourseTopicsFromStorage() {
  let page = window.calculator.mainPage;
  let currentCourse = page.storage.variables.currentCourse;
  let exerciseType = currentCourse.exerciseType.getValue();
  if (exerciseType === "" || exerciseType === null || exerciseType === undefined) {
    exerciseType = pathnames.urlFields.exerciseJSON;
    currentCourse.exerciseType.setAndStore(exerciseType);
  }
  let fileName = currentCourse.fileName.getValue();
  let topicList = currentCourse.topicList.getValue();
  let courseHome = currentCourse.courseHome.getValue();
  let environmentRandomSeed = currentCourse.randomSeed.getValue();
  let result = "";
  result += `${pathnames.urls.calculatorAPI}?`;
  result += `${pathnames.urlFields.request}=${exerciseType}&fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}&`;
  if (
    environmentRandomSeed !== null &&
    environmentRandomSeed !== "" &&
    environmentRandomSeed !== undefined
  ) {
    result += `randomSeed=${environmentRandomSeed}&`;
  }
  currentCourse.randomSeed.value = "";
  return result;
}

function ProblemNavigationHints() {
  this.linkType = "problemLinkPractice";
  this.defaultRequest = pathnames.urlFields.exerciseJSON;
  this.isScoredQuiz = false;
}

class ProblemNavigation {
  constructor() {
    /** @type{Problem} */
    this.currentProblem = null;
    /** @type{HTMLElement|null}*/
    this.infoBar = document.getElementById(ids.domElements.divProblemInfoBar);
  }

  setCurrentProblemAndUpdate(
    /** @type{Problem} */
    inputProblem,
  ) {
    this.currentProblem = inputProblem;
    this.writeToHTML();
  }

  writeToHTML() {
    if (this.currentProblem === null) {
      return;
    }
    if (this.infoBar === null) {
      return;
    }
    let problemTitle = document.createElement("DIV");
    problemTitle.className = "problemTitle";

    let problemTitleContainer = document.createElement("DIV");
    problemTitleContainer.className = "problemTitleContainer";
    problemTitle.appendChild(problemTitleContainer);
    if (
      this.currentProblem.problemLabel !== undefined &&
      this.currentProblem.problemLabel !== "" &&
      this.currentProblem.problemLabel !== null
    ) {
      problemTitleContainer.appendChild(document.createTextNode(this.currentProblem.problemLabel));
    }
    problemTitleContainer.appendChild(document.createTextNode(this.currentProblem.title));

    this.infoBar.textContent = "";
    if (!window.calculator.mainPage.flagProblemPageOnly) {
      this.infoBar.appendChild(this.currentProblem.getProblemNavigation());
    }

    if (
      this.currentProblem.links !== undefined &&
      this.currentProblem.links !== null
    ) {
      if (this.currentProblem.links.slides !== null) {
        miscellaneousFrontend.appendHtml(problemTitle, this.currentProblem.links.slides);
      }
      if (this.currentProblem.links.video !== null) {
        miscellaneousFrontend.appendHtml(problemTitle, this.currentProblem.links.video);
      }
    }
    problemTitle.appendChild(this.currentProblem.getEditPanel());
    this.infoBar.appendChild(problemTitle);
    typeset.typesetter.typesetSoft(this.infoBar, "");
  }
}

function modifyWeight(id) {
  let theProblemWeight = allProblems.allProblems[id];
  theProblemWeight.modifyWeight();
}

function convertStringToLaTeXFileName(input) {
  let result = encodeURIComponent(input.split(" ").join("-")).split("%").join("-");
  if (result.length === 0) {
    return "undefined";
  }
  if (result[0] === "-") {
    result = "L" + result;
  }
  return result;
}

let linkSpecs = {
  slidesProjector: {
    request: pathnames.urlFields.requests.slidesFromSource,
    name: "Slides",
    extension: ".pdf",
    options: "layout=projector&",
  },
  slidesPrintable: {
    request: pathnames.urlFields.requests.slidesFromSource,
    name: "Printable",
    extension: ".pdf",
    options: "layout=printable&",
  },
  slidesTex: {
    request: pathnames.urlFields.requests.sourceSlides,
    name: ".tex",
    extension: ".tex",
    options: "",
    download: true,
    adminView: true,
  },
  homeworkWithAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    extension: ".pdf",
    name: "HW",
    options: "answerKey=false&",
  },
  homeworkNoAnswers: {
    request: pathnames.urlFields.requests.homeworkFromSource,
    name: "HW+answ.",
    extension: ".pdf",
    options: "answerKey=true&",
  },
  homeworkTex: {
    request: pathnames.urlFields.requests.sourceHomework,
    name: ".tex",
    extension: ".pdf",
    options: "",
    download: true,
    adminView: true,
  },
};

let linkSlides = [
  "slidesProjector",
  "slidesPrintable",
  "slidesTex",
];

let linkHomework = [
  "homeworkWithAnswers",
  "homeworkNoAnswers",
  "homeworkTex",
];

/** @return{HTMLElement[]} */
function getHTMLfromTopics() {
  let result = [];
  for (let label in allProblems.theChapterIds) {
    let currentProblem = allProblems.getProblemById(label);
    result.push(currentProblem.toHTMLChapter());
  }
  let extraHtml = miscellaneousFrontend.htmlFromCommentsAndErrors(allProblems.topics);
  if (extraHtml !== "") {
    let extraNode = document.createElement("span");
    extraNode.innerHTML = extraHtml;
    result.push(extraNode);
  }
  return result;
}

function initializeDatePickers() {
  let thePickers = document.getElementsByClassName("datePicker");
  for (let counterPicker = 0; counterPicker < thePickers.length; counterPicker++) {
    datePicker.createDatePicker(thePickers[counterPicker]);
  }
}

function initializeProblemWeightsAndDeadlines() {
  let weights = document.getElementsByClassName('panelProblemWeights');
  for (let i = 0; i < weights.length; i++) {
    weights[i].style.maxHeight = '0px';
  }
  let deadlines = document.getElementsByClassName('panelDeadlines');
  for (let i = 0; i < deadlines.length; i++) {
    deadlines[i].style.maxHeight = '0px';
  }
}

function writeEditCoursePagePanel() {
  let page = window.calculator.mainPage;
  let panel = document.getElementById(ids.domElements.courseEditPanel);
  panel.innerHTML = "";
  let courseHome = page.storage.variables.currentCourse.courseHome.getValue()
  panel.appendChild(editPage.getEditPanel(courseHome, page.hasInstructorRightsNotViewingAsStudent(), false));
  let topicList = page.storage.variables.currentCourse.topicList.getValue();
  panel.appendChild(editPage.getEditPanel(topicList, page.hasInstructorRightsNotViewingAsStudent(), false));
  if (
    allProblems.topics.topicBundleFile !== undefined &&
    allProblems.topics.topicBundleFile !== null &&
    allProblems.topics.topicBundleFile !== ""
  ) {
    for (
      let counterTopicBundle = 0;
      counterTopicBundle < allProblems.topics.topicBundleFile.length;
      counterTopicBundle++
    ) {
      let nextToEdit = allProblems.topics.topicBundleFile[counterTopicBundle];
      panel.appendChild(editPage.getEditPanel(nextToEdit, page.hasInstructorRightsNotViewingAsStudent(), false));
    }
  }
}

function processLoadedTopicsWriteToEditPage(incomingTopics, result) {
  allProblems.processLoadedTopics(incomingTopics, result);
  editPage.selectEditPage(null);
}

function processLoadedTopicsWriteToCoursePage(incomingTopics, result) {
  allProblems.processLoadedTopicsWriteToCoursePage(incomingTopics, result);
}

function writeTopicsToCoursePage() {
  let page = window.calculator.mainPage;
  let topicsElements = document.getElementsByTagName("topicList");
  writeEditCoursePagePanel();
  let htmlContentElements = getHTMLfromTopics();
  let extraComments = miscellaneousFrontend.htmlElementsFromCommentsAndErrors(allProblems.topics);
  miscellaneousFrontend.appendHtml(topicsElements[0], extraComments);
  miscellaneousFrontend.appendHtml(topicsElements[0], htmlContentElements);
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  if (page.pages.problemPage.flagLoaded) {
    problemNavigation.writeToHTML();
  }
  typeset.typesetter.typesetSoft(topicsElements[0], "");
}

function updateProblemPage() {
  allProblems.updateProblemPage();
}

function loadProblemIntoElement(
  /** @type{HTMLElement|string} */
  problemElement,
) {
  if (typeof problemElement === "string") {
    problemElement = document.getElementById(problemElement);
  }
  let problemFileName = problemElement.getAttribute("problem");
  let problem = allProblems.getProblemByIdOrRegisterEmpty(problemFileName, problemElement);
  problem.flagForReal = false;
  if (problemElement.getAttribute("forReal") === "true") {
    problem.flagForReal = true;
  }
  let url = `${pathnames.urls.calculatorAPI}?${problem.getCalculatorURLRequestFileCourseTopics(problem.flagForReal)}`;
  submitRequests.submitGET({
    url: url,
    callback: (input, outputComponent) => {
      problem.writeProblemPage(input, outputComponent);
    },
    progress: ids.domElements.spanProgressReportGeneral,
    result: problemElement,
  });
}

/** @return{Problem|null} */
function getCurrentProblem() {
  let problemFileName = storage.storage.variables.currentCourse.problemFileName.getValue();
  if (
    problemFileName === "" ||
    problemFileName === null ||
    problemFileName === undefined
  ) {
    return null;
  }
  let element = document.getElementById(ids.domElements.problemPageContentContainer);
  return allProblems.getProblemByIdOrRegisterEmpty(problemFileName, element);
}

let problemNavigation = new ProblemNavigation();
let allProblems = new ProblemCollection();

module.exports = {
  Problem,
  allProblems,
  problemNavigation,
  updateProblemPage,
  loadProblemIntoElement,
  processLoadedTopicsWriteToCoursePage,
  processLoadedTopicsWriteToEditPage,
  writeEditCoursePagePanel,
  selectCurrentProblem,
  modifyWeight,
};