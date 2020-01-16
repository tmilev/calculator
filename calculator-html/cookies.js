"use strict"
function getCookie(cookieName) {
  var cookie = document.cookie.split(";");
  for (var i = 0; i < cookie.length; i ++) {
    var x = cookie[i].substr(0, cookie[i].indexOf("="));
    var y = cookie[i].substr(cookie[i].indexOf("=") + 1);
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
  cookieValue += "; SameSite=Strict";
  document.cookie = `${theName}=${cookieValue}; Path=/;`;
}

module.exports = {
  setCookie,
  getCookie,
};