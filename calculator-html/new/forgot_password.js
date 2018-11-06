"use strict";

function callbackForgotLogin(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  output.innerHTML = input;
}

var recaptchaIdForForgotLogin = null;
function forgotLoginPage() {
  if (recaptchaIdForForgotLogin === null) {
    recaptchaIdForForgotLogin = grecaptcha.render("recaptchaForgotPassword", {sitekey : "6LcSSSAUAAAAAIx541eeGZLoKx8iJehZPGrJkrql"});
  }
  thePage.selectPage(thePage.pages.forgotLogin.name);
}

function submitForgotPassword() {  
  if (grecaptcha === undefined || grecaptcha === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  var desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForForgotLogin').value);
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?request=forgotLogin&email=${desiredEmailEncoded}&`;
  var theToken = grecaptcha.getResponse(recaptchaIdForForgotLogin);
  if (theToken === '' || theToken === null)  { 
    document.getElementById('forgotLoginResult').innerHTML = "<span style ='color:red'><b>Please don't forget to solve the captcha. </b></span>";
    return false;
  }
  theURL += `recaptchaToken=${encodeURIComponent(theToken)}&`;
  submitRequests.submitGET({
    url: theURL,
    callback: callbackForgotLogin,
    result: "forgotLoginResult",
    progress: "spanProgressReportGeneral"
  });
}
