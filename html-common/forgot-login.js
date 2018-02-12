function resetRecaptchaOnLoad()
{ grecaptcha.reset();
}

function submitForgotLogin()
{ var theInput = "request=forgotLogin&";
  theInput += "email=" + encodeURIComponent(document.getElementById('email').value) + "&";
  if (grecaptcha===undefined || grecaptcha===null)
  { document.getElementById('forgotLoginResult').innerHTML =
    "<span style='color:red'><b>The google captcha script appears to be missing (no Internet?). </b></span>";
    return false;
  }
  if (document.getElementById('email').value==="")
  { document.getElementById('forgotLoginResult').innerHTML = "<span style='color:red'><b>Please enter an email address. </b></span>";
    return false;
  }
  var theToken = grecaptcha.getResponse();
  if (theToken==='' || theToken===null)
  { document.getElementById('forgotLoginResult').innerHTML = "<span style='color:red'><b>Please don't forget to solve the captcha. </b></span>";
    return false;
  }
  theInput += "recaptchaToken=" + encodeURIComponent(theToken) + "&";
    submitStringCalculatorArgument(theInput, 'forgotLoginResult', resetRecaptchaOnLoad, 'forgotLoginResultReport');
}
