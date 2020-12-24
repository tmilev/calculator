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
    var thePage = window.calculator.mainPage;
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
        scriptIds: [],
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
    this.storage = storage;
    this.initializeStorageCallbacks();
    this.scriptInjector = new AllScripts();
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
    document.getElementById(ids.domElements.calculatorBuildVersion).innerHTML = `Build version ${serverInformation.serverInformation.version}`;
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
    for (let page in this.pages) {
      this.pages[page].container = document.getElementById(this.pages[page].id);
      if (this.pages[page].menuButtonId !== null && this.pages[page].menuButtonId !== undefined) {
        let currentButton = document.getElementById(this.pages[page].menuButtonId);
        currentButton.pageToSelect = page;
        currentButton.addEventListener("click", this.selectPage.bind(this, page));
      }
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
      var theProfilePicElement = document.createElement("IMG");
      theProfilePicElement.setAttribute("src", this.user.googleProfile.picture);
      theProfilePicElement.setAttribute("alt", this.user.googleProfile.name);
      theProfilePicElement.setAttribute("id", "imgProfilePicture");
      theProfilePicElement.setAttribute("title", this.user.googleProfile.name);
      theProfilePicElement.setAttribute("className", "profilePicture");
      //theProfilePicElement.setAttribute("width", 50);
      document.getElementById("divProfilePicture").appendChild(theProfilePicElement);
    } catch (e) {
      console.log("Failed to set profile picture: " + e);
    }
  }

  initializeCalculatorPage() {
    this.initializeCalculatorPagePartOne();
    this.initializeCalculatorPagePartTwo();
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
    this.theCourses = {
    };
    this.logoutRequestFromUrl = null;
    this.locationRequestFromUrl = null;
    this.storage.loadSettings();
    this.hashHistory = [];
    this.lastKnownGoodProblemFileName = "";
    this.user = new User();
    this.aceEditorAutoCompletionWordList = [];
    this.flagDoSubmitCalculatorComputation = true;
    //////////////////////////////////////
    //////////////////////////////////////
    //Page manipulation functions
    //////////////////////////////////////
    //////////////////////////////////////
    //////////////////////////////////////
    //////////////////////////////////////
    //Select page on first load
    //////////////////////////////////////
    //////////////////////////////////////
    this.selectPage(this.storage.variables.currentPage.getValue());
    if (this.storage.variables.currentPage.getValue() != this.pages.activateAccount.name) {
      login.loginTry();
    }
    document.getElementById("divOnePageApp").style.display = "";
    document.getElementById("divOnePageApp").className = "divOnePageApp";
  }

  toStringProblem() {
    return this.storage.toStringProblem();
  }

  initializeCalculatorPagePartTwo() {
    initializeButtons.initializeButtons();
    mathTypeSet.typesetter.typesetHard(ids.domElements.divMathjaxProblematicRender);
  }

  sectionSelect(sectionNumber) {
    this.storage.variables.currentSectionComputed.setAndStore(sectionNumber);
    this.user.sectionComputed = this.user.sectionsTaught[sectionNumber];
    var deadlineSpans = document.getElementsByClassName(ids.domElements.classSpanDeadlineContainer);
    for (var i = 0; i < deadlineSpans.length; i++) {
      var currentDeadlineSpan = deadlineSpans[i];
      var currentDeadlineId = currentDeadlineSpan.id.substr(
        ids.stringResources.prefixDeadlineContainer.length
      );
      var currentProblem = problemPage.allProblems[currentDeadlineId];
      if (currentProblem === undefined) {
        continue;
      }
      currentDeadlineSpan.innerHTML = currentProblem.toStringDeadlinePanel();
    }
  }

  onDebugValueChange() {
    var sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
    var debugOn = this.storage.variables.flagDebug.isTrue();
    sliderDebug.checked = debugOn;
    var debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
    if (debugOn) {
      solve.solver.setDebugLogContainer();
      debugSpan.innerHTML = "Debug <b style = 'color:red'>on</b>";
    } else {
      solve.solver.setDebugLogContainer();
      debugSpan.innerHTML = "Debug <b style = 'color:green'>off</b>";
    }
  }

  setSwitchDebug() {
    var sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
    this.storage.variables.flagDebug.setAndStore(sliderDebug.checked);
    this.pages.problemPage.flagLoaded = false;
    this.selectPage(this.storage.variables.currentPage.getValue());
  }

  setSwitchStudentView() {
    var sliderStudentView = document.getElementById(ids.domElements.sliderStudentView);
    this.storage.variables.flagStudentView.setAndStore(sliderStudentView.checked);
    this.pages.problemPage.flagLoaded = false;
    this.selectPage(this.storage.variables.currentPage.getValue());
  }

  studentView() {
    return this.storage.variables.flagStudentView.isTrue();
  }

  onStudentViewChange() {
    let studentView = this.storage.variables.flagStudentView.isTrue();
    document.getElementById(ids.domElements.sliderStudentView).checked = studentView;
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
        radioHTML += `<span class = "radioMark"></span>`;
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

  injectScript(scriptId, scriptContent) {
    this.scriptInjector.injectScript(scriptId, scriptContent);
  }

  selectPage(
    /** @type{string} */
    inputPage,
  ) {
    if (this.pages[inputPage] === undefined) {
      inputPage = "calculator";
    }
    this.storage.variables.currentPage.setAndStore(inputPage);
    if (!this.flagProblemPageOnly) {
      for (let page in this.pages) {
        this.pages[page].container.style.display = "none";
        if (this.pages[page].menuButtonId !== null && this.pages[page].menuButtonId !== undefined) {
          document.getElementById(this.pages[page].menuButtonId).classList.remove("buttonSelectPageSelected");
        }
      }
      this.pages[inputPage].container.style.display = "";
      if (this.pages[inputPage].menuButtonId !== null && this.pages[inputPage].menuButtonId !== undefined) {
        document.getElementById(this.pages[inputPage].menuButtonId).classList.add("buttonSelectPageSelected");
      }
    }
    if (this.pages[inputPage].selectFunction !== null && this.pages[inputPage].selectFunction !== undefined) {
      this.pages[inputPage].selectFunction();
    }
    //location.href = `#${this.getHash()}`;
  }

  getCurrentProblem() {
    var problemFileName = this.storage.variables.currentCourse.problemFileName.getValue();
    if (
      problemFileName === "" ||
      problemFileName === null ||
      problemFileName === undefined
    ) {
      return null;
    }
    return problemPage.allProblems.getProblemByIdOrRegisterEmpty(problemFileName);
  }

  getProblemByIdOrNull(label) {
    return problemPage.allProblems.getProblemByIdOrNull(label);
  }

  getProblemById(label) {
    return problemPage.allProblems.getProblemByIdOrRegisterEmpty(label);
  }

  cleanUpLoginSpan(componentToCleanUp) {
    var loginInfo = document.getElementById("spanLoginRequired");
    if (loginInfo !== null) {
      if (loginInfo.parentElement === componentToCleanUp) {
        loginInfo.innerHTML = "<b>...</b>";
      }
    }
  }

  toggleMonitoring() {
    var monitoring = this.storage.variables.calculator.monitoring;
    if (monitoring.value !== "false") {
      monitoring.setAndStore("false")
    } else {
      monitoring.setAndStore("true")
    }
  }

  setMonitoringComponent() {
    var monitoring = this.storage.variables.calculator.monitoring.value;
    if (monitoring !== "false") {
      monitoring = "true";
    }
    var monitorResult = document.getElementById(ids.domElements.monitoring.spanStatus);
    if (monitoring === "true") {
      monitorResult.innerHTML = "Monitor <b style = 'color:red'>on</b>";
      document.getElementById(ids.domElements.switch.monitoring).checked = true;
      document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).style.display = "";
    } else {
      document.getElementById(ids.domElements.switch.monitoring).checked = false;
      monitorResult.innerHTML = "Monitor <b style = 'color:green'>off</b>";
      document.getElementById(ids.domElements.monitoring.buttonTogglePauseRequest).style.display = "none";
    }
  }
}

class Script {
  constructor() {
    this.id = "";
    this.content = "";
  }
}

class AllScripts {
  constructor() {
    /**@{Script[]} */
    this.scriptsInjected = {};
  }

  removeOneScript(scriptId) {
    var theScript = document.getElementById(scriptId);
    if (theScript === null) {
      return;
    }
    var parent = theScript.parentNode;
    parent.removeChild(theScript);
  }

  removeScripts(scriptIds) {
    for (var counter = 0; counter < scriptIds.length; counter++) {
      this.removeOneScript(scriptIds[counter]);
    }
  }

  injectScript(scriptId, scriptContent) {
    this.removeOneScript(scriptId);
    if (scriptContent !== undefined && scriptContent !== null) {
      this.scriptsInjected[scriptId] = new Script();
      var theScript = this.scriptsInjected[scriptId];
      theScript.id = scriptId;
      theScript.content = scriptContent;
    }
    var theScript = this.scriptsInjected[scriptId];
    var scriptChild = document.createElement('script');
    scriptChild.setAttribute('id', scriptId);
    scriptChild.innerHTML = theScript.content;
    scriptChild.type = 'text/javascript';
    document.getElementsByTagName('head')[0].appendChild(scriptChild);
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
