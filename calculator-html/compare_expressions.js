"use strict";
const EquationEditorOptions = require("./equation_editor").EquationEditorOptions;
const EquationEditor = require("./equation_editor").EquationEditor;
const ids = require("./ids_dom_elements");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const storage = require("./storage");


class CompareExpressions {
  constructor() {
    /**@type{boolean} */
    this.initialized = false;
    /**@type{InputPanelData|null} */
    this.givenPanel = null;
    /**@type{InputPanelData|null} */
    this.desiredPanel = null;
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

    if (storage.storage.variables.compare.given.getValue() !== "") {
      this.givenPanel.equationEditor.writeLatex(storage.storage.variables.compare.given.getValue());
      this.givenPanel.equationEditor.writeLatexToInput();
    }
    if (storage.storage.variables.compare.desired.getValue() !== "") {
      this.desiredPanel.equationEditor.writeLatex(storage.storage.variables.compare.desired.getValue());
      this.desiredPanel.equationEditor.writeLatexToInput();
    }
  }

  compare() {
    let givenContainer = document.getElementById(ids.domElements.pages.compareExpressions.givenRawInput);
    let givenData = givenContainer.value;
    let desiredContainer = document.getElementById(ids.domElements.pages.compareExpressions.desiredRawInput);
    let desiredData = desiredContainer.value;

    storage.storage.variables.compare.given.setAndStore(givenData);
    storage.storage.variables.compare.desired.setAndStore(desiredData);
    this.doCompare();
  }

  doCompare() { }
}

const compareExpressions = new CompareExpressions();

module.exports = {
  compareExpressions,
};
