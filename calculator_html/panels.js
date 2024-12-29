/**
 * This file needs to function as stand-alone javascript
 * as well be used as a commonJS module included via
 * require('panels.js')).
 * Please don't require(...) any modules from this file.
 **/
"use strict";

var module;
if (module === undefined) {
  module = {};
}

function writeHTML(
  /** @type {HTMLElement} */
  element,
  /** @type {string} */
  htmlContent,
) {
  try {
    element.setHTML(htmlContent);
  } catch (e) {
    element.innerHTML = htmlContent;
  }
}

function modifyHeightForTimeout(currentButtonPanel, newHeight) {
  currentButtonPanel.style.maxHeight = newHeight;
  currentButtonPanel.style.height = newHeight;
}

function toggleHeight(currentButton, currentPanelID) {
  let currentPanel = document.getElementById(currentPanelID);
  if (currentPanel === null) {
    return;
  }
  currentPanel.buttonThatModifiesMe = currentButton;
  if (currentPanel.startingMaxHeight === undefined || currentPanel.startingMaxHeight === null) {
    currentPanel.startingMaxHeight = window.getComputedStyle(currentPanel).height;
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
  }
  let currentContainer = currentPanel;
  while (true) {
    currentContainer = currentContainer.parentNode;
    //console.log("processing "+ currentContainer.tagName +"."+ currentContainer.className);
    if (!currentContainer.className.startsWith("body")) {
      break;
    }
    currentContainer.style.maxHeight = "";
    currentContainer.style.height = "";
    currentContainer.startingMaxHeight = null;
  }
  if (currentPanel.style.display === "none" || currentPanel.transitionState === "collapsed") {
    currentPanel.style.display = "";
  }
  if (currentPanel.transitionState === "collapsing" ||
    currentPanel.transitionState === "collapsed") {
    currentPanel.transitionState = "expanding";
    writeHTML(currentButton, "&#9660;");
  } else if (
    currentPanel.transitionState === null || currentPanel.transitionState === undefined ||
    currentPanel.transitionState === "expanding" || currentPanel.transitionState === "expanded"
  ) {
    currentPanel.transitionState = "collapsing";
    writeHTML(currentButton, "&#9656;");
  }
  currentPanel.addEventListener("transitionend", transitionDone);
  setTimeout(
    function () {
      toggleHeightForTimeout(currentPanel);
    }, 0
  );
}

function transitionDone(event) {
  this.removeEventListener("transitionend", transitionDone);
  if (this.transitionState === "collapsing") {
    this.style.display = "none";
    this.transitionState = "collapsed";
  } else if (this.transitionState === "expanding") {
    this.style.display = "";
    this.transitionState = "expanded";
  }
}

function toggleHeightForTimeout(currentPanel) {
  if (currentPanel.transitionState === "expanding") {
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
    currentPanel.style.opacity = "1";
  } else if (currentPanel.transitionState === "collapsing") {
    currentPanel.style.opacity = "0";
    currentPanel.style.maxHeight = "0px";
    currentPanel.style.height = "0px";
  }
}

class PanelExpandableData {
  constructor(
    /** @type {string} */
    content,
    /** 
     * Id of the desired container element, if known, leave empty if you don't have an id. 
     * @type {HTMLElement|string} 
     */
    container,
    /** @type {number} */
    minimalCharacterLengthForPanel,
    /** @type {boolean} */
    startHidden,
    /** @type {string} */
    label,
    /** @type {boolean} */
    allowFullExpand,
    /** @type {boolean|undefined} */
    createCopyButton
  ) {
    this.content = content;
    this.container = container;
    this.minimalCharacterLengthForPanel = minimalCharacterLengthForPanel;
    this.startHidden = startHidden;
    this.allowFullExpand = allowFullExpand;
    this.label = label;
    if (this.startHidden === undefined) {
      this.startHidden = false;
    }
    if (this.label === undefined) {
      this.label = "";
    }
    if (this.allowFullExpand === undefined) {
      this.allowFullExpand = false;
    }
    this.createCopyButton = false;
    if (createCopyButton !== undefined) {
      this.createCopyButton = createCopyButton;
    }
  }
}

class PanelExpandable {
  constructor(
    /** @type {HtmlElement|string} */
    container,
  ) {
    this.originalHeight = 0;
    this.originalWidth = 0;
    this.originalDimensionsKnown = true;
    this.collapsed = true;
    this.containerId = "";
    /** @type {HTMLElement} */
    this.container = container;
    /** @type {HtmlElement|null} */
    this.panelContent = null;
    /** @type {HtmlElement|null} */
    this.panelLabel = null;
    /** @type {HtmlElement|null} */
    this.expandedMark = null;
    /** @type {HtmlElement|null} */
    this.button = null;
    /** @type {HtmlElement|null} */
    this.buttonFullExpand = null;
    /** @type {HtmlButtonElement|null} */
    this.buttonCopy = null;
    this.contentCopyButton = "";
    /** @type {HtmlElement|null} */
    this.spanContainerButtons = null;
    /** @type {boolean} */
    this.fullyExpanded = false;
    this.transitionStyle = "all 0.6s"
    if (typeof container === "string") {
      this.containerId = container;
      this.container = document.getElementById(this.containerId);
    } else {
      this.containerId = this.container.id;
    }
  }

  matchPanelStatus() {
    if (this.collapsed) {
      this.panelContent.style.maxHeight = "0px";
      this.panelContent.style.maxWidth = "0px";
    } else {
      if (!this.fullyExpanded && this.originalDimensionsKnown) {
        this.panelContent.style.maxHeight = `${this.originalHeight + 55}px`;
        this.panelContent.style.maxWidth = `${this.originalWidth + 35}px`;
      } else {
        this.panelContent.style.maxHeight = "";
        this.panelContent.style.maxWidth = "";
      }
    }
  }

  computeOriginalDimensions() {
    let computedStyle = window.getComputedStyle(this.panelContent);
    let height = parseInt(computedStyle.height);
    let width = parseInt(computedStyle.widows);
    this.originalHeight = Math.max(
      height,
      this.panelContent.scrollHeight,
    );
    this.originalWidth = Math.max(
      width,
      this.panelContent.scrollWidth,
    );
    if (isNaN(this.originalHeight) || isNaN(this.originalWidth)) {
      this.originalDimensionsKnown = false;
      this.originalHeight = 0;
      this.originalWidth = 0;
    }
  }

  setPanelContent(
    /** @type {HTMLElement|string} */
    input,
  ) {
    this.panelContent.style.maxHeight = "";
    this.panelContent.style.maxWidth = "";
    if (typeof input === "string") {
      let element = document.createElement("span");
      writeHTML(element, input);
      input = element;
    }
    this.panelContent.textContent = "";
    this.panelContent.appendChild(input);
    this.computeOriginalDimensions();
    this.matchPanelStatus();
  }

  setPanelLabel(input) {
    writeHTML(this.panelLabel, input);
  }

  initialize(
    /** @type {boolean} */
    startsCollapsed,
    /** @type {boolean} */
    allowFullExpand,
  ) {
    if (allowFullExpand === null || allowFullExpand === undefined) {
      allowFullExpand = false;
    }
    this.panelLabel = document.createElement("span");
    this.spanContainerButtons = document.createElement("span");
    this.button = document.createElement("button");
    this.button.className = "buttonProgress";
    this.button.classList.add("accordionLikeIndividual");
    this.button.addEventListener("click", () => {
      this.doToggleContent();
    });
    this.button.title = "Expand/collapse";
    this.button.appendChild(this.panelLabel);
    this.expandedMark = document.createElement("span");
    this.button.appendChild(this.expandedMark);
    this.spanContainerButtons.appendChild(this.button);
    if (allowFullExpand) {
      this.buttonFullExpand = document.createElement("button");
      // Character looks like four corners of a screen.
      this.buttonFullExpand.textContent = "\u26F6";
      this.buttonFullExpand.title = "toggle full expansion";
      this.buttonFullExpand.addEventListener("click", () => {
        this.toggleFullExpand();
      })
      this.buttonFullExpand.className = "buttonProgress";
      this.spanContainerButtons.appendChild(this.buttonFullExpand);
    }
    this.panelContent = document.createElement("div");
    this.panelContent.className = "PanelExpandable";
    this.container.textContent = "";
    this.container.appendChild(this.spanContainerButtons);
    this.container.appendChild(this.panelContent);
    this.setCollapsed(startsCollapsed);
  }

  addCopyButton() {
    if (this.panelContent === null || this.buttonFullExpand === null) {
      return;
    }
    /** @type {string} */
    let textContent = this.panelContent.textContent;
    if (!textContent.startsWith("\\(") || !textContent.endsWith("\\)")) {
      return;
    }
    this.contentCopyButton = textContent.substring(2, textContent.length - 2);
    this.buttonCopy = document.createElement("button");
    this.buttonCopy.title = "copy";
    this.buttonCopy.className = "buttonProgress";
    let tiny = document.createElement("tiny");
    writeHTML(tiny, "&#x1F4CB;");
    this.buttonCopy.appendChild(tiny);
    this.buttonCopy.addEventListener("click", () => {
      this.container.style.transition = this.transitionStyle;
      this.doCopy();
    });
    this.spanContainerButtons.appendChild(this.buttonCopy);
  }

  doCopy() {
    if (this.contentCopyButton === "") {
      return;
    }
    navigator.clipboard.writeText(this.contentCopyButton);
    /** @type {HTMLElement} */
    this.container.style.backgroundColor = "lightgreen";
    setTimeout(() => {
      this.container.style.backgroundColor = "";
    }, 1000);
  }

  setCollapsed(/** @type {boolean} */ newValue) {
    this.collapsed = newValue;
    if (this.collapsed) {
      this.expandedMark.textContent = "\u25C2"; // triangle pointing left
      this.container.setAttribute("collapsed", `${this.collapsed}`);
    } else {
      this.expandedMark.textContent = "\u25BC"; // triangle pointing down
      this.container.setAttribute("collapsed", `${this.collapsed}`);
    }
    setTimeout(() => {
      this.matchPanelStatus();
    }, 0);
  }

  toggleFullExpand() {
    this.fullyExpanded = !this.fullyExpanded;
    this.panelContent.style.transition = this.transitionStyle;
    if (this.fullyExpanded) {
      this.setCollapsed(false);
      return;
    }
    this.matchPanelStatus();
  }

  doToggleContent() {
    this.panelContent.style.transition = this.transitionStyle;
    this.setCollapsed(!this.collapsed);
  }
}

/**
 * Returns the panel, or null if the content is too small for a panel. 
 * @return {PanelExpandable|null}  
 */
function makePanelFromData(
  /** @type {PanelExpandableData} */
  data,
) {
  let doCreatePanel = false;
  if (data.content.length > data.minimalCharacterLengthForPanel) {
    doCreatePanel = true;
  }
  if (doCreatePanel) {
    let inputPanel = new PanelExpandable(data.container);
    inputPanel.initialize(data.startHidden, data.allowFullExpand);
    let content = data.content;
    inputPanel.setPanelContent(content);
    inputPanel.setPanelLabel(data.label);
    inputPanel.addCopyButton();
    return inputPanel;
  } else {
    let element = data.container;
    if (typeof element === "string") {
      element = document.getElementById(element);
    }
    if (element !== null) {
      writeHTML(element, data.content);
    }
    return null;
  }
}

class PanelFromToggleAndContent {
  constructor(
    /** @type {HTMLElement} */
    expandButton,
    /** @type {HTMLElement} */
    panel,
    /** @type {boolean} */
    startsExpanded
  ) {
    this.panel = panel;
    this.panel.style.overflow = "hidden";
    this.expandButton = expandButton;
    this.toggleMark = document.createElement("span");
    this.toggleMark.textContent = "◀";
    this.toggleMark.style.display = "inline-block";
    this.toggleMark.style.transition = "0.2s all";
    this.expandButton.appendChild(this.toggleMark);
    this.expanded = startsExpanded;
    this.originalElementHeight = this.panel.getBoundingClientRect().height;
    if (this.originalElementHeight === 0) {
      this.originalElementHeight = 50;
    }
    this.panel.style.transition = "0.2s all";
    this.expandButton.addEventListener("click", () => {
      this.expanded = !this.expanded;
      this.matchPanelStatus();
    });
    this.matchPanelStatus();
  }

  matchPanelStatus() {
    if (this.expanded) {
      this.panel.style.maxHeight = `${this.originalElementHeight}px`;
      this.panel.style.height = `${this.originalElementHeight}px`;
      this.toggleMark.style.transform = "rotate(-0.25turn)";
    } else {
      this.panel.style.maxHeight = "0px";
      this.panel.style.height = "0px";
      this.toggleMark.style.transform = "";
    }
  }
}

module.exports = {
  writeHTML,
  modifyHeightForTimeout,
  PanelExpandable,
  PanelExpandableData,
  PanelFromToggleAndContent,
  makePanelFromData,
  toggleHeight
};