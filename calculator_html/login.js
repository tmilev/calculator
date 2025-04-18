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
      this.mainPage.selectPage('signUp');
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

  handlePasswordInputKeyPress(
    /** @type {KeyboardEvent} */ e
  ) {
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
    if (globalUser.loginSequenceInProgress) {
      const span = this.spanLoginStatus();
      span.textContent = "<b style='color:red'>" +
        "Login sequence already started." +
        "</b>" +
        "<br>Refresh the page to reset. ";
      return;
    }
    globalUser.loginSequenceInProgress = true;
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

  spanLoginStatus() {
    return document.getElementById(
      ids.domElements.pages.login.spanLoginStatus
    );
  }

  loginWithServerCallback(
    /** @type {string} */
    incomingString,
    output,
  ) {
    globalUser.loginSequenceInProgress = false;
    let spanLoginStatus = this.spanLoginStatus();
    spanLoginStatus.textContent = "";
    let success = false;
    let loginErrorMessage = "";
    let parsedAuthentication = JSON.parse(incomingString);
    this.mainPage.resetPagesNeedingReload();
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
      loginInfo += "<b style = 'color:red'>DB inactive,";
      loginInfo += "<br>everyone is admin.</b>";
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
      loginInfo += "<b style='color:purple;'>debugLogin is set. </b> " +
        "<b style='color:red; font-size:30px' " +
        "title='Set debugLogin to false in " +
        "configuration/configuration.json to get rid of this message.'>" +
        "Unsafe website. " +
        "LEAKS SERVER PRIVATE KEYS. " +
        "Do not create accounts or enter passwords." +
        "</b>";
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
    let database = parsedAuthentication[
      pathnames.urlFields.requests.database
    ];
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
      globalUser.makeFromUserInfo(parsedAuthentication);
      this.toggleAccountPanels();
      this.setAdminPanels();
      this.hideLoginCalculatorButtons();
      this.showLogoutButton();
      this.mainPage.onStudentViewChange();
    } else if (pathnames.standardResponses.isNotLoggedInResponse(
      parsedAuthentication
    )) {
      const errorString = parsedAuthentication[
        pathnames.urlFields.result.error
      ];
      if (errorString !== undefined) {
        loginErrorMessage = decodeURIComponent(
          errorString
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
      storage.variables.user.email.setAndStore("");
      globalUser.flagLoggedIn = false;
      this.showLoginCalculatorButtons();
      this.toggleAccountPanels();
      this.setAdminPanels();
    }
  }

  toggleAccountPanels() {
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

  showLoginCalculatorButtons() {
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

  logout() {
    storage.variables.user.name.setAndStore("");
    storage.variables.user.authenticationToken.setAndStore("");
    storage.variables.user.role.setAndStore("");
    storage.variables.user.email.setAndStore("");
    this.hideLogoutButton();
    globalUser.hideProfilePicture();
    globalUser.flagLoggedIn = false;
    globalUser.sectionsTaught = [];
    document.getElementById("inputPassword").value = "";
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
    this.showLoginCalculatorButtons();
    this.toggleAccountPanels();
    this.setAdminPanels();
    this.mainPage.afterLogout();
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
    globalUser.googleToken = token;
    this.mainPage.storage.variables.user.name.setAndStore("");
    globalUser.googleProfile = window.calculator.jwt.decode(token);
    this.mainPage.showProfilePicture();
    this.showLogoutButton();
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    const userInfo = pathnames.urlFields.requests.userInfoJSON;
    let url = "";
    url += `${api}?${request}=${userInfo}&`;
    url += `googleToken=${token}&`;
    submitRequests.submitGET({
      url: url,
      callback: (input, output) => {
        this.loginWithServerCallback(input, output);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  loginTry() {
    const api = pathnames.urls.calculatorAPI;
    const request = pathnames.urlFields.request;
    const userInfoJson = pathnames.urlFields.requests.userInfoJSON;
    submitRequests.submitGET({
      url: `${api}?${request}=${userInfoJson}`,
      callback: (input, output) => {
        this.loginWithServerCallback(input, output);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  showLogoutButton() {
    for (; ;) {
      let logoutLinks = document.getElementsByClassName("linkLogoutInactive");
      if (logoutLinks.length === 0) {
        break;
      }
      logoutLinks[0].classList.add("linkLogoutActive");
      logoutLinks[0].classList.remove("linkLogoutInactive");
    }
  }

  hideLoginCalculatorButtons() {
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

  hideLogoutButton() {
    for (; ;) {
      let logoutLinks = document.getElementsByClassName("linkLogoutActive");
      if (logoutLinks.length === 0) {
        break;
      }
      logoutLinks[0].classList.add("linkLogoutInactive");
      logoutLinks[0].classList.remove("linkLogoutActive");
    }
  }
}

function onGoogleSignIn(googleUser) {
  authenticator.onGoogleSignIn(googleUser);
}

let authenticator = new Authenticator();

module.exports = {
  onGoogleSignIn,
  authenticator,
};