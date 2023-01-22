"use strict";
let pathnames = require("./pathnames");

function switchMenu(
  /** @type{string|HTMLElement} */
  object
) {
  if (typeof object === "string") {
    object = document.getElementById(object);
  }
  if (object.classList.contains("hiddenClass")) {
    object.classList.remove("hiddenClass");
  } else {
    object.classList.add("hiddenClass");
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
    for (let i = 0; i < contentToAppend.length; i++) {
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
    for (let i = 0; i < contentToAppend.length; i++) {
      appendHtmlToArray(targetArray, contentToAppend[i]);
    }
    return;
  }
  throw (`Could not recognize the html content ${contentToAppend}`);
}

/** @return{string} */
function htmlFromCommentsAndErrors(input) {
  let extraTags = [
    pathnames.urlFields.result.comments,
    pathnames.urlFields.result.error,
    pathnames.urlFields.result.commentsGlobal,
    pathnames.urlFields.result.crashReport,
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

/** @return{HTMLElement[]} */
function htmlElementsFromCommentsAndErrors(input) {
  let extraTags = [
    pathnames.urlFields.result.comments,
    pathnames.urlFields.result.error,
    pathnames.urlFields.result.commentsGlobal,
    pathnames.urlFields.result.crashReport,
  ];
  let result = [];
  for (let i = 0; i < extraTags.length; i++) {
    let current = input[extraTags[i]];
    if (
      current === undefined ||
      current === null ||
      current === ""
    ) {
      continue;
    }
    let incoming = document.createElement("div");
    if (extraTags[i] === pathnames.urlFields.result.crashReport) {
      incoming.innerHTML = current; 
    } else {
      incoming.textContent = current;
    }
    if (extraTags[i] === pathnames.urlFields.result.error) {
      let errorTag = document.createElement("b");
      errorTag.style.color = "red";
      errorTag.textContent = "Error.";
      result.push(errorTag);
    }
    result.push(incoming);
    result.push(document.createElement("br"));
  }
  return result;
}

function writeHtmlFromCommentsAndErrors(
  /** @type{any} */
  input,
  /** @type{HTMLElement} */
  output,
) {
  output.innerHTML = htmlFromCommentsAndErrors(input);
}

function writeHtmlElementsFromCommentsAndErrors(
  /** @type{any} */
  input,
  /** @type{HTMLElement} */
  output,
) {
  let elements = htmlElementsFromCommentsAndErrors(input);
  output.textContent = "";
  for (let i = 0; i < elements.length; i++) {
    output.appendChild(elements[i]);
  }
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

/** @return{string} */
function jsonParseGetHtmlStandard(
  /** @type{string} */
  input,
) {
  if (input.search("\n") !== - 1) {
    throw `Input to json parser has newlines.\n${input}`;
  }
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

/** @return{string} */
function unescapeInequalitiesAmpersands(input) {
  input = input.replace(/&lt;/g, "<");
  input = input.replace(/&gt;/g, ">");
  return input.replace(/&amp;/g, "&");
}

/** @return{Object} */
function jsonUnescapeParse(
  /** @type{string} */
  input,
) {
  let unescaped = unescapeInequalitiesAmpersands(input);
  if (unescaped.search("\n") !== - 1) {
    throw `Unexpected newline in input. ${unescaped}`;
  }
  try {
    return JSON.parse(unescaped);
  } catch (e) {
    console.log("Bad unescaped input, content follows.");
    console.log(unescaped);
    return {
      error: e + "<hr>" + unescaped,
    };
  }
}

/** @return{string} */
function toHex(
  /** @type{string} */
  input,
) {
  let result = "";
  for (var i = 0; i < input.length; i++) {
    result += input.charCodeAt(i).toString(16);
  }
  return result;
}

module.exports = {
  toHex,
  switchMenu,
  appendHtml,
  appendHtmlToArray,
  writeHtmlFromCommentsAndErrors,
  writeHtmlElementsFromCommentsAndErrors,
  htmlFromCommentsAndErrors,
  htmlElementsFromCommentsAndErrors,
  shortenString,
  deepCopy,
  jsonParseGetHtmlStandard,
  unescapeInequalitiesAmpersands,
  jsonUnescapeParse,
};