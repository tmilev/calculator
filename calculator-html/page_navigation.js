"use strict";
const selectCourse = require("./select_course");
const coursePage = require("./course_page");
const problemPage = require("./problem_page");
const editPage = require("./edit_page");
const database = require("./database");
const serverStatus = require("./server_status");
const accountPage = require("./account");
const activateAccount = require("./account_activation");
const accountManagement = require("./manage_accounts");
const cookies = require("./cookies");
const ids = require("./ids_dom_elements");
const serverInformation = require("./server_information");
const login = require("./login");
const initializeButtons = require("./initialize_buttons");
const calculatorPage = require("./calculator_page");
const signUp = require("./signup").signUp;
const mathTypeSet = require("./math_typeset");
const themes = require("./themes");
const solver = require("./solve");
const solve = require("./solve");
const solveSocratic = require("./solve_socratic");
const compareExpressions = require("./compare_expressions").compareExpressions;
const compareExpressionsSocratic = require("./compare_expressions_socratic").compareExpressionsSocratic;
const storage = require("./storage").storage;

class User {
  constructor() {
    this.flagLoggedIn = false;
    this.googleProfile = null;
    this.sectionsTaught = [];
    this.instructor = "";
    this.sectionInDB = "";
    this.deadlineSchema = "";
    this.sectionComputed = "";
    this.flagDatabaseInactiveEveryoneIsAdmin = false;
  }

  isLoggedIn() {
    return this.flagLoggedIn;
  }

  getRole() {
    return mainPage().storage.variables.user.role.getValue();
  }

  hasAdminRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hasProblemEditRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hasInstructorRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hideProfilePicture() {
    document.getElementById("divProfilePicture").classList.add("divInvisible");
    document.getElementById("divProfilePicture").classList.remove("divVisible");
  }

  makeFromUserInfo(inputData) {
    let thePage = window.calculator.mainPage;
    // Please note: the authentication token is silently set through the cookie headers.
    // Please do not take explicit action as
    // inputdata.authenticationToken may not contain the authentication token.
    // not ok: thePage.storage.variables.user.authenticationToken.setAndStore(inputData.authenticationToken);
    thePage.storage.variables.user.name.setAndStore(inputData.username);
    mainPage().storage.variables.user.role.setAndStore(inputData.userRole);
    this.flagLoggedIn = true;
    this.sectionsTaught = inputData.sectionsTaught;
    this.instructor = inputData.instructor;
    this.sectionInDB = inputData.studentSection;
    this.sectionComputed = inputData.studentSection;
    this.deadlineSchema = inputData.deadlineSchema;
    document.getElementById(ids.domElements.spanUserIdInAccountsPage).innerHTML = thePage.storage.variables.user.name.value;
    document.getElementById(ids.domElements.inputUsername).value = thePage.storage.variables.user.name.value;
  }
}

class Page {
  constructor() {
    window.calculator.mainPage = this;
    this.pages = {
      login: {
        name: "login", //<-for autocomplete
        id: "divLoginPage",
        menuButtonId: "buttonLoginPage",
        container: null,
        selectFunction: null,
        initialized: false,
      },
      solve: {
        name: "solve",
        menuButtonId: ids.domElements.pages.solve.buttonSelectSolvePage,
        id: ids.domElements.pages.solve.div,
        container: null,
        selectFunction: solver.selectSolverPage,
      },
      solveSocratic: {
        name: "solveSocratic",
        id: ids.domElements.pages.solveSocratic.div,
        selectFunction: () => {
          solveSocratic.solver.selectPage();
        }
      },
      compareExpressions: {
        name: "compareExpressions",
        id: ids.domElements.pages.compareExpressions.div,
        selectFunction: () => {
          compareExpressions.selectPage();
        },
      },
      compareExpressionsSocratic: {
        name: "compareExpressionsSocratic",
        id: ids.domElements.pages.compareExpressionsSocratic.div,
        selectFunction: () => {
          compareExpressionsSocratic.selectPage();
        },
      },
      selectCourse: {
        name: "selectCourse",
        id: "divSelectCourse",
        menuButtonId: "buttonSelectCourse",
        container: null,
        selectFunction: selectCourse.selectCoursePage,
      },
      currentCourse: {
        name: "currentCourse",
        id: "divCurrentCourse",
        menuButtonId: "buttonCurrentCourse",
        container: null,
        selectFunction: coursePage.selectCurrentCoursePage,
      },
      problemPage: {
        name: "problemPage",
        id: "divProblemPage",
        menuButtonId: "buttonProblemPage",
        container: null,
        selectFunction: problemPage.updateProblemPage,
        flagLoaded: false,
      },
      editPage: {
        name: "editPage",
        id: ids.domElements.pages.editPage.div,
        menuButtonId: ids.domElements.pages.editPage.button,
        container: null,
        selectFunction: editPage.selectEditPage,
        flagLoaded: false,
        editor: null,
      },
      calculator: {
        name: "calculator",
        id: "divCalculatorPage",
        menuButtonId: "buttonSelectCalculator",
        container: null,
        selectFunction: () => {
          calculatorPage.calculator.selectCalculatorPage();
        },
      },
      signUp: {
        name: "signUp",
        id: "divSignUpPage",
        container: null,
        selectFunction: signUp.signUp.bind(signUp),
      },
      forgotLogin: {
        name: "forgotLogin",
        id: "divForgotLogin",
        container: null,
        selectFunction: null,
      },
      about: {
        name: "about",
        id: "divAboutPage",
        menuButtonId: "buttonAboutPage",
        container: null,
        selectFunction: null,
      },
      privacyPolicy: {
        name: "privacyPolicy",
        id: ids.domElements.pages.privacyPolicy.div,
        menuButtonId: ids.domElements.pages.privacyPolicy.button,
      },
      themes: {
        name: "themes",
        id: ids.domElements.pages.themes.div,
        menuButtonId: ids.domElements.pages.themes.button,
        container: null,
      },
      database: {
        name: "database",
        id: "divDatabase",
        menuButtonId: "buttonSelectDatabase",
        container: null,
        selectFunction: database.updateDatabasePage,
      },
      server: {
        name: "server",
        id: "divServer",
        menuButtonId: "buttonSelectServer",
        container: null,
        selectFunction: serverStatus.updateServerStatus,
      },
      account: {
        name: "account",
        id: "divAccount",
        menuButtonId: "buttonSelectAccount",
        container: null,
        selectFunction: accountPage.updateAccountPage,
      },
      activateAccount: {
        name: "activateAccount",
        id: "divActivateAccount",
        container: null,
        selectFunction: activateAccount.updateAccountActivationPage,
      },
      accounts: {
        name: "accounts",
        id: "divAccounts",
        menuButtonId: "buttonSelectAccounts",
        container: null,
        selectFunction: accountManagement.updateAccountsPage,
      },
    };
    this.defaultPage = "calculator";
    if (document.getElementById(ids.domElements.applicationIdentifiers.compareExpressions)) {
      this.defaultPage = "compareExpressions";
    }
    this.storage = storage;
    this.initializeStorageCallbacks();
    this.flagProblemPageOnly = false;
  }

  initializeStorageCallbacks() {
    this.storage.variables.currentPage.callbackOnValueChange = (value) => {
      this.selectPage(value);
    };
    this.storage.variables.flagDebug.callbackOnValueChange = (value) => {
      this.onDebugValueChange();
    };
    this.storage.variables.flagStudentView.callbackOnValueChange = (value) => {
      this.onStudentViewChange();
    };
    this.storage.variables.calculator.input.callbackOnValueChange = (value) => {
      calculatorPage.calculator.submitComputationPartTwo(value);
    };
    this.storage.variables.calculator.monitoring.callbackOnValueChange = (value) => {
      this.setMonitoringComponent();
    };
    this.storage.variables.theme.callbackOnValueChange = (value) => {
      themes.theme.doChangeTheme(value);
    };
    this.storage.variables.solve.problemToAutoSolve.callbackOnValueChange = (value) => {
      solve.solver.solveFromStorage(value);
    };
    this.storage.variables.solve.problemToAutoSolve.callbackSetValueFromStorage = (value) => {
      solve.solver.setAutoSolveProblemBox(value);
    };
  }

  isLoggedIn() {
    if (this.flagProblemPageOnly) {
      return false;
    }
    return this.user.isLoggedIn();
  }

  initBuildVersion() {
    let buildVersion = document.getElementById(ids.domElements.calculatorBuildVersion);
    if (buildVersion === null) {
      return;
    }
    buildVersion.innerHTML = `Build version ${serverInformation.serverInformation.version}`;
  }

  serverIsOnLocalHost() {
    if (window.location.hostname === "localhost" || window.location.hostname === "127.0.0.1") {
      return true;
    }
    return false;
  }

  initHandlers() {
    window.addEventListener("hashchange", this.storage.loadSettings.bind(this.storage));
  }

  initMenuBar() {
    for (let label in this.pages) {
      let page = this.pages[label];
      page.container = document.getElementById(page.id);
      if (page.menuButtonId === null || page.menuButtonId === undefined) {
        continue;
      }
      let currentButton = document.getElementById(page.menuButtonId);
      if (currentButton === null) {
        continue;
      }
      currentButton.pageToSelect = label;
      currentButton.addEventListener(
        "click", () => {
          this.selectPage(label);
        }
      );
    }
  }

  resetTopicProblems() {
    problemPage.allProblems.resetTopicProblems();
  }

  showProfilePicture() {
    document.getElementById("divProfilePicture").classList.remove("divInvisible");
    document.getElementById("divProfilePicture").classList.add("divVisible");
    if (this.user.googleProfile.picture === undefined) {
      return;
    }
    if (document.getElementById("divProfilePicture").children.length > 0) {
      return;
    }
    try {
      let profilePicElement = document.createElement("IMG");
      profilePicElement.setAttribute("src", this.user.googleProfile.picture);
      profilePicElement.setAttribute("alt", this.user.googleProfile.name);
      profilePicElement.setAttribute("id", "imgProfilePicture");
      profilePicElement.setAttribute("title", this.user.googleProfile.name);
      profilePicElement.setAttribute("className", "profilePicture");
      //profilePicElement.setAttribute("width", 50);
      document.getElementById("divProfilePicture").appendChild(profilePicElement);
    } catch (e) {
      console.log("Failed to set profile picture: " + e);
    }
  }

  initializeCalculatorPage() {
    this.initializeCalculatorPagePartOne();
    this.initializeCalculatorPagePartTwo();
  }

  /**@type {boolean} */
  loginAttemptDesired() {
    if (this.storage.variables.currentPage.getValue() === this.pages.activateAccount.name) {
      return false;
    }
    if (document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel) === null) {
      return false;
    }
    return true;
  }

  initializeCalculatorPagePartOne() {
    cookies.setCookie("useJSON", true, 300, false);
    this.initMenuBar();
    this.initBuildVersion();
    this.initHandlers();
    //////////////////////////////////////
    //////////////////////////////////////
    //Initialize global variables
    //////////////////////////////////////
    //////////////////////////////////////
    this.theCourses = {};
    this.logoutRequestFromUrl = null;
    this.locationRequestFromUrl = null;
    this.storage.loadSettings();
    this.hashHistory = [];
    this.lastKnownGoodProblemFileName = "";
    this.user = new User();
    this.aceEditorAutoCompletionWordList = [];
    this.flagDoSubmitCalculatorComputation = true;
    // Select page on first load
    this.selectPage(this.storage.variables.currentPage.getValue());
    if (this.loginAttemptDesired()) {
      login.loginTry();
    }
    let appElement = document.getElementById(ids.domElements.applicationIdentifiers.default);
    if (appElement === null) {
      return;
    }
    appElement.style.display = "";
    appElement.className = "divOnePageApp";
  }

  toStringProblem() {
    return this.storage.toStringProblem();
  }

  initializeCalculatorPagePartTwo() {
    initializeButtons.initializeButtons();
    mathTypeSet.typesetter.typesetSoft(ids.domElements.divMathjaxProblematicRender);
  }

  sectionSelect(sectionNumber) {
    this.storage.variables.currentSectionComputed.setAndStore(sectionNumber);
    this.user.sectionComputed = this.user.sectionsTaught[sectionNumber];
    let deadlineSpans = document.getElementsByClassName(ids.domElements.classSpanDeadlineContainer);
    for (let i = 0; i < deadlineSpans.length; i++) {
      let currentDeadlineSpan = deadlineSpans[i];
      let currentDeadlineId = currentDeadlineSpan.id.substr(
        ids.stringResources.prefixDeadlineContainer.length
      );
      let currentProblem = problemPage.allProblems[currentDeadlineId];
      if (currentProblem === undefined) {
        continue;
      }
      currentDeadlineSpan.innerHTML = currentProblem.toStringDeadlinePanel();
    }
  }

  onDebugValueChange() {
    let sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
    if (sliderDebug === null) {
      return;
    }
    let debugOn = this.storage.variables.flagDebug.isTrue();
    sliderDebug.checked = debugOn;
    let debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
    if (debugOn) {
      solve.solver.setDebugLogContainer();
      debugSpan.innerHTML = "Debug <b style='color:red'>on</b>";
    } else {
      solve.solver.setDebugLogContainer();
      debugSpan.innerHTML = "Debug <b style='color:green'>off</b>";
    }
  }

  setSwitchDebug() {
    let sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
    this.storage.variables.flagDebug.setAndStore(sliderDebug.checked);
    this.pages.problemPage.flagLoaded = false;
    this.selectPage(this.storage.variables.currentPage.getValue());
  }

  setSwitchStudentView() {
    let sliderStudentView = document.getElementById(ids.domElements.sliderStudentView);
    this.storage.variables.flagStudentView.setAndStore(sliderStudentView.checked);
    this.pages.problemPage.flagLoaded = false;
    this.selectPage(this.storage.variables.currentPage.getValue());
  }

  studentView() {
    return this.storage.variables.flagStudentView.isTrue();
  }

  onStudentViewChange() {
    let studentView = this.storage.variables.flagStudentView.isTrue();
    let slider = document.getElementById(ids.domElements.sliderStudentView);
    if (slider === null) {
      return;
    }
    slider.checked = studentView;

    let spanView = document.getElementById(ids.domElements.spanStudentViewFlagToggleReport);
    let radioHTML = "";
    if (studentView) {
      spanView.innerHTML = "Student view";
      for (let counterSections = 0; counterSections < this.user.sectionsTaught.length; counterSections++) {
        radioHTML += `<br><label class = "containerRadioButton">`;
        radioHTML += `<input type = "radio" name = "radioSection" onchange = "window.calculator.mainPage.sectionSelect(${counterSections}); " `;
        let counterFromStorage = parseInt(this.storage.variables.currentSectionComputed.getValue());
        if (counterSections === counterFromStorage) {
          radioHTML += "checked = 'true'";
        }
        radioHTML += `>`;
        radioHTML += `<span class='radioMark'></span>`;
        radioHTML += this.user.sectionsTaught[counterSections];
        radioHTML += `</label>`
      }
    } else {
      spanView.innerHTML = "Admin view";
    }
    login.resetPagesNeedingReload();
    login.setAdminPanels();
    document.getElementById(ids.domElements.spanStudentViewSectionSelectPanel).innerHTML = radioHTML;
  }

  removeOneScript(scriptId) {
    this.scriptInjector.removeOneScript(scriptId);
  }

  removeScripts(scriptIds) {
    this.scriptInjector.removeScripts(scriptIds);
  }

  selectPage(
    /** @type{string} */
    inputPage,
  ) {
    if (!(inputPage in this.pages)) {
      inputPage = this.defaultPage;
    }
    this.storage.variables.currentPage.setAndStore(inputPage);
    if (this.flagProblemPageOnly) {
      this.selectPageFunction(inputPage);
      return;
    }
    for (let label in this.pages) {
      let page = this.pages[label];
      if (page.container === null) {
        continue;
      }
      page.container.style.display = "none";
      if (page.menuButtonId !== null && page.menuButtonId !== undefined) {
        let button = document.getElementById(page.menuButtonId);
        if (button !== null) {
          button.classList.remove("buttonSelectPageSelected");
        }
      }
    }
    let selectedPage = this.pages[inputPage];
    if (selectedPage.container === null) {
      return;
    }
    selectedPage.container.style.display = "";
    if (selectedPage.menuButtonId !== null && selectedPage.menuButtonId !== undefined) {
      let button = document.getElementById(selectedPage.menuButtonId);
      if (button !== null) {
        button.classList.add("buttonSelectPageSelected");
      }
    }
    this.selectPageFunction(inputPage);
  }

  selectPageFunction(
    /** @type{string} */
    inputPage,
  ) {
    let selectedPage = this.pages[inputPage];
    if (selectedPage.selectFunction !== null && selectedPage.selectFunction !== undefined) {
      selectedPage.selectFunction();
    }
  }

  getProblemByIdOrNull(label) {
    return problemPage.allProblems.getProblemByIdOrNull(label);
  }

  getProblemById(label) {
    let element = document.getElementById(ids.domElements.problemPageContentContainer);
    return problemPage.allProblems.getProblemByIdOrRegisterEmpty(label, element);
  }


  toggleMonitoring() {
    let monitoring = this.storage.variables.calculator.monitoring;
    if (monitoring.value !== "false") {
      monitoring.setAndStore("false")
    } else {
      monitoring.setAndStore("true")
    }
  }

  /**@returns {HTMLButtonElement} */
  pauseButton() {
    return document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle);
  }

  setMonitoringComponent() {
    let monitoring = this.storage.variables.calculator.monitoring.value;
    if (monitoring !== "false") {
      monitoring = "true";
    }
    let monitorResult = document.getElementById(ids.domElements.pages.calculator.monitoring.spanStatus);
    if (monitorResult === null) {
      return;
    }
    let pauseButton = this.pauseButton();
    if (monitoring === "true") {
      monitorResult.innerHTML = "Monitor <b style='color:red'>on</b>";
      document.getElementById(ids.domElements.switch.monitoring).checked = true;
      pauseButton.style.display = "";
    } else {
      document.getElementById(ids.domElements.switch.monitoring).checked = false;
      monitorResult.innerHTML = "Monitor <b style='color:green'>off</b>";
      pauseButton.style.display = "none";
    }
  }
}

/**
 * @returns {Page}
 * */
function mainPage() {
  return window.calculator.mainPage;
}

/**@returns {String} */
function getCleanedUpURL(input) {
  return mainPage().storage.getCleanedUpURL(input);
}

module.exports = {
  Page,
  mainPage,
  getCleanedUpURL,
};
