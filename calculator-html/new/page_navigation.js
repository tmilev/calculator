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
  this.currentPage  = null;
  if (Storage !== undefined && localStorage !== undefined) {
    this.currentPage = localStorage.currentPage; 
  }
  //////////////////////////////////////
  //////////////////////////////////////
  //Page manipulation functions
  //////////////////////////////////////
  //////////////////////////////////////
  this.selectPage = function(inputPage){
    if (inputPage === undefined){
      inputPage = this.currentPage;
    }
    this.currentPage = inputPage;
    for (var page in this.pages){
      this.pages[page].container.style.display = "none";
    }
    if (Storage !== undefined && localStorage !== undefined) {
      localStorage.setItem("currentPage", this.currentPage); 
    }
    if (this.currentPage === null || this.currentPage === undefined){
      return;
    }
    this.pages[inputPage].container.style.display = "";
    if (this.pages[inputPage].selectFunction !== null && this.pages[inputPage].selectFunction !== undefined){
      this.pages[inputPage].selectFunction();
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
