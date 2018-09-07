"use strict";

function submitAccountActivationRequestCallback(result, outputComponent) {
  outputComponent = document.getElementById(idDOMElements.spanVerificationActivation).innerHTML = result;
  document.getElementById(idDOMElements.inputPassword).value = document.getElementById(
    idDOMElements.inputNewPasswordInActivationAccount
  ).value;
  document.getElementById(idDOMElements.inputNewPasswordInActivationAccount).value = "";
  document.getElementById(idDOMElements.inputReenteredPasswordInActivationAccount).value = "";
  //loginCalculator();
}

function submitActivateAccountRequest() {
  var inputNewPassword = document.getElementById(idDOMElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(idDOMElements.inputReenteredPasswordInActivationAccount).value;
  var userName = thePage.storage.user.name.getValue();
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?request=changePassword&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function submitDoActivateAccount() {
  var inputNewPassword = document.getElementById(idDOMElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(idDOMElements.inputReenteredPasswordInActivationAccount).value;
  var activationToken = thePage.storage.user.activationToken.getValue();
  thePage.storage.user.activationToken.setAndStore("");
  var userName = thePage.storage.user.name.getValue();
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?request=activateAccountJSON&`;
  theURL += `activationToken=${encodeURIComponent(activationToken)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function updateAccountActivationPage() {
  var emailSpan = document.getElementById(idDOMElements.spanCurrentActivationEmail);
  var usernameInput = document.getElementById(idDOMElements.spanUserIdInActivateAccountPage);
  usernameInput.innerHTML = thePage.storage.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.user.email.getValue();
  var activationToken = thePage.storage.user.activationToken.getValue();
  if (activationToken !== null && activationToken !== undefined && activationToken !== "") {
    submitDoActivateAccount();
  }
}
