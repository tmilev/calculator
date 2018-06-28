function User() {
  this.name = "";
  this.googleToken = null;
  this.flagLoggedIn = false;
  this.authenticationToken = "";
  this.googleProfile = null;
  this.role = "";
}

User.prototype.isLoggedIn = function() {
  return this.flagLoggedIn;
}

User.prototype.hasAdminRights = function() {
  return this.role === "admin" && this.flagLoggedIn === true;  
}

User.prototype.hasProblemEditRights = function() {
  return this.role === "admin" && this.isLoggedIn();  
}

User.prototype.hideProfilePicture = function() {
  document.getElementById("divProfilePicture").classList.add("divInvisible");
  document.getElementById("divProfilePicture").classList.remove("divVisible");
}

function Page() {
  thePage = this;
  this.pages = {
    login: {
      name: "login", //<-for autocomplete
      id: "divLoginPage", 
      menuButtonId: "buttonLoginPage",
      container: null,
      selectFunction: null,
      initialized: false
    },
    selectCourse : {
      name: "selectCourse", //<-for autocomplete
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectSelectCoursePage
    },
    currentCourse : {
      name: "currentCourse", //<-for autocomplete
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: selectCurrentCoursePage
    },
    problemPage : {
      name: "problemPage", //<-for autocomplete
      id: "divProblemPage",
      menuButtonId: "buttonProblemPage",
      container: null,
      selectFunction: updateProblemPage,
      problems: {},
      flagLoaded: false
    },
    editPage : {
      name: "editPage", //<-for autocomplete
      id: "divEditPage",
      menuButtonId: "buttonEditPage",
      container: null,
      selectFunction: selectEditPage,
      flagLoaded: false,
      editor: null,
      storage: {
        currentlyEditedPage: null,
        AceEditorAutoCompletionWordList: []
      }
    },
    calculator: {
      name: "calculator", //<-for autocomplete
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: null,
      scriptIds: []
    },
    signUp: {
      name: "signUp", //<-for autocomplete
      id: "divSignUpPage",
      container: null,
      selectFunction: null
    },
    about: {
      name: "about", //<-for autocomplete
      id: "divAboutPage",
      menuButtonId: "buttonAboutPage",
      container: null,
      selectFunction: null
    },
    database: {
      name: "database", //<-for autocomplete
      id: "divDatabase",
      menuButtonId: "buttonSelectDatabase",
      container: null,
      selectFunction: updateDatabasePage,
      storage: {
        currentTable: null
      }
    },
    server: {
      name: "server", //<-for autocomplete
      id: "divServer",
      menuButtonId: "buttonSelectServer",
      container: null,
      selectFunction: updateServerStatus
    },
    account: {
      name: "account", //<-for autocomplete
      id: "divAccount",
      menuButtonId: "buttonSelectAccount",
      container: null,
      selectFunction: updateAccountPage
    },
    accounts: {
      name: "accounts", //<-for autocomplete
      id: "divAccounts",
      menuButtonId: "buttonSelectAccounts",
      container: null,
      selectFunction: updateAccountsPage
    }
  }
  addCookie("useJSON", true, 300, false);
  //////////////////////////////////////
  //////////////////////////////////////
  //Common page initializations
  //////////////////////////////////////
  //////////////////////////////////////
  for (var page in this.pages) {
    this.pages[page].container = document.getElementById(this.pages[page].id);
    if (this.pages[page].menuButtonId !== null && this.pages[page].menuButtonId !== undefined) {
      var currentButton = document.getElementById(this.pages[page].menuButtonId);
      currentButton.pageToSelect = page;
      currentButton.addEventListener("click", function() {
        thePage.selectPage(this.pageToSelect);
      });
    }
  }
  //////////////////////////////////////
  //////////////////////////////////////
  //Initialize global variables
  //////////////////////////////////////
  //////////////////////////////////////
  this.theTopics = {};
  this.theCourses = {}; 
  this.currentPage  = null;
  this.flagDebug = false;
  this.currentCourse = {
    courseHome: "",
    topicList: "",
    fileName: ""
  };
  this.scriptsInjected = {};
  this.logoutRequestFromUrl = null;
  this.locationRequestFromUrl = null;
  this.user = new User();
  this.loadSettingsFromCookies();
  this.loadSettingsFromLocalStorage();
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
  this.selectPage(this.currentPage);
  loginTry();
  this.setDebugSwitch();
  document.getElementById("divPage").style.display = "";
  document.getElementById("divPage").className = "divPage";
}

Page.prototype.correctSettings = function () {
}

Page.prototype.loadSettingsFromCookies = function() {
  try {
    for (label in this.currentCourse) {
      this.currentCourse[label] = getCookie(label);
      //console.log(label);
    }
    //console.log("DEBUG: Current course: " + JSON.stringify(this.currentCourse));
    this.user.googleToken = getCookie("googleToken");
    this.user.name = getCookie("username");
    this.user.authenticationToken = getCookie("authenticationToken");
    this.flagDebug = getCookie("debugFlag");
  } catch (e) {
    console.log("Error loading settings from cookies: " + e);
  }
  this.correctSettings();
}

Page.prototype.storeSettingsToCookies = function() {
  for (label in this.currentCourse) {
    addCookie(label, this.currentCourse[label], 300);
  }
  //console.log("DEBUG: storing to cookies: " + JSON.stringify(this.currentCourse));
  addCookie("googleToken", this.user.googleToken, 300, true); 
  addCookie("username", this.user.name, 300, true);
  addCookie("authenticationToken", this.user.authenticationToken, 300, true);
  addCookie("debugFlag", this.flagDebug);
}

Page.prototype.loadSettingsFromLocalStorage = function() {    
  if (Storage === undefined && localStorage === undefined) {
    return;
  }
  try {
    this.currentPage = localStorage.currentPage;
    this.user.googleProfile = JSON.parse(localStorage.googleProfile); 
    for (label in this.pages){
      if (localStorage[label] === undefined || localStorage[label] === null) {
        continue;
      }
      this.pages[label].storage = JSON.parse(localStorage[label]);
    }
  } catch (e) {
    console.log("Error loading settings from local storage: " + e);
  }
}

Page.prototype.storeSettingsToLocalStorage = function() {
  if (Storage === undefined && localStorage === undefined) {
    return;
  }
  localStorage.googleProfile = JSON.stringify(this.user.googleProfile);
  for (label in this.pages) {
    if (this.pages[label].storage === undefined) {
      continue;
    }
    localStorage[label] = JSON.stringify(this.pages[label].storage);
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
  initializeButtons();
  initializeCalculatorPage();
  //console.log("Submit missing");
  if (document.getElementById('mainInputID').value !== "") {
    console.log("Debug: about to submit: " + document.getElementById('mainInputID').value);
    submitStringAsMainInput(
      document.getElementById('calculatorOutput'),
      'calculatorOutput', 
      'compute',
      defaultOnLoadInjectScriptsAndProcessLaTeX,
      'mainComputationStatus'
    );
  }
}

Page.prototype.flipDebugSwitch = function () {
  this.flagDebug = document.getElementById("sliderDebugFlag").checked;
  var debugSpan = document.getElementById("spanDebugFlagToggleReport");
  if (this.flagDebug) {
    debugSpan.innerHTML = "Debug on";
  } else {
    debugSpan.innerHTML = "Debug off";
  }
  this.storeSettingsToCookies();
}

Page.prototype.setDebugSwitch = function () {
  //console.log ("DEBUG flag: " + this.flagDebug);
  if (this.flagDebug === true || this.flagDebug === "true") {
    document.getElementById("sliderDebugFlag").checked = true;
  } else {
    document.getElementById("sliderDebugFlag").checked = false;
  }
  this.flipDebugSwitch();
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
  if (theScript === null){
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
  if (inputPage === undefined || inputPage === null || typeof(inputPage) !== "string") {
    if (typeof(this.currentPage) === "string") {
      inputPage = this.currentPage;
    } else {
      inputPage = "calculator";
    }
  }
  this.currentPage = inputPage;
  if (this.pages[this.currentPage] === undefined) {
    this.currentPage = "calculator";
  }
  for (var page in this.pages) {
    this.pages[page].container.style.display = "none";
    if (this.pages[page].menuButtonId !== null && this.pages[page].menuButtonId !== undefined) {
      document.getElementById(this.pages[page].menuButtonId).classList.remove("buttonSelectPageSelected");
    }
  }
  if (Storage !== undefined && localStorage !== undefined) {
    localStorage.setItem("currentPage", this.currentPage); 
  }
  this.pages[this.currentPage].container.style.display = "";
  if (this.pages[this.currentPage].menuButtonId !== null && this.pages[this.currentPage].menuButtonId !== undefined) {
    document.getElementById(this.pages[this.currentPage].menuButtonId).classList.add("buttonSelectPageSelected");
  }
  if (this.pages[this.currentPage].selectFunction !== null && this.pages[this.currentPage].selectFunction !== undefined) {
    this.pages[this.currentPage].selectFunction();
  }
  location.href = `#${this.currentPage}`;
}

Page.prototype.getCurrentProblem = function() {
  return this.getProblem(this.currentCourse.fileName);
}

Page.prototype.cleanUpLoginSpan = function(componentToCleanUp) {
  var loginInfo = document.getElementById("spanLoginRequired");
  if (loginInfo !== null) {
    if (loginInfo.parentElement === componentToCleanUp) {
      loginInfo.innerHTML = "<b>...</b>";
    }
  }
}

Page.prototype.getProblem = function(fileName) {
  if (this.pages.problemPage.problems[fileName] === undefined) {
    this.pages.problemPage.problems[fileName] = new Problem(fileName);
  }
  return this.pages.problemPage.problems[fileName];
}

function loadProfilePic(){

}