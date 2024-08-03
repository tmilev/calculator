"use strict";
const ids = require("./ids_dom_elements");
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const login = require('./login');
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage");

class AccountPage {
  constructor() {
    this.mainPage = null;
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
      submitChangePassRequest();
    });
    document.getElementById(
      ids.domElements.pages.account.buttonChangeEmail
    ).addEventListener('click', () => {
      submitChangePassRequest();
    });
    this.mainPage = mainPage;
  }
}

function submitChangePassRequestCallback(result) {
  let spanVerification = document.getElementById("spanVerification");
  miscellaneous.writeHTML(
    spanVerification,
    miscellaneous.jsonParseGetHtmlStandard(result),
  );
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
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.changePassword}&`;
  url += `${pathnames.urlFields.password}=${encodeURIComponent(inputOldPassword.value)}&`;
  url += `${pathnames.urlFields.newPassword}=${encodeURIComponent(inputNewPassword.value)}&`;
  url += `${pathnames.urlFields.reenteredPassword}=${encodeURIComponent(inputReenteredPassword.value)}&`;
  url += `${pathnames.urlFields.email}=${encodeURIComponent(inputEmail.value)}&`;
  url += "doReload=false&"
  submitRequests.submitGET({
    url: url,
    callback: submitChangePassRequestCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function updateAccountPage() {
  let usernameInput = document.getElementById(
    ids.domElements.spanUserIdInAccountsPage
  );
  let emailSpan = document.getElementById(ids.domElements.spanOldEmail);
  usernameInput.textContent = storage.variables.user.name.getValue();
  emailSpan.textContent = storage.variables.user.email.getValue();
  let spanExtraInfo = document.getElementById(ids.domElements.spanUserExtraInfo);
  let table = document.createElement("table");
  let resultCell = document.createElement("th");
  resultCell.textContent = "Role:";
  let row = table.insertRow();
  row.appendChild(resultCell);
  resultCell = document.createElement("th");
  resultCell.textContent = this.mainPage.user.getRole();
  if (
    this.mainPage.user.sectionsTaught.length > 0 &&
    !this.mainPage.studentView()
  ) {
    row = table.insertRow();
    row.insertCell().appendChild(
      document.createTextNode(
        "Sections taught: "
      )
    );
    row.insertCell().appendChild(
      document.createTextNode(
        this.mainPage.user.sectionsTaught.join(", ")
      )
    );
  }
  function insertRow(first, second) {
    row = table.insertRow();
    row.insertCell().appendChild(
      document.createTextNode(first)
    );
    row.insertCell().appendChild(
      document.createTextNode(second)
    );
  }
  insertRow("Instructor: ", this.mainPage.user.instructor);
  insertRow("Section in database: ", this.mainPage.user.sectionInDB);
  insertRow("Section computed: ", this.mainPage.user.sectionComputed);
  insertRow("Deadline schema: ", this.mainPage.user.deadlineSchema);
  spanExtraInfo.textContent = "";
  spanExtraInfo.appendChild(table);
}

let accountPage = new AccountPage();

module.exports = {
  accountPage,
  updateAccountPage,
  submitChangePassRequest
};