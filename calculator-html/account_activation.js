"use strict";
const ids = require('./ids_dom_elements');
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');

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
  var thePage = window.calculator.mainPage;
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var userName = thePage.storage.variables.user.name.getValue();
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?request=changePassword&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function submitDoActivateAccount() {
  var thePage = window.calculator.mainPage;
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var activationToken = thePage.storage.variables.user.activationToken.getValue();
  thePage.storage.variables.user.activationToken.setAndStore("");
  var userName = thePage.storage.variables.user.name.getValue();
  var theURL = "";
  var email = thePage.storage.variables.user.email.getValue();
  theURL += `${pathnames.urls.calculatorAPI}?request=activateAccountJSON&`;
  theURL += `activationToken=${encodeURIComponent(activationToken)}&`;
  theURL += `email=${encodeURIComponent(email)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `reenteredPassword=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `username=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback,
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

function updateAccountActivationPage() {
  var thePage = window.calculator.mainPage;
  var emailSpan = document.getElementById(ids.domElements.spanCurrentActivationEmail);
  var usernameInput = document.getElementById(ids.domElements.spanUserIdInActivateAccountPage);
  usernameInput.innerHTML = thePage.storage.variables.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.variables.user.email.getValue();
  var activationToken = thePage.storage.variables.user.activationToken.getValue();
  if (activationToken !== null && activationToken !== undefined && activationToken !== "") {
    submitDoActivateAccount();
  }
}

module.exports = {
  updateAccountActivationPage,
  submitActivateAccountRequest,
}
