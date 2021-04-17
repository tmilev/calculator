const panels = require("./panels");

let ids = {
  panelClassName: "lieAlgebraPanel",
  label: "label",
};

function bootstrap() {
  console.log("Bootstrapping Lie algebra page.");
  let elementsWithPanels = document.getElementsByClassName(ids.panelClassName);
  for (let i = 0; i < elementsWithPanels.length; i++) {
    let current = elementsWithPanels[i];
    let content = current.children[0];
    // let content = current.textContent;
    // let label = current.getAttribute(ids.label);
    // current.textContent = "";
    let currentPanel = new panels.PanelExpandable(current);
    currentPanel.initialize(true, true);
    currentPanel.setPanelContent(content);
    // currentPanel.setPanelContent(content);
    // currentPanel.setPanelLabel(label);
    currentPanel.setCollapsed(true);
  }
}

module.exports = {
  bootstrap,
  ids,
};