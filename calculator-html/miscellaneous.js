"use srict";

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

module.exports = {
  shortenString,
  deepCopy,
}