"use srict";
const pathnames = require("./pathnames");

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
  shortenString,
  deepCopy,
  jsonParseGetHtmlStandard,
  unescapeInequalitiesAmpersands,
  jsonUnescapeParse,
};