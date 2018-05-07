"use strict";

function loginCalculator(){
  var password = document.getElementById("inputPassword").value;
  document.getElementById("inputPassword").value = "";
  var username = document.getElementById("inputUsername").value;
  submitGET({
    "url": `${pathnames.calculatorAPI}?request=userInfoJSON&password=${password}&username=${username}`,
    callback: loginWithServerCallback,
    progress: "spanProgressReportGeneral"
  });
}

function doReload(){
  location.reload();
}

var reloadPageTimer = null;

function reloadPage(reason, time){
  clearTimeout(reloadPageTimer);
  reloadPageTimer = null;
  if (time < 0){
    time = 0;
  }
  var timeRemaining = time / 1000;
  document.getElementById("spanLoginStatus").innerHTML = reason + `[in ${timeRemaining} (s)]`; 
  if (time <= 0){
    doReload();
  } else { 
    reloadPageTimer = setTimeout(reloadPage.bind(null, reason, time - 1000), 1000);
  }
}

function logout(){
  logoutGoogle();
  if (thePage.userRole === "admin") {
    reloadPage("<b>Logging out admin: mandatory page reload. </b>", 3000);
  } else { 
    document.getElementById("spanLoginStatus").innerHTML = `
<b><a href='#' onclick='reloadPage("<b>User requested reload. </b>", 3000);'>Reload page</a> 
for complete logout (when <span style='color:red'>using public computer</span>).</b>`;
  }
  thePage.username = "";
  thePage.userRole = "";
  thePage.flagUserLoggedIn = false;
  thePage.pages.problemPage.flagLoaded = false;
  document.getElementById("inputPassword").value = "";
  document.getElementById("divProblemPageContentContainer").innerHTML = "";
  document.getElementById("divCurrentCourse").innerHTML = "";
  thePage.authenticationToken = "";
  thePage.storeSettingsToCookies();
  thePage.storeSettingsToLocalStorage();
  showLoginCalculatorButtons();
  toggleAccountPanels();
  toggleAdminPanels();
}

function loginTry(){
  submitGET({
    "url": `${pathnames.calculatorAPI}?request=userInfoJSON`,
    callback: loginWithServerCallback,
    progress: "spanProgressReportGeneral"
  });
  startGoogleLogin();
}

function toggleAccountPanels(){
  var accountPanels = document.getElementsByClassName("divAccountPanel");
  for (var counterPanels = 0; counterPanels < accountPanels.length; counterPanels ++) {
    if (thePage.flagUserLoggedIn === true){
      accountPanels[counterPanels].classList.remove("divInvisible");
      accountPanels[counterPanels].classList.add("divVisible");
    } else {
      accountPanels[counterPanels].classList.remove("divVisible");
      accountPanels[counterPanels].classList.add("divInvisible");
    }
  }
}

function toggleAdminPanels(){
  var adminPanels = document.getElementsByClassName("divAdminPanel");
  for (var counterPanels = 0; counterPanels < adminPanels.length; counterPanels ++) {
    if (thePage.userRole === "admin"){
      adminPanels[counterPanels].classList.remove("divInvisible");
      adminPanels[counterPanels].classList.add("divVisible");
    } else {
      adminPanels[counterPanels].classList.remove("divVisible");
      adminPanels[counterPanels].classList.add("divInvisible");
    }
  }
}

function loginWithServerCallback(incomingString, result){
  console.log(incomingString);
  document.getElementById("spanLoginStatus").innerHTML = "";
  var success = false;
  var loginErrorMessage = "";
  try {
    var parsedAuthentication = JSON.parse(incomingString);
    if (parsedAuthentication["status"] === "logged in") {
      success = true;
      thePage.authenticationToken = parsedAuthentication.authenticationToken;
      thePage.username = parsedAuthentication.username;
      thePage.userRole = parsedAuthentication.userRole;
      thePage.flagUserLoggedIn = true;
      document.getElementById("spanUserIdInAccountsPage").innerHTML = thePage.username;
      document.getElementById("inputUsername").value = thePage.username;
      toggleAccountPanels();
      toggleAdminPanels();
      thePage.storeSettingsToCookies();
      hideLoginCalculatorButtons();
      showLogoutButton();
    } else if (parsedAuthentication["status"] === "not logged in"){
      if (parsedAuthentication["error"] !== undefined) {
        loginErrorMessage = parsedAuthentication["error"];
      }
    }
  } catch (e) {
    console.log(`Bad authentication ${e}. Incoming login: ${incomingString}.`);
  }
  if (!success) {
    if (loginErrorMessage!== undefined && loginErrorMessage !== "") {
      document.getElementById("spanLoginStatus").innerHTML = decodeURIComponent(loginErrorMessage);
    }
    thePage.authenticationToken = "";
    thePage.username = "";
    thePage.userRole = "";
    thePage.flagUserLoggedIn = false;
    showLoginCalculatorButtons();
    toggleAccountPanels();
    toggleAdminPanels();
    return;
  }
}

function onGoogleSignIn(googleUser){ 
  var theToken = googleUser.getAuthResponse().id_token;
  thePage.googleToken = theToken;
  thePage.username = "";
  try {
    thePage.googleProfile = window.calculator.jwt.decode(theToken);
    thePage.storeSettingsToCookies();
    thePage.storeSettingsToLocalStorage();
    thePage.showProfilePicture();
    showLogoutButton();
    submitGET({
      url: `${pathnames.calculatorAPI}?request=userInfoJSON`,
      callback: loginWithServerCallback,
      progress: "spanProgressReportGeneral"
    });
  } catch (e) {
    console.log("Error decoding google token: " + e);
  }
}

function startGoogleLogin() {
  //console.log("Got to here");
  if (typeof (gapi) !== "undefined") {
    if (!thePage.pages.login.initialized){
      gapi.signin2.render('divGoogleLoginButton', {
        'scope': 'profile email',
        'onsuccess': onGoogleSignIn,
        'onfailure': null
      });
    }
    gapi.load('auth2', function() {
      //auth2Google = 
      gapi.auth2.init({
        client_id: '538605306594-n43754vb0m48ir84g8vp5uj2u7klern3.apps.googleusercontent.com',
        // Scopes to request in addition to 'profile' and 'email'
        //scope: 'additional_scope'
      });
    });
  }
}

function showLoginCalculatorButtons() {
  document.getElementById("divLoginCalculatorPanel").classList.remove("divInvisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divVisible");
  document.getElementById("divLoginPanelUsernameReport").classList.remove("divVisible");
  document.getElementById("divLoginPanelUsernameReport").classList.add("divInvisible");
}

function hideLoginCalculatorButtons() {
  document.getElementById("divLoginCalculatorPanel").classList.remove("divVisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divInvisible");
  document.getElementById("divLoginPanelUsernameReport").innerHTML = thePage.username;
  document.getElementById("divLoginPanelUsernameReport").classList.remove("divInvisible");
  document.getElementById("divLoginPanelUsernameReport").classList.add("divVisible");
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
  thePage.googleToken = "";
  thePage.googleProfile = {};
  thePage.storeSettingsToCookies();
  thePage.storeSettingsToLocalStorage();
  if (typeof (gapi) !== "undefined") {
    gapi.auth2.getAuthInstance().disconnect();
  }
  hideLogoutButton();
  thePage.hideProfilePicture();
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