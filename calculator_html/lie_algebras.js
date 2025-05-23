const equationEditor = require("./equation_editor/src/equation_editor");
const panels = require("./panels");

let ids = {
  panelClassName: "lieAlgebraPanel",
  label: "label",
};

function bootstrap() {
  console.log("Bootstrapping Lie algebra page.");
  equationEditor.typeset(document.body, null, { mathML: true, copyButton: true });

  let elementsWithPanels = document.getElementsByClassName(ids.panelClassName);
  for (let i = 0; i < elementsWithPanels.length; i++) {
    let current = elementsWithPanels[i];
    let content = current.children[0];
    let currentPanel = new panels.PanelExpandable(current);
    currentPanel.initialize(true, true);
    currentPanel.setPanelContent(content);
    currentPanel.setCollapsed(true);
  }
}

module.exports = {
  bootstrap,
  ids,
};