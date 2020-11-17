const equationEditor = require("./equation_editor");
const mathjaxSetup = require("./mathjax-calculator-setup");

class TypeSetter {
  constructor() {
    this.defaultStyle = "vertical-align:text-bottom; font-family: 'Times New Roman', Times, serif; display:inline-block; margin-bottom:-0.05em";
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
    if (styleOverride === "") {
      styleOverride = this.defaultStyle;
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
    // Style overrides works only when transforming \(\) to mathcalculator tags.
    // Mathcalculator tags are not affected by styleOverride.
    if (styleOverride === "") {
      styleOverride = this.defaultStyle;
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