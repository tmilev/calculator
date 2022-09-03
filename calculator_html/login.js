"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");

class Authenticator {
  constructor() {
    this.flagInitialized = false;
  }
  
  initialize() {
    if (this.flagInitialized === true) {
      return;
    } 
    this.flagInitialized = true;
    document.getElementById(
      ids.domElements.pages.login.buttonLogin
    ).addEventListener("click", () => {
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
      logout();
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
    this.initialize();
    let password = this.passwordInput().value;
    this.passwordInput().value = "";
    let username = encodeURIComponent(this.usernameInput().value);
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}&`;
    let parameters = `password=${password}&username=${username}&email=${username}`;
    submitRequests.submitPOST({
      url: url,
      parameters: parameters,
      callback: (
        /** @type{string}*/
        input,
        output,
      ) => {
        this.loginWithServerCallback(input, output);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  loginWithServerCallback(
    /** @type{string}*/
    incomingString,
    output,
  ) {
    let spanLoginStatus = document.getElementById(ids.domElements.pages.login.spanLoginStatus);
    spanLoginStatus.innerHTML = "";
    let page = window.calculator.mainPage;
    let success = false;
    let loginErrorMessage = "";
    let parsedAuthentication = JSON.parse(incomingString);
    resetPagesNeedingReload();
    if (pathnames.standardResponses.isLoggedInResponse(parsedAuthentication)) {
      success = true;
    }
    let loginInfo = "";
    if (
      parsedAuthentication[pathnames.urlFields.requests.loginDisabledEveryoneIsAdmin] === "true" ||
      parsedAuthentication[pathnames.urlFields.requests.loginDisabledEveryoneIsAdmin] === true
    ) {
      parsedAuthentication[pathnames.urlFields.username] = "anonymous";
      parsedAuthentication[pathnames.urlFields.userRole] = "admin";
      loginInfo += "<b style = 'color:red'>DB inactive,<br>everyone is admin.</b>"
      success = true;
      page.user.flagDatabaseInactiveEveryoneIsAdmin = true;
    }
    if (
      parsedAuthentication[pathnames.urlFields.requests.debugLogin] === "true" ||
      parsedAuthentication[pathnames.urlFields.requests.debugLogin] === true
    ) {
      page.user.debugLogin = true;
      loginInfo += "<b style='color:red'>Debugging login</b>";
    }
    if (
      parsedAuthentication[pathnames.urlFields.requests.httpsSupport] !== "true" &&
      parsedAuthentication[pathnames.urlFields.requests.httpsSupport] !== true
    ) {
      if (loginInfo !== "") {
        loginInfo += "<br>";
      }
      loginInfo += "<b style='color:red'>Https off.</b>";
    }
    if (
      parsedAuthentication[pathnames.urlFields.requests.useFallbackDatabase] === "true" ||
      parsedAuthentication[pathnames.urlFields.requests.useFallbackDatabase] === true
    ) {
      let databaseInfo = document.getElementById(ids.domElements.divLoginPanelDatabaseInfo);
      databaseInfo.innerHTML = "<b style='color:red'>Fallback database.</b>";
    }
    let loginInfoComponent = document.getElementById(ids.domElements.divLoginPanelInfo);
    loginInfoComponent.innerHTML = loginInfo;
    if (success) {
      page.user.makeFromUserInfo(parsedAuthentication);
      toggleAccountPanels();
      setAdminPanels();
      hideLoginCalculatorButtons();
      showLogoutButton();
    } else if (pathnames.standardResponses.isNotLoggedInResponse(parsedAuthentication)) {
      if (parsedAuthentication[
        pathnames.urlFields.result.error
      ] !== undefined) {
        loginErrorMessage = decodeURIComponent(
          parsedAuthentication[pathnames.urlFields.result.error]
        );
        parsedAuthentication[pathnames.urlFields.result.error] = "";
      }
    }
    if (!success) {
      if (page.user.debugLoginIsOn()) {
        let spanLoginExtra = document.getElementById(
          ids.domElements.pages.login.spanLoginStatusExtra
        );
        miscellaneous.writeHtmlElementsFromCommentsAndErrors(parsedAuthentication, spanLoginExtra);
      }
      if (loginErrorMessage !== undefined && loginErrorMessage !== "") {
        spanLoginStatus.innerHTML = loginErrorMessage;
      }
      page.storage.variables.user.authenticationToken.setAndStore("");
      page.storage.variables.user.name.setAndStore("");
      page.storage.variables.user.role.setAndStore("");
      page.user.flagLoggedIn = false;
      showLoginCalculatorButtons();
      toggleAccountPanels();
      setAdminPanels();
    }
  }
}

function doReload() {
  location.reload();
}

var reloadPageTimer = null;

function reloadPage(reason, time) {
  clearTimeout(reloadPageTimer);
  reloadPageTimer = null;
  if (time < 0) {
    time = 0;
  }
  let timeRemaining = time / 1000;
  document.getElementById("spanLoginStatus").innerHTML = `${reason} [in ${timeRemaining} (s)]`;
  if (time <= 0) {
    doReload();
  } else {
    reloadPageTimer = setTimeout(reloadPage.bind(null, reason, time - 1000), 1000);
  }
}

var oldUserRole;
function logout() {
  let page = window.calculator.mainPage;
  page.storage.variables.user.name.setAndStore("");
  page.storage.variables.user.authenticationToken.setAndStore("");
  page.storage.variables.user.role.setAndStore("");
  hideLogoutButton();
  page.user.hideProfilePicture();
  page.user.flagLoggedIn = false;
  page.user.sectionsTaught = [];
  page.pages.problemPage.flagLoaded = false;
  document.getElementById("inputPassword").value = "";
  document.getElementById(ids.domElements.problemPageContentContainer).innerHTML = "";
  document.getElementById(ids.domElements.divCurrentCourseBody).innerHTML = "";
  logoutPartTwo();
}

function logoutPartTwo() {
  var page = window.calculator.mainPage;
  if (oldUserRole === "admin") {
    reloadPage("<b>Logging out admin: mandatory page reload. </b>", 0);
  } else {
    var loginStatus = "";
    loginStatus += `<b><a href='#' onclick = "window.calculator.login.reloadPage('<b>User requested reload. </b>', 0);">Reload page</a>`;
    loginStatus += ` for complete logout (when <span style='color:red'>using public computer</span>).</b>`;
    document.getElementById("spanLoginStatus").innerHTML = loginStatus;
    showLoginCalculatorButtons();
    toggleAccountPanels();
    setAdminPanels();
    page.selectPage(page.pages.login.name);
  }
}

function loginTry() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}`,
    callback: loginWithServerCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function toggleAccountPanels() {
  let page = window.calculator.mainPage;
  let accountPanels = document.getElementsByClassName("divAccountPanel");
  for (let counterPanels = 0; counterPanels < accountPanels.length; counterPanels++) {
    if (page.user.flagLoggedIn === true) {
      accountPanels[counterPanels].classList.remove("divInvisible");
      accountPanels[counterPanels].classList.add("divVisible");
    } else {
      accountPanels[counterPanels].classList.remove("divVisible");
      accountPanels[counterPanels].classList.add("divInvisible");
    }
  }
}

function setAdminPanels() {
  let page = window.calculator.mainPage;
  let adminPanels = document.getElementsByClassName("divAdminPanel");
  let currentRole = page.user.getRole();
  let studentView = page.studentView();
  for (let counterPanels = 0; counterPanels < adminPanels.length; counterPanels++) {
    if (currentRole === "admin" && !studentView) {
      adminPanels[counterPanels].classList.remove("divInvisible");
      adminPanels[counterPanels].classList.add("divVisible");
    } else {
      adminPanels[counterPanels].classList.remove("divVisible");
      adminPanels[counterPanels].classList.add("divInvisible");
    }
  }
  let studentViewPanel = document.getElementById(ids.domElements.spanStudentViewPanel);
  if (currentRole === "admin") {
    studentViewPanel.classList.remove("divInvisible")
    studentViewPanel.classList.add("divVisible");
  } else {
    studentViewPanel.classList.remove("divVisible");
    studentViewPanel.classList.add("divInvisible");
  }
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
  let token = googleUser.credential;
  let page = window.calculator.mainPage;
  page.user.googleToken = token;
  page.storage.variables.user.name.setAndStore("");
  page.user.googleProfile = window.calculator.jwt.decode(token);
  page.showProfilePicture();
  showLogoutButton();
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}&`;
  theURL += `googleToken=${token}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: loginWithServerCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function showLoginCalculatorButtons() {
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.remove("divInvisible");
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.add("divVisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.remove("divVisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.add("divInvisible");
}

function hideLoginCalculatorButtons() {
  let page = window.calculator.mainPage;
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.remove("divVisible");
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.add("divInvisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).innerHTML = page.storage.variables.user.name.value;
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.remove("divInvisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.add("divVisible");
}

function showLogoutButton() {
  for (; ;) {
    let theLogoutLinks = document.getElementsByClassName("linkLogoutInactive");
    if (theLogoutLinks.length === 0) {
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutActive");
    theLogoutLinks[0].classList.remove("linkLogoutInactive");
  }
}

function hideLogoutButton() {
  for (; ;) {
    let theLogoutLinks = document.getElementsByClassName("linkLogoutActive");
    if (theLogoutLinks.length === 0) {
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutInactive");
    theLogoutLinks[0].classList.remove("linkLogoutActive");
  }
}

function getQueryVariable(variable) {
  let query = window.location.search.substring(1);
  let vars = query.split('&');
  for (let i = 0; i < vars.length; i++) {
    let pair = vars[i].split('=');
    if (decodeURIComponent(pair[0]) === variable) {
      return decodeURIComponent(pair[1]);
    }
  }
  return null;
}

function init() {
  logoutRequestFromUrl = getQueryVariable("logout");
  locationRequestFromUrl = getQueryVariable("location");
  if (logoutRequestFromUrl === "true") {
    logout();
  }
}

let authenticator = new Authenticator();

module.exports = {
  resetPagesNeedingReload,
  reloadPage,
  init,
  logout,
  loginTry,
  setAdminPanels,
  onGoogleSignIn,
  authenticator,
};