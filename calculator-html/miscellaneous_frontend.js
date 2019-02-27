"use strict";

function switchMenu(obj) { 
  var el = document.getElementById(obj);
  if (el.classList.contains("hiddenClass")) {
    el.classList.remove("hiddenClass");
  } else {
    el.classList.add("hiddenClass");
  }
}

function hideItem(obj) { 
  document.getElementById(obj).style.display = "none";
}

function showItem(obj) { 
  document.getElementById(obj).style.display = "";
}

module.exports = {
  switchMenu
};