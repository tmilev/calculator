"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscallaneous = require("./miscellaneous_frontend");

function getRecaptchaId() {
  let localRecaptcha = '6LcSSSAUAAAAAIx541eeGZLoKx8iJehZPGrJkrql';
  let calculatorRecaptcha = "6LcgwR8UAAAAALQq02qZShfA1ZUZvZKRNWJ2CPPf";
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

class SignUp {
  constructor() {
    this.recaptchaIdForSignUp = null;
  }
  signUp() {
    if (this.recaptchaIdForSignUp === null) {
      this.recaptchaIdForSignUp = grecaptcha.render("recaptchaSignUp", { 'sitekey': getRecaptchaId() });
    }
  }

  resetRecaptchaOnLoad() {
    grecaptcha.reset();
  }

  submitSignUpInfo() {
    if (grecaptcha === undefined || grecaptcha === null) {
      document.getElementById('signUpResult').innerHTML = "<b style ='color:red'>The google captcha script appears to be missing (no Internet?). </b>";
      return false;
    }
    var desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
    var desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForSignUp').value);
    var url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.signUp}&desiredUsername=${desiredUsernameEncoded}&`;
    url += `email=${desiredEmailEncoded}&`;
    var token = grecaptcha.getResponse(this.recaptchaIdForSignUp);
    if (token === '' || token === null) {
      document.getElementById('signUpResult').innerHTML = "<b style ='color:red'>Please don't forget to solve the captcha. </b>";
      return false;
    }
    this.recaptchaIdForSignUp = null;
    url += `${pathnames.urlFields.recaptchaToken}=${encodeURIComponent(token)}&`;
    submitRequests.submitGET({
      url: url,
      callback: callbackSignUp,
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

}

function callbackSignUp(input) {
  let outputStatus = document.getElementById(ids.domElements.pages.login.signUpResultReport);
  let output = document.getElementById(ids.domElements.pages.login.signUpResult);
  try {
    let inputParsed = JSON.parse(decodeURIComponent(input));
    miscallaneous.writeHtmlElementsFromCommentsAndErrors(inputParsed, outputStatus);
    if (inputParsed.result !== undefined) {
      output.textContent = inputParsed.result;
    }
  } catch (e) {
    output.textContent = `Result: ${input}. Error: ${e}.`;
  }
}

var signUp = new SignUp();

module.exports = {
  getRecaptchaId,
  signUp,
};
