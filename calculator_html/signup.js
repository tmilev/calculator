"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");

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

class SignUp {
  constructor() {
    this.recaptchaIdForSignUp = null;
    this.grecaptcha = null;
    this.buttonsInitialized = false;
    /** @type {HTMLButtonElement|null} */
    this.submitSignUpButton = null;
  }

  signUp() {
    this.initialize();
    this.writeDebugLoginStatus("");
    if (this.grecaptcha !== null && this.grecaptcha !== undefined) {
      if (this.recaptchaIdForSignUp === null) {
        this.recaptchaIdForSignUp = this.grecaptcha.render(
          ids.domElements.pages.signUp.recaptchaSignUp, {
          'sitekey': getRecaptchaId()
        });
      }
    }
  }

  initialize() {
    if (this.grecaptcha !== null && this.grecaptcha !== undefined) {
      // Already initialized.
      return;
    }
    if (
      window.calculator.grecaptcha !== null &&
      window.calculator.grecaptcha !== undefined
    ) {
      this.grecaptcha = window.calculator.grecaptcha;
    }
    this.initializeButtons();
  }

  initializeButtons() {
    if (this.buttonsInitialized === true) {
      return;
    }
    this.initializedButtons = true;
    this.submitSignUpButton =
      document.getElementById(
        ids.domElements.pages.signUp.buttonSignUpStart
      );
    if (this.submitSignUpButton === null) {
      console.log("Sign up page initialization skipped.");
      return;
    }
    this.submitSignUpButton.addEventListener('click', () => {
      this.submitSignUpInfo();
    });
  }

  writeDebugLoginStatus(
    /** @type {string} */
    extraMessage
  ) {
    let recaptchaElement = document.getElementById(
      ids.domElements.pages.signUp.signUpResultReport
    );
    if (this.grecaptcha === null || this.grecaptcha === undefined) {
      miscellaneous.writeHTML(
        recaptchaElement,
        `<b style='color:red'>Missing google (re)captcha: no Internet?</b> ` +
        `<b style='color:orange'>${extraMessage}</b>`,
      );
      return;
    }
    if (window.calculator.mainPage.user.debugLoginIsOn()) {
      miscellaneous.writeHTML(
        recaptchaElement,
        "<b style='color:blue'>Debugging login, recaptcha is off.</b>",
      );
      return;
    }
  }

  submitSignUpInfo() {
    let debugLogin = window.calculator.mainPage.user.debugLoginIsOn();

    let token = "";
    if (this.grecaptcha === undefined || this.grecaptcha === null) {
      this.writeDebugLoginStatus("Can't submit.");
      if (!debugLogin) {
        return false;
      }
    } else {
      token = this.grecaptcha.getResponse(this.recaptchaIdForSignUp);
    }
    let desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
    let desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForSignUp').value);
    let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.signUp}&desiredUsername=${desiredUsernameEncoded}&`;
    url += `email=${desiredEmailEncoded}&`;
    if (token === '' || token === null) {
      if (!debugLogin) {
        let element = document.getElementById(
          ids.domElements.pages.signUp.signUpResult
        );
        miscellaneous.writeHTML(
          element,
          "<b style ='color:red'>Please don't forget to solve the captcha. </b>",
        );
        return false;
      }
      let element = document.getElementById(
        ids.domElements.pages.signUp.signUpResult
      );
      miscellaneous.writeHTML(
        element,
        "<b style='color:red'>Debug login: recaptcha ignored.</b>",
      );
    }
    this.recaptchaIdForSignUp = null;
    url += `${pathnames.urlFields.recaptchaToken}=${encodeURIComponent(token)}&`;
    submitRequests.submitGET({
      url: url,
      callback: (input) => {
        this.callbackSignUp(input);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }

  callbackSignUp(input) {
    let outputStatus = document.getElementById(ids.domElements.pages.login.signUpResultReport);
    let output = document.getElementById(ids.domElements.pages.login.signUpResult);
    try {
      let inputParsed = miscellaneous.jsonUnescapeParse(input);
      miscellaneous.writeHtmlFromCommentsAndErrors(inputParsed, outputStatus);
      let resultHtml = inputParsed[pathnames.urlFields.result.resultHtml];
      if (resultHtml !== undefined) {
        miscellaneous.writeHTML(output, resultHtml);
      }
    } catch (e) {
      output.textContent = `Result: ${input}. Error: ${e}.`;
    }
  }
}

let signUp = new SignUp();
signUp.initialize();

module.exports = {
  getRecaptchaId,
  signUp,
};
