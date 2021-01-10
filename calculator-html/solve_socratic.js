"use strict";
const ids = require("./ids_dom_elements");
const equationEditor = require("./equation_editor");
const EquationEditor = require("./equation_editor").EquationEditor;
const storage = require("./storage").storage;
const InputPanelData = require("./initialize_buttons").InputPanelData;

class SolverSocratic {
  constructor() {
    /**@type{EquationEditor|null} */
    this.equationEditor = null;
    /**@type{InputPanelData|null} */
    this.panel = null;
    /**@type{string|null} */
    this.pendingTypeset = null;
    /**@type{HTMLElement} */
    this.debugDiv = document.getElementById(ids.domElements.pages.solve.editorSolveProblemDebug);
    /**@type{HTMLElement} */
    this.flagPendingSolutionTypeset = false;
  }

  selectPage() {
    if (this.equationEditor !== null) {
      return;
    }
    this.panel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.solve.editor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.solve.editorButtonPanel,
      flagCalculatorPanel: true,
      buttonsPerLine: 8,
      valueChangeHandler: (text) => {
        this.valueChange(text);
      }
    });
    this.panel.initialize();
    this.equationEditor = this.panel.equationEditor;
    this.setDebugLogContainer();
    this.equationEditor.updateDOM();
    this.equationEditor.rootNode.focus(0);
    this.processPendingTypeset();
  }

  setDebugLogContainer() {
    let doDebug = storage.variables.flagDebug.isTrue();
    if (doDebug) {
      this.debugDiv.style.display = "";
    } else {
      this.debugDiv.style.display = "none";
    }
    if (this.equationEditor === null) {
      return;
    }
    if (doDebug) {
      this.equationEditor.options.debugLogContainer = this.debugDiv;
    } else {
      this.equationEditor.options.debugLogContainer = null;
    }
  }

  processPendingTypeset() {
    if (storage.variables.currentPage.getValue() !== "solveSocratic") {
      return;
    }
    if (this.flagPendingSolutionTypeset) {
      this.flagPendingSolutionTypeset = false;
      equationEditor.typeset(this.solutionBox);
    }
    if (this.pendingTypeset === null) {
      return;
    }
    if (this.equationEditor === null) {
      return;
    }
    let input = this.pendingTypeset;
    this.pendingTypeset = null;
    if (input === "") {
      if (this.equationEditor.rootNode.toLatex() === "") {
        return;
      }
    }
    this.setDebugLogContainer();
    this.equationEditor.writeLatex(input);
  }

  valueChange(
    /**@type{string} */
    text,
  ) {
    let anchor = document.getElementById(ids.domElements.pages.solveSocratic.link);
    anchor.textContent = text;
    let encoded = encodeURIComponent(text);
    anchor.setAttribute("href", `https://bloom-autopush.sandbox.google.com/math/solver?q=${encoded}`);
  }
}

const solver = new SolverSocratic();

module.exports = {
  solver,
};
