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
    currentButton.innerHTML = "&#9660;";
  } else if (
    currentPanel.transitionState === null || currentPanel.transitionState === undefined ||
    currentPanel.transitionState === "expanding" || currentPanel.transitionState === "expanded"
  ) {
    currentPanel.transitionState = "collapsing";
    currentButton.innerHTML = "&#9656;";
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
    window.frontend.thePage.storeOneCollapsiblePanelSettings(this, true);
  } else if (this.transitionState === "expanding") {
    this.style.display = "";
    this.transitionState = "expanded";
    window.frontend.thePage.storeOneCollapsiblePanelSettings(this, false);
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

let mainMenuIsHidden = false;
let mainMenuExpandedLength = null;
function toggleMenu() {
  let theMenuDiv = document.getElementById("divTheMainMenuPanel");
  let theToggleButton = document.getElementById("buttonToggleTheMainMenu");
  if (!mainMenuIsHidden) {
    if (mainMenuExpandedLength === null) {
      mainMenuExpandedLength = theMenuDiv.style.width;
    }
    for (let counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv++) {
      let currentNode = theMenuDiv.childNodes[counterDiv];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div") {
        currentNode.style.display = "none";
      }
    }
    theMenuDiv.classList.add("divMainMenuCollapsed");
    mainMenuIsHidden = true;
    theToggleButton.innerHTML = "&#9656;";
    document.getElementById("divProfilePicture").classList.add("divProfilePictureContainerCollapsed");
  } else {
    for (let counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv++) {
      let currentNode = theMenuDiv.childNodes[counterDiv];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div") {
        currentNode.style.display = "";
      }
    }
    theMenuDiv.classList.remove("divMainMenuCollapsed");
    mainMenuIsHidden = false;
    theToggleButton.innerHTML = "&#9660;";
    document.getElementById("divProfilePicture").classList.remove("divProfilePictureContainerCollapsed");
  }
}

class PanelExpandableData {
  constructor(
    /** @type {string} */
    content,
    /** @type {string} */
    id,
    /** @type {number} */
    minimalCharacterLengthForPanel,
    /** @type {boolean} */
    startHidden,
    /** @type {string} */
    label,
  ) {
    this.content = content;
    this.id = id;
    this.minimalCharacterLengthForPanel = minimalCharacterLengthForPanel;
    this.startHidden = startHidden;
    this.label = label;
    if (this.startHidden === undefined) {
      this.startHidden = false;
    }
    if (this.label === undefined) {
      this.label = "";
    }
  }
}

class PanelExpandable {
  constructor(
    /**@type{HtmlElement|string} */
    container,
  ) {
    this.originalHeight = "0px";
    this.originalWidth = "0px";
    this.collapsed = true;
    this.containerId = "";
    this.container = container;
    /**@type{HtmlElement|null} */
    this.panelContent = null;
    /**@type{HtmlElement|null} */
    this.panelLabel = null;
    /**@type{HtmlElement|null} */
    this.expandedMark = null;
    /**@type{HtmlElement|null} */
    this.button = null;
    if (typeof container === "string") {
      this.containerId = container;
      this.container = document.getElementById(this.containerId);
    } else {
      this.containerId = this.container.id;
    }
  }

  matchPanelStatus() {
    this.panelContent.style.maxHeight = "";
    this.panelContent.style.maxWidth = "";
    if (this.collapsed) {
      this.panelContent.style.maxHeight = "0px";
      this.panelContent.style.maxWidth = "0px";
    } else {
      this.panelContent.style.maxHeight = `${this.originalHeight + 45}`;
      this.panelContent.style.maxWidth = `${this.originalWidth + 35}`;
    }
  }

  setPanelContent(
    /**@type{HTMLElement|string} */
    input,
  ) {
    this.panelContent.style.maxHeight = "";
    this.panelContent.style.maxWidth = "";
    if (typeof input === "string") {
      let element = document.createElement("span");
      element.innerHTML = input;
      input = element;
    }
    this.panelContent.textContent = "";
    this.panelContent.appendChild(input);
    this.originalHeight = parseInt(window.getComputedStyle(this.panelContent).height);
    this.originalWidth = parseInt(window.getComputedStyle(this.panelContent).width);
    this.matchPanelStatus();
  }

  setPanelLabel(input) {
    this.panelLabel.innerHTML = input;
  }

  initialize(
    /**@type{boolean} */
    startsCollapsed,
  ) {
    let spanContainer = document.createElement("span");
    this.panelLabel = document.createElement("span");
    this.button = document.createElement("button");
    this.button.className = "buttonProgress";
    this.button.classList.add("accordionLikeIndividual");
    this.button.addEventListener("click", () => {
      this.doToggleContent();
    });
    this.button.appendChild(this.panelLabel);
    this.expandedMark = document.createElement("span");
    this.button.appendChild(this.expandedMark);
    spanContainer.appendChild(this.button);
    this.panelContent = document.createElement("div");
    this.panelContent.className = "PanelExpandable";
    spanContainer.appendChild(this.panelContent);
    this.container.textContent = "";
    this.container.appendChild(spanContainer);
    this.setCollapsed(startsCollapsed);
  }

  setCollapsed(/**@type{boolean} */ newValue) {
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

  doToggleContent() {
    this.setCollapsed(!this.collapsed);
  }
}

function makePanelFromData(
  /** @type {PanelExpandableData} */
  data,
) {
  let doCreatePanel = false;
  if (data.content.length > data.minimalCharacterLengthForPanel) {
    doCreatePanel = true;
  }
  if (doCreatePanel) {
    let inputPanel = new PanelExpandable(data.id);
    inputPanel.initialize(data.startHidden);
    inputPanel.setPanelContent(data.content);
    inputPanel.setPanelLabel(data.label);
  } else {
    let element = document.getElementById(data.id);
    if (element != null) {
      element.innerHTML = data.content;
    }
  }
}

let numberOfButtonToggleProgressReport = 0;

if (window.calculator === undefined) {
  window.calculator = {};
}

if (window.calculator.panels === undefined) {
  window.calculator.panels = {};
}

module.exports = {
  modifyHeightForTimeout,
  toggleMenu,
  PanelExpandable,
  PanelExpandableData,
  makePanelFromData,
  toggleHeight
};