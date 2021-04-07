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
    this.theTopics = {};
  }

  /** @returns{Problem} */
  getProblemById(
    /**@type{string} */
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

  /** @returns{Problem} */
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

  /**@return{Problem} */
  getProblemByIdOrRegisterEmpty(
    problemFileName,
    /**@type{HTMLElement} */
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
}

function selectCurrentProblem(problemIdURLed, exerciseType) {
  let thePage = window.calculator.mainPage;
  thePage.storage.variables.currentCourse.problemFileName.setAndStore(decodeURIComponent(problemIdURLed));
  thePage.storage.variables.currentCourse.exerciseType.setAndStore(exerciseType);
  let theProblem = getCurrentProblem();
  theProblem.flagForReal = false;
  if (
    exerciseType === pathnames.urlFields.scoredQuizJSON &&
    !thePage.user.flagDatabaseInactiveEveryoneIsAdmin
  ) {
    theProblem.flagForReal = true;
  }
  thePage.pages.problemPage.flagLoaded = false;
  thePage.selectPage(thePage.pages.problemPage.name);
}

class Problem {
  constructor(
    /** @type{HTMLElement}*/
    outputElement,
  ) {
    /**@type{string}*/
    this.nextProblemId = "";
    /**@type{string} */
    this.previousProblemId = "";
    /**@type{number} */
    this.totalChildren = 0;
    /** @type{HTMLElement}*/
    this.outputElement = outputElement;
    /**@type{string} */
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
    if (incomingRandomSeed === "" || incomingRandomSeed === null || incomingRandomSeed === undefined) {
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
     * This id is for problem navigation only, does not include the entire panel.
     */
    this.idNavigationProblemNotEntirePanel = `navigationPanel${this.problemId}`;
    /**@type {AnswerPanel[]} */
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

  /** @returns{number} Number of children processed. */
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
    this.idButtonPoints = `modifyPoints${this.problemId}`;
    this.idTextareaPoints = `points${this.problemId}`;
    this.idModifyReportDeadline = `deadlines${this.problemId}`;
    this.idDeadlinePanel = `deadlinesPanel${this.problemId}`;
    this.idDeadlineContainer = `${ids.stringResources.prefixDeadlineContainer}${this.problemId}`;
    this.idModifyReportPoints = `report${this.problemId}`;
    this.correctlyAnswered = problemData.correctlyAnswered;
    this.totalQuestions = problemData.totalQuestions;
    this.deadlines = [];
    this.deadlineString = null;
    this.deadline = null;
    this.weight = problemData.weight;
    this.links = {
      /**@type{HTMLElement[]} */
      video: [],
      /**@type{HTMLElement[]} */
      slides: [],
      /**@type{HTMLElement[]} */
      homework: [],
    };
    /**@type{HTMLElement[]} */
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
    if (Array.isArray(problemData.children)) {
      for (let counterChildren = 0; counterChildren < problemData.children.length; counterChildren++) {
        const currentChild = allProblems.createOrUpdateProblem(problemData.children[counterChildren]);
        this.totalChildren += currentChild.totalChildren + 1;
        this.childrenIds.push(currentChild.problemId);
      }
    }
  }

  /**@returns{HTMLElement[]} */
  computeBadProblemExplanation() {
    let userHasInstructorRights = true;
    let pageLastKnownGoodProblemName = "";
    let thePage = window.calculator.mainPage;
    if (!thePage.flagProblemPageOnly) {
      userHasInstructorRights = thePage.user.hasInstructorRights();
    }
    this.badProblemExplanation = [];
    if (!this.decodedProblem.includes(pathnames.urlFields.problem.failedToLoadProblem)) {
      thePage.lastKnownGoodProblemFileName = this.fileName;
      return;
    }
    pageLastKnownGoodProblemName = thePage.lastKnownGoodProblemFileName;
    let badProblemExplanationPartOne = document.createElement("div");
    badProblemExplanationPartOne.innerHTML += "It appears your problem failed to load.<br>";
    if (this.lastKnownGoodProblemFileName !== "" && userHasInstructorRights) {
      badProblemExplanationPartOne.innerHTML += "Perhaps you may like to clone the last good known problem.<br>";
    }
    this.badProblemExplanation.push(badProblemExplanationPartOne);
    miscellaneousFrontend.appendHtmlToArray(this.badProblemExplanation, editPage.getClonePanel(pageLastKnownGoodProblemName, this.fileName));
    let epilogueElement = document.createElement("hr");
    this.badProblemExplanation.push(epilogueElement);
  }

  initializeProblemContent(problemData) {
    if (pathnames.standardResponses.isNotLoggedInResponse(problemData)) {
      this.title = "";
      this.decodedProblem = "Not logged in. Please select practice mode or login.";
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
    for (let counterAnswers = 0; counterAnswers < answerVectors.length; counterAnswers++) {
      let currentVector = answerVectors[counterAnswers];
      this.answerPanels[counterAnswers] = new AnswerPanel({
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
    /**@type {boolean} */
    isScoredQuiz,
    /**@type {boolean} */
    includeRandomSeed,
  ) {
    let thePage = window.calculator.mainPage;
    let exerciseType = pathnames.urlFields.exerciseJSON;
    if (isScoredQuiz && !thePage.user.flagDatabaseInactiveEveryoneIsAdmin) {
      exerciseType = pathnames.urlFields.scoredQuizJSON;
    }
    let requestJSON = {
      currentPage: thePage.pages.problemPage.name,
      exerciseType: exerciseType,
      problemFileName: this.fileName,
      courseHome: thePage.storage.variables.currentCourse.courseHome.getValue(),
      topicList: thePage.storage.variables.currentCourse.topicList.getValue(),
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
    let stringifiedHash = thePage.storage.getPercentEncodedURL(requestJSON);
    return stringifiedHash;
  }

  getCalculatorURLFileCourseTopics() {
    let thePage = window.calculator.mainPage;
    return this.getCalculatorURLInput(
      this.fileName,
      thePage.storage.variables.currentCourse.courseHome.getValue(),
      thePage.storage.variables.currentCourse.topicList.getValue(),
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
    let thePage = window.calculator.mainPage;
    let result = "";
    if (isScoredQuiz === undefined) {
      isScoredQuiz = this.flagForReal;
    }
    result += `${pathnames.urlFields.request}=`;
    if (isScoredQuiz && !thePage.user.flagDatabaseInactiveEveryoneIsAdmin) {
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

  /**@returns {HTMLElement} */
  getProblemNavigation() {
    let result = document.createElement("DIV");
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
  /**@returns {HTMLElement} */
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

  /** @returns {HTMLElement} */
  getPreviousProblemButton(
    /** @type{ProblemNavigationHints} */
    hints
  ) {
    if (
      this.previousProblemId === null ||
      this.previousProblemId === "" ||
      this.previousProblemId === undefined
    ) {
      return document.createTextNode("");
    }
    let previousProblem = allProblems.getProblemById(this.previousProblemId);
    let previousURL = previousProblem.getAppAnchorRequestFileCourseTopics(hints.isScoredQuiz, false);
    let previousLink = document.createElement("a");
    previousLink.className = hints.linkType;
    previousLink.href = `#${previousURL}`;
    previousLink.addEventListener(
      "click",
      () => {
        selectCurrentProblem(this.previousProblemId, hints.defaultRequest);
      }
    );
    previousLink.innerHTML = "&#8592;";
    return previousLink;
  }

  /** @returns{ProblemNavigationHints} */
  getProblemNavigationHints() {
    let thePage = window.calculator.mainPage;
    let result = new ProblemNavigationHints();
    if (this.flagForReal && thePage.user.flagLoggedIn && !thePage.user.flagDatabaseInactiveEveryoneIsAdmin) {
      result.defaultRequest = pathnames.urlFields.scoredQuizJSON;
      result.linkType = "problemLinkQuiz";
      result.isScoredQuiz = true;
    }
    return result;
  }

  /**@returns {HTMLElement[]} */
  getProblemNavigationContent() {
    let thePage = window.calculator.mainPage;
    let result = [];
    let hints = this.getProblemNavigationHints();
    result.push(this.getPreviousProblemButton(hints));
    if (this.flagForReal && thePage.user.flagLoggedIn) {
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
    if (!this.flagForReal && thePage.user.flagLoggedIn && !thePage.user.flagDatabaseInactiveEveryoneIsAdmin) {
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
      let randomSeedElement = document.createElement("SPAN");
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
    return editPage.getEditPanel(decodeURIComponent(this.problemId));
  }

  /**@returns{HTMLElement[]} */
  getHTMLProblems() {
    let nextElement = document.createElement("div");
    nextElement.className = "bodySubsection";
    let table = document.createElement("table");
    nextElement.appendChild(table);
    table.className = "topicList";
    for (let counterSubSection = 0; counterSubSection < this.childrenIds.length; counterSubSection++) {
      let currentProblem = allProblems.getProblemById(this.childrenIds[counterSubSection]);
      let row = table.insertRow(- 1);
      currentProblem.getHTMLOneProblemTr(row);
    }
    return [nextElement];
  }

  /**@returns{HTMLElement[]} */
  getHTMLSubSection() {
    let result = [];
    let nextElement = document.createElement("div");
    nextElement.className = "headSubsection";
    nextElement.innerHTML = this.problemNumberString + " " + this.title + " " + this.toStringDeadlineContainer();
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

  /** @returns{HTMLElement[]} */
  getHTMLSection() {
    let result = [];
    if (this.type === "Section") {
      let sectionElement = document.createElement("div");
      sectionElement.className = "headSection";
      sectionElement.innerHTML = this.problemNumberString + " " + this.title + " " + this.toStringDeadlineContainer();
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
      for (let counterSection = 0; counterSection < this.childrenIds.length; counterSection++) {
        let currentSubSection = allProblems.getProblemById(this.childrenIds[counterSection]);
        miscellaneousFrontend.appendHtml(nextElement, currentSubSection.getHTMLSubSection());
      }
    } else {
      miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
    }
    return result;
  }

  /**@returns{HTMLElement[]} */
  toHTMLChapter() {
    let result = [];
    let headChapterElement = document.createElement("div");
    headChapterElement.className = "headChapter";
    headChapterElement.innerHTML = this.problemNumberString + " " + this.title + " " + this.toStringDeadlineContainer();
    result.push(headChapterElement);
    let bodyChapterElement = document.createElement("div");
    bodyChapterElement.className = "bodyChapter";

    if (this.isProblemContainer()) {
      let incomingProblems = this.getHTMLProblems();
      for (let i = 0; i < incomingProblems.length; i++) {
        bodyChapterElement.appendChild(incomingProblems[i]);
      }
    } else {
      for (let counterSection = 0; counterSection < this.childrenIds.length; counterSection++) {
        let currentSection = allProblems.getProblemById(this.childrenIds[counterSection]);
        miscellaneousFrontend.appendHtml(bodyChapterElement, currentSection.getHTMLSection());
      }
    }
    result.push(bodyChapterElement);
    return result;
  }

  /**@returns {HTMLElement[]} */
  getProblemMaterialLinks() {
    let result = [];
    this.links.slides = [];
    this.links.video = [];
    this.links.homework = [];
    if (this.video !== "" && this.video !== undefined && this.video !== null) {
      let videoLink = document.createElement("a");
      videoLink.classList = "videoLink";
      videoLink.href = this.video;
      videoLink.target = "_blank";
      videoLink.innerHTML = "Video";
      this.links.video.push(videoLink);
    }
    if (this.querySlides !== "" && this.querySlides !== null && this.querySlides !== undefined) {
      for (let counter in linkSlides) {
        this.links.slides.push(this.getLinkFromSpec(linkSpecs[linkSlides[counter]], this.querySlides));
      }
    }
    miscellaneousFrontend.appendHtmlToArray(result, this.links.slides);
    if (this.queryHomework !== "" && this.queryHomework !== null && this.queryHomework !== undefined) {
      for (let counter in linkHomework) {
        miscellaneousFrontend.appendHtmlToArray(this.links.homework, this.getLinkFromSpec(linkSpecs[linkHomework[counter]], this.queryHomework));
      }
    }
    miscellaneousFrontend.appendHtmlToArray(result, this.links.homework);
    return result;
  }

  getHTMLOneProblemTr(
    outputRow,
  ) {
    let thePage = window.calculator.mainPage;
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
      if (thePage.user.flagLoggedIn) {
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
    nextCell.innerHTML = this.toStringDeadlineContainer();
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
    for (let counterAnswers = 0; counterAnswers < this.answerPanels.length; counterAnswers++) {
      let answer = this.answerPanels[counterAnswers];
      let answerElement = document.getElementById(answer.input.answerPanelId);
      answer.writeToElement(answerElement, this.outputElement);
    }
    initializeButtons.initializeAccordionButtons();
    dynamicJavascript.typeset(this.outputElement)
    dynamicJavascript.bootstrapAllScripts(this.outputElement);
  }

  toStringDeadline() {
    let thePage = window.calculator.mainPage;
    if (thePage.user.hasInstructorRights() && !thePage.studentView()) {
      return "Deadlines";
    }
    if (thePage.user.hasInstructorRights() && thePage.studentView()) {
      let sectionIndex = thePage.storage.variables.currentSectionComputed.getValue();
      this.deadlineString = this.deadlines[sectionIndex];
    }
    if (this.deadlineString === "" || this.deadlineString === null || this.deadlineString === undefined) {
      if (this.parentIdURLed !== null && this.parentIdURLed !== undefined && this.parentIdURLed !== "") {
        let parentProblem = allProblems.getProblemById(this.parentIdURLed);
        let inheritedResult = parentProblem.toStringDeadline();
        if (inheritedResult !== "") {
          if (!inheritedResult.endsWith("[inherited]")) {
            inheritedResult += " [inherited]";
          }
        }
        return inheritedResult;
      }
      return "";
    }
    this.deadline = new Date(this.deadlineString);
    let remainingInHours = (this.deadline - (new Date())) / 1000 / 60 / 60;
    remainingInHours += 24;
    let resultString = "";
    if (this.isSolvedForSure()) {
      resultString += "<b style='color:green'>" + this.deadline.toLocaleDateString() + "</b>";
      return resultString;
    }
    if (remainingInHours < 48 && remainingInHours >= 0) {
      resultString += "<b style='color:red'>" + this.deadline.toLocaleDateString();
      resultString += `, ~${remainingInHours.toFixed(1)} hrs left</b>`;
      return resultString;
    }
    if (remainingInHours < 0) {
      resultString += "<b style='color:red'>" + this.deadline.toLocaleDateString() + "</b>";
      resultString += "<b>[passed]</b>";
      return resultString;
    }
    resultString += "<b>" + this.deadline.toLocaleDateString() + "</b>";
    return resultString;
    //  this.deadlines[];
  }

  toStringDeadlineContainer() {
    let result = "";
    result += `<span id='${this.idDeadlineContainer}' class='${ids.domElements.classSpanDeadlineContainer}'>${this.toStringDeadlinePanel()}</span>`;
    return result;
  }

  toStringDeadlinePanel() {
    let thePage = window.calculator.mainPage;
    if (!thePage.user.hasInstructorRights() || thePage.studentView()) {
      return this.toStringDeadline();
    }
    if (this.type === "Problem" && this.fileName === "") {
      return "";
    }
    let result = "";
    result += `<button class="accordionLike" `;
    result += `onclick="window.calculator.coursePage.toggleDeadline('${this.idDeadlinePanel}', '${this.problemId}', this);">`;
    result += `${this.toStringDeadline()} &#9666;</button>`;
    result += `<span class="panelDeadlines" id="${this.idDeadlinePanel}">`;
    result += "<table>";
    result += "<tr><th>Grp.</th><th>Deadline</th></tr>";
    for (let counterGroup = 0; counterGroup < thePage.user.sectionsTaught.length; counterGroup++) {
      result += `<tr><td>${thePage.user.sectionsTaught[counterGroup]}</td>`;
      result += `<td><input type="date" class="datePicker" name="datePicker${this.problemId}" `;
      if (this.deadlines[counterGroup] !== "" && this.deadlines[counterGroup] !== undefined) {
        let deadline = this.deadlines[counterGroup];
        result += `value = "${deadline}"`;
      }
      result += `></input></td></tr>`;
    }
    result += "</table>";
    //console.log("Problem data problem: " + JSON.stringify(this.fileName));
    //console.log("Problem data title: " + JSON.stringify(this.title));
    result += `<button onclick = "window.calculator.coursePage.modifyDeadlines('${this.problemId}')">Set</button> `;
    result += `<span id = '${this.idModifyReportDeadline}'></span>`;
    result += `</span>`;
    return result;
  }

  toHTMLWeights() {
    let result = "";
    result += "<span class='panelProblemWeights' style = 'opacity: 1; max-height: 200px;'>";
    result += `Pts: <textarea class='textareaStudentPoints' rows='1' cols='2' id='${this.idTextareaPoints}'>`;
    if (this.weight !== undefined && this.weight !== null) {
      result += this.weight;
    }
    result += "</textarea>";
    result += `<button id = '${this.idButtonPoints}' onclick = "window.calculator.problemPage.modifyWeight('${this.problemId}')">Modify</button><br>`;
    result += `<span id = '${this.idModifyReportPoints}'></span>`;
    result += "</span>";
    return result;
  }

  modifyWeight() {
    let problemWeightTextareaId = `points${this.problemId}`;
    let incomingPoints = document.getElementById(problemWeightTextareaId).value;
    let modifyObject = {};
    let idDecoded = decodeURIComponent(this.problemId);
    //let problemModifyWeightReport = `report${id}`;
    modifyObject[idDecoded] = {
      weight: incomingPoints
    };
    let theURL = "";
    theURL += `${pathnames.urls.calculatorAPI}?`;
    theURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.setProblemWeight}&`;
    theURL += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(modifyObject))}`;
    submitRequests.submitGET({
      url: theURL,
      progress: ids.domElements.spanProgressReportGeneral,
      result: this.idModifyReportPoints,
    });
  }

  /**@returns{HTMLElement[]} */
  getProblemWeightContent(inputRow) {
    let thePage = window.calculator.mainPage;
    if (this.type !== "Problem" || this.fileName === "") {
      return [];
    }
    let content = document.createElement("span");
    if (!thePage.user.hasInstructorRights() || thePage.studentView()) {
      content.innerHTML = this.toStringProblemWeight();
      return [content];
    }
    let pointsString = "";
    pointsString += `<button class = 'accordionLikeProblemWeight' onclick = "window.calculator.coursePage.toggleProblemWeights()" `;
    pointsString += `name = "${this.problemId}">${this.toStringProblemWeight()} &#9666;</button>`;
    let problemWeightString = this.toHTMLWeights();
    content.innerHTML = `${pointsString}<br> ${problemWeightString}`;
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
    return `<b style = "color:${color}">${result}</b>`;
  }

  /** @returns{HTMLElement} */
  getLinkFromSpec(
    /**@type {{request: string, name: string, extension: string, options: string, download: boolean}} */
    linkSpec,
    /**@type {string} */
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
  let thePage = window.calculator.mainPage;
  let currentCourse = thePage.storage.variables.currentCourse;
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
    /**@type{Problem} */
    this.currentProblem = null;
    /** @type{HTMLElement|null}*/
    this.infoBar = document.getElementById(ids.domElements.divProblemInfoBar);
  }

  setCurrentProblemAndUpdate(
    /**@type{Problem} */
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
    //topPart += "<br>"
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

/**@returns{HTMLElement[]} */
function getHTMLfromTopics() {
  let result = [];
  for (let label in allProblems.theChapterIds) {
    let currentProblem = allProblems.getProblemById(label);
    result.push(currentProblem.toHTMLChapter());
  }
  let extraHtml = miscellaneousFrontend.htmlFromCommentsAndErrors(allProblems.theTopics);
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
  let theWeights = document.getElementsByClassName('panelProblemWeights');
  for (let i = 0; i < theWeights.length; i++) {
    //theScores[i].style.transition ='opacity 1s linear';
    theWeights[i].style.maxHeight = '0px';
  }
  let theDeadlines = document.getElementsByClassName('panelDeadlines');
  for (let i = 0; i < theDeadlines.length; i++) {
    //theScores[i].style.transition ='opacity 1s linear';
    theDeadlines[i].style.maxHeight = '0px';
  }
}

function writeEditCoursePagePanel() {
  let thePage = window.calculator.mainPage;
  let panel = document.getElementById(ids.domElements.courseEditPanel);
  panel.innerHTML = "";
  let courseHome = thePage.storage.variables.currentCourse.courseHome.getValue()
  panel.appendChild(editPage.getEditPanel(courseHome));
  let topicList = thePage.storage.variables.currentCourse.topicList.getValue();
  panel.appendChild(editPage.getEditPanel(topicList));
  if (
    allProblems.theTopics.topicBundleFile !== undefined &&
    allProblems.theTopics.topicBundleFile !== null &&
    allProblems.theTopics.topicBundleFile !== ""
  ) {
    for (
      let counterTopicBundle = 0;
      counterTopicBundle < allProblems.theTopics.topicBundleFile.length;
      counterTopicBundle++
    ) {
      let nextToEdit = allProblems.theTopics.topicBundleFile[counterTopicBundle];
      panel.appendChild(editPage.getEditPanel(nextToEdit));
    }
  }
}

function processLoadedTopics(incomingTopics, result) {
  const startTime = new Date().getTime();
  allProblems.previousProblemId = null;
  allProblems.resetTopicProblems();
  allProblems.theTopics = miscellaneous.jsonUnescapeParse(incomingTopics);
  if (!Array.isArray(allProblems.theTopics["children"])) {
    return;
  }
  let totalChildren = 0;
  for (let counterChapter = 0; counterChapter < allProblems.theTopics["children"].length; counterChapter++) {
    const currentChapter = allProblems.theTopics["children"][counterChapter];
    const problem = allProblems.createOrUpdateProblem(currentChapter);
    totalChildren += problem.totalChildren;
  }
  const finalTime = new Date().getTime() - startTime;
  const timePerChild = finalTime / totalChildren;
  if (finalTime > 1000) {
    console.log(`Final time of processLoadedTopics: ${finalTime} ms for ${totalChildren} children at ${timePerChild.toFixed()} ms per child.`);
  }
}

function processLoadedTopicsWriteToEditPage(incomingTopics, result) {
  processLoadedTopics(incomingTopics, result);
  editPage.selectEditPage(null);
}

function processLoadedTopicsWriteToCoursePage(incomingTopics, result) {
  const startTime = new Date().getTime();
  processLoadedTopics(incomingTopics, result);
  writeTopicsToCoursePage();
  const writeTime = new Date().getTime() - startTime;
  if (writeTime > 1000) {
    console.log(`Less than a second to generate html expected, needed ${writeTime} ms instead.`);
  }
}

function writeTopicsToCoursePage() {
  let thePage = window.calculator.mainPage;
  let topicsElements = document.getElementsByTagName("topicList");
  writeEditCoursePagePanel();
  let htmlContentElements = getHTMLfromTopics();
  let extraComments = miscellaneousFrontend.htmlElementsFromCommentsAndErrors(allProblems.theTopics);
  miscellaneousFrontend.appendHtml(topicsElements[0], extraComments);
  miscellaneousFrontend.appendHtml(topicsElements[0], htmlContentElements);
  initializeProblemWeightsAndDeadlines();
  initializeDatePickers();
  if (thePage.pages.problemPage.flagLoaded) {
    problemNavigation.writeToHTML();
  }
  typeset.typesetter.typesetSoft(topicsElements[0], "");
}

function updateProblemPage() {
  let thePage = window.calculator.mainPage;
  window.calculator.coursePage.selectCurrentCoursePage();
  // thePage.pages.problemPage.flagLoaded is modified by the following
  // functions: selectCurrentProblem, logout, callbackClone,
  // the present function updateProblemPage
  /**@type {Problem} */
  let problem = getCurrentProblem();
  if (thePage.pages.problemPage.flagLoaded) {
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
  let theURL;
  if (problem !== undefined && problem !== null) {
    let forReal = problem.flagForReal;
    theURL = `${pathnames.urls.calculatorAPI}?${problem.getCalculatorURLRequestFileCourseTopics(forReal)}`;
  } else {
    let fileName = thePage.storage.variables.currentCourse.fileName.getValue();
    if (fileName === "" || fileName === undefined || fileName === null) {
      let courseBody = document.getElementById(ids.domElements.problemPageContentContainer);
      let temporarySelectProblem = "buttonTemporarySelectProblem";
      let selectProblemHtml = "";
      selectProblemHtml += `Problems are selected from the <button id = '${temporarySelectProblem}'`;
      selectProblemHtml += `class = "buttonSelectPage buttonSlowTransition buttonFlash" style = "width:150px"`;
      selectProblemHtml += `onclick = "window.calculator.mainPage.selectPage('currentCourse')">Current course</button>`;
      selectProblemHtml += "<br>To select a problem, click Practice or Quiz within the course page. ";
      courseBody.innerHTML = selectProblemHtml;
      setTimeout(() => {
        document.getElementById(temporarySelectProblem).classList.remove("buttonFlash");
      }, 100);
      return;
    }
    theURL = getCalculatorURLRequestFileCourseTopicsFromStorage();
  }
  thePage.pages.problemPage.flagLoaded = true;
  submitRequests.submitGET({
    url: theURL,
    callback: (input, outputComponent) => {
      problem.writeProblemPage(input, outputComponent);
    },
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

function loadProblemIntoElement(
  /**@type{HTMLElement|string} */
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
  let theURL = `${pathnames.urls.calculatorAPI}?${problem.getCalculatorURLRequestFileCourseTopics(problem.flagForReal)}`;
  submitRequests.submitGET({
    url: theURL,
    callback: (input, outputComponent) => {
      problem.writeProblemPage(input, outputComponent);
    },
    progress: ids.domElements.spanProgressReportGeneral,
    result: problemElement,
  });
}

/**@returns{Problem|null} */
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