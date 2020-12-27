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
    for (var i = 0; i < contentToAppend.length; i++) {
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
    for (var i = 0; i < contentToAppend.length; i++) {
      appendHtmlToArray(targetArray, contentToAppend[i]);
    }
    return;
  }
  throw (`Could not recognize the html content ${contentToAppend}`);
}

/**@returns{string} */
function HTMLFromCommentsAndErrors(input) {
  let extraTags = [
    pathnames.urlFields.result.comments,
    pathnames.urlFields.result.error,
    pathnames.urlFields.result.commentsGlobal,
  ];
  let resultHTML = "";
  for (let i = 0; i < extraTags.length; i++) {
    let current = input[extraTags[i]];
    if (current === undefined || current === null || current === "") {
      continue;
    }
    resultHTML += "<hr>" + current;
  }
  return resultHTML;
}

/**@returns{HTMLElement[]} */
function HTMLElementsFromCommentsAndErrors(input) {
  let extraTags = [
    pathnames.urlFields.result.comments,
    pathnames.urlFields.result.error,
    pathnames.urlFields.result.commentsGlobal,
  ];
  let result = [];
  for (let i = 0; i < extraTags.length; i++) {
    let current = input[extraTags[i]];
    if (current === undefined || current === null || current === "") {
      continue;
    }
    let incoming = document.createElement("div");
    incoming.textContent = current;
    result.push(incoming);
    result.push(document.createElement("br"));
  }
  return result;
}

module.exports = {
  switchMenu,
  appendHtml,
  appendHtmlToArray,
  HTMLFromCommentsAndErrors,
  HTMLElementsFromCommentsAndErrors
};