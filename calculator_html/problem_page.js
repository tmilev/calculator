"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const editPage = require("./edit_page");
const typeset = require("./math_typeset");
const miscellaneous = require("./miscellaneous_frontend");
const miscellaneousFrontend = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;
const AnswerPanel = require("./answer_panel").AnswerPanel;
const dynamicJavascript = require("./dynamic_javascript").dynamicJavascript;

class TopicCollection {
  constructor() {
    /** @type {Object<string,TopicElement>} */
    this.allTopics = {};
    /** @type {string} */
    this.lastLoadedProblemId = null;
    /** @type {string} */
    this.nextProblemId = null;
    /** @type {Object<string, boolean>} */
    this.chapterIds = {};
    this.topics = {};
    this.flagLoaded = false;
    this.mainPage = null;
    this.lastKnownGoodProblemFileName = "";
  }

  initialize(mainPage, coursePage) {
    this.mainPage = mainPage;
    this.coursePage = coursePage;
  }

  /** @return {TopicElement} */
  getTopicElementById(
    /** @type {string} */
    normalizedTopicId,
  ) {
    if (!(normalizedTopicId in this.allTopics)) {
      throw (`Error: problem label ${normalizedTopicId} not found. `);
    }
    return this.allTopics[normalizedTopicId];
  }

  /** @return {TopicElement|null} */
  getTopicElementByIdOrNull(
    /** @type {string} */
    normalizedTopicId,
  ) {
    if (!(normalizedTopicId in this.allTopics)) {
      return null;
    }
    return this.allTopics[normalizedTopicId];
  }

  resetAllTopicElements() {
    this.chapterIds = {};
  }

  /** @return {TopicElement} */
  updateOrCreateTopicElement(
    problemData,
  ) {
    if (problemData.id.includes("%")) {
      console.log("Unexpected percent sign in problem id.");
    }
    // problemId is percent encoded, safe to embed in html.
    const problemId = problemData.id;
    const fileName = problemData["fileName"];
    if (fileName === undefined || fileName === null) {
      fileName = "";
    }
    let currentProblem = this.getExistingTopicElementOrCreate(
      problemId, fileName,
    );
    currentProblem.initializeTopicElement(problemData, null);
    return currentProblem;
  }

  /** @return {TopicElement} */
  getExistingTopicElementOrCreate(
    /** @type {string} */
    problemId,
    /** @type {string} */
    fileName,
  ) {
    if (problemId in this.allTopics) {
      return this.allTopics[problemId];
    }
    const problem = new TopicElement(problemId, fileName);
    this.allTopics[problemId] = problem;
    if (problem.flagProblemPageOnly) {
      problem.flagForReal = false;
    } else {
      problem.setRandomSeedFromEnvironment();
    }

    return problem;
  }

  processLoadedTopics(incomingTopics) {
    const startTime = new Date().getTime();
    this.lastLoadedProblemId = null;
    this.resetAllTopicElements();
    this.topics = miscellaneous.jsonUnescapeParse(incomingTopics);
    if (!Array.isArray(this.topics["children"])) {
      return;
    }
    let totalChildren = 0;
    for (let i = 0; i < this.topics["children"].length; i++) {
      const currentChapter = this.topics["children"][i];
      const problem = this.updateOrCreateTopicElement(currentChapter);
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
    problemNavigation.writeToHTML();
  }

  updateProblemPage() {
    this.coursePage.selectCurrentCourse();
    /** @type {TopicElement|null} */
    let problem = this.getCurrentProblem();
    if (problem !== null && problem.flagProblemDownloadStarted) {
      // We are already downloading the problem.
      return;
    }
    let url = "";
    if (problem !== undefined && problem !== null) {
      let forReal = problem.flagForReal;
      const api = pathnames.urls.calculatorAPI;
      const urlParameters = problem.getCalculatorURLRequestFileCourseTopics(
        forReal
      );
      url = `${api}?${urlParameters}`;
    } else {
      const courseStorage = storage.variables.currentCourse;
      let problemFileName = courseStorage.problemFileName.getValue();
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
    problem.flagProblemDownloadStarted = true;
    submitRequests.submitGET({
      url: url,
      callback: (input, outputComponent) => {
        problem.flagProblemDownloadStarted = false;
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
    button.addEventListener("click", () => {
      this.mainPage.selectPage('currentCourse');
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

  /** @return {HTMLElement[]} */
  getHTMLfromTopics() {
    let result = [];
    for (let label in this.chapterIds) {
      let currentProblem = this.getTopicElementById(label);
      result.push(currentProblem.toHTMLChapter());
    }
    let extraHtml = miscellaneousFrontend.htmlFromCommentsAndErrors(
      allTopics.topics
    );
    if (extraHtml !== "") {
      let extraNode = document.createElement("span");
      miscellaneous.writeHTML(extraNode, extraHtml);
      result.push(extraNode);
    }
    return result;
  }

  /** @return {TopicElement|null} */
  getCurrentProblem() {
    const courseStorage = storage.variables.currentCourse;
    let problemFileName = courseStorage.problemFileName.getValue();
    if (
      problemFileName === "" ||
      problemFileName === null ||
      problemFileName === undefined
    ) {
      return null;
    }
    return this.getExistingTopicElementOrCreate(
      problemFileName, problemFileName
    );
  }
}

function selectProblemById(
  /** @type{string} */
  problemId,
  /** @type{string} */
  exerciseType,
) {
  storage.variables.currentCourse.problemFileName.setAndStore(
    problemId
  );
  storage.variables.currentCourse.exerciseType.setAndStore(exerciseType);
  let problem = allTopics.getCurrentProblem();
  problem.flagForReal = false;
  if (
    exerciseType === pathnames.urlFields.scoredQuizJSON &&
    !globalUser.flagDatabaseInactiveEveryoneIsAdmin
  ) {
    problem.flagForReal = true;
  }
  allTopics.flagLoaded = false;
  allTopics.updateProblemPage();
}

class TopicElement {
  constructor(
    /** @type {string} */
    problemId,
    /** @type {string} */
    fileName
  ) {
    if (fileName !== "" && problemId.includes("%")) {
      console.log(
        "Unusual problemId contains the % symbol. " +
        "We either have an unusual file name or a %-encoding bug. " +
        "The file name: " + problemId
      );
    }
    /** @type {string} */
    this.problemId = problemId;
    /** @type {string} */
    this.fileName = fileName;
    if (this.fileName === undefined || this.fileName === null) {
      this.fileName = "";
    }
    /** @type {string} */
    this.nextProblemId = "";
    /** @type {string} */
    this.previousProblemId = "";
    /** @type {number} */
    this.totalChildren = 0;
    /** @type {HTMLElement|null} */
    this.outputElement = document.getElementById(
      ids.domElements.problemPageContentContainer
    );
    /** @type {string} */
    this.title = "";
    /** @type {string} */
    this.randomSeed = "";
    this.flagProblemPageOnly = false;
    /** @type {AnswerPanel[]} */
    this.answerPanels = [];
    this.flagProblemDownloadStarted = false;
  }

  setRandomSeed(input) {
    if (input === undefined) {
      input = null;
    }
    this.randomSeed = input;
  }

  setRandomSeedFromEnvironment() {
    let currentCourse = storage.variables.currentCourse;
    const exerciseType = currentCourse.exerciseType.getValue();
    this.flagForReal = (exerciseType !== pathnames.urlFields.exerciseJSON);
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
  }

  writeProblemPage(input, outputComponent) {
    let problemData = null;
    this.outputElement.textContent = "";
    try {
      problemData = miscellaneous.jsonUnescapeParse(input);
    } catch (e) {
      this.outputElement.textContent =
        `Error parsing: ${e}. Failed to parse: ${input}`;
      return;
    }
    if (
      problemData.crashReport !== undefined &&
      problemData.crashReport !== null
    ) {
      let html = miscellaneousFrontend.htmlFromCommentsAndErrors(problemData);
      miscellaneous.writeHTML(this.outputElement, html);
      return;
    }
    this.initializeProblemContent(problemData);
  }

  /** @return {number} Number of children processed. */
  initializeTopicElement(problemData, inputParentIdURLed) {
    this.decodedProblem = "";
    this.commentsProblem = "";
    this.parentIdURLed = inputParentIdURLed;
    this.problemLabel = "";
    this.previousProblemId = "";
    this.nextProblemId = "";
    this.scriptIds = null;
    this.title = problemData["title"];
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
      /** @type {HTMLElement[]} */
      video: [],
      /** @type {HTMLElement[]} */
      slides: [],
      /** @type {HTMLElement[]} */
      homework: [],
    };
    /** @type {HTMLElement[]} */
    this.badProblemExplanation = [];
    if (problemData.deadlines !== undefined) {
      this.deadlines = problemData.deadlines;
    }
    if (problemData.deadline !== undefined) {
      this.deadlineString = problemData.deadline;
    }
    if (this.fileName !== "") {
      this.previousProblemId = allTopics.lastLoadedProblemId;
      if (
        this.previousProblemId !== null &&
        this.previousProblemId !== undefined &&
        this.previousProblemId !== ""
      ) {
        let previousProblem = allTopics.allTopics[this.previousProblemId];
        previousProblem.nextProblemId = this.problemId;
      }
      allTopics.lastLoadedProblemId = this.problemId;
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
      allTopics.chapterIds[this.problemId] = true;
    }
    this.childrenIds = [];
    if (!Array.isArray(problemData.children)) {
      return;
    }
    for (let i = 0; i < problemData.children.length; i++) {
      const currentChild = allTopics.updateOrCreateTopicElement(
        problemData.children[i]
      );
      this.totalChildren += currentChild.totalChildren + 1;
      this.childrenIds.push(currentChild.problemId);
    }
  }

  /** @return {HTMLElement[]} */
  computeBadProblemExplanation() {
    let userHasInstructorRights = true;
    if (!this.flagProblemPageOnly) {
      userHasInstructorRights = globalUser.hasInstructorRights();
    }
    this.badProblemExplanation = [];
    if (!this.decodedProblem.includes(
      pathnames.urlFields.problem.failedToLoadProblem
    )) {
      allTopics.lastKnownGoodProblemFileName = this.fileName;
      return;
    }
    let badProblemExplanationPartOne = document.createElement("div");
    let html = "It appears your problem failed to load.<br>";
    if (allTopics.lastKnownGoodProblemFileName !== "" && userHasInstructorRights) {
      html += "Perhaps you may like to clone the last good known problem.<br>";
    }
    miscellaneous.writeHTML(badProblemExplanationPartOne, html);
    this.badProblemExplanation.push(badProblemExplanationPartOne);
    miscellaneousFrontend.appendHtmlToArray(
      this.badProblemExplanation,
      editPage.getClonePanel(
        allTopics.lastKnownGoodProblemFileName,
        this.fileName
      ),
    );
    let epilogueElement = document.createElement("hr");
    this.badProblemExplanation.push(epilogueElement);
  }

  initializeProblemContent(problemData) {
    const isLoggedIn = globalUser.flagLoggedIn;
    if (pathnames.standardResponses.isNotLoggedInResponse(problemData)) {
      this.title = "";
      this.decodedProblem = "Not logged in. Please select practice mode or login.";
      this.flagForReal = true;
      this.answerPanels = [];
    } else {
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
    this.commentsProblem += miscellaneousFrontend.htmlFromCommentsAndErrors(
      problemData
    );
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
        flagForReal: this.flagForReal,
        isLoggedIn: isLoggedIn,
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
    let exerciseType = pathnames.urlFields.exerciseJSON;
    if (isScoredQuiz && !globalUser.flagDatabaseInactiveEveryoneIsAdmin) {
      exerciseType = pathnames.urlFields.scoredQuizJSON;
    }
    let requestJSON = {
      currentPage: "problemPage",
      exerciseType: exerciseType,
      problemFileName: this.fileName,
      courseHome: storage.variables.currentCourse.courseHome.getValue(),
      topicList: storage.variables.currentCourse.topicList.getValue(),
    };
    if (
      includeRandomSeed &&
      this.randomSeed !== "" &&
      this.randomSeed !== null &&
      this.randomSeed !== undefined
    ) {
      requestJSON["randomSeed"] = this.randomSeed;
    } else {
      requestJSON["randomSeed"] = "";
    }
    let stringifiedHash = storage.objectToURL(requestJSON);
    return stringifiedHash;
  }

  getCalculatorURLFileCourseTopics() {
    return this.getCalculatorURLInput(
      this.fileName,
      storage.variables.currentCourse.courseHome.getValue(),
      storage.variables.currentCourse.topicList.getValue(),
    );
  }

  getCalculatorURLInput(
    /** @type {string} */
    inputFileName,
    /** @type {string} */
    inputCourseHome,
    /** @type {string} */
    inputTopicList,
  ) {
    let result = "";
    if (
      inputFileName.includes("%") ||
      inputCourseHome.includes("%") ||
      inputTopicList.includes("%")
    ) {
      console.log(
        "The file name, course name or topic list contains the % sign. " +
        "We may have an encoding error " +
        "- or end users who like to use unusual symbols in their file names."
      );
    }
    result += `fileName=${encodeURIComponent(inputFileName)}&`;
    result += `courseHome=${encodeURIComponent(inputCourseHome)}&`;
    result += `topicList=${encodeURIComponent(inputTopicList)}`;
    return result;
  }

  getCalculatorURLRequestPartOne(
    /** @type {boolean} */
    isScoredQuiz,
  ) {
    let result = "";
    if (isScoredQuiz === undefined) {
      isScoredQuiz = this.flagForReal;
    }
    result += `${pathnames.urlFields.request}=`;
    if (isScoredQuiz && !globalUser.flagDatabaseInactiveEveryoneIsAdmin) {
      result += pathnames.urlFields.scoredQuizJSON;
    } else {
      result += pathnames.urlFields.exerciseJSON;
      if (
        this.randomSeed !== null &&
        this.randomSeed !== "" &&
        this.randomSeed !== undefined
      ) {
        result += `&${pathnames.urlFields.randomSeed}=${this.randomSeed}`;
      } else {
        const desiredRandomSeed = Math.floor(Math.random() * 10000000);
        // The desiredRandomSeed may not yield a valid problem.
        // If that is the case, the backend should iterate to the next seed.
        // Here, "the next seed" is extracted from the desired one with
        // a deterministic operation - can be as simple as adding one.
        // This iteration happens until a valid problem is generated.
        // If that happens, the desired random seed may not end up
        // being used in the end, so we are not storing it in
        // this.randomSeed. Rather, we will get the random seed
        // bounced back (and possibly altered) from the backend.
        result += `&${pathnames.urlFields.randomSeed}=${desiredRandomSeed}`;
      }
    }
    return result;
  }

  getCalculatorURLRequestInput(
    isScoredQuiz,
    inputFileName,
    inputCourseHome,
    inputTopicList,
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
  getProblemNavigation(
    /** @type {HTMLElement} */
    containerToWriteTo
  ) {
    containerToWriteTo.textContent = "";
    let children = this.getProblemNavigationContent();
    for (let i = 0; i < children.length; i++) {
      containerToWriteTo.append(children[i]);
    }
    return containerToWriteTo;
  }

  /** @return {HTMLElement} */
  getNextProblemButton(
    /** @type {ProblemNavigationHints} */
    hints
  ) {
    if (
      this.nextProblemId === null ||
      this.nextProblemId === "" ||
      this.nextProblemId === undefined
    ) {
      return document.createTextNode("");
    }
    let nextProblem = allTopics.getTopicElementById(this.nextProblemId);
    let nextURL = nextProblem.getAppAnchorRequestFileCourseTopics(
      hints.isScoredQuiz, false
    );
    let nextProblemTag = document.createElement("a");
    nextProblemTag.className = hints.linkType;
    nextProblemTag.href = `#${nextURL}`;
    nextProblemTag.addEventListener(
      "click",
      () => {
        selectProblemById(this.nextProblemId, hints.defaultRequest);
      }
    );
    miscellaneous.writeHTML(nextProblemTag, "&#8594;");
    return nextProblemTag;
  }

  /** @return {HTMLElement} */
  getPreviousProblemButton(
    /** @type {ProblemNavigationHints} */
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
    miscellaneous.writeHTML(previousLink, "&#8592;");
    if (enabled) {
      let previousProblem = allTopics.getTopicElementById(
        this.previousProblemId
      );
      let previousURL = previousProblem.getAppAnchorRequestFileCourseTopics(
        hints.isScoredQuiz, false,
      );
      previousLink.href = `#${previousURL}`;
      previousLink.addEventListener(
        "click",
        () => {
          selectProblemById(this.previousProblemId, hints.defaultRequest);
        }
      );
    } else {
      previousLink.style.visibility = "hidden";
    }
    return previousLink;
  }

  /** @return {ProblemNavigationHints} */
  getProblemNavigationHints() {
    let result = new ProblemNavigationHints();
    if (
      this.flagForReal &&
      globalUser.flagLoggedIn &&
      !globalUser.flagDatabaseInactiveEveryoneIsAdmin
    ) {
      result.defaultRequest = pathnames.urlFields.scoredQuizJSON;
      result.linkType = "problemLinkQuiz";
      result.isScoredQuiz = true;
    }
    return result;
  }

  /** @return {HTMLElement[]} */
  getProblemNavigationContent() {
    let result = [];
    let hints = this.getProblemNavigationHints();
    // When the title is empty, the problem 
    // is not attached to the current course.
    if (this.title !== "") {
      result.push(this.getPreviousProblemButton(hints));
    }
    if (this.flagForReal) {
      let practiceURL = this.getAppAnchorRequestFileCourseTopics(false, false);
      let practiceTag = document.createElement("a");
      practiceTag.className = "problemLinkPractice";
      practiceTag.href = "#" + practiceURL;
      practiceTag.addEventListener("click", () => {
        selectProblemById(this.problemId, "exerciseJSON");
      });
      practiceTag.textContent = "Practice";
      result.push(practiceTag);
    } else {
      let selectedPracticeTag = document.createElement("span");
      selectedPracticeTag.className = "problemLinkSelectedPractice";
      selectedPracticeTag.style.color = "green";
      selectedPracticeTag.textContent = "Practice";
      result.push(selectedPracticeTag)
    }
    if (
      !this.flagForReal &&
      globalUser.flagLoggedIn &&
      !globalUser.flagDatabaseInactiveEveryoneIsAdmin
    ) {
      let quizURL = this.getAppAnchorRequestFileCourseTopics(true, false);
      let quizTag = document.createElement("a");
      quizTag.className = "problemLinkQuiz";
      quizTag.href = "#" + quizURL;
      quizTag.addEventListener(
        "click",
        () => {
          selectProblemById(this.problemId, "scoredQuizJSON");
        }
      );
      quizTag.textContent = "Quiz";
      result.push(quizTag);
    } else {
      if (this.flagForReal) {
        let quizTag = document.createElement("span");
        quizTag.className = "problemLinkSelectedQuiz";
        quizTag.style.color = "brown";
        quizTag.textContent = "Quiz";
        result.push(quizTag);
      }
    }
    if (this.title !== "") {
      result.push(this.getNextProblemButton(hints));
    }
    if (this.flagForReal !== true && this.flagForReal !== "true") {
      let scoresTag = document.createElement("b");
      scoresTag.style.color = "green";
      scoresTag.textContent = "Scores not recorded.";
      result.push(scoresTag);
      result.push(document.createTextNode(" "));
      let randomSeedElement = document.createElement("span");
      randomSeedElement.id = ids.domElements.spanProblemLinkWithRandomSeed;
      let randomSeedAnchor = document.createElement("a");
      randomSeedAnchor.href = "#" + this.getAppAnchorRequestFileCourseTopics(
        false, true,
      );
      randomSeedAnchor.textContent = this.randomSeed;
      randomSeedElement.appendChild(randomSeedAnchor);
      result.push(randomSeedElement);
    } else {
      let scoresTag = document.createElement("b");
      scoresTag.style.color = "brown";
      scoresTag.textContent = "Scores are recorded. ";
      result.push(scoresTag);
    }
    return result;
  }

  /** @return {HTMLElement} */
  getEditPanel() {
    return editPage.getEditPanel(
      decodeURIComponent(this.problemId),
      globalUser.hasInstructorRightsNotViewingAsStudent(),
      true,
    );
  }

  /** @return {HTMLElement[]} */
  getHTMLProblems() {
    let nextElement = document.createElement("div");
    nextElement.className = "bodySubsection";
    let table = document.createElement("table");
    nextElement.appendChild(table);
    table.className = "topicList";
    for (let i = 0; i < this.childrenIds.length; i++) {
      let currentProblem = allTopics.getTopicElementById(this.childrenIds[i]);
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

  /** @return {HTMLElement[]} */
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
        let currentProblem = allTopics.getTopicElementById(this.childrenIds[0]);
        if (currentProblem.type === "Problem") {
          return true;
        }
      }
    }
    return false;
  }

  /** @return {HTMLElement[]} */
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
        let currentSubSection = allTopics.getTopicElementById(this.childrenIds[i]);
        miscellaneousFrontend.appendHtml(
          nextElement, currentSubSection.getHTMLSubSection()
        );
      }
    } else {
      miscellaneousFrontend.appendHtml(nextElement, this.getHTMLSubSection());
    }
    return result;
  }

  /** @return {HTMLElement[]} */
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
        let currentSection = allTopics.getTopicElementById(this.childrenIds[i]);
        miscellaneousFrontend.appendHtml(
          bodyChapterElement, currentSection.getHTMLSection(),
        );
      }
    }
    result.push(bodyChapterElement);
    return result;
  }

  /** @return {HTMLElement[]} */
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
      videoLink.textContent = "Video";
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
    let nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListTitle";
    miscellaneous.writeHTML(
      nextCell, this.problemNumberString + " " + this.title
    );
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListMaterials";
    let materialLinks = this.getProblemMaterialLinks();
    miscellaneousFrontend.appendHtml(nextCell, materialLinks);
    nextCell = outputRow.insertCell(- 1);
    nextCell.className = "topicListPracticeQuiz";
    if (this.fileName !== "") {
      if (globalUser.flagLoggedIn) {
        let nextElement = document.createElement("a");
        nextElement.className = "problemLinkQuiz";
        nextElement.href = "#" + this.getAppAnchorRequestFileCourseTopics(true);
        nextElement.addEventListener(
          "click",
          () => {
            selectProblemById(this.problemId, "scoredQuizJSON");
          }
        );
        nextElement.textContent = "Quiz";
        nextCell.appendChild(nextElement);
      }
      let nextElement = document.createElement("a");
      nextElement.className = "problemLinkPractice";
      nextElement.href = "#" + this.getAppAnchorRequestFileCourseTopics(false);
      nextElement.textContent = "Practice";
      nextElement.addEventListener(
        "click",
        () => {
          selectProblemById(this.problemId, "exerciseJSON");
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
    miscellaneousFrontend.appendHtmlToArray(
      contentArray, this.badProblemExplanation,
    );
    let problemBody = document.createElement("span");
    console.log("DEBUG: about to write: ", this.decodedProblem);
    miscellaneous.writeHTML(
      problemBody,
      this.decodedProblem + this.commentsProblem,
    );
    contentArray.push(problemBody);
    miscellaneousFrontend.appendHtml(this.outputElement, contentArray);
    for (let i = 0; i < this.answerPanels.length; i++) {
      let answer = this.answerPanels[i];
      let answerElement = document.getElementById(answer.input.answerPanelId);
      answer.writeToElement(answerElement, this.outputElement);
    }
    let elementWithScripts = null;
    try {
      elementWithScripts = dynamicJavascript.bootstrapAllScripts(
        this.outputElement
      );
    } catch (e) {
      console.log(e);
    }
    dynamicJavascript.typeset(
      this.outputElement, {}, (editor, element) => {
        if (elementWithScripts === null) {
          return;
        }
      });
  }

  /** @return {HTMLElement} */
  toHTMLDeadlineElement() {
    let result = document.createElement("span");
    if (globalUser.hasInstructorRightsNotViewingAsStudent()) {
      result.textContent = "Deadlines";
      return result;
    }
    if (globalUser.hasInstructorRights() && globalUser.studentView()) {
      let sectionIndex = storage.variables.currentSectionComputed.getValue();
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
        let parentProblem = allTopics.getTopicElementById(this.parentIdURLed);
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
      miscellaneous.writeHTML(deadline, this.deadline.toLocaleDateString());
      deadline.style.color = "green";
      result.appendChild(deadline);
      return result;
    }
    if (remainingInHours < 48 && remainingInHours >= 0) {
      let deadline = document.createElement("b");
      deadline.style.color = "red";
      miscellaneous.writeHTML(
        deadline,
        `${this.deadline.toLocaleDateString()}, ~${remainingInHours.toFixed(1)} hrs left`,
      );
      result.appendChild(deadline)
      return result;
    }
    if (remainingInHours < 0) {
      let deadline = document.createElement("b");
      deadline.style.color = "red";
      miscellaneous.writeHTML(deadline, this.deadline.toLocaleDateString());
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

  /** @return {HTMLElement} */
  toHTMLDeadline() {
    let result = document.createElement("span");
    result.className = ids.domElements.classSpanDeadlineContainer;
    result.appendChild(this.toHTMLDeadlinePanel());
    return result;
  }

  /** @return {HTMLElement} */
  toHTMLDeadlinePanel() {
    let result = document.createElement("span");
    result.textContent = "";
    if (!globalUser.hasInstructorRights() || globalUser.studentView()) {
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
      allTopics.coursePage.toggleDeadline(
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
    for (let i = 0; i < globalUser.sectionsTaught.length; i++) {
      let row = table.insertRow();
      let cell = row.insertCell();
      cell.textContent = globalUser.sectionsTaught[i];
      let inputCell = row.insertCell();
      let input = document.createElement("input");
      input.type = "date";
      input.className = "datePicker";
      input.name = `datePicker`;
      if (
        this.deadlines[i] !== "" &&
        this.deadlines[i] !== undefined
      ) {
        let deadline = this.deadlines[i];
        input.value = deadline;
      }
      inputCell.appendChild(input);
    }
    panelDeadlines.appendChild(table);
    let buttonSet = document.createElement("button");
    buttonSet.textContent = "Set";
    panelDeadlines.appendChild(buttonSet);
    let report = document.createElement("span");
    panelDeadlines.appendChild(report);
    buttonSet.addEventListener("click", () => {
      allTopics.coursePage.modifyDeadlines(
        this.problemId,
        panelDeadlines,
        report
      );
    });
    return result;
  }

  /** @return {HTMLElement} */
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
      miscellaneous.writeHTML(textArea, this.weight);
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
    modifyObject[this.problemId] = {
      weight: incomingPoints
    };
    let url = "";
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    const problemWeightQuery = pathnames.urlFields.requests.setProblemWeight;
    url += `${api}?`;
    url += `${request}=${problemWeightQuery}&`;
    url += `${pathnames.urlFields.mainInput}=${encodeURIComponent(JSON.stringify(modifyObject))}`;
    submitRequests.submitGET({
      url: url,
      progress: ids.domElements.spanProgressReportGeneral,
      result: reportSpan,
    });
  }

  /** @return {HTMLElement[]} */
  getProblemWeightContent() {
    if (this.type !== "Problem" || this.fileName === "") {
      return [];
    }
    let content = document.createElement("span");
    if (!globalUser.hasInstructorRights() || globalUser.studentView()) {
      miscellaneous.writeHTML(content, this.toStringProblemWeight());
      return [content];
    }

    let button = document.createElement("button");
    button.className = "accordionLikeProblemWeight";
    miscellaneous.writeHTML(button, `${this.toStringProblemWeight()} &#9666`);
    button.name = this.problemId;
    button.addEventListener("click", () => {
      allTopics.coursePage.toggleProblemWeights();
    });
    content.appendChild(button);
    content.appendChild(document.createElement("br"));
    content.appendChild(this.toHTMLWeights());
    return [content];
  }

  isSolvedForSure() {
    if (
      this.correctlyAnswered === undefined ||
      this.correctlyAnswered === NaN
    ) {
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
    if (
      this.correctlyAnswered !== undefined &&
      this.correctlyAnswered !== NaN
    ) {
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
        let points = (
          0.0 + this.correctlyAnswered * problemWeightConverted
        ) / this.totalQuestions;
        points = Number(points.toFixed(2));
        result += ` = ${points} pts`;
        if (this.correctlyAnswered < this.totalQuestions) {
          color = "red";
        }
      } else {
        result += ` = ? pts`;
      }
    }
    if (
      globalUser.hasInstructorRights() &&
      !globalUser.studentView() &&
      result === ""
    ) {
      result = "modify";
    }
    return `<b style = "color:${color}">${result}</b>`;
  }

  /** @return {HTMLElement} */
  getLinkFromSpec(
    /** 
     * @type {{
     * request: string, 
     * name: string, 
     * extension: string, 
     * options: string, 
     * download: boolean}}
     */
    linkSpec,
    /** @type {string} */
    query
  ) {
    if (linkSpec.adminView === true) {
      let studentView = globalUser.studentView();
      if (studentView !== false && studentView !== "false") {
        return null;
      }
    }
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    let href = "";
    href += `${api}?${request}=${linkSpec.request}&`;
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
    miscellaneous.writeHTML(result, linkSpec.name);
    return result;
  }
}

function getCalculatorURLRequestFileCourseTopicsFromStorage() {
  let currentCourse = storage.variables.currentCourse;
  let exerciseType = currentCourse.exerciseType.getValue();
  if (
    exerciseType === "" ||
    exerciseType === null ||
    exerciseType === undefined
  ) {
    exerciseType = pathnames.urlFields.exerciseJSON;
    currentCourse.exerciseType.setAndStore(exerciseType);
  }
  let fileName = currentCourse.fileName.getValue();
  let topicList = currentCourse.topicList.getValue();
  let courseHome = currentCourse.courseHome.getValue();
  let environmentRandomSeed = currentCourse.randomSeed.getValue();
  let result = "";
  result += `${pathnames.urls.calculatorAPI}?`;
  result += `${pathnames.urlFields.request}=${exerciseType}&`;
  result += `fileName=${fileName}&`;
  result += `topicList=${topicList}&courseHome=${courseHome}&`;
  if (
    environmentRandomSeed !== null &&
    environmentRandomSeed !== "" &&
    environmentRandomSeed !== undefined
  ) {
    result += `randomSeed=${environmentRandomSeed}&`;
  }
  return result;
}

class ProblemNavigationHints {
  constructor() {
    this.linkType = "problemLinkPractice";
    this.defaultRequest = pathnames.urlFields.exerciseJSON;
    this.isScoredQuiz = false;
  }
}

class ProblemNavigation {
  constructor() {
    /** @type {TopicElement} */
    this.currentProblem = null;
    /** @type {HTMLElement|null} */
    this.infoBar = document.getElementById(ids.domElements.divProblemInfoBar);
    this.flagProblemPageOnly = false;
    /** @type{HTMLElement|null} */
    this.editPanel = null;
    /** @type{HTMLElement|null} */
    this.problemNavigationSubpanel = null;
  }

  setCurrentProblemAndUpdate(
    /** @type {TopicElement} */
    inputProblem,
  ) {
    this.currentProblem = inputProblem;
    this.writeToHTML();
  }

  problemHasTitle() {
    return this.currentProblem.title !== ""
  }

  writeToHTML() {
    if (this.currentProblem === null) {
      return;
    }
    if (this.infoBar === null) {
      return;
    }
    let problemTitle = document.createElement("div");
    problemTitle.className = "problemTitle";

    let problemTitleContainer = document.createElement("div");
    problemTitleContainer.className = "problemTitleContainer";
    problemTitle.appendChild(problemTitleContainer);
    if (
      this.currentProblem.problemLabel !== undefined &&
      this.currentProblem.problemLabel !== "" &&
      this.currentProblem.problemLabel !== null
    ) {
      problemTitleContainer.appendChild(
        document.createTextNode(this.currentProblem.problemLabel)
      );
    }
    if (this.problemHasTitle()) {
      problemTitleContainer.appendChild(
        document.createTextNode(this.currentProblem.title)
      );
    } else {
      const title = document.createElement("b");
      title.textContent = "Problem not part of the current course.";
      title.style.color = "orange";
      problemTitleContainer.appendChild(title);
    }
    this.infoBar.textContent = "";
    this.problemNavigationSubpanel = document.createElement("div")
    this.problemNavigationSubpanel.className = "problemNavigation";
    this.infoBar.appendChild(this.problemNavigationSubpanel);

    if (
      this.currentProblem.links !== undefined &&
      this.currentProblem.links !== null
    ) {
      if (this.currentProblem.links.slides !== null) {
        miscellaneousFrontend.appendHtml(
          problemTitle, this.currentProblem.links.slides,
        );
      }
      if (this.currentProblem.links.video !== null) {
        miscellaneousFrontend.appendHtml(
          problemTitle, this.currentProblem.links.video,
        );
      }
    }
    this.infoBar.appendChild(problemTitle);
    this.editPanel = document.createElement("div");
    this.infoBar.appendChild(this.editPanel);
    this.updateExceptTitle();
    typeset.typesetter.typesetSoft(this.infoBar, "");
  }

  updateExceptTitle() {
    if (this.currentProblem === null) {
      return;
    }
    this.editPanel.textContent = "";
    this.editPanel.appendChild(this.currentProblem.getEditPanel());
    if (!this.flagProblemPageOnly) {
      this.currentProblem.getProblemNavigation(
        this.problemNavigationSubpanel
      );
    }
  }
}

function modifyWeight(id) {
  let problemWeight = allTopics.allTopics[id];
  problemWeight.modifyWeight();
}

function convertStringToLaTeXFileName(input) {
  let result = encodeURIComponent(
    input.split(" ").join("-")
  ).split("%").join("-");
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

function writeEditCoursePagePanel() {
  let panel = document.getElementById(ids.domElements.courseEditPanel);
  panel.textContent = "";
  let courseHome = storage.variables.currentCourse.courseHome.getValue()
  const courseHomePanel = editPage.getEditPanel(
    courseHome, globalUser.hasInstructorRightsNotViewingAsStudent(), false,
  );
  panel.appendChild(courseHomePanel);
  let topicList = storage.variables.currentCourse.topicList.getValue();
  const topicListPanel = editPage.getEditPanel(
    topicList, globalUser.hasInstructorRightsNotViewingAsStudent(), false,
  );
  panel.appendChild(topicListPanel);
  if (
    allTopics.topics.topicBundleFile === undefined ||
    allTopics.topics.topicBundleFile === null ||
    allTopics.topics.topicBundleFile === ""
  ) {
    return;
  }
  for (
    let i = 0;
    i < allTopics.topics.topicBundleFile.length;
    i++
  ) {
    let nextToEdit = allTopics.topics.topicBundleFile[i];
    const nextToEditPanel = editPage.getEditPanel(
      nextToEdit,
      globalUser.hasInstructorRightsNotViewingAsStudent(),
      false
    );
    panel.appendChild(nextToEditPanel);
  }
}

function processLoadedTopicsWriteToEditPage(incomingTopics, result) {
  allTopics.processLoadedTopics(incomingTopics, result);
  editPage.selectEditPage(null);
}

function processLoadedTopicsWriteToCoursePage(incomingTopics, result) {
  allTopics.processLoadedTopicsWriteToCoursePage(incomingTopics, result);
}

function updateProblemPage() {
  allTopics.updateProblemPage();
}

function loadProblemIntoElement(
  /** @type {HTMLElement|string} */
  problemElement,
) {
  if (typeof problemElement === "string") {
    problemElement = document.getElementById(problemElement);
  }
  let problemFileName = problemElement.getAttribute("problem");
  let problem = allTopics.getExistingTopicElementOrCreate(
    problemFileName, problemFileName
  );
  problem.outputElement = problemElement;
  problem.flagForReal = false;
  if (problemElement.getAttribute("forReal") === "true") {
    problem.flagForReal = true;
  }
  const endpoint = pathnames.urls.calculatorAPI;
  const parameters = problem.getCalculatorURLRequestFileCourseTopics(
    problem.flagForReal
  );
  let url = `${endpoint}?${parameters}`;
  submitRequests.submitGET({
    url: url,
    callback: (input, outputComponent) => {
      problem.writeProblemPage(input, outputComponent);
    },
    progress: ids.domElements.spanProgressReportGeneral,
    result: problemElement,
  });
}

let problemNavigation = new ProblemNavigation();
let allTopics = new TopicCollection();

module.exports = {
  TopicElement,
  allTopics,
  problemNavigation,
  updateProblemPage,
  loadProblemIntoElement,
  processLoadedTopicsWriteToCoursePage,
  processLoadedTopicsWriteToEditPage,
  writeEditCoursePagePanel,
  modifyWeight,
};