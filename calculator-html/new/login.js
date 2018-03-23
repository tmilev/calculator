"use strict";

//var auth2Google = null;

function loginCalculator(){
  submitGET({
    "url": `${thePage.calculator}?request=userInfoJSON&password=${document.getElementById("password").value}&username=${document.getElementById("username").value}`,
    callback: loginWithServerCallback,
    progress: "spanProgressReportGeneral"
  });
}

function logout(){
  logoutGoogle();
  thePage.username = "";
  thePage.authenticationToken = "";
  thePage.storeSettingsToCookies();
  thePage.storeSettingsToLocalStorage();
  showLoginCalculatorButtons();
}

function selectLoginPage(){
  startGoogleLogin();
}

function loginWithServerCallback(incomingString, result){
  if (incomingString === "not logged in"){
    thePage.authenticationToken = "";
    thePage.username = "";
    showLoginCalculatorButtons();
    return;
  }
  try {
    var parsedAuthentication = JSON.parse(incomingString);
    thePage.authenticationToken = parsedAuthentication.authenticationToken;
    thePage.username = parsedAuthentication.username;
    thePage.storeSettingsToCookies();
    hideLoginCalculatorButtons();
  } catch (e){
    console.log("Bad authentication" + e);
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
    showGoogleLogoutButton();
    submitGET({
      "url": `${thePage.calculator}?request=userInfoJSON`,
      callback: loginWithServerCallback,
      progress: "spanProgressReportGeneral"
    });
  } catch (e) {
    console.log("Error decoding google token: " + e);
  }
}

function startGoogleLogin() {
  //console.log("Got to here");
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

function showLoginCalculatorButtons(){
  document.getElementById("divLoginCalculatorPanel").classList.remove("divInvisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divVisible");
  document.getElementById("divLoginPanelUsernameReport").classList.remove("divVisible");
  document.getElementById("divLoginPanelUsernameReport").classList.add("divInvisible");
}

function hideLoginCalculatorButtons(){
  document.getElementById("divLoginCalculatorPanel").classList.remove("divVisible");
  document.getElementById("divLoginCalculatorPanel").classList.add("divInvisible");
  document.getElementById("divLoginPanelUsernameReport").innerHTML = thePage.username;
  document.getElementById("divLoginPanelUsernameReport").classList.remove("divInvisible");
  document.getElementById("divLoginPanelUsernameReport").classList.add("divVisible");
}

function showGoogleLogoutButton(){
  for (;;){
    var theLogoutLinks = document.getElementsByClassName("linkLogoutInactive");
    if (theLogoutLinks.length === 0){
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutActive");
    theLogoutLinks[0].classList.remove("linkLogoutInactive");
  }
}

function hideGoogleLogoutButton(){
  for (;;){
    var theLogoutLinks = document.getElementsByClassName("linkLogoutActive");
    if (theLogoutLinks.length === 0){
      break;
    }
    theLogoutLinks[0].classList.add("linkLogoutInactive");
    theLogoutLinks[0].classList.remove("linkLogoutActive");
  }
}

function logoutGoogle(){
  thePage.googleToken = "";
  thePage.googleProfile = {};
  thePage.storeSettingsToCookies();
  thePage.storeSettingsToLocalStorage();
  gapi.auth2.getAuthInstance().disconnect();
  hideGoogleLogoutButton();
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

function init(){
  logoutRequestFromUrl = getQueryVariable("logout");
  locationRequestFromUrl = getQueryVariable("location");
  if (logoutRequestFromUrl === "true"){
    logout();
  }
}