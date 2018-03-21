function Page(){
  this.pages = {
    currentCourse : {
      id: "divCurrentCourse",
      menuButtonId: "buttonCurrentCourse",
      container: null,
      selectFunction: selectCurrentCoursePage,
    },
    selectCourse : {
      id: "divSelectCourse",
      menuButtonId: "buttonSelectCourse",
      container: null,
      selectFunction: selectSelectCoursePage,
    },
    calculator: {
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null,
      selectFunction: null,
    }
  }
  //////////////////////////////////////
  //////////////////////////////////////
  //Common page initializations
  //////////////////////////////////////
  //////////////////////////////////////
  for (var page in this.pages){
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
  if (Storage !== undefined && localStorage !== undefined) {
    this.currentPage = localStorage.currentPage; 
  }
  this.currentCourse = null;
  this.loadCurrentCourse = function(){
    try { 
      var currentCourseString = getCookie("currentCourse");
      this.currentCourse = JSON.parse(currentCourseString);
    } catch (e){
      console.log("Error reading current course. " + e);
      this.currentCourse = null;
    }  
  }
  this.storeCurrentCourse = function(){
    addCookie("currentCourse", JSON.stringify(this.currentCourse), 300);
  }
  this.loadCurrentCourse();
  //////////////////////////////////////
  //////////////////////////////////////
  //Page manipulation functions
  //////////////////////////////////////
  //////////////////////////////////////
  this.selectPage = function(inputPage){
    if (inputPage === undefined || inputPage === null || typeof(inputPage) !== "string"){
      if (typeof(this.currentPage) === "string"){
        inputPage = this.currentPage;
      } else {
        inputPage = "calculator";
      }
    }
    this.currentPage = inputPage;
    if (this.pages[this.currentPage] === undefined){
      this.currentPage = "calculator";
    }
    for (var page in this.pages){
      this.pages[page].container.style.display = "none";
    }
    if (Storage !== undefined && localStorage !== undefined) {
      localStorage.setItem("currentPage", this.currentPage); 
    }
    this.pages[this.currentPage].container.style.display = "";
    if (this.pages[this.currentPage].selectFunction !== null && this.pages[this.currentPage].selectFunction !== undefined){
      this.pages[this.currentPage].selectFunction();
    }
  };
  this.initializeCalculatorPage = function(){
    initializeButtons();
    initializeCalculatorPage();
    console.log("Submit missing");
    if (document.getElementById('mainInputID').value !== ""){
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
}

function loadProfilePic(){

}
