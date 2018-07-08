"use strict"
function getCookie(cookieName) { 
  var VPFcookie = document.cookie.split(";");
  for (var i = 0; i < VPFcookie.length; i ++) { 
    var x = VPFcookie[i].substr(0, VPFcookie[i].indexOf("="));
    var y = VPFcookie[i].substr(VPFcookie[i].indexOf("=") + 1);
    x = x.replace(/^\s+|\s+$/g,"");
    if (x === cookieName) {
      return unescape(y);
    }
  }
  return "";
}

function setCookie(theName, theValue, expiryDays, secure) { 
  var expiryDate = new Date();
  expiryDate.setDate(expiryDate.getDate() + expiryDays);
  if (theValue === true) {
    theValue = "true";
  }
  if (theValue === false) {
    theValue = "false";
  }
  var cookieValue = escape(theValue) + ((expiryDays === null) ? "" : "; expires =" + expiryDate.toUTCString());
  if (secure === undefined || secure === null || secure == "") {
    secure = true;
  }
  if (secure) {
    cookieValue += "; Secure";
  }
  document.cookie = `${theName}=${cookieValue}; Path=/;`;
}
