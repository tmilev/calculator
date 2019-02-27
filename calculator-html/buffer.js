"use strict";

function BufferCalculator() {
  this.internal = [];
}

BufferCalculator.prototype.write = function (any) {
  for (var i = 0; i < any.length; i ++) {
    this.internal.push(any[i]);
  }
}

BufferCalculator.prototype.toString = function() {
  return this.internal.join("");
}

module.exports = {
  BufferCalculator
};