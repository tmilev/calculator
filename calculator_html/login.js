"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");

function loginCalculator() {
  let password = document.getElementById("inputPassword").value;
  document.getElementById("inputPassword").value = "";
  let username = document.getElementById("inputUsername").value;
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}&`;
  url += `password=${password}&username=${username}&`;
  submitRequests.submitGET({
    url: url,
    callback: loginWithServerCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
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
  let thePage = window.calculator.mainPage;
  thePage.storage.variables.user.name.setAndStore("");
  thePage.storage.variables.user.authenticationToken.setAndStore("");
  thePage.storage.variables.user.role.setAndStore("");
  hideLogoutButton();
  thePage.user.hideProfilePicture();
  thePage.user.flagLoggedIn = false;
  thePage.user.sectionsTaught = [];
  thePage.pages.problemPage.flagLoaded = false;
  document.getElementById("inputPassword").value = "";
  document.getElementById(ids.domElements.problemPageContentContainer).innerHTML = "";
  document.getElementById(ids.domElements.divCurrentCourseBody).innerHTML = "";
  logoutPartTwo();
}

function logoutPartTwo() {
  var thePage = window.calculator.mainPage;
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
    thePage.selectPage(thePage.pages.login.name);
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
  let thePage = window.calculator.mainPage;
  let accountPanels = document.getElementsByClassName("divAccountPanel");
  for (let counterPanels = 0; counterPanels < accountPanels.length; counterPanels++) {
    if (thePage.user.flagLoggedIn === true) {
      accountPanels[counterPanels].classList.remove("divInvisible");
      accountPanels[counterPanels].classList.add("divVisible");
    } else {
      accountPanels[counterPanels].classList.remove("divVisible");
      accountPanels[counterPanels].classList.add("divInvisible");
    }
  }
}

function setAdminPanels() {
  let thePage = window.calculator.mainPage;
  let adminPanels = document.getElementsByClassName("divAdminPanel");
  let currentRole = thePage.user.getRole();
  let studentView = thePage.studentView();
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
  document.getElementById("spanLoginStatus").innerHTML = "";
  let thePage = window.calculator.mainPage;
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
    thePage.user.flagDatabaseInactiveEveryoneIsAdmin = true;
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
  let loginExtraInfo = document.getElementById(ids.domElements.divLoginPanelExtraInfo);
  loginExtraInfo.innerHTML = loginInfo;
  if (success) {
    thePage.user.makeFromUserInfo(parsedAuthentication);
    toggleAccountPanels();
    setAdminPanels();
    hideLoginCalculatorButtons();
    showLogoutButton();
  } else if (pathnames.standardResponses.isNotLoggedInResponse(parsedAuthentication)) {
    if (parsedAuthentication["error"] !== undefined) {
      loginErrorMessage = parsedAuthentication["error"];
    }
  }
  if (!success) {
    if (loginErrorMessage !== undefined && loginErrorMessage !== "") {
      document.getElementById("spanLoginStatus").innerHTML = decodeURIComponent(loginErrorMessage);
    }
    thePage.storage.variables.user.authenticationToken.setAndStore("");
    thePage.storage.variables.user.name.setAndStore("");
    thePage.storage.variables.user.role.setAndStore("");
    thePage.user.flagLoggedIn = false;
    showLoginCalculatorButtons();
    toggleAccountPanels();
    setAdminPanels();
  }
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
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.remove("divVisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.add("divInvisible");
}

function hideLoginCalculatorButtons() {
  let thePage = window.calculator.mainPage;
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.remove("divVisible");
  document.getElementById(ids.domElements.pages.login.divLoginCalculatorPanel).classList.add("divInvisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).innerHTML = thePage.storage.variables.user.name.value;
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.remove("divInvisible");
  document.getElementById(ids.domElements.pages.login.userNameReport).classList.add("divVisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.remove("divInvisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.add("divVisible");
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

module.exports = {
  resetPagesNeedingReload,
  reloadPage,
  init,
  logout,
  loginTry,
  setAdminPanels,
  loginCalculator,
  onGoogleSignIn,
};