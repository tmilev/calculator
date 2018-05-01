"use strict";

function updateAccountsPageCallback(input, outputComponent){
  if (typeof outputComponent == "string"){
    outputComponent = document.getElementById(outputComponent);
  }
  outputComponent.innerHTML = input;
}

function updateAccountsPage(){
  var theURL = `${pathnames.calculatorAPI}?request=accountsJSON`;

  submitGET({
    url: theURL,
    callback: updateAccountsPageCallback,
    progress: "spanProgressReportGeneral",
    result: "idAccountsOutput"
  });

}