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
    this.debugDiv = document.getElementById(ids.domElements.pages.solveSocratic.editorSolveProblemDebug);
    /**@type{HTMLElement} */
    this.flagPendingSolutionTypeset = false;
  }

  selectPage() {
    if (this.equationEditor !== null) {
      return;
    }
    /**@type{HTMLInputElement} */
    let latexElement = document.getElementById(ids.domElements.pages.solveSocratic.input);
    let desiredLatex = storage.variables.solveSocratic.problemToAutoSolve.getValue();
    this.panel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.solveSocratic.editor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.solveSocratic.editorButtonPanel,
      flagCalculatorPanel: false,
      buttonsPerLine: 8,
      pureLatexElement: latexElement,
      valueChangeHandler: (text) => {
        this.valueChange(text);
      }
    });
    if (latexElement !== null) {
      latexElement.addEventListener("input", () => {
        this.valueChange(latexElement.value);
      });
      latexElement.addEventListener("change", () => {
        this.valueChange(latexElement.value);
      });
    }
    this.panel.initialize();
    this.equationEditor = this.panel.equationEditor;
    this.setDebugLogContainer();
    this.equationEditor.updateDOM();
    this.equationEditor.rootNode.focus(0);
    this.processPendingTypeset();
    latexElement.value = desiredLatex;
    this.equationEditor.writeLatexFromInput();
    this.setAnchor(desiredLatex);
  }

  setDebugLogContainer() {
    let doDebug = storage.variables.flagDebug.isTrue();
    // TODO(tmilev): For now, disallow debug data.
    doDebug = false;
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
    this.setAnchor(text);
    storage.variables.solveSocratic.problemToAutoSolve.setAndStore(text, true, false);
  }

  setAnchor(
    /**@type{string} */
    text,
  ) {
    let anchor = document.getElementById(ids.domElements.pages.solveSocratic.link);
    anchor.textContent = text;
    let encoded = encodeURIComponent(text);
    anchor.setAttribute("href", `https://bloom-autopush.sandbox.google.com/math/solver?q=${encoded}`);
    let anchorThisPage = document.getElementById(ids.domElements.pages.solveSocratic.anchorThisPage);
    if (text === "") {
      anchorThisPage.style.display = "hidden";
    } else {
      anchorThisPage.style.display = "";
    }
    anchorThisPage.href = window.location;
  }
}

const solver = new SolverSocratic();

module.exports = {
  solver,
};
