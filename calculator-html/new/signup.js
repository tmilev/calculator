"use strict";

var recaptchaIdForSignUp = null;

function signUp() {
  if (recaptchaIdForSignUp === null) {
    recaptchaIdForSignUp = grecaptcha.render("recaptchaSignUp", {'sitekey' : '6LcSSSAUAAAAAIx541eeGZLoKx8iJehZPGrJkrql'});
  }
  thePage.selectPage(thePage.pages.signUp.name);
}

function resetRecaptchaOnLoad() { 
  grecaptcha.reset();
}

function callbackSignUp(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  try {
    var inputParsed = JSON.parse(decodeURIComponent(input));
    var result = "";
    if (inputParsed.result !== undefined) {
      result += `${inputParsed.result}<hr>`;
    }
    result += `<span style='color:red; font-weight:bold;'>${inputParsed.error}</span>`;
    result += `<hr>${inputParsed.comments}`;
    output.innerHTML = result;
  } catch (e) {
    output.innerHTML = `Result:${input}. Error: ${e}. ${input}`;    
  }
}

function submitSignUpInfo() {  
  if (grecaptcha === undefined || grecaptcha === null) { 
    document.getElementById('signUpResult').innerHTML = "<span style ='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  var desiredUsernameEncoded = encodeURIComponent(document.getElementById('desiredUsername').value);
  var desiredEmailEncoded = encodeURIComponent(document.getElementById('emailForSignUp').value);
  var theURL = `${pathnames.calculatorAPI}?request=signUp&desiredUsername=${desiredUsernameEncoded}&`;
  theURL += `email=${desiredEmailEncoded}&`;
  var theToken = grecaptcha.getResponse(recaptchaIdForSignUp);
  if (theToken === '' || theToken === null) { 
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
