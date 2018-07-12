"use strict";

function submitAccountActivationRequestCallback(result, outputComponent) {
  outputComponent = document.getElementById("spanVerification").innerHTML = result;
  document.getElementById("inputPassword").value = document.getElementById("inputNewPasswordInAccount").value;
  document.getElementById("inputOldPasswordInAccount").value = "";
  document.getElementById("inputNewPasswordInAccount").value = "";
  document.getElementById("inputReenteredPasswordInAccount").value = "";
  loginCalculator();
}

function submitActivateAccountRequest() {
  var inputOldPassword = document.getElementById("inputOldPasswordInAccount");
  var inputNewPassword = document.getElementById("inputNewPasswordInAccount");
  var inputReenteredPassword = document.getElementById("inputReenteredPasswordInAccount");
  var inputEmail = document.getElementById("inputEmail");
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?request=changePassword&`;
  theURL += `password=${encodeURIComponent(inputOldPassword.value)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword.value)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputReenteredPassword.value)}&`;
  theURL += `email=${encodeURIComponent(inputEmail.value)}&`;
  theURL += "doReload=false&"
  submitGET({
    url: theURL,
    callback: submitChangePassRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function updateAccountActivationPage() {
  var emailSpan = document.getElementById(idDOMElements.spanCurrentActivationEmail);
  var usernameInput = document.getElementById(idDOMElements.spanUserIdInActivateAccountPage);
  usernameInput.innerHTML = thePage.storage.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.user.email.getValue();
}
