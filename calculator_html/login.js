"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;

class Authenticator {
  constructor() {
    this.flagInitialized = false;
    this.reloadPageTimer = null;
    this.mainPage = null;
  }

  initialize(mainPage) {
    if (this.flagInitialized === true) {
      return;
    }
    this.mainPage = mainPage;
    this.flagInitialized = true;
    const buttonLogin = document.getElementById(
      ids.domElements.pages.login.buttonLogin
    );
    if (buttonLogin === null) {
      // No login page.
      return;
    }
    buttonLogin.addEventListener("click", () => {
      this.loginCalculator();
    });
    this.passwordInput().addEventListener(
      "keydown", (
        /** @type {KeyboardEvent} */ e
      ) => {
      this.handlePasswordInputKeyPress(e);
    });
    document.getElementById(
      ids.domElements.pages.login.buttonSignUp
    ).addEventListener('click', () => {
      window.calculator.mainPage.selectPage('signUp');
    });
    document.getElementById(
      ids.domElements.pages.login.linkLogout
    ).addEventListener('click', () => {
      this.logout();
    });
    document.getElementById(
      ids.domElements.pages.login.linkLogoutTop
    ).addEventListener('click', () => {
      this.logout();
    });
  }

  handlePasswordInputKeyPress(/** @type {KeyboardEvent} */ e) {
    if (e.key !== "Enter") {
      return;
    }
    this.loginCalculator();
  }

  passwordInput() {
    return document.getElementById(
      ids.domElements.pages.login.inputPassword
    );
  }

  usernameInput() {
    return document.getElementById(
      ids.domElements.pages.login.inputUsername
    );
  }

  loginCalculator() {
    let password = this.passwordInput().value;
    this.passwordInput().value = "";
    let username = encodeURIComponent(this.usernameInput().value);
    let url = "";
    const request = pathnames.urlFields.request;
    const userInfoJSON = pathnames.urlFields.requests.userInfoJSON;
    url += `${pathnames.urls.calculatorAPI}?${request}=${userInfoJSON}&`;
    let parameters = `password=${password}&` +
      `username=${username}&email=${username}`;
    submitRequests.submitPOST({
      url: url,
      parameters: parameters,
      callback: (
        /** @type {string} */
        input,
        output,
      ) => {
        this.loginWithServerCallback(input, output);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  loginWithServerCallback(
    /** @type {string} */
    incomingString,
    output,
  ) {
    let spanLoginStatus = document.getElementById(
      ids.domElements.pages.login.spanLoginStatus
    );
    spanLoginStatus.textContent = "";
    let success = false;
    let loginErrorMessage = "";
    let parsedAuthentication = JSON.parse(incomingString);
    resetPagesNeedingReload();
    if (pathnames.standardResponses.isLoggedInResponse(
      parsedAuthentication
    )) {
      success = true;
    }
    let loginInfo = "";
    const loginDisabledEveryoneIsAdmin = parsedAuthentication[
      pathnames.urlFields.requests.loginDisabledEveryoneIsAdmin
    ];
    if (
      loginDisabledEveryoneIsAdmin === "true" ||
      loginDisabledEveryoneIsAdmin === true
    ) {
      parsedAuthentication[pathnames.urlFields.username] = "anonymous";
      parsedAuthentication[pathnames.urlFields.userRole] = "admin";
      loginInfo += "<b style = 'color:red'>DB inactive," +
        "<br>everyone is admin.</b>";
      success = true;
      globalUser.flagDatabaseInactiveEveryoneIsAdmin = true;
    }
    const debugLogin = parsedAuthentication[
      pathnames.urlFields.requests.debugLogin
    ];
    if (
      debugLogin === "true" || debugLogin === true
    ) {
      globalUser.debugLogin = true;
      loginInfo += "<b style='color:red'>Debugging login</b>";
    }
    const httpsSupport = parsedAuthentication[
      pathnames.urlFields.requests.httpsSupport
    ];
    if (
      httpsSupport !== "true" &&
      httpsSupport !== true
    ) {
      if (loginInfo !== "") {
        loginInfo += "<br>";
      }
      loginInfo += "<b style='color:red'>Https off.</b>";
    }
    let database = parsedAuthentication[pathnames.urlFields.requests.database];
    if (database === undefined || database === null) {
      database = "?";
    }
    if (database === "local") {
      database = "<b style='color:green'>local database</b>";
    } else if (database === "fallback") {
      database = "<b style='color:red'>fallback database</b>";
    }
    let databaseInfo = document.getElementById(
      ids.domElements.divLoginPanelDatabaseInfo,
    );
    miscellaneous.writeHTML(
      databaseInfo,
      database,
    );
    let loginInfoComponent = document.getElementById(
      ids.domElements.divLoginPanelInfo,
    );
    miscellaneous.writeHTML(loginInfoComponent, loginInfo);
    if (success) {
      this.mainPage.user.makeFromUserInfo(parsedAuthentication);
      toggleAccountPanels();
      setAdminPanels();
      hideLoginCalculatorButtons();
      showLogoutButton();
    } else if (pathnames.standardResponses.isNotLoggedInResponse(
      parsedAuthentication
    )) {
      if (parsedAuthentication[
        pathnames.urlFields.result.error
      ] !== undefined) {
        loginErrorMessage = decodeURIComponent(
          parsedAuthentication[pathnames.urlFields.result.error]
        );
        parsedAuthentication[pathnames.urlFields.result.error] = "";
      }
    }
    let spanLoginExtra = document.getElementById(
      ids.domElements.pages.login.spanLoginStatusExtra
    );
    miscellaneous.writeHtmlElementsFromCommentsAndErrors(
      parsedAuthentication, spanLoginExtra
    );
    if (!success) {
      if (loginErrorMessage !== undefined && loginErrorMessage !== "") {
        miscellaneous.writeHTML(spanLoginStatus, loginErrorMessage);
      }
      storage.variables.user.authenticationToken.setAndStore("");
      storage.variables.user.name.setAndStore("");
      storage.variables.user.role.setAndStore("");
      globalUser.flagLoggedIn = false;
      showLoginCalculatorButtons();
      toggleAccountPanels();
      setAdminPanels();
    }
  }

  logout() {
    storage.variables.user.name.setAndStore("");
    storage.variables.user.authenticationToken.setAndStore("");
    storage.variables.user.role.setAndStore("");
    hideLogoutButton();
    globalUser.hideProfilePicture();
    globalUser.flagLoggedIn = false;
    globalUser.sectionsTaught = [];
    document.getElementById("inputPassword").value = "";
    this.logoutPartTwo();
  }

  logoutPartTwo() {
    let loginStatus = document.getElementById("spanLoginStatus");
    let boldComponent = document.createElement("b");
    let anchor = document.createElement("a");
    boldComponent.appendChild(anchor);
    anchor.href = '#';
    anchor.addEventListener('click', () => {
      this.reloadPage('<b>User requested reload. </b>', 0);
    });
    anchor.textContent = "Reload page";
    let textNode = document.createElement("span");
    miscellaneous.writeHTML(
      textNode,
      " for complete logout " +
      "(when <span style='color:red'>using public computer</span>).",
    );
    loginStatus.appendChild(boldComponent);
    loginStatus.appendChild(textNode);
    showLoginCalculatorButtons();
    toggleAccountPanels();
    this.setAdminPanels();
    this.mainPage.selectAndStorePage(this.mainPage.pages.login.name);
  }

  reloadPage(reason, time) {
    clearTimeout(this.reloadPageTimer);
    this.reloadPageTimer = null;
    if (time < 0) {
      time = 0;
    }
    let timeRemaining = time / 1000;
    let element = document.getElementById("spanLoginStatus");
    miscellaneous.writeHTML(
      element,
      `${reason} [in ${timeRemaining} (s)]`,
    );
    if (time <= 0) {
      this.doReload();
      return;
    }
    this.reloadPageTimer = setTimeout(() => {
      this.reloadPage(reason, time - 1000);
    }, 1000);
  }

  doReload() {
    location.reload();
  }

  setAdminPanels() {
    let adminPanels = document.getElementsByClassName("divAdminPanel");
    let currentRole = globalUser.getRole();
    let studentView = this.mainPage.studentView();
    for (let i = 0; i < adminPanels.length; i++) {
      if (currentRole === "admin" && !studentView) {
        adminPanels[i].classList.remove("divInvisible");
        adminPanels[i].classList.add("divVisible");
      } else {
        adminPanels[i].classList.remove("divVisible");
        adminPanels[i].classList.add("divInvisible");
      }
    }
    let studentViewPanel = document.getElementById(
      ids.domElements.spanStudentViewPanel
    );
    if (currentRole === "admin") {
      studentViewPanel.classList.remove("divInvisible")
      studentViewPanel.classList.add("divVisible");
    } else {
      studentViewPanel.classList.remove("divVisible");
      studentViewPanel.classList.add("divInvisible");
    }
  }

  onGoogleSignIn(googleUser) {
    let token = googleUser.credential;
    this.mainPage.user.googleToken = token;
    this.mainPage.storage.variables.user.name.setAndStore("");
    this.mainPage.user.googleProfile = window.calculator.jwt.decode(token);
    this.mainPage.showProfilePicture();
    showLogoutButton();
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    const userInfo = pathnames.urlFields.requests.userInfoJSON;
    let url = "";
    url += `${api}?${request}=${userInfo}&`;
    url += `googleToken=${token}&`;
    submitRequests.submitGET({
      url: url,
      callback: loginWithServerCallback,
      progress: ids.domElements.spanProgressReportGeneral
    });
  }
}

function loginTry() {
  const api = pathnames.urls.calculatorAPI;
  const request = pathnames.urlFields.request;
  const userInfoJson = pathnames.urlFields.requests.userInfoJSON;
  submitRequests.submitGET({
    url: `${api}?${request}=${userInfoJson}`,
    callback: loginWithServerCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function toggleAccountPanels() {
  let accountPanels = document.getElementsByClassName("divAccountPanel");
  for (let i = 0; i < accountPanels.length; i++) {
    if (globalUser.flagLoggedIn === true) {
      accountPanels[i].classList.remove("divInvisible");
      accountPanels[i].classList.add("divVisible");
    } else {
      accountPanels[i].classList.remove("divVisible");
      accountPanels[i].classList.add("divInvisible");
    }
  }
}

function setAdminPanels() {
  authenticator.setAdminPanels();
}

function resetPagesNeedingReload() {
  window.calculator.selectCourse.pageSetup.needsLoad = true;
  window.calculator.coursePage.lastLoadedCourse.courseHome = null;
  window.calculator.coursePage.lastLoadedCourse.topicList = null;
}

function loginWithServerCallback(incomingString, result) {
  authenticator.loginWithServerCallback(incomingString);
}

function onGoogleSignIn(googleUser) {
  authenticator.onGoogleSignIn(googleUser);
}

function showLoginCalculatorButtons() {
  const loginIds = ids.domElements.pages.login;
  document.getElementById(
    loginIds.divLoginCalculatorPanel
  ).classList.remove("divInvisible");
  document.getElementById(
    loginIds.divLoginCalculatorPanel
  ).classList.add("divVisible");
  document.getElementById(
    loginIds.userNameReport
  ).classList.remove("divVisible");
  document.getElementById(
    loginIds.userNameReport
  ).classList.add("divInvisible");
}

function hideLoginCalculatorButtons() {
  const loginIds = ids.domElements.pages.login;
  document.getElementById(
    loginIds.divLoginCalculatorPanel
  ).classList.remove("divVisible");
  document.getElementById(
    loginIds.divLoginCalculatorPanel
  ).classList.add("divInvisible");
  document.getElementById(
    loginIds.userNameReport
  ).textContent = storage.variables.user.name.value;
  document.getElementById(
    loginIds.userNameReport
  ).classList.remove("divInvisible");
  document.getElementById(
    loginIds.userNameReport
  ).classList.add("divVisible");
}

function showLogoutButton() {
  for (; ;) {
    let logoutLinks = document.getElementsByClassName("linkLogoutInactive");
    if (logoutLinks.length === 0) {
      break;
    }
    logoutLinks[0].classList.add("linkLogoutActive");
    logoutLinks[0].classList.remove("linkLogoutInactive");
  }
}

function hideLogoutButton() {
  for (; ;) {
    let logoutLinks = document.getElementsByClassName("linkLogoutActive");
    if (logoutLinks.length === 0) {
      break;
    }
    logoutLinks[0].classList.add("linkLogoutInactive");
    logoutLinks[0].classList.remove("linkLogoutActive");
  }
}

let authenticator = new Authenticator();

module.exports = {
  resetPagesNeedingReload,
  loginTry,
  setAdminPanels,
  onGoogleSignIn,
  authenticator,
};