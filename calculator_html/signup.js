"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const globalUser = require('./user').globalUser;

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
    /** @type {HTMLInputElement} */
    this.doSendEmailCheckbox = document.getElementById(
      ids.domElements.pages.signUp.inputSignUpDoSendEmail
    );
  }

  signUp() {
    this.initialize();
    if (globalUser.debugLoginIsOn()) {
      document.getElementById(
        ids.domElements.pages.signUp.inputSignUpDoSendEmailContainer
      ).style.display = '';
    }
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
    if (globalUser.debugLoginIsOn()) {
      miscellaneous.writeHTML(
        recaptchaElement,
        "<b style='color:blue'>Debugging login, recaptcha is off.</b>",
      );
      return;
    }
  }

  submitSignUpInfo() {
    let debugLogin = globalUser.debugLoginIsOn();

    let token = "";
    if (this.grecaptcha === undefined || this.grecaptcha === null) {
      this.writeDebugLoginStatus("Can't submit.");
      if (!debugLogin) {
        return false;
      }
    } else if (this.recaptchaIdForSignUp !== null) {
      token = this.grecaptcha.getResponse(this.recaptchaIdForSignUp);
    }
    let signUpResultElement = document.getElementById(
      ids.domElements.pages.signUp.signUpResult
    );
    signUpResultElement.textContent = "";
    let desiredUsernameEncoded = encodeURIComponent(
      document.getElementById('desiredUsername').value
    );
    let desiredEmailEncoded = encodeURIComponent(
      document.getElementById('emailForSignUp').value
    );
    let fields = pathnames.urlFields;
    let url = `${pathnames.urls.calculatorAPI}?`;
    url += `${fields.request}=${fields.signUp}&`;
    url += `desiredUsername=${desiredUsernameEncoded}&`;
    url += `email=${desiredEmailEncoded}&`;
    if (token === '' || token === null) {
      if (!debugLogin) {
        const errorElement = document.createElement("b");
        errorElement.textContent = "Please don't forget to solve the captcha.";
        errorElement.style.color = "red";
        signUpResultElement.appendChild(errorElement);
        errorElement.style.backgroundColor = "orange";
        errorElement.style.transition = "all 1s";
        setTimeout(() => {
          errorElement.style.backgroundColor = "";
        }, 0);
        return false;
      }
      miscellaneous.writeHTML(
        signUpResultElement,
        "<b style='color:red'>Debug login: recaptcha ignored.</b>",
      );
    }
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
      if (this.grecaptcha !== undefined && this.grecaptcha !== null) {
        this.grecaptcha.reset();
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
