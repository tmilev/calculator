"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;

class AccountActivator {
  constructor() {
    this.panelBackToLoginPage = document.getElementById(
      ids.domElements.pages.activateAccount.panelBackToLoginPage
    );
    this.buttonBackToLoginPage = document.getElementById(
      ids.domElements.pages.activateAccount.buttonBackToLoginPage
    );
    if (this.buttonBackToLoginPage !== null) {
      this.buttonBackToLoginPage.addEventListener("click", () => {
        this.selectPage("login")
      });
    }
    /** @type {function|null} */
    this.selectPage = null;
    this.emailSpan = document.getElementById(
      ids.domElements.spanCurrentActivationEmail
    );
    this.usernameInput = document.getElementById(
      ids.domElements.spanUserIdInActivateAccountPage
    );
    this.inputNewPassword = document.getElementById(
      ids.domElements.inputNewPasswordInActivationAccount
    );
    this.inputNewPasswordReentered = document.getElementById(
      ids.domElements.inputReenteredPasswordInActivationAccount
    );
    this.buttonChangePassword = document.getElementById(
      ids.domElements.pages.activateAccount.buttonChangePassword
    );
    if (this.buttonChangePassword !== null) {
      this.buttonChangePassword.addEventListener("click", () => {
        this.submitActivateAccountRequest();
      });
    }
    this.spanActivation = document.getElementById(
      ids.domElements.spanVerificationActivation
    );
  }

  updateAccountActivationPage() {
    this.panelBackToLoginPage.style.display = "none";
    this.usernameInput.textContent = storage.variables.user.name.getValue();
    this.emailSpan.textContent = storage.variables.user.email.getValue();
    let activationToken = storage.variables.user.activationToken.getValue();
    this.activationTokenSpan().textContent = activationToken;
    if (
      storage.variables.user.confirmEmailOnlyNoPasswordChange.isTrue()
    ) {
      this.buttonChangePassword.disabled = true;
      this.submitDoActivateAccount();
    } else {
      this.buttonChangePassword.disabled = false;
    }
  }

  activationTokenSpan() {
    return document.getElementById(ids.domElements.spanCurrentActivationToken);
  }

  submitDoActivateAccount() {
    let inputNewPasswordValue = this.inputNewPassword.value;
    let inputNewPasswordReenteredValue = this.inputNewPasswordReentered.value;
    let activationToken = storage.variables.user.activationToken.getValue();
    let userName = storage.variables.user.name.getValue();
    let url = "";
    let email = storage.variables.user.email.getValue();
    const urlFields = pathnames.urlFields;
    url += `${pathnames.urls.calculatorAPI}?${urlFields.request}=activateAccountJSON&`;
    url += `${urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
    url += `${urlFields.email}=${encodeURIComponent(email)}&`;
    url += `${urlFields.newPassword}=${encodeURIComponent(inputNewPasswordValue)}&`;
    url += `${urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReenteredValue)}&`;
    url += `${urlFields.username}=${encodeURIComponent(userName)}&`;
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
    let inputNewPasswordValue = this.inputNewPassword.value;
    let inputNewPasswordReenteredValue = this.inputNewPasswordReentered.value;
    let userName = storage.variables.user.name.getValue();
    let url = "";
    const urlFields = pathnames.urlFields;
    url += `${pathnames.urls.calculatorAPI}?${urlFields.request}=${urlFields.changePassword}&`;
    url += `${urlFields.newPassword}=${encodeURIComponent(inputNewPasswordValue)}&`;
    url += `${urlFields.reenteredPassword}=${encodeURIComponent(inputNewPasswordReenteredValue)}&`;
    url += `${urlFields.username}=${encodeURIComponent(userName)}&`;
    let activationToken = this.activationTokenSpan().textContent;
    if (
      activationToken !== "" &&
      activationToken !== null &&
      activationToken !== undefined
    ) {
      url += `${urlFields.activationToken}=${encodeURIComponent(activationToken)}&`;
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

  submitAccountActivationRequestCallback(wipeOffActivationToken, result) {
    try {
      this.processAccountSubmissionResponse(result);
    } catch (e) {
      this.spanActivation.textContent = "Result: " + result + ". Error: " + e;
    }
    document.getElementById(ids.domElements.inputNewPasswordInActivationAccount).value = "";
    document.getElementById(ids.domElements.inputReenteredPasswordInActivationAccount).value = "";
    if (wipeOffActivationToken === true) {
      storage.variables.user.activationToken.setAndStore("");
    }
  }

  processAccountSubmissionResponse(
    /** @type {string} */
    result
  ) {
    ;
    let inputParsed = miscellaneous.jsonUnescapeParse(result);
    miscellaneous.writeHtmlElementsFromCommentsAndErrors(
      inputParsed,
      this.spanActivation
    );
    const resultFields = pathnames.urlFields.result;
    let resultSpan = document.createElement("div");
    const resultHtml = inputParsed[resultFields.resultHtml];
    if (resultHtml !== undefined) {
      miscellaneous.writeHTML(
        resultSpan, resultHtml,
      );
    }
    const success = inputParsed[resultFields.success];
    if (success === true || success === "true") {
      const successDiv = document.createElement("b");
      successDiv.style.color = "green";
      successDiv.textContent = "Successfully activated the account."
      resultSpan.append(successDiv);
      this.panelBackToLoginPage.style.display = "";
      this.panelBackToLoginPage.style.backgroundColor = "green";
      this.panelBackToLoginPage.style.transition = "1s all";
      setTimeout(() => {
        this.panelBackToLoginPage.style.backgroundColor = "";
      }, 0);
    }
    this.spanActivation.append(resultSpan);
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
