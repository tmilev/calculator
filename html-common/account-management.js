function submitChangePassRequest(spanVerificationID)
{ var spanVerification = document.getElementById(spanVerificationID);
  var inputUser = document.getElementById("username");
  var inputActivationToken = document.getElementById("activationToken");
  var hasActivationToken=true;
  if (inputActivationToken===null || inputActivationToken===undefined)
    hasActivationToken=false;
  var inputPassword = document.getElementById("password");
  var inputNewPassword = document.getElementById("newPassword");
  var inputReenteredPassword = document.getElementById("reenteredPassword");
  var inputEmail = document.getElementById("emailInputID");
  var params = "request=changePassword";
  params+="&username=" + encodeURIComponent(inputUser.value);
  if (hasActivationToken)
    params+= "&activationToken="+encodeURIComponent(inputActivationToken.value);
  else
    params+="&password="+encodeURIComponent(inputPassword.value);
  params+= "&newPassword="+encodeURIComponent(inputNewPassword.value);
  params+="&reenteredPassword="+encodeURIComponent(inputReenteredPassword.value);
  params+="&email="+encodeURIComponent(inputEmail.value);
  var https = new XMLHttpRequest();
  https.open("POST", "./calculator", true);
  https.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  https.onload = function() {
    spanVerification.innerHTML=https.responseText;
  }
  https.send(params);
}
