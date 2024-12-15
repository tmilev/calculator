"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const miscellaneous = require("./miscellaneous_frontend");
const signUp = require('./signup');
const globalUser = require('./user').globalUser;

class ForgotLogin {
  constructor() {
    this.recaptchaIdForForgotLogin = null;
    this.grecaptcha = null;
    /** @type {HTMLButtonElement} */
    this.sendRecoveryEmail = document.getElementById(
      ids.domElements.pages.forgotLogin.buttonSendRecoveryEmail
    );
    if (this.sendRecoveryEmail !== null) {
      this.sendRecoveryEmail.addEventListener("click", () => {
        this.submitForgotPassword();
      });
    }
  }

  initialize() {
    if (this.grecaptcha !== null && this.grecaptcha !== undefined) {
      return;
    }
    if (
      window.calculator.grecaptcha !== null &&
      window.calculator.grecaptcha !== undefined
    ) {
      this.grecaptcha = window.calculator.grecaptcha;
    }
  }

  writeDebugLoginStatus(
    /** @type {string} */ extraMessage
  ) {
    let recaptchaElement = document.getElementById(
      ids.domElements.pages.forgotLogin.forgotLoginResult
    );
    if (globalUser.debugLoginIsOn()) {
      miscellaneous.writeHTML(
        recaptchaElement,
        "<b style='color:blue'>Debugging login, recaptcha is off.</b>"
      );
      return;
    }
    if (this.grecaptcha === undefined || this.grecaptcha === null) {
      miscellaneous.writeHTML(
        recaptchaElement,
        `<b style='color:red'>Missing google (re)captcha: no Internet?</b> ` +
        `<b style='color:orange'>${extraMessage}</b>`,
      );
      return;
    }
  }

  callbackForgotLogin(input) {
    const output = document.getElementById(
      ids.domElements.pages.forgotLogin.forgotLoginResult
    );
    try {
      let parsed = miscellaneous.jsonUnescapeParse(input);
      miscellaneous.writeHtmlFromCommentsAndErrors(parsed, output)
    } catch (e) {
      output.textContent = e + " Received input: " + input;
    }
  }

  forgotLoginPage() {
    this.initialize();
    this.writeDebugLoginStatus("");
    if (this.grecaptcha === null || this.grecaptcha === undefined) {
      return;
    }
    if (this.recaptchaIdForForgotLogin === null && !this.debugLogin()) {
      this.recaptchaIdForForgotLogin = this.grecaptcha.render(
        "recaptchaForgotPassword", {
        sitekey: signUp.getRecaptchaId()
      });
    }
  }

  debugLogin() {
    return globalUser.debugLoginIsOn();
  }

  submitForgotPassword() {
    let debugLogin = this.debugLogin();
    let desiredEmailEncoded = encodeURIComponent(
      document.getElementById('emailForForgotLogin').value
    );
    let url = "";
    const urlFields = pathnames.urlFields;
    url += `${pathnames.urls.calculatorAPI}?`;
    url += `${urlFields.request}=${urlFields.requests.forgotLogin}&`;
    url += `${urlFields.email}=${desiredEmailEncoded}&`;
    let token = "";
    if (this.grecaptcha === undefined || this.grecaptcha === null) {
      this.writeDebugLoginStatus("Can't submit.");
      if (!debugLogin) {
        return false;
      }
    } else if (!debugLogin) {
      token = this.grecaptcha.getResponse(this.recaptchaIdForForgotLogin);
    }

    if (token === '' || token === null) {
      if (debugLogin) {
        let reportElement = document.getElementById(
          ids.domElements.pages.forgotLogin.forgotLoginResultReport,
        );
        miscellaneous.writeHTML(
          reportElement,
          "<b style='color:blue'>Debug login: recaptcha ignored.</b>",
        );
      } else {
        let element = document.getElementById(
          ids.domElements.pages.forgotLogin.forgotLoginResult
        );
        miscellaneous.writeHTML(
          element,
          "<b style='color:red'>Please don't forget to solve the captcha.</b>",
        );
        return false;
      }
    }
    url += `${urlFields.recaptchaToken}=${encodeURIComponent(token)}&`;
    submitRequests.submitGET({
      url: url,
      callback: (result) => {
        this.callbackForgotLogin(result);
      },
      progress: ids.domElements.spanProgressReportGeneral
    });
  }
}

let forgotLogin = new ForgotLogin();

module.exports = {
  forgotLogin
};