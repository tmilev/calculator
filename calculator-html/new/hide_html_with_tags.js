"use strict";

function switchMenu(obj)
{ var el = document.getElementById(obj);
  if (el.style.display !== "none" )
    el.style.display = "none";
  else
    el.style.display = '';
}

function hideItem(obj)
{ document.getElementById(obj).style.display = "none";
}

function showItem(obj)
{ document.getElementById(obj).style.display = "";
}
