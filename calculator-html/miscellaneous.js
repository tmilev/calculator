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
  var result = {};
  for (var label in from) {
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
  var unescaped = unescapeInequalitiesAmpersands(input)
  var parsed = JSON.parse(unescaped);
  var result = "";
  if (parsed[pathnames.urlFields.result.resultHtml] !== undefined) {
    result += parsed[pathnames.urlFields.result.resultHtml];
  }
  if (parsed[pathnames.urlFields.result.error] !== undefined) {
    result += `<b style='red'>Error.</b> ${parsed[pathnames.urlFields.result.error]}`;
  }
  if (parsed[pathnames.urlFields.result.crashReport] !== undefined) {
    result += `<b style='red'>Crash.</b> ${parsed[pathnames.urlFields.result.crashReport]}`;
  }
  return result;
}

/**@returns{string} */
function unescapeInequalitiesAmpersands(input) {
  input = input.replace(/&lt;/g,"<");
  input = input.replace(/&gt;/g,">");
  return input.replace(/&amp;/g,"&");
}

/**@returns{Object} */
function jsonUnescapeParse(input) {
  var unescaped = unescapeInequalitiesAmpersands(input);
  return JSON.parse(unescaped);
}

module.exports = {
  shortenString,
  deepCopy,
  jsonParseGetHtmlStandard,
  unescapeInequalitiesAmpersands,
  jsonUnescapeParse,
};