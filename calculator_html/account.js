"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const login = require('./login');
const miscellaneous = require("./miscellaneous_frontend");

class AccountPage {
  initialize() {
    document.getElementById(
      ids.domElements.pages.account.buttonChangePasswordFromAccountPage
    ).addEventListener('click', () => {
      submitChangePassRequest();
    });
    document.getElementById(
      ids.domElements.pages.account.buttonChangeEmail
    ).addEventListener('click', () => {
      submitChangePassRequest();
    });
  }
}

function submitChangePassRequestCallback(result, outputComponent) {
  outputComponent = document.getElementById("spanVerification").innerHTML = miscellaneous.jsonParseGetHtmlStandard(result);
  document.getElementById("inputPassword").value = document.getElementById("inputNewPasswordInAccount").value;
  document.getElementById("inputOldPasswordInAccount").value = "";
  document.getElementById("inputNewPasswordInAccount").value = "";
  document.getElementById("inputReenteredPasswordInAccount").value = "";
  login.authenticator.loginCalculator();
}

function submitChangePassRequest() {
  let inputOldPassword = document.getElementById("inputOldPasswordInAccount");
  let inputNewPassword = document.getElementById("inputNewPasswordInAccount");
  let inputReenteredPassword = document.getElementById("inputReenteredPasswordInAccount");
  let inputEmail = document.getElementById("inputEmail");
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.changePassword}&`;
  theURL += `${pathnames.urlFields.password}=${encodeURIComponent(inputOldPassword.value)}&`;
  theURL += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword.value)}&`;
  theURL += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputReenteredPassword.value)}&`;
  theURL += `${pathnames.urlFields.email}=${encodeURIComponent(inputEmail.value)}&`;
  theURL += "doReload=false&"
  submitRequests.submitGET({
    url: theURL,
    callback: submitChangePassRequestCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function updateAccountPage() {
  let thePage = window.calculator.mainPage;
  let usernameInput = document.getElementById("spanUserIdInAccountsPage");
  let emailSpan = document.getElementById(ids.domElements.spanOldEmail);
  usernameInput.innerHTML = thePage.storage.variables.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.variables.user.email.getValue();
  let spanExtraInfo = document.getElementById(ids.domElements.spanUserExtraInfo);
  let extraInfo = "";
  extraInfo += "<table>";
  extraInfo += `<tr><td>Role: </td><td>${thePage.user.getRole()}</td><tr>`;
  if (thePage.user.sectionsTaught.length > 0 && !thePage.studentView()) {
    extraInfo += `<tr><td>Sections taught: </td><td>${thePage.user.sectionsTaught.join(", ")}</td><tr>`;
  }
  extraInfo += `<tr><td>Instructor: </td><td>${thePage.user.instructor}</td><tr>`;
  extraInfo += `<tr><td>Section in database: </td><td>${thePage.user.sectionInDB}</td><tr>`;
  extraInfo += `<tr><td>Section computed: </td><td>${thePage.user.sectionComputed}</td></tr>`;
  extraInfo += `<tr><td>Deadline schema: </td><td>${thePage.user.deadlineSchema}</td><tr>`;
  extraInfo += "</table>";
  spanExtraInfo.innerHTML = extraInfo;
}

let accountPage = new AccountPage();
accountPage.initialize();

module.exports = {
  accountPage,
  updateAccountPage,
  submitChangePassRequest
};