const equationEditor = require("./equation_editor/src/equation_editor");

class TypeSetter {
  constructor() {
    this.logTiming = false;
    this.logExcessiveTiming = true;
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
    const startTime = new Date().getTime();
    equationEditor.typeset(
      element,
      callbackEquationCreation, {
      style: styleOverride,
      sanitizeLatexSource: false,
      removeDisplayStyle: true,
      logTiming: this.logTiming,
      logExcessiveTiming: this.logExcessiveTiming,
      extraAttributes: extraAttributes,
      mathML: true,
      svgAndDOM: false,
      copyButton: true,
    });
    const endTime = new Date().getTime();
    const elapsedMilliseconds = endTime - startTime;
    if (elapsedMilliseconds > 200 && this.logExcessiveTiming) {
      console.log(`Typesetting took too long: ${elapsedMilliseconds} ms.`);
    }
  }
}

let typesetter = new TypeSetter();

module.exports = {
  typesetter,
};