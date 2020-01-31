"use strict";
const selectCourse = require('./select_course');
const coursePage = require('./course_page');
const problemPage = require('./problem_page');
const editPage = require("./edit_page");
const database = require('./database');
const serverStatus = require('./server_status');
const accountPage = require('./account');
const activateAccount = require('./account_activation');
const accountManagement = require('./manage_accounts');
const cookies = require('./cookies');
const ids = require("./ids_dom_elements");
const serverInformation = require('./server_information');
const login = require('./login');
const initializeButtons = require('./initialize_buttons');
const calculatorPage = require('./calculator_page');
const signUp = require('./signup').signUp;
const mathjax = require('./mathjax-calculator-setup');
const themes = require('./themes');

function User() {
  this.flagLoggedIn = false;
  this.googleProfile = null;
  this.sectionsTaught = [];
  this.instructor = "";
  this.sectionInDB = "";
  this.deadlineSchema = "";
  this.sectionComputed = "";
}

User.prototype.isLoggedIn = function() {
  return this.flagLoggedIn;
}

User.prototype.getRole = function() {
  return mainPage().storage.variables.user.role.getValue();
}

User.prototype.hasAdminRights = function() {
  return this.getRole() === "admin" && this.isLoggedIn();
}

User.prototype.hasProblemEditRights = function() {
  return this.getRole() === "admin" && this.isLoggedIn();
}

User.prototype.hasInstructorRights = function() {
  return this.getRole() === "admin" && this.isLoggedIn();
}

User.prototype.hideProfilePicture = function() {
  document.getElementById("divProfilePicture").classList.add("divInvisible");
  document.getElementById("divProfilePicture").classList.remove("divVisible");
}

User.prototype.makeFromUserInfo = function(inputData) {
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

function StorageVariable(
/**@type @{{name: string, nameURL: string, nameCookie: string, nameLocalStorage: string, associatedDOMId: string, type: string, secure: string, showInURLByDefault: bool, showInURLOnPages: Object, callbackOnValueChange: function}} */
  inputs
) {
  this.value = "";
  this.name = inputs.name;
  this.nameURL = "";
  this.nameCookie = "";
  this.nameLocalStorage = "";
  this.associatedDOMId = "";
  this.type = "string";
  this.secure = true;
  this.showInURLByDefault = false;
  /**@type {Function} */
  this.callbackOnValueChange = null;
  var labelsToRead = [
    "nameURL",
    "nameCookie",
    "nameLocalStorage",
    "associatedDOMId",
    "type",
    "secure",
    "callbackOnValueChange",
    "showInURLByDefault",
    "showInURLOnPages",
  ];
  for (var counterLabel = 0; counterLabel < labelsToRead.length; counterLabel ++) {
    var currentLabel = labelsToRead[counterLabel];
    var incoming = inputs[currentLabel];
    if (incoming !== "" && incoming !== null && incoming !== undefined) {
      this[currentLabel] = incoming;
    }
  }
}

/**@returns {Boolean}  */
StorageVariable.prototype.isTrue = function() {
  if (this.value === "true" || this.value === true) {
    return true;
  }
  return false;
}

StorageVariable.prototype.getValue = function() {
  return this.value;
}

/**@returns{string} */
StorageVariable.prototype.loadMe = function(hashParsed) {
  if (mainPage().flagProblemPageOnly) {
    // TODO: fix this by uncommenting the code below.
    console.log("Warning: using cookies in standalone mode.")
    // return "";
  }
  var candidate = "";
  if (Storage !== undefined || localStorage !== undefined && this.nameLocalStorage !== "") {
    var incoming = localStorage.getItem(this.nameLocalStorage);
    if (incoming !== "" && incoming !== null && incoming !== undefined) {
      candidate = incoming;
    }
  }
  if (this.nameCookie !== "") {
    var incoming = cookies.getCookie(this.nameCookie);
    if (incoming !== "" && incoming !== null && incoming !== undefined) {
      candidate = incoming;
    }
  }
  if (hashParsed !== null && hashParsed !== undefined) {
    if (this.nameURL !== "") {
      if (this.nameURL in hashParsed) {
        var incoming = hashParsed[this.nameURL];
        if (incoming !== null && incoming !== undefined) {
          candidate = incoming;
        }
      }
    }
  }
  this.setAndStore(candidate, false, true);
  return candidate;
}

StorageVariable.prototype.storeMePersistent = function(
  /**@type {boolean} */
  updateURL,
) {
  if (mainPage().flagProblemPageOnly) {
    console.log("WARNING: Using local storage in stand-alone mode.")
    // TODO: fix this by uncommenting the code below.
    // page is embedded, no persistent storage.
    // return;
  }
  if (Storage !== undefined || localStorage !== undefined) {
    if (this.nameLocalStorage !== "" && this.nameLocalStorage !== null && this.nameLocalStorage !== undefined) {
      localStorage[this.nameLocalStorage] = this.value;
    }
  }
  if (this.nameCookie !== "") {
    cookies.setCookie(this.nameCookie, this.value, 150, this.secure);
  }
  if (mainPage().flagProblemPageOnly) {
    // TODO: this should be removed when the preceding todo is resolved.
    return;
  }
  if (updateURL !== false) {
    mainPage().storage.setURL();
  }
}

StorageVariable.prototype.storeMe = function(
  /**@type {boolean} */
  updateURL,
  /**@type {boolean} */
  updateAssociatedInput,
) {
  this.storeMePersistent(updateURL);
  if (updateAssociatedInput === true) {
    if (this.associatedDOMId !== null && this.associatedDOMId !== undefined && this.associatedDOMId !== "") {
      document.getElementById(this.associatedDOMId).value = this.value;
    }
  }
}

StorageVariable.prototype.setAndStore = function(
  newValue,
  /**@type {Boolean} */
  updateURL,
  /**@type {Boolean} */
  updateAssociatedInput,
) {
  if (updateURL === undefined) {
    updateURL = true;
  }
  var changed = (this.value !== newValue);
  this.value = newValue;
  this.storeMe(updateURL, updateAssociatedInput);
  if (changed) {
    if (this.callbackOnValueChange !== null && this.callbackOnValueChange !== undefined) {
      //calling function with timeout ensures the current function sequence is finished first.
      setTimeout(() => {
        this.callbackOnValueChange(this.value);
      }, 0);
    }
  }
}

var pageNamesOnWhichToShowProblemURLs = {
  selectCourse: true,
  currentCourse: true,
  problemPage: true,
  editPage: true,
};

function StorageCalculator() {
  this.variables = {
    currentPage: new StorageVariable({
      name: "currentPage",
      nameLocalStorage: "currentPage", //<- when given and non-empty, local storage will be used to store variable
      nameCookie: "", //<- when given and non-empty, cookies will be used to store variable
      nameURL: "currentPage", //<- when given and non-empty, url will be used to store variable
      showInURLByDefault: true, // <- when given and true, url will be added to the window hash
      callbackOnValueChange: mainPage().selectPage.bind(mainPage()), //<- when given will be triggered after a genuine change of the variable
    }),
    database: {
      labels: new StorageVariable({
        name: "databaseLabels",
        nameLocalStorage: "databaseLabels"
      }),
    },
    editor: {
      currentlyEditedPage: new StorageVariable({
        name: "currentlyEditedPage",
        nameLocalStorage: "currentlyEditedPage"
      })
    },
    currentSectionComputed: new StorageVariable({
      name: "currentSectionComputed",
      nameLocalStorage: "currentSectionComputed"
    }),
    currentCourse: {
      courseHome: new StorageVariable({
        name: "courseHome",
        nameCookie: "courseHome",
        nameURL: "courseHome",
        showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
      }),
      topicList: new StorageVariable({
        name: "topicList",
        nameCookie: "topicList",
        nameURL: "topicList",
        showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
      }),
      fileName: new StorageVariable({
        name: "fileName",
        nameCookie: "fileName",
        nameURL: "fileName",
        showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
      }),
      problemFileName: new StorageVariable({
        name: "problemFileName",
        nameCookie: "problemFileName",
        nameURL: "problemFileName",
        showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
      }),
      exerciseType: new StorageVariable({
        name: "exerciseType",
        nameLocalStorage: "exerciseType",
        nameURL: "exerciseType"
      }),
      randomSeed: new StorageVariable({
        name: "randomSeed",
        nameURL: "randomSeed",
        // Since this variable has no local storage outside of the URL,
        // to avoid wiping the variable while normalizing the URL,
        // we need to keep it displayed there by default.
        showInURLOnPages: pageNamesOnWhichToShowProblemURLs,
      }),
    },
    flagDebug: new StorageVariable({
      name: "debugFlag",
      nameURL: "debugFlag",
      nameCookie: "debugFlag",
      secure: false,
      callbackOnValueChange: mainPage().onDebugValueChange.bind(mainPage()),
    }),
    flagStudentView: new StorageVariable({
      name: "studentView",
      nameURL: "studentView",
      nameCookie: "studentView",
      secure: true,
      callbackOnValueChange: mainPage().onStudentViewChange.bind(mainPage()),
    }),
    calculator: {
      input: new StorageVariable({
        name: "calculatorInput",
        nameURL: "calculatorInput",
        associatedDOMId: ids.domElements.inputMain,
        callbackOnValueChange: calculatorPage.calculator.submitComputationPartTwo.bind(calculatorPage.calculator),
        showInURLByDefault: true,
      }),
      request: new StorageVariable({
        name: "calculatorRequest",
        nameURL: "calculatorRequest",
        nameLocalStorage: "calculatorRequest",
      }),
      monitoring: new StorageVariable({
        name: "monitoring",
        nameLocalStorage: "monitoring",
        nameURL: "monitoring",
        nameCookie: "monitoring",
        callbackOnValueChange: mainPage().setMonitoringComponent.bind(mainPage()),
        showInURLByDefault: true,
      }),
    },
    user: {
      activationToken: new StorageVariable({
        name: "activationToken",
        nameURL: "activationToken",
      }),
      googleToken: new StorageVariable({
        name: "googleToken",
      }),
      name: new StorageVariable({
        name: "username",
        nameCookie: "username",
        nameURL: "username",
      }),
      authenticationToken: new StorageVariable({
        name: "authenticationToken",
        nameCookie: "authenticationToken",
        nameURL: "authenticationToken",
      }),
      role: new StorageVariable({
        name: "userRole",
        nameCookie: "userRole",
        nameURL: "userRole",
      }),
      email: new StorageVariable({
        name: "email",
        nameURL: "email",
      }),
    },
    theme: new StorageVariable({
      name: "theme",
      nameLocalStorage: "theme",
      callbackOnValueChange: themes.theme.doChangeTheme.bind(themes.theme),
    }),
  };
  this.oldHash = "";
  this.currentHash = "";
  this.urlObject = {};
}

/**@returns {String} */
StorageCalculator.prototype.getPercentEncodedURL = function(input) {
  return encodeURIComponent(JSON.stringify(input));
}

/**@returns {String} */
StorageCalculator.prototype.getCleanedUpURL = function(input) {
  var stringifiedInput = JSON.stringify(input);
  var isGood = true;
  try {
    var decodedAsURL = decodeURIComponent(stringifiedInput);
    if (stringifiedInput !== decodedAsURL) {
      isGood = false;
    }
  } catch (e) {
    isGood = false;
  }
  if (!isGood) {
    stringifiedInput = encodeURIComponent(stringifiedInput);
  }
  return stringifiedInput;
}

StorageCalculator.prototype.parseURL = function() {
  this.oldHash = this.currentHash;
  try {
    this.currentHash = decodeURIComponent(window.location.hash);
    if (this.currentHash.startsWith('#')) {
      this.currentHash = this.currentHash.slice(1);
    }
    if (this.oldHash !== this.currentHash && this.currentHash !== "") {
      this.urlObject = JSON.parse(this.currentHash);
    }
  } catch (e) {
    console.log(`Failed to parse your url hash ${this.currentHash} obtained from ${window.location.hash}. ${e}`);
  }
}

StorageCalculator.prototype.loadSettings = function() {
  this.parseURL();
  this.loadSettingsRecursively(this.variables, this.urlObject);
}

StorageCalculator.prototype.loadSettingsRecursively = function(
  /**@type {StorageVariable} */
  currentStorage,
  inputHashParsed,
) {
  if (currentStorage instanceof StorageVariable) {
    currentStorage.loadMe(inputHashParsed);
    return;
  }
  if (typeof currentStorage === "object") {
    for (var subLabel in currentStorage) {
      this.loadSettingsRecursively(currentStorage[subLabel], inputHashParsed);
    }
  }
}

StorageCalculator.prototype.computeURLRecursively = function(currentStorage, recursionDepth) {
  if (recursionDepth === undefined) {
    recursionDepth = 0;
  }
  if (recursionDepth > 100) {
    throw("Recursion is too deeply nested. This must be a programming error. ");
  }
  var result = {};
  if (currentStorage instanceof StorageVariable) {
    var urlName = currentStorage.nameURL;
    if (urlName === undefined || urlName === null || urlName === "") {
      return null;
    }
    var shouldShow = false;
    if (currentStorage.showInURLByDefault === true) {
      shouldShow = true;
    }
    if (currentStorage.showInURLOnPages !== null && currentStorage.showInURLOnPages !== undefined) {
      if (mainPage().storage.variables.currentPage.value in currentStorage.showInURLOnPages) {
        shouldShow = true;
      }
    }
    if (!shouldShow) {
      return null;
    }
    if (currentStorage.value === null || currentStorage.value == undefined || currentStorage.value == "") {
      return null;
    }
    result[urlName] = currentStorage.value;
    return result;
  }
  if (typeof currentStorage !== "object") {
    throw (`Unexpected currentStorage input while computing url: ${currentStorage}`);
  }
  for (var label in currentStorage) {
    var incoming = this.computeURLRecursively(currentStorage[label], recursionDepth + 1);
    if (incoming === null) {
      continue;
    }
    Object.assign(result, incoming);
  }
  return result;
}

StorageCalculator.prototype.setURL = function () {
  this.urlObject = this.computeURLRecursively(this.variables);
  var incomingHash = this.getPercentEncodedURL(this.urlObject);
  if (incomingHash !== this.currentHash) {
    window.location.hash = incomingHash;
  }
}

function Page() {
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
    selectCourse : {
      name: "selectCourse",
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectCourse.selectCoursePage,
    },
    currentCourse : {
      name: "currentCourse",
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: coursePage.selectCurrentCoursePage,
    },
    problemPage : {
      name: "problemPage",
      id: "divProblemPage",
      menuButtonId: "buttonProblemPage",
      container: null,
      selectFunction: problemPage.updateProblemPage,
      flagLoaded: false,
    },
    editPage : {
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
      selectFunction: calculatorPage.calculator.submitComputation.bind(calculatorPage.calculator),
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
  this.storage = new StorageCalculator();
  this.scriptInjector = new AllScripts();
  this.flagProblemPageOnly = false;
}

Page.prototype.isLoggedIn = function() {
  if (this.flagProblemPageOnly) {
    return false;
  }
  return this.user.isLoggedIn();
}

Page.prototype.initBuildVersion = function() {
  document.getElementById(ids.domElements.calculatorBuildVersion).innerHTML = `Build version ${serverInformation.serverInformation.version}`;
}

Page.prototype.serverIsOnLocalHost = function() {
  if (window.location.hostname === "localhost" || window.location.hostname === "127.0.0.1") {
    return true;
  }
  return false;
}

Page.prototype.initHandlers = function() {
  window.addEventListener("hashchange", this.storage.loadSettings.bind(this.storage));
}

Page.prototype.initMenuBar = function() {
  for (var page in this.pages) {
    this.pages[page].container = document.getElementById(this.pages[page].id);
    if (this.pages[page].menuButtonId !== null && this.pages[page].menuButtonId !== undefined) {
      var currentButton = document.getElementById(this.pages[page].menuButtonId);
      currentButton.pageToSelect = page;
      currentButton.addEventListener("click", this.selectPage.bind(this, page));
    }
  }
}

Page.prototype.resetTopicProblems = function() {
  problemPage.allProblems.resetTopicProblems();
}

Page.prototype.showProfilePicture = function() {
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

Page.prototype.initializeCalculatorPage = function() {
  this.initializeCalculatorPagePartOne();
  this.initializeCalculatorPagePartTwo();
}

Page.prototype.initializeCalculatorPagePartOne = function() {
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

Page.prototype.initializeCalculatorPagePartTwo = function() {
  initializeButtons.initializeButtons();
  initializeButtons.initializeCalculatorPage();
  mathjax.typeSetHard(ids.domElements.divMathjaxProblematicRender);
}

Page.prototype.sectionSelect = function(sectionNumber) {
  //console.log(`DEBUG: section select: ${sectionNumber}`);
  this.storage.variables.currentSectionComputed.setAndStore(sectionNumber);
  this.user.sectionComputed = this.user.sectionsTaught[sectionNumber];
  var deadlineSpans = document.getElementsByClassName(ids.domElements.classSpanDeadlineContainer);
  for (var i = 0; i < deadlineSpans.length; i ++) {
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

Page.prototype.onStudentViewChange = function () {
  var studentView = this.storage.variables.flagStudentView.isTrue();
  document.getElementById(ids.domElements.sliderStudentView).checked = studentView;
  var spanView = document.getElementById(ids.domElements.spanStudentViewFlagToggleReport);
  var radioHTML = "";
  if (studentView) {
    spanView.innerHTML = "Student view";
    for (var counterSections = 0; counterSections < this.user.sectionsTaught.length; counterSections ++) {
      radioHTML += `<br><label class = "containerRadioButton">`;
      radioHTML += `<input type = "radio" name = "radioSection" onchange = "window.calculator.mainPage.sectionSelect(${counterSections});" `;
      var counterFromStorage = parseInt(this.storage.variables.currentSectionComputed.getValue());
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

Page.prototype.onDebugValueChange = function () {
  var sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
  var debugOn = this.storage.variables.flagDebug.isTrue();
  sliderDebug.checked = debugOn;
  var debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
  if (debugOn) {
    debugSpan.innerHTML = "Debug <b style = 'color:red'>on</b>";
  } else {
    debugSpan.innerHTML = "Debug <b style = 'color:green'>off</b>";
  }
}

Page.prototype.setSwitchDebug = function () {
  var sliderDebug = document.getElementById(ids.domElements.sliderDebugFlag);
  this.storage.variables.flagDebug.setAndStore(sliderDebug.checked);
  this.pages.problemPage.flagLoaded = false;
  this.selectPage(this.storage.variables.currentPage.getValue());
}

Page.prototype.setSwitchStudentView = function () {
  var sliderStudentView = document.getElementById(ids.domElements.sliderStudentView);
  this.storage.variables.flagStudentView.setAndStore(sliderStudentView.checked);
  this.pages.problemPage.flagLoaded = false;
  this.selectPage(this.storage.variables.currentPage.getValue());
}

Page.prototype.studentView = function () {
  return this.storage.variables.flagStudentView.isTrue();
}

function Script() {
  this.id = "";
  this.content = "";
}

function AllScripts() {
  /**@{Script[]} */
  this.scriptsInjected = {};
}

AllScripts.prototype.removeOneScript = function(scriptId) {
  var theScript = document.getElementById(scriptId);
  if (theScript === null) {
    return;
  }
  var parent = theScript.parentNode;
  parent.removeChild(theScript);
}

AllScripts.prototype.removeScripts = function(scriptIds) {
  for (var counter = 0; counter < scriptIds.length; counter ++) {
    this.removeOneScript(scriptIds[counter]);
  }
}

AllScripts.prototype.injectScript = function(scriptId, scriptContent) {
  this.removeOneScript(scriptId);
  if (scriptContent !== undefined && scriptContent !== null) {
    this.scriptsInjected[scriptId] = new Script();
    var theScript = this.scriptsInjected[scriptId];
    theScript.id = scriptId;
    theScript.content = scriptContent;
  }
  var theScript = this.scriptsInjected[scriptId];
  var scriptChild = document.createElement('script');
  scriptChild.setAttribute('id', scriptId)
  scriptChild.innerHTML = theScript.content;
  scriptChild.type = 'text/javascript';
  document.getElementsByTagName('head')[0].appendChild(scriptChild);
}

Page.prototype.removeOneScript = function(scriptId) {
  this.scriptInjector.removeOneScript(scriptId);
}

Page.prototype.removeScripts = function(scriptIds) {
  this.scriptInjector.removeScripts(scriptIds);
}

Page.prototype.injectScript = function(scriptId, scriptContent) {
  this.scriptInjector.injectScript(scriptId, scriptContent);
}

Page.prototype.selectPage = function(inputPage) {
  if (this.pages[inputPage] === undefined) {
    inputPage = "calculator";
  }
  this.storage.variables.currentPage.setAndStore(inputPage);
  if (!this.flagProblemPageOnly) {
    for (var page in this.pages) {
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

Page.prototype.getCurrentProblem = function() {
  var problemFileName = this.storage.variables.currentCourse.problemFileName.getValue();
  return problemPage.allProblems.getProblemByIdOrRegisterEmpty(problemFileName);
}

Page.prototype.getProblemById = function(label) {
  return problemPage.allProblems.getProblemByIdOrRegisterEmpty(label);
}

Page.prototype.cleanUpLoginSpan = function(componentToCleanUp) {
  var loginInfo = document.getElementById("spanLoginRequired");
  if (loginInfo !== null) {
    if (loginInfo.parentElement === componentToCleanUp) {
      loginInfo.innerHTML = "<b>...</b>";
    }
  }
}

Page.prototype.toggleMonitoring = function () {
  var monitoring = this.storage.variables.calculator.monitoring;
  if (monitoring.value !== "false") {
    monitoring.setAndStore("false")
  } else {
    monitoring.setAndStore("true")
  }
}

Page.prototype.setMonitoringComponent = function () {
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
/**
 * @returns {Page}
 * */
function mainPage () {
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
