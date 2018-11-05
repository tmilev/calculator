"use strict";
const ids = require('./ids_dom_elements');
const submitRequests = require('./submit_requests');

function submitAccountActivationRequestCallback(result, outputComponent) {
  outputComponent = document.getElementById(ids.domElements.spanVerificationActivation).innerHTML = result;
  document.getElementById(ids.domElements.inputPassword).value = document.getElementById(
    ids.domElements.inputNewPasswordInActivationAccount
  ).value;
  document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value = "";
  document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value = "";
  //loginCalculator();
}

function submitActivateAccountRequest() {
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var userName = thePage.storage.user.name.getValue();
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?request=changePassword&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function submitDoActivateAccount() {
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var activationToken = thePage.storage.user.activationToken.getValue();
  thePage.storage.user.activationToken.setAndStore("");
  var userName = thePage.storage.user.name.getValue();
  var theURL = "";
  var email = thePage.storage.user.email.getValue();
  theURL += `${pathnames.calculatorAPI}?request=activateAccountJSON&`;
  theURL += `activationToken=${encodeURIComponent(activationToken)}&`;
  theURL += `email=${encodeURIComponent(email)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function updateAccountActivationPage() {
  var emailSpan = document.getElementById(ids.domElements.spanCurrentActivationEmail);
  var usernameInput = document.getElementById(ids.domElements.spanUserIdInActivateAccountPage);
  usernameInput.innerHTML = thePage.storage.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.user.email.getValue();
  var activationToken = thePage.storage.user.activationToken.getValue();
  if (activationToken !== null && activationToken !== undefined && activationToken !== "") {
    submitDoActivateAccount();
  }
}

module.exports = {
  updateAccountActivationPage,
}
