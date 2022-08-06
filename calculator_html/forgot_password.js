"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const miscellaneous = require("./miscellaneous_frontend");
const signUp = require('./signup');

function callbackForgotLogin(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  try {
    let parsed = miscellaneous.jsonUnescapeParse(input);
    miscellaneous.writeHtmlFromCommentsAndErrors(parsed, output)
  } catch (e) {
    output.textContent = e + " Received input: " +input;
    
  }
}

class ForgotLogin {
  constructor() {
    this.recaptchaIdForForgotLogin = null;
    this.grecaptcha = null;
  }

  initialize() {
    if (this.grecaptcha !== null && this.grecaptcha !== undefined) {
      return;
    }
    if (window.calculator.grecaptcha !== null && window.calculator.grecaptcha !== undefined) {
      this.grecaptcha = window.calculator.grecaptcha;
    }
  }

  writeDebugLoginStatus(
    /** @type{string} */ extraMessage
  ) {
    let recaptchaElement = document.getElementById(
      ids.domElements.pages.forgotLogin.forgotLoginResult
    );
    if (window.calculator.mainPage.user.debugLoginIsOn()) {
      recaptchaElement.innerHTML = "<b style='color:blue'>Debugging login, recaptcha is off.</b>";
      return;
    }    
    if (this.grecaptcha === undefined || this.grecaptcha === null) {
      recaptchaElement.innerHTML = `<b style='color:red'>Missing google (re)captcha: no Internet?</b> `+
        `<b style='color:orange'>${extraMessage}</b>`;
      return;
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
    return window.calculator.mainPage.user.debugLoginIsOn();
  }

  submitForgotPassword() {
    let debugLogin = this.debugLogin();
    let desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForForgotLogin').value);
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.forgotLogin}&`
    url += `${pathnames.urlFields.email}=${desiredEmailEncoded}&`;
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
        let reportElement = document.getElementById(ids.domElements.pages.forgotLogin.forgotLoginResultReport);
        reportElement.innerHTML = "<b style='color:blue'>Debug login: recaptcha ignored.</b>";
      } else {
        document.getElementById(
          ids.domElements.pages.forgotLogin.forgotLoginResult
        ).innerHTML = "<b style ='color:red'>Please don't forget to solve the captcha.</b>";
        return false;
      }
    }
    url += `${pathnames.urlFields.recaptchaToken}=${encodeURIComponent(token)}&`;
    submitRequests.submitGET({
      url: url,
      callback: callbackForgotLogin,
      result: "forgotLoginResult",
      progress: ids.domElements.spanProgressReportGeneral
    });
  }
}

var forgotLogin = new ForgotLogin();

module.exports = {
  forgotLogin
};