"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements');

function loginCalculator() {
  var password = document.getElementById("inputPassword").value;
  document.getElementById("inputPassword").value = "";
  var username = document.getElementById("inputUsername").value;
  var url = "";
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
  var timeRemaining = time / 1000;
  document.getElementById("spanLoginStatus").innerHTML = `${reason} [in ${timeRemaining} (s)]`; 
  if (time <= 0) {
    doReload();
  } else { 
    reloadPageTimer = setTimeout(reloadPage.bind(null, reason, time - 1000), 1000);
  }
}

var oldUserRole;
function logout() {
  var thePage = window.calculator.mainPage;
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
  logoutGoogle();
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
  initGoogleLogin();
}

function toggleAccountPanels() {
  var thePage = window.calculator.mainPage;
  var accountPanels = document.getElementsByClassName("divAccountPanel");
  for (var counterPanels = 0; counterPanels < accountPanels.length; counterPanels ++) {
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
  var thePage = window.calculator.mainPage;
  var adminPanels = document.getElementsByClassName("divAdminPanel");
  var currentRole = thePage.user.getRole();
  var studentView = thePage.studentView();
  for (var counterPanels = 0; counterPanels < adminPanels.length; counterPanels ++) {
    if (currentRole === "admin" && !studentView) {
      adminPanels[counterPanels].classList.remove("divInvisible");
      adminPanels[counterPanels].classList.add("divVisible");
    } else {
      adminPanels[counterPanels].classList.remove("divVisible");
      adminPanels[counterPanels].classList.add("divInvisible");
    }
  }
  var studentViewPanel = document.getElementById(ids.domElements.spanStudentViewPanel);
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
  var thePage = window.calculator.mainPage;
  var success = false;
  var loginErrorMessage = "";
  var parsedAuthentication = JSON.parse(incomingString);
  resetPagesNeedingReload();
  if (parsedAuthentication["status"] === "logged in") {
    success = true;  
  } 
  if (
    parsedAuthentication[pathnames.urlFields.requests.loginDisabledEveryoneIsAdmin] === "true" || 
    parsedAuthentication[pathnames.urlFields.requests.loginDisabledEveryoneIsAdmin] === true
  ) {
    parsedAuthentication[pathnames.urlFields.username] = "anonymous"; 
    document.getElementById(ids.domElements.divLoginPanelExtraInfo).innerHTML = "<b style = 'color:red'>DB inactive,<br>everyone is admin.</b>";
    parsedAuthentication[pathnames.urlFields.userRole] = "admin";
    success = true;
  } else {
    document.getElementById(ids.domElements.divLoginPanelExtraInfo).innerHTML = "";
  } 
  if (success) {
    thePage.user.makeFromUserInfo(parsedAuthentication);
    toggleAccountPanels();
    setAdminPanels();
    hideLoginCalculatorButtons();
    showLogoutButton();
  } else if (parsedAuthentication["status"] === "not logged in") {
    if (parsedAuthentication["error"] !== undefined) {
      loginErrorMessage = parsedAuthentication["error"];
    }
  }
  if (!success) {
    if (loginErrorMessage!== undefined && loginErrorMessage !== "") {
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
  window.calculator.calculator.callbackToggleStatus(incomingString);
}

function onGoogleSignIn(googleUser) { 
  var theToken = googleUser.getAuthResponse().id_token;
  var thePage = window.calculator.mainPage;
  thePage.user.googleToken = theToken;
  thePage.storage.variables.user.name.setAndStore("");
  thePage.user.googleProfile = window.calculator.jwt.decode(theToken);
  thePage.showProfilePicture();
  showLogoutButton();
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.userInfoJSON}&`;
  theURL += `googleToken=${theToken}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: loginWithServerCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function initGoogleLogin() {
  if (typeof (gapi) === "undefined") {
    return;
  }
  var thePage = window.calculator.mainPage;
  if (thePage.pages.login.initialized === true) {
    return;
  }

  gapi.load('auth2', function() {
    //auth2Google = 
    gapi.auth2.init({
      client_id: '538605306594-n43754vb0m48ir84g8vp5uj2u7klern3.apps.googleusercontent.com',
      // Scopes to request in addition to 'profile' and 'email'
      //scope: 'additional_scope'
    });
    gapi.signin2.render('divGoogleLoginButton', {
      scope: 'profile email',
      prompt: "select_account",
      onsuccess: onGoogleSignIn,
      onfailure: null
    });  
  });
}

function showLoginCalculatorButtons() {
  document.getElementById("divLoginCalculatorPanel").classList.remove("divInvisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divVisible");
  document.getElementById(ids.domElements.divLoginPanelUsernameReport).classList.remove("divVisible");
  document.getElementById(ids.domElements.divLoginPanelUsernameReport).classList.add("divInvisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.remove("divVisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.add("divInvisible");
}

function hideLoginCalculatorButtons() {
  var thePage = window.calculator.mainPage;
  document.getElementById("divLoginCalculatorPanel").classList.remove("divVisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divInvisible");
  document.getElementById(ids.domElements.divLoginPanelUsernameReport).innerHTML = thePage.storage.variables.user.name.value;
  document.getElementById(ids.domElements.divLoginPanelUsernameReport).classList.remove("divInvisible");
  document.getElementById(ids.domElements.divLoginPanelUsernameReport).classList.add("divVisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.remove("divInvisible");
  document.getElementById(ids.domElements.divLoginPanelExtraInfo).classList.add("divVisible");
}

function showLogoutButton() {
  for (;;) {
    var theLogoutLinks = document.getElementsByClassName("linkLogoutInactive");
    if (theLogoutLinks.length === 0) {
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutActive");
    theLogoutLinks[0].classList.remove("linkLogoutInactive");
  }
}

function hideLogoutButton() {
  for (;;) {
    var theLogoutLinks = document.getElementsByClassName("linkLogoutActive");
    if (theLogoutLinks.length === 0) {
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutInactive");
    theLogoutLinks[0].classList.remove("linkLogoutActive");
  }
}

function logoutGoogle() {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.user.googleToken.setAndStore("");
  thePage.user.googleProfile = {};
  if (gapi !== undefined && gapi !== null) {
    gapi.auth2.getAuthInstance().signOut();
  }
}

function getQueryVariable(variable) {
  var query = window.location.search.substring(1);
  var vars = query.split('&');
  for (var i = 0; i < vars.length; i ++) {
    var pair = vars[i].split('=');
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
  logoutGoogle,
  logout,
  loginTry,
  setAdminPanels,
  loginCalculator,
};