const panels = require("./panels");

let ids = {
  panelClassName: "lieAlgebraPanel",
  label: "label",
};

function bootstrap() {
  let elementsWithPanels = document.getElementsByClassName(ids.elementsWithPanels);
  for (let i = 0; i < elementsWithPanels.length; i++) {
    let current = elementsWithPanels[i];
    let content = current.textContent;
    let label = current.getAttribute(ids.label);
    current.textContent = "";
    let currentPanel = new panels.PanelExpandable(elementsWithPanels[i]);
    currentPanel.setPanelContent(content);
    currentPanel.setPanelLabel(label);
    currentPanel.setCollapsed(true);
  }
}

module.exports = {
  bootstrap,
};