"use strict";

function submitForgotPassword() {  
  if (grecaptcha === undefined || grecaptcha === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  var desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
  var desiredEmailEncoded = encodeURIComponent(document.getElementById('email').value);
  var theURL = `${pathnames.calculatorAPI}?request=signUp&desiredUsername=${desiredUsernameEncoded}&`;
  theURL += `email=${desiredEmailEncoded}&`;
  var theToken = grecaptcha.getResponse();
  if (theToken === '' || theToken === null)  { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>Please don't forget to solve the captcha. </b></span>";
    return false;
  }
  theURL += `recaptchaToken=${encodeURIComponent(theToken)}&`;
  submitGET({
    url: theURL,
    callback: callbackSignUp,
    result: "signUpResultReport",
    progress: "spanProgressReportGeneral"
  });
}
