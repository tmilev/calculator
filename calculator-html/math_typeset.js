const equationEditor = require("./equation_editor");
// const mathjaxSetup = require("./mathjax-calculator-setup");

class TypeSetter {
  constructor() {
    this.defaultStyle = "vertical-align:text-bottom; font-family:'Times New Roman'; display:inline-block; margin-bottom:-0.05em";
    this.logTiming = false;
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
    equationEditor.typeset(element, styleOverride, false, true, this.logTiming);
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
    equationEditor.typeset(element, styleOverride, false, true, this.logTiming);
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
}