"use strict";

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
  var theURL = `${pathnames.calculatorAPI}?request=changePassword&password=${encodeURIComponent(inputOldPassword.value)}&`;
  theURL += `newPassword=${encodeURIComponent(inputNewPassword.value)}&reenteredPassword=${encodeURIComponent(inputReenteredPassword.value)}&`;
  theURL += `email=${encodeURIComponent(inputEmail.value)}&`;
  theURL += "doReload=false&"
  submitGET({
    url: theURL,
    callback: submitChangePassRequestCallback,
    progress: "spanProgressReportGeneral"
  });
}

function updateAccountPage() {
  var activationToken = thePage.storage.activationToken.getValue();
  var username = thePage.storage.user.name.getValue();
  var oldPassword = document.getElementById("inputOldPasswordInAccount");
  var usernameInput = document.getElementById("spanUserIdInAccountsPage");
  var activationSpan = document.getElementById("spanOldPasswordActivationLink");
  var emailSpan = document.getElementById("spanOldEmail");

  if (activationToken !== "") {
    oldPassword.disabled = true;
    activationSpan.innerHTML = `<b style='color:green'>Using activation link</b>`;
    usernameInput.innerHTML = thePage.storage.user.name.getValue();
    emailSpan.innerHTML = thePage.storage.user.email.getValue();
    console.log(`DEbug: activation token, username: ${activationToken}, ${username}`);
  } else {
    activationSpan.innerHTML = "";
  }
}