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
const calculatorPage = require("./calculator_page");
const signUp = require("./signup").signUp;
const mathTypeSet = require("./math_typeset");
const themes = require("./themes");
const solver = require("./solve");
const solve = require("./solve");
const solveSocratic = require("./solve_socratic");
const forgotPassword = require("./forgot_password");
const compareExpressions = require("./compare_expressions").compareExpressions;
const compareExpressionsSocratic = require("./compare_expressions_socratic").compareExpressionsSocratic;
const storage = require("./storage").storage;
const miscellaneous = require("./miscellaneous_frontend");
const user = require("./user");


class Page {
  constructor() {
    window.calculator.mainPage = this;
    this.appElement = document.getElementById(
      ids.domElements.applicationIdentifiers.default
    );

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
        id: ids.domElements.pages.calculator.divCalculatorPage,
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
        selectFunction: () => {
          signUp.signUp();
        }
      },
      forgotLogin: {
        name: "forgotLogin",
        id: "divForgotLogin",
        container: null,
        selectFunction: () => {
          forgotPassword.forgotLogin.forgotLoginPage();
        },
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
    this.currentPage = "";
    if (
      document.getElementById(
        ids.domElements.applicationIdentifiers.compareExpressions
      )
    ) {
      this.defaultPage = "compareExpressions";
    }
    this.storage = storage;
  }

  logoutCallbackAllUsers() {
    this.user.hideProfilePicture();
    this.user.flagLoggedIn = false;
    this.user.sectionsTaught = [];
  }

  logoutCallbackAdditionalForNonAdmins() {
    this.selectAndStorePage(this.pages.login.name);
  }

  initializeLoginPage() {
    login.authenticator.initialize(
      () => {
        this.logoutCallbackAllUsers();
      }, () => {
        this.logoutCallbackAdditionalForNonAdmins();
      },
      this
    );
    let forgotLogin = document.getElementById(
      ids.domElements.pages.login.buttonForgotLogin
    );
    if (forgotLogin === null) {
      // No forgot login button.
      return;
    }
    forgotLogin.addEventListener("click", () => {
      this.selectPage(this.pages.forgotLogin.name);
    });
    let sendRecoveryEmail = document.getElementById(
      ids.domElements.pages.forgotLogin.buttonSendRecoveryEmail
    );
    sendRecoveryEmail.addEventListener("click", () => {
      forgotPassword.forgotLogin.submitForgotPassword();
    });
  }

  initializeAccountButtons() {
    const button = document.getElementById(
      ids.domElements.pages.activateAccount.buttonChangePassword
    );
    if (button === null) {
      // No accordion buttons.
      return;
    }
    button.addEventListener("click", () => {
      activateAccount.accountActivator.submitActivateAccountRequest();
    });
  }

  initializeSliders() {
    this.setWebAssemblySlider();
    this.initializeMonitoringComponent();
    themes.theme.initializeThemes(storage.variables.theme.getValue());
    this.initializeDebugSlider();
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
    miscellaneous.writeHTML(
      buildVersion,
      `Build version ${serverInformation.serverInformation.version}`,
    );
  }

  serverIsOnLocalHost() {
    if (
      window.location.hostname === "localhost" ||
      window.location.hostname === "127.0.0.1"
    ) {
      return true;
    }
    return false;
  }

  initializeHandlers() {
    window.addEventListener("hashchange", () => {
      storage.loadSettings();
      this.selectPage(storage.variables.currentPage.getValue());
    });
    window.addEventListener("popstate", () => {
      storage.loadSettings();
      this.selectPage(storage.variables.currentPage.getValue());
    });
    let monitor = document.getElementById(ids.domElements.switch.monitoring);
    if (monitor !== null) {
      monitor.addEventListener("change", () => {
        this.setMonitoringComponent();
      });
    }
    let webAssembly = document.getElementById(
      ids.domElements.switch.sliderWebAssembly
    );
    if (webAssembly !== null) {
      webAssembly.addEventListener("change", () => {
        this.toggleWebAssembly();
      });
    }
  }

  initializeMenuBar() {
    const menuDiv = document.getElementById(
      ids.domElements.menu.divMainMenuPanel
    );
    menuDiv.style.display = "";
    this.hideOrUnhideMainMenu();
    document.getElementById(ids.domElements.divLoading).style.display = "none";

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
          this.selectAndStorePage(label);
        }
      );
    }
  }

  selectAndStorePage(
    /** @type {string} */
    label
  ) {
    storage.variables.currentPage.setAndStore(label);
    this.selectPage(label);
  }

  resetTopicProblems() {
    problemPage.allProblems.resetTopicProblems();
  }

  showProfilePicture() {
    document.getElementById(ids.domElements.divProfilePicture).classList.remove("divInvisible");
    document.getElementById(ids.domElements.divProfilePicture).classList.add("divVisible");
    if (this.user.googleProfile.picture === undefined) {
      return;
    }
    if (document.getElementById(ids.domElements.divProfilePicture).children.length > 0) {
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
      document.getElementById(ids.domElements.divProfilePicture).appendChild(profilePicElement);
    } catch (e) {
      console.log("Failed to set profile picture: " + e);
    }
  }

  initializeCalculatorPage() {
    if (this.appElement === null) {
      return;
    }
    this.initializeCalculatorPagePartOne();
    this.initializeCalculatorPagePartTwo();
  }

  /** @type {boolean} */
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
    this.initializeMenuBar();
    this.initBuildVersion();
    this.initializeHandlers();
    //////////////////////////////////////
    //////////////////////////////////////
    //Initialize global variables
    //////////////////////////////////////
    //////////////////////////////////////
    this.courses = {};
    this.logoutRequestFromUrl = null;
    this.locationRequestFromUrl = null;
    this.storage.loadSettings();

    this.initializeSliders();
    this.flagProblemPageOnly = false;
    this.mainMenuExpandedLength = null;
    this.initializeLoginPage();
    accountPage.accountPage.initialize(this);
    editPage.problemEditor.initialize(this);
    database.databasePage.initialize(this);
    coursePage.courseSelector.initialize(this);
    this.initializeAccountButtons();
    this.initializeMenuBar();
    this.hashHistory = [];
    this.lastKnownGoodProblemFileName = "";
    this.user = user.globalUser;
    this.aceEditorAutoCompletionWordList = [];
    this.flagDoSubmitCalculatorComputation = true;
    // Select page on first load
    this.selectPage(this.storage.variables.currentPage.getValue());
    if (this.loginAttemptDesired()) {
      login.loginTry();
    }
    this.appElement.style.display = "";
    this.appElement.className = "divOnePageApp";
  }

  toStringProblem() {
    return this.storage.toStringProblem();
  }

  initializeCalculatorPagePartTwo() {
    this.initializeButtons();
    mathTypeSet.typesetter.typesetSoft(
      ids.domElements.divMathjaxProblematicRender
    );
    document.getElementById(
      ids.domElements.sliderStudentView
    ).addEventListener('change', () => {
      this.setSwitchStudentView();
    });
  }

  sectionSelect(sectionNumber) {
    this.storage.variables.currentSectionComputed.setAndStore(sectionNumber);
    this.user.sectionComputed = this.user.sectionsTaught[sectionNumber];
    let deadlineSpans = document.getElementsByClassName(
      ids.domElements.classSpanDeadlineContainer
    );
    for (let i = 0; i < deadlineSpans.length; i++) {
      let currentDeadlineSpan = deadlineSpans[i];
      let currentDeadlineId = currentDeadlineSpan.id.substr(
        ids.stringResources.prefixDeadlineContainer.length
      );
      let currentProblem = problemPage.allProblems[currentDeadlineId];
      if (currentProblem === undefined) {
        continue;
      }
      currentDeadlineSpan.textContent = "";
      currentDeadlineSpan.appendChild(currentProblem.toHTMLDeadlinePanel());
    }
  }

  initializeDebugSlider() {
    const slider = document.getElementById(
      ids.domElements.sliderDebugFlag
    );
    if (storage.variables.flagDebug.isTrue()) {
      slider.checked = true;
    }
    this.setSwitchDebug();
    slider.addEventListener('change', () => {
      this.setSwitchDebug();
    });
  }

  setSwitchDebug() {
    let sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
    if (sliderDebug === null) {
      return;
    }
    let debugOn = sliderDebug.checked;
    problemPage.allProblems.flagLoaded = false;
    let debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
    if (debugOn) {
      storage.variables.flagDebug.setAndStore("true");
      solve.solver.setDebugLogContainer();
      miscellaneous.writeHTML(debugSpan, "Debug <b style='color:red'>on</b>");
    } else {
      storage.variables.flagDebug.setAndStore("false");
      solve.solver.setDebugLogContainer();
      miscellaneous.writeHTML(debugSpan, "Debug <b style='color:green'>off</b>");
    }
  }

  setSwitchStudentView() {
    let sliderStudentView = document.getElementById(ids.domElements.sliderStudentView);
    this.storage.variables.flagStudentView.setAndStore(sliderStudentView.checked);
    problemPage.allProblems.flagLoaded = false;
    this.selectPage(this.storage.variables.currentPage.getValue());
  }

  hasInstructorRightsNotViewingAsStudent() {
    return this.user.hasInstructorRights() && !this.studentView();
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
    let radioPanel = document.getElementById(
      ids.domElements.spanStudentViewSectionSelectPanel
    );
    radioPanel.textContent = '';
    if (studentView) {
      spanView.textContent = "Student view";
      for (let i = 0; i < this.user.sectionsTaught.length; i++) {
        radioPanel.appendChild(document.createElement("br"));
        let label = document.createElement("label");
        label.className = "containerRadioButton";
        radioPanel.appendChild(label);
        let input = document.createElement("input")
        input.type = "radio";
        input.name = "radioSection";
        input.addEventListener("change", () => {
          this.sectionSelect(i);
        });
        radioPanel.appendChild(input);

        let counterFromStorage = parseInt(this.storage.variables.currentSectionComputed.getValue());
        if (i === counterFromStorage) {
          input.checked = true;
        }
        let radioMarkSpan = document.createElement("radioMark");
        radioMarkSpan.textContent = this.user.sectionsTaught[i];
        radioPanel.appendChild(radioMarkSpan)
      }
    } else {
      spanView.textContent = "Admin view";
    }
    login.resetPagesNeedingReload();
    login.setAdminPanels();
  }

  removeOneScript(scriptId) {
    this.scriptInjector.removeOneScript(scriptId);
  }

  removeScripts(scriptIds) {
    this.scriptInjector.removeScripts(scriptIds);
  }

  selectPage(
    /** @type {string} */
    inputPage,
  ) {
    if (!(inputPage in this.pages)) {
      inputPage = this.defaultPage;
    }
    if (inputPage === this.currentPage) {
      return;
    }
    this.currentPage = inputPage;
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
    if (
      selectedPage.menuButtonId !== null &&
      selectedPage.menuButtonId !== undefined
    ) {
      let button = document.getElementById(selectedPage.menuButtonId);
      if (button !== null) {
        button.classList.add("buttonSelectPageSelected");
      }
    }
    this.selectPageFunction(inputPage);
  }

  selectPageFunction(
    /** @type {string} */
    inputPage,
  ) {
    let selectedPage = this.pages[inputPage];
    if (
      selectedPage.selectFunction !== null &&
      selectedPage.selectFunction !== undefined
    ) {
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

  /** @return {HTMLButtonElement} */
  pauseButton() {
    return document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle);
  }

  toggleWebAssembly() {
    let webAssembly = this.storage.variables.calculator.useWebAssembly;
    if (webAssembly.value !== "true") {
      webAssembly.setAndStore("true");
    } else {
      webAssembly.setAndStore("false");
    }
    this.setWebAssemblySlider();
  }

  setWebAssemblySlider() {
    let webAssembly = this.storage.variables.calculator.useWebAssembly.value;
    if (webAssembly !== "true") {
      webAssembly = "false";
    }
    let webAssemblyStatus = document.getElementById(ids.domElements.switch.spanWebAssemblyStatus);
    let webAssemblySlider = document.getElementById(ids.domElements.switch.sliderWebAssembly);
    if (webAssemblyStatus === null) {
      return;
    }
    let buttonGo = document.getElementById(ids.domElements.pages.calculator.buttonGoCalculatorPage);

    if (webAssembly === "true") {
      webAssemblySlider.checked = true;
      miscellaneous.writeHTML(webAssemblyStatus, "Web assembly <b style='color:red'>ON</b>");
      miscellaneous.writeHTML(buttonGo, "Go <b style='color:red'>wasm</b>");
    } else {
      webAssemblySlider.checked = false;
      miscellaneous.writeHTML(webAssemblyStatus, "Web assembly <b style='color:green'>off</b>");
      buttonGo.textContent = "Go";
    }
  }

  initializeMonitoringComponent() {
    let monitoringStorage = this.storage.variables.calculator.monitoring;
    let monitor = document.getElementById(ids.domElements.switch.monitoring);
    if (monitoringStorage.isTrue()) {
      monitor.checked = true;
    } else {
      monitor.checked = false;
    }
    this.setMonitoringComponent();
  }

  setMonitoringComponent() {
    let monitoringStorage = this.storage.variables.calculator.monitoring;
    let monitoring = document.getElementById(ids.domElements.switch.monitoring).checked;
    if (monitoring) {
      monitoringStorage.setAndStore("true");
    } else {
      monitoringStorage.setAndStore("false");
    }
    let monitorResult = document.getElementById(
      ids.domElements.pages.calculator.monitoring.spanStatus
    );
    if (monitorResult === null) {
      return;
    }
    let pauseButton = this.pauseButton();
    if (monitoring) {
      miscellaneous.writeHTML(monitorResult, "Monitor <b style='color:red'>on</b>");
    } else {
      miscellaneous.writeHTML(monitorResult, "Monitor <b style='color:green'>off</b>");
      pauseButton.style.display = "none";
    }
  }

  hideOrUnhideMainMenu() {
    if (storage.variables.mainMenuIsHidden.isTrue()) {
      this.hideMainMenu();
    } else {
      this.unhideMainMenu();
    }
  }

  unhideMainMenu() {
    storage.variables.mainMenuIsHidden.setAndStore(false);
    let menuDiv = document.getElementById(
      ids.domElements.menu.divMainMenuPanel
    );
    let toggleButton = document.getElementById(
      ids.domElements.menu.buttonToggleTheMainMenu
    );
    for (let i = 3; i < menuDiv.childNodes.length; i++) {
      let currentNode = menuDiv.childNodes[i];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div") {
        currentNode.style.display = "";
      }
    }
    menuDiv.classList.remove("divMainMenuCollapsed");
    miscellaneous.writeHTML(toggleButton, "&#9660;");
    document.getElementById(
      ids.domElements.divProfilePicture
    ).classList.remove(
      "profilePictureContainerCollapsed"
    );
  }

  hideMainMenu() {
    storage.variables.mainMenuIsHidden.setAndStore(true);
    let menuDiv = document.getElementById(
      ids.domElements.menu.divMainMenuPanel
    );
    let toggleButton = document.getElementById(
      ids.domElements.menu.buttonToggleTheMainMenu
    );
    if (this.mainMenuExpandedLength === null) {
      this.mainMenuExpandedLength = menuDiv.style.width;
    }
    for (let i = 3; i < menuDiv.childNodes.length; i++) {
      let currentNode = menuDiv.childNodes[i];
      if (
        currentNode.nodeName === "DIV" ||
        currentNode.nodeName === "div"
      ) {
        currentNode.style.display = "none";
      }
    }
    menuDiv.classList.add("divMainMenuCollapsed");
    miscellaneous.writeHTML(toggleButton, "&#9656;");
    document.getElementById(
      ids.domElements.divProfilePicture
    ).classList.add(
      "profilePictureContainerCollapsed"
    );
  }

  initializeButtons() {
    let buttonToggleMainMenu = document.getElementById(
      ids.domElements.menu.buttonToggleTheMainMenu
    );
    buttonToggleMainMenu.addEventListener('click', () => {
      this.hideOrUnhideMainMenu();
    });
  }
}


/**
 * @return {Page}
 */
function mainPage() {
  return window.calculator.mainPage;
}

/** @return {String} */
function getCleanedUpURL(input) {
  return mainPage().storage.getCleanedUpURL(input);
}

module.exports = {
  Page,
  mainPage,
  getCleanedUpURL,
};
