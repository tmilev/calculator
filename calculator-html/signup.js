"use strict";
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');
const pathnames = require('./pathnames');

var recaptchaIdForSignUp = null;

function SignUp() {

}
function getRecaptchaId () {
  var localRecaptcha = '6LcSSSAUAAAAAIx541eeGZLoKx8iJehZPGrJkrql';
  var calculatorRecaptcha = "6LcgwR8UAAAAALQq02qZShfA1ZUZvZKRNWJ2CPPf";
  if (
    window.location.hostname == "localhost" || 
    window.location.hostname == "127.0.0.1"
  ) {
    return localRecaptcha;
  }
  if (
    window.location.hostname == "calculator-algebra.org" ||
    window.location.hostname == "www.calculator-algebra.org"
  ) {
    return calculatorRecaptcha;
  }
  throw (`Recaptcha not supported for your hostname: ${window.location.hostname}. Post an issue on our project's github if you need this feature. `)
  return "";
}

SignUp.prototype.signUp = function() {

  if (recaptchaIdForSignUp === null) {
    recaptchaIdForSignUp = grecaptcha.render("recaptchaSignUp", {'sitekey' : getRecaptchaId()});
  }
}

SignUp.prototype.resetRecaptchaOnLoad = function() { 
  grecaptcha.reset();
}

function callbackSignUp(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  try {
    var inputParsed = JSON.parse(decodeURIComponent(input));
    var result = "";
    if (inputParsed.result !== undefined) {
      result += `${inputParsed.result}<hr>`;
    }
    result += `<span style='color:red; font-weight:bold;'>${inputParsed.error}</span>`;
    result += `<hr>${inputParsed.comments}`;
    output.innerHTML = result;
  } catch (e) {
    output.innerHTML = `Result:${input}. Error: ${e}. ${input}`;    
  }
}

SignUp.prototype.submitSignUpInfo = function () {  
  if (grecaptcha === undefined || grecaptcha === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  var desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
  var desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForSignUp').value);
  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.signUp}&desiredUsername=${desiredUsernameEncoded}&`;
  theURL += `email=${desiredEmailEncoded}&`;
  var theToken = grecaptcha.getResponse(recaptchaIdForSignUp);
  if (theToken === '' || theToken === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>Please don't forget to solve the captcha. </b></span>";
    return false;
  }
  recaptchaIdForSignUp = null;
  theURL += `${pathnames.urlFields.recaptchaToken}=${encodeURIComponent(theToken)}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: callbackSignUp,
    result: "signUpResultReport",
    progress: ids.domElements.spanProgressReportGeneral
  });
}

var signUp = new SignUp();

module.exports = {
  signUp,
}
