"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const login = require('./login');
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;

class AccountPage {
  constructor() {
    this.mainPage = null;
    this.spanVerification = document.getElementById("spanVerification");
    this.usernameInput = document.getElementById(
      ids.domElements.spanUserIdInAccountsPage
    );
    this.emailSpan = document.getElementById(ids.domElements.spanOldEmail);
  }

  initialize(mainPage) {
    const buttonChangePasswordFromAccountPage = document.getElementById(
      ids.domElements.pages.account.buttonChangePasswordFromAccountPage
    );
    if (buttonChangePasswordFromAccountPage === null) {
      // Account page not found.
      return;
    }
    buttonChangePasswordFromAccountPage.addEventListener('click', () => {
      this.submitChangePassRequest();
    });
    document.getElementById(
      ids.domElements.pages.account.buttonChangeEmail
    ).addEventListener('click', () => {
      this.submitChangePassRequest();
    });
    this.mainPage = mainPage;
  }

  submitChangePassRequestCallback(result) {
    const resultParsed = miscellaneous.jsonUnescapeParse(result);
    miscellaneous.writeHtmlFromCommentsAndErrors(
      resultParsed,
      this.spanVerification,
    );
    document.getElementById("inputPassword").value = document.getElementById(
      "inputNewPasswordInAccount"
    ).value;
    document.getElementById("inputOldPasswordInAccount").value = "";
    document.getElementById("inputNewPasswordInAccount").value = "";
    document.getElementById("inputReenteredPasswordInAccount").value = "";
    login.authenticator.loginCalculator();
  }

  submitChangePassRequest() {
    let inputOldPassword = document.getElementById("inputOldPasswordInAccount");
    let inputNewPassword = document.getElementById("inputNewPasswordInAccount");
    let inputReenteredPassword = document.getElementById("inputReenteredPasswordInAccount");
    let inputEmail = document.getElementById("inputEmail");
    const urlFields = pathnames.urlFields;
    const calculatorAPI = pathnames.urls.calculatorAPI;
    let url = "";
    const encodedOldPassword = encodeURIComponent(inputOldPassword.value);
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
    let spanExtraInfo = document.getElementById(ids.domElements.spanUserExtraInfo);
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
}

function updateAccountPage() {
  accountPage.updatePage();
}

let accountPage = new AccountPage();

module.exports = {
  accountPage,
  updateAccountPage,
};