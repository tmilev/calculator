"use strict";
var pathnames = require("./pathnames");

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

function appendHtml(
  /** @type{HTMLElement}*/
  targetToAppendTo,
  /** @type{HTMLElement|HTMLElement[]}*/
  contentToAppend,
) {
  if (contentToAppend === null) {
    return;
  }
  if (!(targetToAppendTo instanceof HTMLElement)) {
    throw ("appendHtml called with non-html element target. ");
  }
  if (contentToAppend instanceof HTMLElement) {
    targetToAppendTo.appendChild(contentToAppend);
    return;
  }
  if (Array.isArray(contentToAppend)) {
    for (var i = 0; i < contentToAppend.length; i ++) {
      appendHtml(targetToAppendTo, contentToAppend[i]);
    }
    return;
  }
  throw (`Could not recognize the html content ${contentToAppend}`);
}

function appendHtmlToArray(
  /** @type{HTMLElement[]}*/
  targetArray,
  /** @type{HTMLElement|HTMLElement[]}*/
  contentToAppend,
) {
  if (contentToAppend === null) {
    return;
  }
  if (!Array.isArray(targetArray)) {
    throw ("appendHtmlToArray called with non-array target. ");
  }
  if (contentToAppend instanceof HTMLElement) {
    targetArray.push(contentToAppend);
    return;
  }
  if (Array.isArray(contentToAppend)) {
    for (var i = 0; i < contentToAppend.length; i ++) {
      appendHtmlToArray(targetArray, contentToAppend[i]);
    }
    return;
  }
  throw (`Could not recognize the html content ${contentToAppend}`);
}

function HTMLFromCommentsAndErrors(input) {
  var extraTags = [
    pathnames.urlFields.result.comments,
    pathnames.urlFields.result.error,
    pathnames.urlFields.result.commentsGlobal,
  ];
  var resultHTML = "";
  for (var i = 0; i < extraTags.length; i ++) {
    var current = input[extraTags[i]];
    if (current === undefined || current === null || current === "") {
      continue;
    }
    resultHTML += "<hr>" + current;
  }
  return resultHTML;
}

module.exports = {
  switchMenu,
  appendHtml,
  appendHtmlToArray,
  HTMLFromCommentsAndErrors,
};