const equationEditor = require("./equation_editor");
// const mathjaxSetup = require("./mathjax-calculator-setup");

class TypeSetter {
  constructor() {
    this.defaultStyle = "vertical-align:text-bottom; font-family:'Times New Roman'; font-size:120%; display:inline-block; margin-bottom:-0.1em";
    this.logTiming = false;
  }

  typesetSoft(
    /**@type {HTMLElement|string} */
    element,
    /**@type {string} */
    styleOverride,
    /**@type{Function|null} */
    callbackEquationCreation,
    /**@type{Object<string, string>} */
    extraAttributes,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    if (styleOverride === "") {
      styleOverride = this.defaultStyle;
    }
    equationEditor.typeset(
      element,
      styleOverride,
      false,
      true,
      this.logTiming,
      callbackEquationCreation,
      extraAttributes,
    );
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
};