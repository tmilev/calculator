"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const login = require('./login');
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;
const PanelFromToggleAndContent = require("./panels").PanelFromToggleAndContent;

class AccountPage {
  constructor() {
    this.spanVerification = document.getElementById("spanVerification");
    this.usernameInput = document.getElementById(
      ids.domElements.spanUserIdInAccountsPage
    );
    this.emailSpan = document.getElementById(ids.domElements.spanOldEmail);
    const accountIds = ids.domElements.pages.account;
    this.buttonRevealDeleteAccount = document.getElementById(
      accountIds.buttonRevealDeleteAccount
    );
    this.divDoDeleteAccountPanel = document.getElementById(
      accountIds.divDoDeleteAccountPanel
    );
    this.buttonDoDeleteAccount = document.getElementById(
      accountIds.buttonDoDeleteAccount
    );
    this.buttonChangeEmail = document.getElementById(
      accountIds.buttonChangeEmail
    );
    this.buttonChangePasswordFromAccountPage = document.getElementById(
      accountIds.buttonChangePasswordFromAccountPage
    );
    this.divDoDeleteAccountFinalPanel = document.getElementById(
      accountIds.divDoDeleteAccountFinalPanel
    );
    this.buttonDoDeleteAccountFinal = document.getElementById(
      accountIds.buttonDoDeleteAccountFinal
    );
    this.divAccountDeletionStatus = document.getElementById(
      accountIds.divAccountDeletionStatus
    );
    this.inputOldPasswordInAccount = document.getElementById(
      accountIds.inputOldPasswordInAccount
    );
    this.inputNewPasswordInAccount = document.getElementById(
      accountIds.inputNewPasswordInAccount
    );
    /** @type {PanelFromToggleAndContent|null} */
    this.panel = null;
    this.deleteAccountToken = "";
  }

  initialize(mainPage) {
    if (this.buttonChangePasswordFromAccountPage === null) {
      // Account page not found.
      return;
    }
    this.buttonChangePasswordFromAccountPage.addEventListener('click', () => {
      this.submitChangePassRequest();
    });
    this.buttonChangeEmail.addEventListener('click', () => {
      this.submitChangePassRequest();
    });
    this.panel = new PanelFromToggleAndContent(
      this.buttonRevealDeleteAccount,
      this.divDoDeleteAccountPanel,
      false,
    );
    this.panel.originalElementHeight = 150;
    this.buttonDoDeleteAccount.addEventListener("click", () => {
      this.deleteAccountRequest();
    });
    this.buttonDoDeleteAccountFinal.addEventListener("click", () => {
      this.deleteAccountFinal();
    });
  }

  submitChangePassRequestCallback(result) {
    const resultParsed = miscellaneous.jsonUnescapeParse(result);
    const success = resultParsed[pathnames.urlFields.result.success];
    if (success === true) {
      this.spanVerification.style.color = "green";
    } else {
      this.spanVerification.style.color = "red";
    }
    miscellaneous.writeHtmlFromCommentsAndErrors(
      resultParsed,
      this.spanVerification,
    );
    if (globalUser.debugLogin) {
      const links = miscellaneous.extractLinksFromStandardJsonReturnAnchorCollection(
        resultParsed
      );
      if (links !== null) {
        this.spanVerification.appendChild(links);
      }
    }
    const wasJustEmailChange = (this.inputNewPasswordInAccount.value === "");
    this.inputOldPasswordInAccount.value = "";
    this.inputNewPasswordInAccount.value = "";
    document.getElementById("inputReenteredPasswordInAccount").value = "";
    if (success === true && !wasJustEmailChange) {
      document.getElementById("inputPassword").value = this.inputNewPasswordInAccount.value;
      const pleaseLoginAgain = document.createElement("span");
      pleaseLoginAgain.textContent = "Please login again";
      this.spanVerification.appendChild(pleaseLoginAgain);
    }
  }

  submitChangePassRequest() {
    this.spanVerification.textContent = "";
    this.spanVerification.style.fontWeight = "bolder";
    let inputNewPassword = document.getElementById("inputNewPasswordInAccount");
    let inputReenteredPassword = document.getElementById("inputReenteredPasswordInAccount");
    let inputEmail = document.getElementById("inputEmail");
    const urlFields = pathnames.urlFields;
    const calculatorAPI = pathnames.urls.calculatorAPI;
    let url = "";
    const encodedOldPassword = encodeURIComponent(this.inputOldPasswordInAccount.value);
    const encodedNewPassword = encodeURIComponent(inputNewPassword.value);
    const encodedReenteredPassword = encodeURIComponent(
      inputReenteredPassword.value
    );
    const encodedEmail = encodeURIComponent(inputEmail.value);
    url += `${calculatorAPI}?`;
    url += `${urlFields.request}=${urlFields.changePassword}&`;
    url += `${urlFields.password}=${encodedOldPassword}&`;
    url += `${urlFields.newPassword}=${encodedNewPassword}&`;
    url += `${urlFields.reenteredPassword}=${encodedReenteredPassword}&`;
    url += `${urlFields.email}=${encodedEmail}&`;
    url += "doReload=false&"
    submitRequests.submitGET({
      url: url,
      callback: (result) => {
        this.submitChangePassRequestCallback(result);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  updatePage() {
    this.usernameInput.textContent = storage.variables.user.name.getValue();
    this.emailSpan.textContent = storage.variables.user.email.getValue();
    let spanExtraInfo = document.getElementById(
      ids.domElements.spanUserExtraInfo
    );
    let table = document.createElement("table");
    let resultCell = document.createElement("th");
    resultCell.textContent = "Role:";
    const row = table.insertRow();
    row.appendChild(resultCell);
    resultCell = document.createElement("th");
    resultCell.textContent = globalUser.getRole();
    if (
      globalUser.sectionsTaught.length > 0 &&
      !globalUser.studentView()
    ) {
      row = table.insertRow();
      row.insertCell().appendChild(
        document.createTextNode(
          "Sections taught: "
        )
      );
      row.insertCell().appendChild(
        document.createTextNode(
          globalUser.sectionsTaught.join(", ")
        )
      );
    }
    this.insertRow(table, "Instructor: ", globalUser.instructor);
    this.insertRow(table, "Section in database: ", globalUser.sectionInDB);
    this.insertRow(table, "Section computed: ", globalUser.sectionComputed);
    this.insertRow(table, "Deadline schema: ", globalUser.deadlineSchema);
    spanExtraInfo.textContent = "";
    spanExtraInfo.appendChild(table);
  }

  insertRow(table, first, second) {
    const row = table.insertRow();
    row.insertCell().appendChild(
      document.createTextNode(first)
    );
    row.insertCell().appendChild(
      document.createTextNode(second)
    );
  }

  deleteAccountRequest() {
    this.divAccountDeletionStatus.textContent = "";
    this.divDoDeleteAccountFinalPanel.style.opacity = 0;
    const urlFields = pathnames.urlFields;
    const requests = urlFields.requests;
    const calculatorAPI = pathnames.urls.calculatorAPI;
    let url = `${calculatorAPI}?`;
    url += `${urlFields.request}=${requests.deleteAccount}&`;
    submitRequests.submitGET({
      url: url,
      callback: (result) => {
        this.deleteAccountCallback(result);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  deleteAccountCallback(result) {
    const parsed = miscellaneous.jsonUnescapeParse(result);
    this.deleteAccountToken = parsed[pathnames.urlFields.requests.deleteAccountToken];
    this.divDoDeleteAccountFinalPanel.style.opacity = 1;
    const success = parsed[pathnames.urlFields.result.success];
    if (success !== true && success !== "true") {
      miscellaneous.writeHtmlElementsFromCommentsAndErrors(
        parsed, this.divAccountDeletionStatus
      );
    }
    if (pathnames.standardResponses.isNotLoggedInResponse(parsed)) {
      const errorElement = document.createElement("b");
      errorElement.style.color = "red";
      errorElement.textContent = "I have trouble authenticating you, " +
        "please login again";
      this.divAccountDeletionStatus.appendChild(errorElement);
    }
  }

  deleteAccountFinal() {
    this.divAccountDeletionStatus.textContent = "";
    const urlFields = pathnames.urlFields;
    const requests = urlFields.requests;
    const calculatorAPI = pathnames.urls.calculatorAPI;
    let url = `${calculatorAPI}?`;
    url += `${urlFields.request}=${requests.deleteAccount}&`;
    url += `${requests.deleteAccountToken}=${this.deleteAccountToken}`;
    submitRequests.submitGET({
      url: url,
      callback: (result) => {
        this.deleteAccountFinalCallback(result);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  deleteAccountFinalCallback(result) {
    const parsed = miscellaneous.jsonUnescapeParse(result);
    const success = parsed[pathnames.urlFields.result.success];
    if (success !== true && success !== "true") {
      miscellaneous.writeHtmlElementsFromCommentsAndErrors(
        parsed, this.divAccountDeletionStatus
      );
    } else {
      this.divAccountDeletionStatus.textContent = "";
      const successElement = document.createElement("b");
      successElement.textContent =
        "Your account was deleted. Please refresh the page. ";
      successElement.style.color = "green";
      this.divAccountDeletionStatus.appendChild(successElement);
    }
  }
}

function updateAccountPage() {
  accountPage.updatePage();
}

let accountPage = new AccountPage();

module.exports = {
  accountPage,
  updateAccountPage,
};