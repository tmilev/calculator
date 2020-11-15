const equationEditor = require("./equation_editor");
const mathjaxSetup = require("./mathjax-calculator-setup");

class TypeSetter {
  constructor() {
  }

  useBuiltInTypeSet() {
    let thePage = window.calculator.mainPage;
    return !thePage.storage.variables.flagMathJax.isTrue();
  }

  typesetSoft(
    /**@type {HTMLElement|string} */
    element,
    /**@type {string} */
    styleOverride,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    if (this.useBuiltInTypeSet()) {
      equationEditor.typeset(element, styleOverride);
    } else {
      mathjaxSetup.typeSetSoft(element);
    }
  }

  typesetHard(
    /**@type {HTMLElement|string} */
    element,
    /**@type {string} */
    styleOverride,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    if (this.useBuiltInTypeSet()) {
      equationEditor.typeset(element, styleOverride);
    } else {
      mathjaxSetup.typeSetHard(element);
    }
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
}