"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");

function submitAccountActivationRequestCallback(wipeOffActivationToken, result, outputComponent) {
  var thePage = window.calculator.mainPage;
  outputComponent = document.getElementById(ids.domElements.spanVerificationActivation).innerHTML = result;
  document.getElementById(ids.domElements.inputPassword).value = document.getElementById(
    ids.domElements.inputNewPasswordInActivationAccount
  ).value;
  document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value = "";
  document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value = "";
  if (wipeOffActivationToken === true) {
    thePage.storage.variables.user.activationToken.setAndStore("");
  }
  //loginCalculator();
}

function submitActivateAccountRequest() {
  var thePage = window.calculator.mainPage;
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var userName = thePage.storage.variables.user.name.getValue();
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.changePassword}&`;
  theURL += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `${pathnames.urlFields.username}=${encodeURIComponent(userName)}&`;
  var activationToken = thePage.storage.variables.user.activationToken.getValue();
  if (activationToken !== "" && activationToken !== null && activationToken !== undefined) {
    theURL += `${pathnames.urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
  }
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback.bind(null, true),
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function submitDoActivateAccount() {
  var thePage = window.calculator.mainPage;
  var inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
  var inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
  var activationToken = thePage.storage.variables.user.activationToken.getValue();
  var userName = thePage.storage.variables.user.name.getValue();
  var theURL = "";
  var email = thePage.storage.variables.user.email.getValue();
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=activateAccountJSON&`;
  theURL += `${pathnames.urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
  theURL += `${pathnames.urlFields.email}=${encodeURIComponent(email)}&`;
  theURL += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword)}&`;
  theURL += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReentered)}&`;
  theURL += `${pathnames.urlFields.username}=${encodeURIComponent(userName)}&`;
  theURL += `doReload=false&`;
  submitRequests.submitGET({
    url: theURL,
    callback: submitAccountActivationRequestCallback.bind(null, false),
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

function updateAccountActivationPage() {
  var thePage = window.calculator.mainPage;
  var emailSpan = document.getElementById(ids.domElements.spanCurrentActivationEmail);
  var activationTokenSpan = document.getElementById(ids.domElements.spanCurrentActivationToken);
  var usernameInput = document.getElementById(ids.domElements.spanUserIdInActivateAccountPage);
  usernameInput.innerHTML = thePage.storage.variables.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.variables.user.email.getValue();
  var activationToken = thePage.storage.variables.user.activationToken.getValue();
  activationTokenSpan.innerHTML = activationToken;
  if (activationToken !== null && activationToken !== undefined && activationToken !== "") {
    submitDoActivateAccount();
  }
}

module.exports = {
  updateAccountActivationPage,
  submitActivateAccountRequest,
};
