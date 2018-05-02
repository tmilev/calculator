"use strict";

function updateAccountsPageCallback(input, outputComponent){
  if (typeof outputComponent == "string"){
    outputComponent = document.getElementById(outputComponent);
  }
  outputComponent.innerHTML = input;
}

function addEmailsOrUsersCallback(input, outputComponent){
  if (typeof outputComponent == "string"){
    outputComponent = document.getElementById(outputComponent);
  }
  outputComponent.innerHTML = decodeURIComponent(input);
}


function addEmailsOrUsers(idEmailList, problemCollectionName, idOutput, userRole, idUserGroup, idPasswords, requestType){
  var spanOutput = document.getElementById(idOutput);
  var spanEmailList = document.getElementById(idEmailList);
  var spanUserGroup = document.getElementById(idUserGroup);
  var spanPasswords = document.getElementById(idPasswords);
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?`;
  theURL += `request=${requestType}&`;
  theURL += `userRole=${userRole}&`;
  theURL += `userList=${encodeURIComponent(spanEmailList.value)}&`;
  theURL += `studentSection=${encodeURIComponent(spanUserGroup.value)}&`;
  theURL += `passwordList=${encodeURIComponent(spanPasswords.value)}&`;
  theURL += `filterAccounts=&`;
  submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    result: idOutput,
    callback: addEmailsOrUsersCallback
  });
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