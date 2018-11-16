"use strict";
const selectCourse = require('./select_course');
const coursePage = require('./course_page'); 
const problemPage = require('./problem_page');
const editPage = require('./edit_page');
const database = require('./database');
const serverStatus = require('./server_status');
const accountPage = require('./account');
const activateAccount = require('./account_activation');
const accountManagement = require('./manage_accounts');
const cookies = require('./cookies');
const ids = require('./ids_dom_elements');
const serverInformation = require('./server_information');
const login = require('./login');
const initializeButtons = require('./initialize_buttons');
const calculatorPage = require('./calculator_page');

function User() {
  this.flagLoggedIn = false;
  this.googleProfile = null;
  this.role = "";
  this.sectionsTaught = [];
  this.instructor = "";
  this.sectionInDB = "";
  this.deadlineSchema = "";
  this.sectionComputed = "";
}

User.prototype.isLoggedIn = function() {
  return this.flagLoggedIn;
}

User.prototype.hasAdminRights = function() {
  return this.role === "admin" && this.isLoggedIn();
}

User.prototype.hasProblemEditRights = function() {
  return this.role === "admin" && this.isLoggedIn();
}

User.prototype.hasInstructorRights = function() {
  return this.role === "admin" && this.isLoggedIn();
}

User.prototype.hideProfilePicture = function() {
  document.getElementById("divProfilePicture").classList.add("divInvisible");
  document.getElementById("divProfilePicture").classList.remove("divVisible");
}

User.prototype.makeFromUserInfo = function(inputData) {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.user.authenticationToken.setAndStore(inputData.authenticationToken);
  thePage.storage.variables.user.name.setAndStore(inputData.username);
  this.role = inputData.userRole;
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
/**@type @{{name: string, nameURL: string, nameCookie: string, nameLocalStorage: string, associatedDOMId: string, type: string, secure: string, showInURLByDefault: bool, callbackOnValueChange: function}} */
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
  /**@type {function} */
  this.callbackOnValueChange = null;
  var labelsToRead = [
    "nameURL", "nameCookie", "nameLocalStorage", 
    "associatedDOMId", "type", "secure", "callbackOnValueChange", "showInURLByDefault"
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

StorageVariable.prototype.loadMe = function(hashParsed) {
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
  if (this.nameURL !== "") {
    if (this.nameURL in hashParsed) {
      var incoming = hashParsed[this.nameURL];
      if (incoming !== null && incoming !== undefined) {
        candidate = incoming;
      }
    }
  }
  this.setAndStore(candidate, false, true);
}

StorageVariable.prototype.storeMe = function(/**@type {boolean} */ updateURL, /**@type {boolean} */ updateAssociatedInput) {
  if (Storage !== undefined || localStorage !== undefined) {
    if (this.nameLocalStorage !== "" && this.nameLocalStorage !== null && this.nameLocalStorage !== undefined) {
      localStorage[this.nameLocalStorage] = this.value;
    }
  }
  if (this.nameCookie !== "") {
    cookies.setCookie(this.nameCookie, this.value, 150, this.secure);
  }
  if (updateURL !== false) {
    mainPage().storage.setURL();
  }
  if (updateAssociatedInput === true) {
    if (this.associatedDOMId !== null && this.associatedDOMId !== undefined && this.associatedDOMId !== "") {
      document.getElementById(this.associatedDOMId).value = this.value;
    }
  }
}

StorageVariable.prototype.setAndStore = function(newValue, /**@type {boolean} */ updateURL, /**@type {boolean} */ updateAssociatedInput) {
  if (updateURL === undefined ) {
    updateURL = true;
  }
  var changed = (this.value === newValue);
  this.value = newValue;
  this.storeMe(updateURL, updateAssociatedInput);
  if (changed) {
    if (this.callbackOnValueChange !== null && this.callbackOnValueChange !== undefined) {
      this.callbackOnValueChange(this.value);
    }
  }
}

function StorageCalculator() {
  this.variables = {
    currentPage: new StorageVariable({
      name: "currentPage", 
      nameLocalStorage: "currentPage", //<- when given and non-empty, local storage will be used to store variable
      nameCookie: "", //<- when given and non-empty, cookies will be used to store variable
      nameURL: "currentPage", //<- when given and non-empty, url will be used to store variable
      showInURLByDefault: true, // <- when given and true, url will be added to the window hash
    }),
    database: {
      currentTable: new StorageVariable({
        name: "currentTable", 
        nameLocalStorage: "currentTable"
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
        nameURL: "courseHome"
      }),
      topicList: new StorageVariable({
        name: "topicList",
        nameCookie: "topicList",
        nameURL: "topicList"
      }),
      fileName: new StorageVariable({
        name: "fileName",
        nameCookie: "fileName",
        nameURL: "fileName"
      }),
      problemFileName: new StorageVariable({
        name: "problemFileName",
        nameCookie: "problemFileName",
        nameURL: "problemFileName"
      }),
      currentProblemId: new StorageVariable({
        name: "currentProblemId",
        nameLocalStorage: "currentProblemId",
        nameURL: "currentProblemId"
      }),
      exerciseType: new StorageVariable({
        name: "exerciseType",
        nameLocalStorage: "exerciseType",
        nameURL: "exerciseType"
      })
    },
    flagDebug: new StorageVariable({
      name: "debugFlag", 
      nameURL: "debugFlag", 
      nameCookie: "debugFlag",
      secure: false
    }),
    flagStudentView: new StorageVariable({
      name: "studentView", 
      nameURL: "studentView", 
      nameCookie: "studentView",
      secure: true
    }),
    calculator: {
      input: new StorageVariable({
        name: "calculatorInput", 
        nameURL: "calculatorInput",
        associatedDOMId: ids.domElements.inputMain,
      }),
      request: new StorageVariable({
        name: "calculatorRequest", 
        nameURL: "calculatorRequest", 
        nameLocalStorage: "calculatorRequest"
      }),
    },
    user: {
      activationToken: new StorageVariable({
        name: "activationToken",
        nameURL: "activationToken",
      }),
      googleToken: new StorageVariable({
        name: "googleToken"
      }),
      name: new StorageVariable({
        name: "username", 
        nameCookie: "username", 
        nameURL: "username"
      }),
      authenticationToken: new StorageVariable({
        name: "authenticationToken",
        nameCookie: "authenticationToken",
        nameURL: "authenticationToken"
      }),
      email: new StorageVariable({
        name: "email",
        nameURL: "email"
      })
    }
  };
  this.oldHash = "";
  this.currentHash = "";
  this.urlObject = {};
}

StorageCalculator.prototype.parseURL = function() {
  console.log(`DEBUG: current hash: ${window.location.hash}`);
  this.oldHash = this.currentHash;
  this.currentHash = decodeURIComponent(window.location.hash);
  if (this.currentHash.startsWith('#')) {
    this.currentHash = this.currentHash.slice(1);
  }
  try {
    if (this.oldHash !== this.currentHash) {
      this.urlObject = JSON.parse(this.currentHash);
    }
  } catch (e) {
    console.log(`Failed to parse your url hash ${this.currentHash}. ${e}`);
  }
}

StorageCalculator.prototype.loadSettings = function() {
  this.parseURL();
  this.loadSettingsRecursively(this.variables, this.urlObject);
}

StorageCalculator.prototype.loadSettingsRecursively = function(/**@type {StorageVariable} */ currentStorage, inputHashParsed) {
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

/** Returns true if output has meaningful information, false otherwise.
 * @returns {boolean} 
 * */
StorageCalculator.prototype.computeURLRecursively = function(currentStorage, currentURL) {
  if (currentStorage instanceof StorageVariable) {
    var urlName = currentStorage.nameURL; 
    if (urlName !== undefined && urlName !== null && urlName !== "") {
      if (currentStorage.showInURLByDefault || (urlName in currentURL)) {
        currentURL[urlName] = currentStorage.value;
        //<- we show url variable only if typed in by the user
        // or if it shown in the url by default
        return true
      } 
    }
    return false;
  }
  if (typeof currentStorage !== "object") {
    throw (`Unexpected currentStorage input while computing url: ${currentStorage}`);
  }
  var hasNonTrivialInformation = false;
  for (var label in currentStorage) {
    if  (typeof currentURL[label] !== "object") {
      var urlAdditionCandidate = {}
      if (this.computeURLRecursively(currentStorage[label], urlAdditionCandidate)) {
        currentURL[label] = urlAdditionCandidate[label];
        hasNonTrivialInformation = true;
      }
      
    } else {
      if (this.computeURLRecursively(currentStorage[label], currentURL[label])) {
        hasNonTrivialInformation = true;
      }
    }
  }
  return hasNonTrivialInformation;
}

StorageCalculator.prototype.setURL = function () {
  this.computeURLRecursively(this.variables, this.urlObject);
  var incomingHash = JSON.stringify(this.urlObject);
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
      name: "selectCourse", //<-for autocomplete
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectCourse.selectCoursePage,
    },
    currentCourse : {
      name: "currentCourse", //<-for autocomplete
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: coursePage.selectCurrentCoursePage,
    },
    problemPage : {
      name: "problemPage", //<-for autocomplete
      id: "divProblemPage",
      menuButtonId: "buttonProblemPage",
      container: null,
      selectFunction: problemPage.updateProblemPage,
      flagLoaded: false,
    },
    editPage : {
      name: "editPage", //<-for autocomplete
      id: "divEditPage",
      menuButtonId: "buttonEditPage",
      container: null,
      selectFunction: editPage.selectEditPage,
      flagLoaded: false,
      editor: null,
    },
    calculator: {
      name: "calculator", //<-for autocomplete
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: calculatorPage.calculator.submitComputation.bind(calculatorPage.calculator),
      scriptIds: [],
    },
    signUp: {
      name: "signUp", //<-for autocomplete
      id: "divSignUpPage",
      container: null,
      selectFunction: null,
    },
    forgotLogin: {
      name: "forgotLogin",
      id: "divForgotLogin",
      container: null,
      selectFunction: null,
    },
    about: {
      name: "about", //<-for autocomplete
      id: "divAboutPage",
      menuButtonId: "buttonAboutPage",
      container: null,
      selectFunction: null,
    },
    database: {
      name: "database", //<-for autocomplete
      id: "divDatabase",
      menuButtonId: "buttonSelectDatabase",
      container: null,
      selectFunction: database.updateDatabasePage,
    },
    server: {
      name: "server", //<-for autocomplete
      id: "divServer",
      menuButtonId: "buttonSelectServer",
      container: null,
      selectFunction: serverStatus.updateServerStatus,
    },
    account: {
      name: "account", //<-for autocomplete
      id: "divAccount",
      menuButtonId: "buttonSelectAccount",
      container: null,
      selectFunction: accountPage.updateAccountPage,
    },
    activateAccount: {
      name: "activateAccount", //<-for autocomplete
      id: "divActivateAccount",
      container: null,
      selectFunction: activateAccount.updateAccountActivationPage,
    },
    accounts: {
      name: "accounts", //<-for autocomplete
      id: "divAccounts",
      menuButtonId: "buttonSelectAccounts",
      container: null,
      selectFunction: accountManagement.updateAccountsPage,
    }
  };
  this.storage = new StorageCalculator();
  
  cookies.setCookie("useJSON", true, 300, false);
  this.initMenuBar();
  this.initBuildVersion();
  //////////////////////////////////////
  //////////////////////////////////////
  //Initialize global variables
  //////////////////////////////////////
  //////////////////////////////////////
  this.theTopics = {};
  this.theCourses = {}; 
  this.scriptsInjected = {};
  this.logoutRequestFromUrl = null;
  this.locationRequestFromUrl = null;
  this.storage.loadSettings(); 
  this.hashHistory = []; 
  this.previousProblemId = null;
  this.problems = {};
  this.theChapterIds = {};
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
  this.setSwitches();
  document.getElementById("divPage").style.display = "";
  document.getElementById("divPage").className = "divPage";
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

Page.prototype.resetProblems = function() {
  this.problems = {};
  this.theChapterIds = {};
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
  initializeButtons.initializeButtons();
  initializeButtons.initializeCalculatorPage();
}

function sectionSelect(sectionNumber) {
  console.log(`DEBUG: section select: ${sectionNumber}`);
  thePage.storage.variables.currentSectionComputed.setAndStore(sectionNumber);
  thePage.user.sectionComputed = thePage.user.sectionsTaught[sectionNumber];
  var deadlineSpans = document.getElementsByClassName(ids.domElements.classSpanDeadlineContainer);
  for (var counterDeadlines = 0; counterDeadlines < deadlineSpans.length; counterDeadlines ++) {
    var currentDeadlineSpan = deadlineSpans[counterDeadlines];
    var currentDeadlineId = currentDeadlineSpan.id.substr(ids.stringResources.prefixDeadlineContainer.length);
    var currentProblem = thePage.problems[currentDeadlineId];
    if (currentProblem === undefined) {
      continue;
    }
    currentDeadlineSpan.innerHTML = currentProblem.toStringDeadlinePanel();
    //console.log(`DEBUG: current id: ${currentDeadlineId}`);
    //console.log(`DEBUG: counter deadlines ${currentProblem.idURLed}`);
  }
}

Page.prototype.flipStudentView = function () {
  var oldValue = this.storage.variables.flagStudentView.getValue();
  this.storage.variables.flagStudentView.setAndStore(document.getElementById(ids.domElements.sliderStudentView).checked);    
  var spanView = document.getElementById(ids.domElements.spanStudentViewFlagToggleReport);
  var radioHTML = "";
  if (this.storage.variables.flagStudentView.getValue()) {
    spanView.innerHTML = "Student view";
    for (var counterSections = 0; counterSections < this.user.sectionsTaught.length; counterSections ++) {
      radioHTML += `<br><label class = "containerRadioButton">`;
      radioHTML += `<input type = "radio" name = "radioSection" onchange = "sectionSelect(${counterSections});" `; 
      var counterFromStorage = parseInt(thePage.storage.variables.currentSectionComputed.getValue());
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
  document.getElementById(ids.domElements.spanStudentViewSectionSelectPanel).innerHTML = radioHTML;

  if (oldValue !== this.storage.variables.flagStudentView.getValue()) {
    this.pages.problemPage.flagLoaded = false;
    login.toggleAdminPanels();
    this.selectPage(this.storage.variables.currentPage.getValue());
  }
}

Page.prototype.flipDebugSwitch = function () {
  this.storage.variables.flagDebug.setAndStore(document.getElementById(ids.domElements.sliderDebugFlag).checked);
  var debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
  if (this.storage.variables.flagDebug.getValue()) {
    debugSpan.innerHTML = "Debug on";
  } else {
    debugSpan.innerHTML = "Debug off";
  }
}

Page.prototype.setSwitches = function () {

  if (this.storage.variables.flagDebug.isTrue()) {
    document.getElementById(ids.domElements.sliderDebugFlag).checked = true;
  } else {
    document.getElementById(ids.domElements.sliderDebugFlag).checked = false;
  }
  if (this.storage.variables.flagStudentView.getValue() === true || this.storage.variables.flagStudentView.getValue() === "true") {
    document.getElementById(ids.domElements.sliderStudentView).checked = true;
  } else {
    document.getElementById(ids.domElements.sliderStudentView).checked = false;
  }
  this.flipDebugSwitch();
  this.flipStudentView();
}

Page.prototype.studentView = function () {
  if (this.storage.variables.flagStudentView.getValue() === true) {
    return true;
  }
  return false;
}

Page.prototype.defaultOnLoadInjectScriptsAndProcessLaTeX = function(idElement) { 
  var spanVerification = document.getElementById(idElement);
  var incomingScripts = spanVerification.getElementsByTagName('script');
  var oldScripts = this.pages.calculator.scriptIds;
  this.removeScripts(oldScripts);
  var calculator = calculatorPage.calculator;
  calculator.inputBoxNames = [];
  calculator.inputBoxToSliderUpdaters = {};
  calculator.canvases = {};
  this.pages.calculator.sciptIds = [];
  for (var i = 0; i < incomingScripts.length; i ++) { 
    var newId = `calculatorMainPageId_${i}`;
    this.pages.calculator.sciptIds.push(newId);
    this.injectScript(newId, incomingScripts[i].innerHTML);
  }
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById(idElement)]);
  MathJax.Hub.Queue([calculator.addListenersToInputBoxes.bind(calculator)]);
//  alert(theString);
}

function Script() {
  this.id = "";
  this.content = "";

} 

Page.prototype.removeOneScript = function(scriptId) {
  var theScript = document.getElementById(scriptId);
  if (theScript === null) {
    return;
  }
  var parent = theScript.parentNode;
  parent.removeChild(theScript);
}

Page.prototype.removeScripts = function(scriptIds) {
  for (var counter = 0; counter < scriptIds.length; counter ++) {
    this.removeOneScript(scriptIds[counter]);
  }
}

Page.prototype.injectScript = function(scriptId, scriptContent) {
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

Page.prototype.selectPage = function(inputPage) {
  if (this.pages[inputPage] === undefined) {
    inputPage = "calculator";
  }
  this.storage.variables.currentPage.setAndStore(inputPage);
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
  if (this.pages[inputPage].selectFunction !== null && this.pages[inputPage].selectFunction !== undefined) {
    this.pages[inputPage].selectFunction();
  }
  //location.href = `#${this.getHash()}`;
}

Page.prototype.getCurrentProblem = function() {
  var label = this.storage.variables.currentCourse.currentProblemId.getValue();
  return this.problems[label];
}

Page.prototype.cleanUpLoginSpan = function(componentToCleanUp) {
  var loginInfo = document.getElementById("spanLoginRequired");
  if (loginInfo !== null) {
    if (loginInfo.parentElement === componentToCleanUp) {
      loginInfo.innerHTML = "<b>...</b>";
    }
  }
}

function loadProfilePic() {

}

/**
 * @returns {Page} 
 * */
function mainPage () {
  return window.calculator.mainPage;
}

module.exports = {
  Page,
  mainPage,
}
