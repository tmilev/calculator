function getCookie(c_name)
{ VPFcookie = document.cookie.split(";");
  for (var i = 0; i < VPFcookie.length; i ++)
  { x = VPFcookie[i].substr(0,VPFcookie[i].indexOf("="));
    y = VPFcookie[i].substr(VPFcookie[i].indexOf("=") + 1);
    x = x.replace(/^\s+|\s+$/g,"");
    if (x === c_name)
      return unescape(y);
  }
  return "";
}

function addCookie(theName, theValue, exdays, secure)
{ exdate = new Date();
  exdate.setDate(exdate.getDate() + exdays);
  c_value = escape(theValue) + ((exdays===null) ? "" : "; expires=" + exdate.toUTCString());
  if (secure)
    c_value += "; Secure";
  document.cookie = theName + "=" + c_value + "; Path=/;";
}
