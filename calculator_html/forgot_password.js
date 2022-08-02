"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const submitRequests = require("./submit_requests");
const signUp = require('./signup');

function callbackForgotLogin(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  output.innerHTML = input;
}

// Ensure grecaptcha is defined.
var grecaptcha;

class ForgotLogin {
  constructor() {
    this.recaptchaIdForForgotLogin = null;
  }

  forgotLoginPage() {
    if (grecaptcha === undefined || grecaptcha === null) {
      document.getElementById(
        ids.domElements.pages.forgotLogin.forgotLoginResult
      ).innerHTML = "<b style ='color:red'>The google captcha script appears to be missing (no Internet?). </b>";
      return;
    }
    if (this.recaptchaIdForForgotLogin === null) {
      this.recaptchaIdForForgotLogin = grecaptcha.render("recaptchaForgotPassword", { sitekey: signUp.getRecaptchaId() });
    }
  }

  submitForgotPassword() {
    let debugLogin = window.calculator.mainPage.user.debugLoginIsOn();
    let desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForForgotLogin').value);
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.forgotLogin}&`
    url += `${pathnames.urlFields.email}=${desiredEmailEncoded}&`;
    let token = "";
    if (grecaptcha === undefined || grecaptcha === null) {
      document.getElementById(
        ids.domElements.pages.forgotLogin.forgotLoginResult
      ).innerHTML = "<b style ='color:red'>The google captcha script appears to be missing (no Internet?). </b>";
      if (!debugLogin) {
        return false;
      }
    } else {
      token = grecaptcha.getResponse(this.recaptchaIdForForgotLogin);
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