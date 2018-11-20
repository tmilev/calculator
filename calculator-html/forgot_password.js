"use strict";
const ids = require('./ids_dom_elements');
const pathnames = require('./pathnames');
const submitRequests = require('./submit_requests');
const signUp = require('./signup');
function ForgotLogin() {

}

function callbackForgotLogin(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  output.innerHTML = input;
}

var recaptchaIdForForgotLogin = null;
ForgotLogin.prototype.forgotLoginPage = function() {
  var thePage = window.calculator.mainPage;
  if (recaptchaIdForForgotLogin === null) {
    recaptchaIdForForgotLogin = grecaptcha.render("recaptchaForgotPassword", {sitekey : signUp.getRecaptchaId()});
  }
  thePage.selectPage(thePage.pages.forgotLogin.name);
}

ForgotLogin.prototype.submitForgotPassword = function() {  
  if (grecaptcha === undefined || grecaptcha === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  var desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForForgotLogin').value);
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.forgotLogin}&`
  theURL += `${pathnames.urlFields.email}=${desiredEmailEncoded}&`;
  var theToken = grecaptcha.getResponse(recaptchaIdForForgotLogin);
  if (theToken === '' || theToken === null)  { 
    document.getElementById('forgotLoginResult').innerHTML = "<span style ='color:red'><b>Please don't forget to solve the captcha. </b></span>";
    return false;
  }
  theURL += `${pathnames.urlFields.recaptchaToken}=${encodeURIComponent(theToken)}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: callbackForgotLogin,
    result: "forgotLoginResult",
    progress: ids.domElements.spanProgressReportGeneral
  });
}

var forgotLogin = new ForgotLogin();

module.exports = {
  forgotLogin
}