function Page(){
  this.pages = {
    currentCourse : {
      id: "divCurrentCourse",
      menuButtonId: "buttonSelectCurrentCourse",
      container: null
    },
    calculator: {
      id: "divCalculatorPage",
      menuButtonId: "buttonSelectCalculator",
      container: null
    }
  }
  for (var page in this.pages){
    this.pages[page].container = document.getElementById(this.pages[page].id);
    var currentButton = document.getElementById(this.pages[page].menuButtonId);
    currentButton.pageToSelect = page;
    currentButton.addEventListener("click", function(){
      thePage.selectPage(this.pageToSelect);
    });
  }

  this.selectPage = function(inputPage){
    for (var page in this.pages){
      this.pages[page].container.style.display = "none";
    }
    this.pages[inputPage].container.style.display = "";
  }
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
}
