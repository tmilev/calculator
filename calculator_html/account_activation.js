"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;

class AccountActivator {
  constructor() {
  }
  updateAccountActivationPage() {
    let emailSpan = document.getElementById(ids.domElements.spanCurrentActivationEmail);
    let usernameInput = document.getElementById(ids.domElements.spanUserIdInActivateAccountPage);
    usernameInput.textContent = storage.variables.user.name.getValue();
    emailSpan.textContent = storage.variables.user.email.getValue();
    let activationToken = storage.variables.user.activationToken.getValue();
    this.activationTokenSpan().textContent = activationToken;
  }

  activationTokenSpan() {
    return document.getElementById(ids.domElements.spanCurrentActivationToken);
  }

  submitDoActivateAccount() {
    let inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
    let inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
    let activationToken = storage.variables.user.activationToken.getValue();
    let userName = storage.variables.user.name.getValue();
    let url = "";
    let email = storage.variables.user.email.getValue();
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=activateAccountJSON&`;
    url += `${pathnames.urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
    url += `${pathnames.urlFields.email}=${encodeURIComponent(email)}&`;
    url += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword)}&`;
    url += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReentered)}&`;
    url += `${pathnames.urlFields.username}=${encodeURIComponent(userName)}&`;
    url += `doReload=false&`;
    submitRequests.submitGET({
      url: url,
      callback: (result, output) => {
        this.submitAccountActivationRequestCallback(false, result, output);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  submitActivateAccountRequest() {
    let inputNewPassword = document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value;
    let inputNewPasswordReentered = document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value;
    let userName = storage.variables.user.name.getValue();
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.changePassword}&`;
    url += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword)}&`;
    url += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReentered)}&`;
    url += `${pathnames.urlFields.username}=${encodeURIComponent(userName)}&`;
    let activationToken = this.activationTokenSpan().textContent;
    if (activationToken !== "" && activationToken !== null && activationToken !== undefined) {
      url += `${pathnames.urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
    }
    url += `doReload=false&`;
    submitRequests.submitGET({
      url: url,
      callback: (result, output) => {
        this.submitAccountActivationRequestCallback(true, result, output);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  submitAccountActivationRequestCallback(wipeOffActivationToken, result, outputComponent) {
    let spanActivation = document.getElementById(ids.domElements.spanVerificationActivation);
    try {
      let inputParsed = miscellaneous.jsonUnescapeParse(result);
      miscellaneous.writeHtmlElementsFromCommentsAndErrors(
        inputParsed,
        spanActivation
      );
      let resultSpan = document.createElement("div");
      miscellaneous.writeHTML(
        resultSpan,
        inputParsed[pathnames.urlFields.result.resultHtml],
      );
      spanActivation.append(resultSpan);
    } catch (e) {
      spanActivation.textContent = "Result: " + result + ". Error: " + e;
    }
    document.getElementById(ids.domElements.inputPassword).value = document.getElementById(
      ids.domElements.inputNewPasswordInActivationAccount
    ).value;
    document.getElementById(
      ids.domElements.pages.login.inputUsername
    ).value = document.getElementById(
      ids.domElements.spanUserIdInAccountsPage
    ).textContent;
    document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value = "";
    document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value = "";
    if (wipeOffActivationToken === true) {
      storage.variables.user.activationToken.setAndStore("");
    }
  }
}

function updateAccountActivationPage() {
  accountActivator.updateAccountActivationPage();
}

function submitActivateAccountRequest() {
  accountActivator.submitActivateAccountRequest();
}

let accountActivator = new AccountActivator();

module.exports = {
  updateAccountActivationPage,
  submitActivateAccountRequest,
  accountActivator,
};
