"use srict";

function shortenString(charsOnEachSide, input) {
  if (charsOnEachSide * 2 + 3 > input.length) {
    return input;
  }
  return input.slice(0, charsOnEachSide) + "..." + input.slice(input.length - charsOnEachSide, input.length);
}

module.exports = {
  shortenString,
}