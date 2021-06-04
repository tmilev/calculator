"use strict";
const ids = require("./ids_dom_elements");
const equationEditor = require("./equation_editor");
const EquationEditor = require("./equation_editor").EquationEditor;
const storage = require("./storage").storage;
const InputPanelData = require("./initialize_buttons").InputPanelData;
const submitRequest = require("./submit_requests");

let idsSocratic = {
  socraticBackendServer: "socraticBackendServer",
};

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
    /**@type{HTMLOptionElement} */
    this.backendElement = document.getElementById(idsSocratic.socraticBackendServer);
    if (this.backendElement !== null) {
      this.backendElement.addEventListener("change", () => {
        storage.variables.solveSocratic.backend.setAndStore(this.backendElement.value);
        this.setAnchor(this.inputElement().value);
      });
    }
    /**@type{HTMLButtonElement} */
    this.buttonSolve = document.getElementById(ids.domElements.pages.solveSocratic.buttonMain);
    if (this.buttonSolve !== null) {
      this.buttonSolve.addEventListener('click', () => {
        this.solve();
      });
    }
    /**@type{HTMLElement} */
    this.socraticSolution = document.getElementById(ids.domElements.pages.solveSocratic.solution);
    /**@type{string} 
     * We store our socratic key with byte entries shifted so it does not appear in the javascript as plain text. 
     * Of course, this is not intended to be secure, 
     * but a protection that the api key pop up easily in searches.
     */
    this.keyShifted = "FNfX~H|TUY^t{\\y~8INq[OWRMf~5h_YY66gunl";
  }

  /**@return{string} */
  getKey() {
    let result = "";
    for (let i = 0; i < this.keyShifted.length; i++) {
      result += String.fromCharCode(this.keyShifted.charCodeAt(i) - 5);
    }
    return result;
  }

  /**@returns{HTMLInputElement} */
  inputElement() {
    return document.getElementById(ids.domElements.pages.solveSocratic.input);
  }

  selectPage() {
    if (this.equationEditor !== null) {
      return;
    }
    let desiredLatex = storage.variables.solveSocratic.problemToAutoSolve.getValue();
    let latexElement = this.inputElement();
    this.backendElement.value = storage.variables.solveSocratic.backend.getValue();
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

  /**@returns{string} */
  getBackend() {
    return this.backendElement.value;
  }

  /**@returns{string} */
  getSocraticLinkDefault(
  ) {
    return this.getSocraticLink(this.inputElement().value);
  }

  /**@returns{string} */
  getSocraticLink(
    /**@type{string} */
    latex,
  ) {
    let encoded = encodeURIComponent(latex);
    let backend = this.getBackend();
    return `${backend}?q=${encoded}`;
  }

  setAnchor(
    /**@type{string} */
    text,
  ) {
    let anchor = document.getElementById(ids.domElements.pages.solveSocratic.link);
    anchor.textContent = text;
    anchor.setAttribute("href", this.getSocraticLink(text));
    let anchorThisPage = document.getElementById(ids.domElements.pages.solveSocratic.anchorThisPage);
    if (text === "") {
      anchorThisPage.style.display = "hidden";
    } else {
      anchorThisPage.style.display = "";
    }
    anchorThisPage.href = window.location;
  }

  solve() {
    let url = "https://bloom-pa.googleapis.com/v1/solve:math?";
    url += `key=${this.getKey()}&`;
    url += `queries=2x`;
    submitRequest.submitGET({
      url: url,
      callback: (text) => {
        this.solveCallback(text);
      },
    });
  }
  solveCallback(
    /**@type{string} */ text
  ) {
    this.socraticSolution.textContent = text;

  }
}

const solver = new SolverSocratic();

module.exports = {
  idsSocratic,
  solver,
};
