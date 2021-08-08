"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const submit = require("./submit_requests");
const calculator = require("./calculator_page");
const miscellaneous = require("./miscellaneous_frontend");
const equationEditor = require("./equation_editor");
const EquationEditor = require("./equation_editor").EquationEditor;
const storage = require("./storage").storage;
const InputPanelData = require("./initialize_buttons").InputPanelData;

class Solver {
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
    /**@type{HTMLElement} */
    this.solutionBox = document.getElementById(ids.domElements.pages.solve.solutionBox);
    /**@type{HTMLElement} */
    this.commentsBox = document.getElementById(ids.domElements.pages.solve.commentsBox);
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
    });
    this.panel.initialize();

    this.equationEditor = this.panel.equationEditor;
    this.setDebugLogContainer();
    this.equationEditor.updateDOM();
    this.equationEditor.rootNode.focus(0);
    let solveButton = document.getElementById(ids.domElements.pages.solve.buttonSolve);
    solveButton.addEventListener("click", (e) => {
      this.solve();
    });
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
    if (storage.variables.currentPage.getValue() !== "solve") {
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

  setAutoSolveProblemBox(
    /**@type{string} */
    input,
  ) {
    this.pendingTypeset = input;
    this.processPendingTypeset();
  }

  solve() {
    let input = this.equationEditor.rootNode.toLatex();
    let inputCalculator = `SolveJSON(${input})`;
    document.getElementById(ids.domElements.pages.calculator.inputMain).value = inputCalculator;
    calculator.calculator.submitComputation();
    // Will trigger solveFromStorage; 
    // injection of the method happens in Page.initializeStorageCallbacks.
    storage.variables.solve.problemToAutoSolve.setAndStore(input, true, false);
  }

  solveFromStorage(
    /**@type{string} */
    input,
  ) {
    if (document.getElementById(ids.domElements.pages.solve.div) === null) {
      return;
    }
    let debug = storage.variables.flagDebug.isTrue();
    submit.submitGET({
      url: pathnames.addresses.solveJSON(input, debug),
      callback: (input) => {
        this.solveCallback(input);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  solveCallback(
    /**@type{string} */
    input,
  ) {
    try {
      let solution = miscellaneous.jsonUnescapeParse(input);
      miscellaneous.writeHtmlElementsFromCommentsAndErrors(solution, this.commentsBox);
      let steps = solution["solution"]["solution"][pathnames.urlFields.result.solution.steps];
      this.writeSteps(steps);
      this.flagPendingSolutionTypeset = true;
      this.processPendingTypeset();
    } catch (e) {
      this.solutionBox.textContent = "";
      let boldElement = document.createElement("b");
      boldElement.style.color = "red";
      boldElement.textContent = e;
      this.solutionBox.appendChild(boldElement);
      this.solutionBox.appendChild(document.createElement("br"));
      this.solutionBox.appendChild(document.createTextNode(input));
    }
  }

  writeSteps(steps) {
    if (!Array.isArray(steps)) {
      this.solutionBox.textContent = "Error";
      return;
    }
    if (steps.length === 0) {
      this.solutionBox.textContent = "Sorry, I couldn't do anything useful.";
      return;
    }
    this.solutionBox.textContent = "";
    let stepElements = this.getSolutionSteps(steps);
    for (let i = 0; i < stepElements.length; i++) {
      this.solutionBox.appendChild(stepElements[i]);
    }
  }

  /**@returns{Array.<HTMLElement>} */
  getSolutionSteps(steps) {
    let result = [];
    for (let i = 0; i < steps.length; i++) {
      result.push(getOneSolutionStep(steps[i]));
    }
    return result;
  }

  /**@returns{HTMLElement} */
  getOneSolutionStep(step) {
    return document.createElement("div");
  }
}

const solver = new Solver();

function selectSolverPage() {
  solver.selectPage();
}

module.exports = {
  Solver,
  solver,
  selectSolverPage,
};
