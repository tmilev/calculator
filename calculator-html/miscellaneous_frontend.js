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

function shortenString(charsOnEachSide, input) {
  if (charsOnEachSide * 2 + 3 > input.length) {
    return input;
  }
  return `${input.slice(0, charsOnEachSide)}...${input.slice(input.length - charsOnEachSide, input.length)}`;
}

function deepCopy(from) {
  if (typeof from !== "object") {
    return from;
  }
  let result = {};
  for (let label in from) {
    if (typeof from[label] === "object") {
      result[label] = deepCopy(from[label])
      continue;
    }
    result[label] = from[label];
  }
  return result;
}

/**@returns{string} */
function jsonParseGetHtmlStandard(input) {
  let parsed = jsonUnescapeParse(input);
  let result = "";
  if (parsed[pathnames.urlFields.result.resultHtml] !== undefined) {
    result += parsed[pathnames.urlFields.result.resultHtml];
  }
  if (parsed[pathnames.urlFields.result.commentsGlobal] !== undefined) {
    result += parsed[pathnames.urlFields.result.commentsGlobal];
  }
  let lookForComments = false;
  if (parsed[pathnames.urlFields.result.error] !== undefined) {
    result += `<b style='color:red'>Error.</b> ${parsed[pathnames.urlFields.result.error]}`;
    lookForComments = true;
  }
  if (parsed[pathnames.urlFields.result.crashReport] !== undefined) {
    result += parsed[pathnames.urlFields.result.crashReport];
    lookForComments = true;
  }
  if (lookForComments) {
    let comments = parsed[pathnames.urlFields.result.comments];
    if (comments !== undefined && comments !== "") {
      result += `<hr><b>Additional comments.</b><br>${comments}`;
    }
  }
  return result;
}

/**@returns{string} */
function unescapeInequalitiesAmpersands(input) {
  input = input.replace(/&lt;/g, "<");
  input = input.replace(/&gt;/g, ">");
  return input.replace(/&amp;/g, "&");
}

/**@returns{Object} */
function jsonUnescapeParse(input) {
  let unescaped = unescapeInequalitiesAmpersands(input);
  return JSON.parse(unescaped);
}

module.exports = {
  switchMenu,
  appendHtml,
  appendHtmlToArray,
  HTMLFromCommentsAndErrors,
  HTMLElementsFromCommentsAndErrors,
  shortenString,
  deepCopy,
  jsonParseGetHtmlStandard,
  unescapeInequalitiesAmpersands,
  jsonUnescapeParse,
};