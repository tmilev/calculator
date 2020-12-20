"use strict";
const EquationEditor = require("./equation_editor").EquationEditor;
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const submit = require("./submit_requests");
const calculator = require("./calculator_page");

class Solver {
  constructor() {
    /**@type{EquationEditor|null} */
    this.equationEditor = null;
  }

  selectPage() {
    if (this.equationEditor !== null) {
      return;
    }
    this.equationEditor = new EquationEditor(document.getElementById(ids.domElements.pages.solve.editor), null);
    this.equationEditor.updateDOM();
    this.equationEditor.rootNode.focus(0);
    let solveButton = document.getElementById(ids.domElements.pages.solve.buttonSolve);
    solveButton.addEventListener("click", (e) => {
      this.solve();
    });
  }

  solve() {
    let input = this.equationEditor.rootNode.toLatex();
    let mainPage = window.calculator.mainPage;
    let debug = mainPage.storage.variables.flagDebug.isTrue();
    let inputCalculator = `SolveJSON(${input})`;
    document.getElementById(ids.domElements.inputMain).value = inputCalculator;
    calculator.calculator.submitComputation();
    submit.submitGET({
      url: pathnames.addresses.solveJSON(input, debug),
      callback: (input) => { this.solveCallback(input); },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  solveCallback(
    /**@type{string} */
    input,
  ) {
    let output = document.getElementById(ids.domElements.pages.solve.solutionBox);
    output.innerHTML = input;
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
