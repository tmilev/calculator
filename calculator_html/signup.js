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
}

// Ensure that grecaptcha is at least undefined.
var grecaptcha;

class SignUp {
  constructor() {
    this.recaptchaIdForSignUp = null;
  }
  signUp() {
    if (grecaptcha === null || grecaptcha === undefined) {
      document.getElementById(
        ids.domElements.pages.signUp.recaptchaSignUp
      ).innerHTML = "<b style='color:red'>Missing google (re)captcha: no Internet?</b>";
      return;
    }
    if (this.recaptchaIdForSignUp === null) {
      this.recaptchaIdForSignUp = grecaptcha.render(
        ids.domElements.pages.signUp.recaptchaSignUp, {
        'sitekey': getRecaptchaId()
      });
    }
  }

  resetRecaptchaOnLoad() {
    grecaptcha.reset();
  }

  submitSignUpInfo() {
    let debugLogin = window.calculator.mainPage.user.debugLoginIsOn();
    let token = "";
    if (grecaptcha === undefined || grecaptcha === null) {
      document.getElementById(
        ids.domElements.pages.signUp.signUpResult
      ).innerHTML = "<b style ='color:red'>Missing google (re)captcha: no Internet?</b>";
      if (!debugLogin) {
        return false;
      }
    } else {
      token = grecaptcha.getResponse(this.recaptchaIdForSignUp);
    }
    let desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
    let desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForSignUp').value);
    let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.signUp}&desiredUsername=${desiredUsernameEncoded}&`;
    url += `email=${desiredEmailEncoded}&`;
    if (token === '' || token === null) {
      if (!debugLogin) {
        document.getElementById(
          ids.domElements.pages.signUp.signUpResult
        ).innerHTML = "<b style ='color:red'>Please don't forget to solve the captcha. </b>";
        return false;
      } 
      document.getElementById(
        ids.domElements.pages.signUp.signUpResult
      ).innerHTML = "<b style='color:red'>Debug login: recaptcha ignored.</b>";
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

let signUp = new SignUp();

module.exports = {
  getRecaptchaId,
  signUp,
};
