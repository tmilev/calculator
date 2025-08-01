"use strict";
let pathnames = require("./pathnames");


/** 
 * Extracts links from a standard result json.
 * 
 * @return {HTMLElement|null} 
 */
function extractLinksFromStandardJsonReturnAnchorCollection(
  input
) {
  return extractLinksReturnAnchorCollection(
    htmlFromCommentsAndErrors(input)
  );
}

/** 
 * Extracts links from a given text and returns html container.
 * 
 * @return {HTMLElement|null} 
 */
function extractLinksReturnAnchorCollection(
  /** @type {string|null|undefined} */
  inputHTML
) {
  if (inputHTML === null || inputHTML === undefined) {
    return null;
  }
  const links = extractLinks(inputHTML);
  if (links.length === 0) {
    return null;
  }
  const result = document.createElement("table");
  const header = result.insertRow();
  const headerCell = header.insertCell();
  headerCell.textContent = "Links extracted from the input. ";
  headerCell.style.fontWeight = "bolder";
  result.style.border = "1px solid";
  for (const link of links) {
    const anchor = document.createElement("a");
    anchor.href = link;
    anchor.textContent = link;
    const container = result.insertRow().insertCell();
    container.appendChild(anchor);
  }
  return result;
}

/** @return {string[]} */
function extractLinks(
  /** @type {string} */
  inputHTML
) {
  const result = [];
  let sliced = unescapeInequalitiesAmpersands(inputHTML);
  for (let i = 0; i < 10; i++) {
    const linkAndSlice = extractLinksOnce(sliced);
    sliced = linkAndSlice.remaining;
    if (linkAndSlice.link != "") {
      result.push(linkAndSlice.link);
    }
  }
  return result;
}

/** @return {{remaining:string,link:string}} */
function extractLinksOnce(
  /** @type {string} */
  input
) {
  let httpsIndex = input.search("https://");
  if (httpsIndex === -1) {
    return { remaining: "", link: "" };
  }
  const breakingCharacters = {
    " ": true,
    "\n": true,
  }
  const linkArray = [];
  for (let i = httpsIndex; i < input.length; i++) {
    if (input[i] in breakingCharacters) {
      break;
    }
    linkArray.push(input[i]);
  }
  const link = linkArray.join("");
  const remaining = input.slice(httpsIndex + linkArray.length);
  return { remaining: remaining, link: link };
}

function writeHTML(
  /** @type {HTMLElement} */
  element,
  /** @type {string} */
  htmlContent,
) {
  if (typeof htmlContent !== "string") {
    throw new Error(`Unexpected html content: ${htmlContent}`);
  }
  try {
    element.setHTML(htmlContent);
  } catch (e) {
    element.innerHTML = htmlContent;
  }
}

function switchMenu(
  /** @type {string|HTMLElement} */
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
  /** @type {HTMLElement} */
  targetToAppendTo,
  /** @type {HTMLElement|HTMLElement[]} */
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
  /** @type {HTMLElement[]} */
  targetArray,
  /** @type {HTMLElement|HTMLElement[]} */
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

/** @return {string} */
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

/** @return {HTMLElement[]} */
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
      writeHTML(incoming, current);
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
  /** @type {any} */
  input,
  /** @type {HTMLElement} */
  output,
) {
  writeHTML(output, htmlFromCommentsAndErrors(input));
}

function writeHtmlElementsFromCommentsAndErrors(
  /** @type {any} */
  input,
  /** @type {HTMLElement} */
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

/** @return {string} */
function jsonParseGetHtmlStandard(
  /** @type {string} */
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

/** @return {string} */
function unescapeInequalitiesAmpersands(input) {
  input = input.replace(/&lt;/g, "<");
  input = input.replace(/&gt;/g, ">");
  return input.replace(/&amp;/g, "&");
}

/** @return {Object} */
function jsonUnescapeParse(
  /** @type {string} */
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

/** @return {string} */
function toHex(
  /** @type {string} */
  input,
) {
  let result = "";
  for (let i = 0; i < input.length; i++) {
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
  extractLinksReturnAnchorCollection,
  extractLinksFromStandardJsonReturnAnchorCollection,
  writeHTML,
};