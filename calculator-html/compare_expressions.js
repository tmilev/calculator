"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const storage = require("./storage").storage;
const miscellaneous = require("./miscellaneous");
const equationEditor = require("./equation_editor");
const submit = require("./submit_requests");

class CompareExpressions {
  constructor() {
    /**@type{boolean} */
    this.initialized = false;
    /**@type{InputPanelData|null} */
    this.givenPanel = null;
    /**@type{InputPanelData|null} */
    this.desiredPanel = null;
    /**@type{HTMLElement} */
    this.resultBox = document.getElementById(ids.domElements.pages.compareExpressions.result);
  }

  selectPage() {
    if (this.initialized) {
      return;
    }
    let button = document.getElementById(ids.domElements.pages.compareExpressions.buttonCompare);
    if (button === null) {
      return;
    }
    button.addEventListener("click", () => {
      this.compare();
    });

    this.initialized = true;
    this.givenPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressions.givenEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressions.givenPanel,
      idPureLatex: ids.domElements.pages.compareExpressions.givenRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.givenPanel.initialize();

    this.desiredPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressions.desiredEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressions.desiredPanel,
      idPureLatex: ids.domElements.pages.compareExpressions.desiredRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.desiredPanel.initialize();

    if (storage.variables.compare.given.getValue() !== "") {
      this.givenPanel.equationEditor.writeLatex(storage.variables.compare.given.getValue());
      this.givenPanel.equationEditor.writeLatexToInput();
    }
    if (storage.variables.compare.desired.getValue() !== "") {
      this.desiredPanel.equationEditor.writeLatex(storage.variables.compare.desired.getValue());
      this.desiredPanel.equationEditor.writeLatexToInput();
    }
  }

  compare() {
    let givenContainer = document.getElementById(ids.domElements.pages.compareExpressions.givenRawInput);
    let givenData = givenContainer.value;
    let desiredContainer = document.getElementById(ids.domElements.pages.compareExpressions.desiredRawInput);
    let desiredData = desiredContainer.value;

    storage.variables.compare.given.setAndStore(givenData);
    storage.variables.compare.desired.setAndStore(desiredData);
    this.doCompare(givenData, desiredData);
  }

  doCompare(
    /**@type{string} */
    givenData,
    /**@type{string} */
    desiredData,
  ) {
    let debug = storage.variables.flagDebug.isTrue();
    submit.submitGET({
      url: pathnames.addresses.compareExpressions(givenData, desiredData, debug),
      callback: (input) => {
        this.writeResult(input);
      },
      progress: ids.domElements.pages.compareExpressions.progress,
      dontCollapsePanel: true,
    });
  }

  writeResult(
    /**@type{string} */
    input,
  ) {
    try {
      let comparison = miscellaneous.jsonUnescapeParse(input);
      this.resultBox.textContent = `${comparison}`;
      equationEditor.typeset(this.resultBox);
    } catch (e) {
      this.resultBox.innerHTML = `<b style='color:red'>${e}</b><br>${input}`;
    }
  }
}

const compareExpressions = new CompareExpressions();

module.exports = {
  compareExpressions,
};
