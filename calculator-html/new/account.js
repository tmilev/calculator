"use strict";

function submitChangePassRequestCallback(result, outputComponent){
  outputComponent = document.getElementById("spanVerification").innerHTML = result;
  document.getElementById("inputOldPasswordInAccount").value = "";
  document.getElementById("inputNewPasswordInAccount").value = "";
  document.getElementById("inputReenteredPasswordInAccount").value = "";
}


function submitChangePassRequest(){
  var inputOldPassword = document.getElementById("inputOldPasswordInAccount");
  var inputNewPassword = document.getElementById("inputNewPasswordInAccount");
  var inputReenteredPassword = document.getElementById("inputReenteredPasswordInAccount");
  var inputEmail = document.getElementById("inputEmail");


  var theURL = `${pathnames.calculator}?request=changePassword&password=${encodeURIComponent(inputOldPassword.value)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword.value)}&reenteredPassword=${encodeURIComponent(inputReenteredPassword.value)}&`;
  theURL += `email=${encodeURIComponent(inputEmail.value)}&`;
  theURL += "doReload=false&"
  submitGET({
    "url": theURL,
    callback: submitChangePassRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}


function updateAccountPage(){
}