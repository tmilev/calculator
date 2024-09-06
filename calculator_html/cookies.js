"use strict"
function getCookie(cookieName) {
  let cookie = document.cookie.split(";");
  const cookieNameEncoded = encodeURIComponent(cookieName);
  for (let i = 0; i < cookie.length; i++) {
    let x = cookie[i].substring(0, cookie[i].indexOf("="));
    let y = cookie[i].substring(cookie[i].indexOf("=") + 1);
    x = encodeURIComponent(x);
    if (x === cookieNameEncoded) {
      return decodeURIComponent(y);
    }
  }
  return "";
}

function setCookie(name, value, expiryDays, secure) {
  let expiryDate = new Date();
  expiryDate.setDate(expiryDate.getDate() + expiryDays);
  if (value === true) {
    value = "true";
  }
  if (value === false) {
    value = "false";
  }
  let expiration = "";
  if (expiryDate !== null && expiryDate !== undefined) {
    expiration = "; expires =" + expiryDate.toUTCString();
  }
  let cookieValue = encodeURIComponent(value) + expiration;
  if (secure === undefined || secure === null || secure == "") {
    secure = true;
  }
  if (secure) {
    cookieValue += "; Secure";
  }
  cookieValue += "; SameSite=Strict";
  const nameEncoded = encodeURIComponent(name);
  document.cookie = `${nameEncoded}=${cookieValue}; Path=/;`;
}

module.exports = {
  setCookie,
  getCookie,
};