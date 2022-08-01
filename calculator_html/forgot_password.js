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

class ForgotLogin {
  constructor() {
    this.recaptchaIdForForgotLogin = null;
  }

  forgotLoginPage() {
    if (this.recaptchaIdForForgotLogin === null) {
      this.recaptchaIdForForgotLogin = grecaptcha.render("recaptchaForgotPassword", { sitekey: signUp.getRecaptchaId() });
    }
  }

  submitForgotPassword() {
    let page = window.calculator.mainPage;
    if (grecaptcha === undefined || grecaptcha === null) {
      document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
      return false;
    }
    let desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForForgotLogin').value);
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.forgotLogin}&`
    url += `${pathnames.urlFields.email}=${desiredEmailEncoded}&`;
    let token = grecaptcha.getResponse(this.recaptchaIdForForgotLogin);
    
    if (token === '' || token === null) {
      if (page.user.flagDatabaseInactiveEveryoneIsAdmin === true) {
        let reportElement = document.getElementById(ids.domElements.pages.forgotLogin.forgotLoginResultReport);
        reportElement.innerHTML = "<b>Everyone's admin: ignore recaptcha</b>";
      } else {
        document.getElementById('forgotLoginResult').innerHTML = "<b style ='color:red'>Please don't forget to solve the captcha. </b>";
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