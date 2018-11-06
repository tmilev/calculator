"use strict";
const selectCourse = require('./select_course');
const coursePage = require('./course_page'); 
const problemPage = require('./problem_page');
const edigPage = require('./edit_page');
const submitRequests = require('./submit_requests');
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
const pathnames = require('./pathnames');

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
  thePage.storage.user.authenticationToken.setAndStore(inputData.authenticationToken);
  thePage.storage.user.name.setAndStore(inputData.username);
  this.role = inputData.userRole;
  this.flagLoggedIn = true;
  this.sectionsTaught = inputData.sectionsTaught;
  this.instructor = inputData.instructor;
  this.sectionInDB = inputData.studentSection;
  this.sectionComputed = inputData.studentSection;
  this.deadlineSchema = inputData.deadlineSchema;
  document.getElementById(ids.domElements.spanUserIdInAccountsPage).innerHTML = thePage.storage.user.name.value;
  document.getElementById(ids.domElements.inputUsername).value = thePage.storage.user.name.value;

}

function StorageVariable(inputs) {
  this.value = "";
  this.name = inputs.name;
  this.nameURL = "";
  this.nameCookie = "";
  this.nameLocalStorage = "";
  this.associatedDOMId = "";
  this.type = "string";
  this.secure = true;
  var labelsToRead = ["nameURL", "nameCookie", "nameLocalStorage", "associatedDOMId", "type", "secure"];
  for (var counterLabel = 0; counterLabel < labelsToRead.length; counterLabel ++) {
    var currentLabel = labelsToRead[counterLabel];
    var incoming = inputs[currentLabel];
    if (incoming !== "" && incoming !== null && incoming !== undefined) {
      this[currentLabel] = incoming;
    }
  }
}

StorageVariable.prototype.getValue = function() {
  return this.value;
}

StorageVariable.prototype.loadMe = function(hashParsed) {
  var incomingValue = "";
  if (Storage !== undefined || localStorage !== undefined && this.nameLocalStorage !== "") {
    incomingValue = localStorage.getItem(this.nameLocalStorage);
    if (incomingValue !== "" && incomingValue !== null && incomingValue !== undefined) {
      this.value = localStorage[this.nameLocalStorage];
    }
  }
  if (this.nameCookie !== "") {
    incomingValue = cookies.getCookie(this.nameCookie);
    if (incomingValue !== "" && incomingValue !== null && incomingValue !== undefined) {
      this.value = incomingValue;
    }
  }
  if (this.nameURL !== "") {
    if (this.nameURL in hashParsed) {
      this.value = decodeURIComponent(hashParsed[this.nameURL]);
      if (this.associatedDOMId !== "") {
        var theComponent = document.getElementById(this.associatedDOMId);
        theComponent.value = this.value;
      }
    }
  }
}

StorageVariable.prototype.storeMe = function() {
  if (Storage !== undefined || localStorage !== undefined) {
    if (this.nameLocalStorage !== "") {
      localStorage[this.nameLocalStorage] = this.value
    }
  }
  if (this.nameCookie !== "") {
    cookies.setCookie(this.nameCookie, this.value, 150, this.secure);
  }
}

StorageVariable.prototype.setAndStore = function(newValue) {
  this.value = newValue;
  this.storeMe();
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
      flagModifyURL: false
    },
    selectCourse : {
      name: "selectCourse", //<-for autocomplete
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectCourse.selectCoursePage,
      flagModifyURL: true
    },
    currentCourse : {
      name: "currentCourse", //<-for autocomplete
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: coursePage.selectCurrentCoursePage,
      flagModifyURL: true
    },
    problemPage : {
      name: "problemPage", //<-for autocomplete
      id: "divProblemPage",
      menuButtonId: "buttonProblemPage",
      container: null,
      selectFunction: problemPage.updateProblemPage,
      flagLoaded: false,
      flagModifyURL: false
    },
    editPage : {
      name: "editPage", //<-for autocomplete
      id: "divEditPage",
      menuButtonId: "buttonEditPage",
      container: null,
      selectFunction: edigPage.selectEditPage,
      flagLoaded: false,
      editor: null,
      flagModifyURL: false
    },
    calculator: {
      name: "calculator", //<-for autocomplete
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: submitRequests.submitCalculatorComputation,
      scriptIds: [],
      flagModifyURL: false
    },
    signUp: {
      name: "signUp", //<-for autocomplete
      id: "divSignUpPage",
      container: null,
      selectFunction: null,
      flagModifyURL: false
    },
    forgotLogin: {
      name: "forgotLogin",
      id: "divForgotLogin",
      container: null,
      selectFunction: null,
      flagModifyURL: false   
    },
    about: {
      name: "about", //<-for autocomplete
      id: "divAboutPage",
      menuButtonId: "buttonAboutPage",
      container: null,
      selectFunction: null,
      flagModifyURL: true
    },
    database: {
      name: "database", //<-for autocomplete
      id: "divDatabase",
      menuButtonId: "buttonSelectDatabase",
      container: null,
      selectFunction: database.updateDatabasePage,
      flagModifyURL: false
    },
    server: {
      name: "server", //<-for autocomplete
      id: "divServer",
      menuButtonId: "buttonSelectServer",
      container: null,
      selectFunction: serverStatus.updateServerStatus,
      flagModifyURL: true
    },
    account: {
      name: "account", //<-for autocomplete
      id: "divAccount",
      menuButtonId: "buttonSelectAccount",
      container: null,
      selectFunction: accountPage.updateAccountPage,
      flagModifyURL: false
    },
    activateAccount: {
      name: "activateAccount", //<-for autocomplete
      id: "divActivateAccount",
      container: null,
      selectFunction: activateAccount.updateAccountActivationPage,
      flagModifyURL: true
    },
    accounts: {
      name: "accounts", //<-for autocomplete
      id: "divAccounts",
      menuButtonId: "buttonSelectAccounts",
      container: null,
      selectFunction: accountManagement.updateAccountsPage,
      flagModifyURL: true
    }
  };
  this.storage = {
    currentPage: new StorageVariable({
      name: "currentPage", 
      nameLocalStorage: "currentPage", //<- when given and non-empty, local storage will be used to store variable
      nameCookie: "", //<- when given and non-empty, cookies will be used to store variable
      nameURL: "currentPage" //<- when given and non-empty, url will be used to store variable      
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
        associatedDOMId: "mainInputID"
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
        nameURL: "activationToken"
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
  }
  
  cookies.setCookie("useJSON", true, 300, false);
  this.initMenuBar();
  this.initBuildVersion();
  //////////////////////////////////////
  //////////////////////////////////////
  //Initialize global variables
  //////////////////////////////////////
  //////////////////////////////////////
  this.calculatorInputLastSubmitted = "";
  this.theTopics = {};
  this.theCourses = {}; 
  this.scriptsInjected = {};
  this.logoutRequestFromUrl = null;
  this.locationRequestFromUrl = null;
  this.loadSettings(location.hash); 
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
  this.selectPage(this.storage.currentPage.getValue());
  if (this.storage.currentPage.getValue() != this.pages.activateAccount.name) {
    login.loginTry();
  }
  this.setSwitches();
  document.getElementById("divPage").style.display = "";
  document.getElementById("divPage").className = "divPage";
}

Page.prototype.initBuildVersion = function() {
  document.getElementById(ids.domElements.calculatorBuildVersion).innerHTML = `Build version ${serverInformation.version}`;
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

Page.prototype.loadSettings = function(inputHash) {
  var inputHashParsed = {};
  if (typeof inputHash === "string") {
    var inputString = inputHash;
    if (inputHash[0] === '#') {
      inputString = inputHash.slice(1);
    }
    if (inputString === "" || inputString === undefined || inputString === null) {
      inputString = "{}";
    }
    var decodedInputString = decodeURIComponent(inputString);
    inputHashParsed = JSON.parse(decodedInputString);
  }
  this.loadSettingsRecursively(this.storage, inputHashParsed);
}

Page.prototype.loadSettingsRecursively = function(currentStorage, inputHashParsed) {
  if (currentStorage instanceof StorageVariable) {
    currentStorage.loadMe(inputHashParsed);
  } else if (typeof currentStorage === "object") {
    for (var subLabel in currentStorage) {
      this.loadSettingsRecursively(currentStorage[subLabel], inputHashParsed);
    }
  }  
}

Page.prototype.storeSettings = function() {
  try {
    this.storeSettingsRecursively(this.storage);
  } catch (e) {
    console.log("Error loading settings from cookies: " + e);
  }
}

Page.prototype.storeSettingsRecursively = function(currentStorage) {
  if (currentStorage instanceof StorageVariable) {
    currentStorage.storeMe();
  }
  if (typeof currentStorage === "object") {
    for (var subLabel in currentStorage) {
      this.storeSettingsRecursively(currentStorage[subLabel]);
    }
  }  
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
  thePage.storage.currentSectionComputed.setAndStore(sectionNumber);
  thePage.user.sectionComputed = thePage.user.sectionsTaught[sectionNumber];
  var deadlineSpans = document.getElementsByClassName(ids.domElements.classSpanDeadlineContainer);
  for (var counterDeadlines = 0; counterDeadlines < deadlineSpans.length; counterDeadlines ++) {
    var currentDeadlineSpan = deadlineSpans[counterDeadlines];
    var currentDeadlineId = currentDeadlineSpan.id.substr(stringResources.prefixDeadlineContainer.length);
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
  var oldValue = this.storage.flagStudentView.getValue();
  this.storage.flagStudentView.setAndStore(document.getElementById(ids.domElements.sliderStudentView).checked);    
  var spanView = document.getElementById(ids.domElements.spanStudentViewFlagToggleReport);
  var radioHTML = "";
  if (this.storage.flagStudentView.getValue()) {
    spanView.innerHTML = "Student view";
    for (var counterSections = 0; counterSections < this.user.sectionsTaught.length; counterSections ++) {
      radioHTML += `<br><label class = "containerRadioButton">`;
      radioHTML += `<input type = "radio" name = "radioSection" onchange = "sectionSelect(${counterSections});" `; 
      var counterFromStorage = parseInt(thePage.storage.currentSectionComputed.getValue());
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

  if (oldValue !== this.storage.flagStudentView.getValue()) {
    this.pages.problemPage.flagLoaded = false;
    login.toggleAdminPanels();
    this.selectPage(this.storage.currentPage.getValue());
  }
}

Page.prototype.flipDebugSwitch = function () {
  this.storage.flagDebug.setAndStore(document.getElementById(ids.domElements.sliderDebugFlag).checked);
  var debugSpan = document.getElementById(ids.domElements.spanDebugFlagToggleReport);
  if (this.storage.flagDebug.getValue()) {
    debugSpan.innerHTML = "Debug on";
  } else {
    debugSpan.innerHTML = "Debug off";
  }
}

Page.prototype.setSwitches = function () {
  //console.log ("DEBUG flag: " + this.flagDebug);
  if (this.storage.flagDebug.getValue() === true || this.storage.flagDebug.getValue() === "true") {
    document.getElementById(ids.domElements.sliderDebugFlag).checked = true;
  } else {
    document.getElementById(ids.domElements.sliderDebugFlag).checked = false;
  }
  if (this.storage.flagStudentView.getValue() === true || this.storage.flagStudentView.getValue() === "true") {
    document.getElementById(ids.domElements.sliderStudentView).checked = true;
  } else {
    document.getElementById(ids.domElements.sliderStudentView).checked = false;
  }
  this.flipDebugSwitch();
  this.flipStudentView();
}

Page.prototype.studentView = function () {
  if (this.storage.flagStudentView.getValue() === true) {
    return true;
  }
  return false;
}

function defaultOnLoadInjectScriptsAndProcessLaTeX(idElement) { 
  var spanVerification = document.getElementById(idElement);
  var incomingScripts = spanVerification.getElementsByTagName('script');
  var oldScripts = thePage.pages.calculator.scriptIds;
  thePage.removeScripts(oldScripts);
  calculatorInputBoxNames = [];
  calculatorInputBoxToSliderUpdaters = {};
  calculatorCanvases = {};
  thePage.pages.calculator.sciptIds = [];
  for (var i = 0; i < incomingScripts.length; i ++) { 
    var newId = `calculatorMainPageId_${i}`;
    thePage.pages.calculator.sciptIds.push(newId);
    thePage.injectScript(newId, incomingScripts[i].innerHTML);
  }
  MathJax.Hub.Queue(['Typeset', MathJax.Hub, document.getElementById(idElement)]);
  MathJax.Hub.Queue([calculatorAddListenersToInputBoxes]);
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
  this.storage.currentPage.setAndStore(inputPage);
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
  if (this.pages[inputPage].flagModifyURL === true) {
    var urlObject = { 
      currentPage: this.storage.currentPage.getValue()
    };
    location.href = `${pathnames.urls.app}#${encodeURIComponent(JSON.stringify(urlObject))}`;
  }
  if (this.pages[inputPage].selectFunction !== null && this.pages[inputPage].selectFunction !== undefined) {
    this.pages[inputPage].selectFunction();
  }
  //location.href = `#${this.getHash()}`;
}

Page.prototype.getCurrentProblem = function() {
  var label = this.storage.currentCourse.currentProblemId.getValue();
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