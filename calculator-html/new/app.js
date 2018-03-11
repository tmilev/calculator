function initializePage(){
  initializeButtons();
  initializeCalculatorPage();
  console.log("Submit missing");
  if (document.getElementById('mainInputID').value !== "")
  { submitStringAsMainInput(document.getElementById('calculatorOutput'), 'compute', onLoadDefaultFunction, 'mainComputationStatus');
  }
}
