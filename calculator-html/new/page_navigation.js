function Page(){
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
    currentProblem : {
      id: "divCurrentProblem",
      menuButtonId: "buttonCurrentProblem",
      container: null,
      selectFunction: updateProblemPage
    },
    calculator: {
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: null
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
    currentButton.addEventListener("click", function(){
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
  this.flagCurrentProblemLoaded = false;
  this.logoutRequestFromUrl = null;
  this.locationRequestFromUrl = null;
  this.googleProfile = {};
  this.googleToken = null;
  this.username = null;
  this.authenticationToken = null;
  this.flagUserLoggedIn = false;
  this.currentProblem = {};
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
  this.selectPage = function(inputPage) {
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
  };
  this.showProfilePicture = function() {
    document.getElementById("divProfilePicture").classList.remove("divInvisible");
    document.getElementById("divProfilePicture").classList.add("divVisible");
    if (this.googleProfile.picture === undefined){
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
  this.hideProfilePicture = function() {
    document.getElementById("divProfilePicture").classList.add("divInvisible");
    document.getElementById("divProfilePicture").classList.remove("divVisible");
  }
  this.initializeCalculatorPage = function() {
    initializeButtons();
    initializeCalculatorPage();
    console.log("Submit missing");
    if (document.getElementById('mainInputID').value !== "") {
      submitStringAsMainInput(
        document.getElementById('calculatorOutput'),
        'compute',
        onLoadDefaultFunction,
        'mainComputationStatus'
      );
    }
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

function loadProfilePic(){

}
