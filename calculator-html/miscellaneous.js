"use srict";

function shortenString(charsOnEachSide, input) {
  if (charsOnEachSide * 2 + 3 > input.length) {
    return input;
  }
  return `${input.slice(0, charsOnEachSide)}...${input.slice(input.length - charsOnEachSide, input.length)}`;
}

function deepCopy(from, to) { 
  if (typeof from !== "object") {
    to = from;
    return;
  }
  for (var label in from) {
    if (typeof from[label] === "object") {
      to[label] = {};
      deepCopy(from[label], to[label])
      continue;
    } 
    to[label] = from[label];
  }
}

module.exports = {
  shortenString,
  deepCopy
}