"use strict";

//var auth2Google = null;

function selectLoginPage(){
  startGoogleLogin();
}

function onGoogleSignIn(googleUser){ 
  var theToken = googleUser.getAuthResponse().id_token;
  thePage.googleToken = theToken;
  try {
    thePage.googleProfile = window.calculator.jwt.decode(theToken);
    thePage.storeSettingsToCookies();
    thePage.storeSettingsToLocalStorage();
    thePage.updateProfilePic();
    showGoogleLogoutButton();
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

function showGoogleLogoutButton(){
  for (;;){
    var theLogoutLinks = document.getElementsByClassName("linkLogoutInactive");
    if (theLogoutLinks.length === 0){
      break;
    }
    theLogoutLinks[0].className = "linkLogoutActive";
  }
}

function hideGoogleLogoutButton(){
  for (;;){
    var theLogoutLinks = document.getElementsByClassName("linkLogoutActive");
    if (theLogoutLinks.length === 0){
      break;
    }
    theLogoutLinks[0].className = "linkLogoutInactive";
  }
}

function logoutGoogle(){
  thePage.googleToken = "";
  thePage.googleProfile = {};
  thePage.storeSettingsToCookies();
  thePage.storeSettingsToLocalStorage();
  gapi.auth2.getAuthInstance().disconnect();
  hideGoogleLogoutButton();
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