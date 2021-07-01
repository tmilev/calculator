const equationEditor = require("./equation_editor");
// const mathjaxSetup = require("./mathjax-calculator-setup");

class TypeSetter {
  constructor() {
    this.defaultStyle = {
      verticalAlign: "text-bottom",
      fontFamily: "Times New Roman",
      fontSize: "120%",
      display: "inline-block",
      marginBottom: "-0.1em",
    };
    this.logTiming = false;
  }

  typesetSoft(
    /**@type {HTMLElement|string} */
    element,
    /**@type {(Object.<string,string>|null)} */
    styleOverride,
    /**@type{Function|null} */
    callbackEquationCreation,
    /**@type{Object<string, string>} */
    extraAttributes,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    if (styleOverride === null) {
      styleOverride = this.defaultStyle;
    }
    equationEditor.typeset(
      element,
      callbackEquationCreation, {
      style: styleOverride,
      sanitizeLatexSource: false,
      removeDisplayStyle: true,
      logTiming: this.logTiming,
      extraAttributes: extraAttributes,
      svgOnly: false,
      svgAndDOM: false,
    },
    );
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
};