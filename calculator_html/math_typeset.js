const equationEditor = require("./equation_editor/src/equation_editor");

class TypeSetter {
  constructor() {
    this.logTiming = false;
  }

  typesetSoft(
    /** @type {HTMLElement|string} */
    element,
    /** @type {(Object.<string,string>|null)} */
    styleOverride,
    /** @type {Function|null} */
    callbackEquationCreation,
    /** @type {Object<string, string>} */
    extraAttributes,
  ) {
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    equationEditor.typeset(
      element,
      callbackEquationCreation, {
      style: styleOverride,
      sanitizeLatexSource: false,
      removeDisplayStyle: true,
      logTiming: this.logTiming,
      extraAttributes: extraAttributes,
      mathML: true,
      svgAndDOM: false,
      copyButton: true,
    },
    );
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
};