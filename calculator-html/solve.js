"use strict";
const EquationEditor = require("./equation_editor").EquationEditor;
const ids = require("./ids_dom_elements");

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
