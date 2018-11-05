"use strict";
const ids = require('./ids_dom_elements');
const submitRequests = require('./submit_requests');

function submitChangePassRequestCallback(result, outputComponent) {
  outputComponent = document.getElementById("spanVerification").innerHTML = result;
  document.getElementById("inputPassword").value = document.getElementById("inputNewPasswordInAccount").value;
  document.getElementById("inputOldPasswordInAccount").value = "";
  document.getElementById("inputNewPasswordInAccount").value = "";
  document.getElementById("inputReenteredPasswordInAccount").value = "";
  loginCalculator();
}

function submitChangePassRequest() {
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
  submitRequests.submitGET({
    url: theURL,
    callback: submitChangePassRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function updateAccountPage() {
  var usernameInput = document.getElementById("spanUserIdInAccountsPage");
  var emailSpan = document.getElementById(ids.domElements.spanOldEmail);
  usernameInput.innerHTML = thePage.storage.user.name.getValue();
  emailSpan.innerHTML = thePage.storage.user.email.getValue();
  var spanExtraInfo = document.getElementById(ids.domElements.spanUserExtraInfo);
  var extraInfo = "";
  extraInfo += "<table>";
  extraInfo += `<tr><td>Role: </td><td>${thePage.user.role}</td><tr>`;
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

module.exports = {
  updateAccountPage
}