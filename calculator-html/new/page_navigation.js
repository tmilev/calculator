function Page() {
  thePage = this;
  this.pages = {
    login: {
      id: "divLoginPage", 
      menuButtonId: "buttonLoginPage",
      container: null,
      selectFunction: null,
      initialized: false
    },
    selectCourse : {
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectSelectCoursePage
    },
    currentCourse : {
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: selectCurrentCoursePage
    },
    problemPage : {
      id: "divProblemPage",
      menuButtonId: "buttonProblemPage",
      container: null,
      selectFunction: updateProblemPage,
      problems: {}
    },
    calculator: {
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: null,
      scriptIds: []
    },
    database: {
      id: "divDatabase",
      menuButtonId: "buttonSelectDatabase",
      container: null,
      selectFunction: updateDatabasePage,
      storage: {
        currentTable: null
      }
    },
    server: {
      id: "divServer",
      menuButtonId: "buttonSelectServer",
      container: null,
      selectFunction: updateServerStatus
    },
    account: {
      id: "divAccount",
      menuButtonId: "buttonSelectAccount",
      container: null,
      selectFunction: updateAccountPage
    },
    accounts: {
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
    var currentButton = document.getElementById(this.pages[page].menuButtonId);
    currentButton.pageToSelect = page;
    currentButton.addEventListener("click", function() {
      thePage.selectPage(this.pageToSelect);
    });
  }
  //////////////////////////////////////
  //////////////////////////////////////
  //Initialize global variables
  //////////////////////////////////////
  //////////////////////////////////////
  this.theTopics = {};
  this.theCourses = {}; 
  this.currentPage  = null;
  this.currentCourse = {
    courseHome: "",
    topicList: "",
    fileName: "",
    request: ""
  };
  this.scriptsInjected = {};
  this.flagCurrentProblemLoaded = false;
  this.logoutRequestFromUrl = null;
  this.locationRequestFromUrl = null;
  this.googleProfile = {};
  this.googleToken = null;
  this.username = null;
  this.authenticationToken = null;
  this.flagUserLoggedIn = false;
  this.storeSettingsToLocalStorage = function() {
    if (Storage === undefined && localStorage === undefined) {
      return;
    }
    localStorage.googleProfile = JSON.stringify(this.googleProfile);
    for (label in this.pages) {
      if (this.pages[label].storage === undefined) {
        continue;
      }
      localStorage[label] = JSON.stringify(this.pages[label].storage);
    }
  };
  this.loadSettingsFromLocalStorage = function() {    
    if (Storage === undefined && localStorage === undefined){
      return;
    }
    try {
      this.currentPage = localStorage.currentPage;
      this.googleProfile = JSON.parse(localStorage.googleProfile); 
      for (label in this.pages){
        if (localStorage[label] === undefined || localStorage[label] === null) {
          continue;
        }
        this.pages[label].storage = JSON.parse(localStorage[label]);
      }
    } catch (e) {
      console.log("Error loading settings from local storage: " + e);
    }
  };
  this.storeSettingsToCookies = function() {
    for (label in this.currentCourse){
      addCookie(label, this.currentCourse[label], 300);
    }
    addCookie("googleToken", this.googleToken, 300, true); 
    addCookie("username", this.username, 300, true);
    addCookie("authenticationToken", this.authenticationToken, 300, true);
  };
  this.loadSettingsFromCookies = function() {
    try {
      for (label in this.currentCourse){
        this.currentCourse[label] = getCookie(label);
      }
      this.googleToken = getCookie("googleToken");
      this.username = getCookie("username");
      this.authenticationToken = getCookie("authenticationToken");
    } catch (e) {
      console.log("Error loading settings from cookies: " + e);
    }
  };
  this.loadSettingsFromCookies();
  this.loadSettingsFromLocalStorage();
  //////////////////////////////////////
  //////////////////////////////////////
  //Page manipulation functions
  //////////////////////////////////////
  //////////////////////////////////////
  this.hideProfilePicture = function() {
    document.getElementById("divProfilePicture").classList.add("divInvisible");
    document.getElementById("divProfilePicture").classList.remove("divVisible");
  }
  //////////////////////////////////////
  //////////////////////////////////////
  //Select page on first load
  //////////////////////////////////////
  //////////////////////////////////////
  this.selectPage(this.currentPage);
  loginTry();
  document.getElementById("divPage").style.display = "";
  document.getElementById("divPage").className = "divPage";
}

Page.prototype.showProfilePicture = function() {
  document.getElementById("divProfilePicture").classList.remove("divInvisible");
  document.getElementById("divProfilePicture").classList.add("divVisible");
  if (this.googleProfile.picture === undefined) {
    return;
  }
  if (document.getElementById("divProfilePicture").children.length > 0) {
    return;
  }
  try {
    var theProfilePicElement = document.createElement("IMG");
    theProfilePicElement.setAttribute("src", this.googleProfile.picture);
    theProfilePicElement.setAttribute("alt", this.googleProfile.name);
    theProfilePicElement.setAttribute("id", "imgProfilePicture");
    theProfilePicElement.setAttribute("title", this.googleProfile.name);
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
  console.log("Submit missing");
  if (document.getElementById('mainInputID').value !== "") {
    submitStringAsMainInput(
      document.getElementById('calculatorOutput'),
      'compute',
      defaultOnLoadInjectScriptsAndProcessLaTeX,
      'mainComputationStatus'
    );
  }
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
  for (var page in this.pages){
    this.pages[page].container.style.display = "none";
    document.getElementById(this.pages[page].menuButtonId).classList.remove("buttonSelectPageSelected");
  }
  if (Storage !== undefined && localStorage !== undefined) {
    localStorage.setItem("currentPage", this.currentPage); 
  }
  this.pages[this.currentPage].container.style.display = "";
  document.getElementById(this.pages[this.currentPage].menuButtonId).classList.add("buttonSelectPageSelected");
  if (this.pages[this.currentPage].selectFunction !== null && this.pages[this.currentPage].selectFunction !== undefined) {
    this.pages[this.currentPage].selectFunction();
  }
  location.href = `#${this.currentPage}`;
};

Page.prototype.getCurrentProblem = function() {
  return this.getProblem(this.currentCourse.fileName);
}

Page.prototype.getProblem = function(fileName) {
  if (this.pages.problemPage.problems[fileName] === undefined) {
    this.pages.problemPage.problems[fileName] = new Problem(fileName);
  }
  return this.pages.problemPage.problems[fileName];
}

function loadProfilePic(){

}
