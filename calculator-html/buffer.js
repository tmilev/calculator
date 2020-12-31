"use strict";

class BufferCalculator {
  constructor() {
    this.internal = [];
  }

  write(any) {
    for (var i = 0; i < any.length; i++) {
      this.internal.push(any[i]);
    }
  }

  toString() {
    return this.internal.join("");
  }
}

module.exports = {
  BufferCalculator
};