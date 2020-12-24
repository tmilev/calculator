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

function PanelExpandableData(
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

class PanelExpandable {
  constructor(
    container,
  ) {
    this.attributes = {
      buttonId: null,
      panelId: null,
      expandedMarkerId: null,
      panelLabelId: null,
      panelStatus: null,
      originalHeight: "0px",
      originalWidth: "0px",
    }
    this.containerId = "";
    this.container = container;
    if (typeof container === "string") {
      this.containerId = container;
      this.container = document.getElementById(this.containerId);
    } else {
      this.containerId = this.container.id;
    }
  }

  matchPanelStatus() {
    let thePanel = document.getElementById(this.attributes.panelId);
    thePanel.style.maxHeight = "";
    thePanel.style.maxWidth = "";
    if (this.attributes.panelStatus === "collapsed") {
      thePanel.style.maxHeight = "0px";
      thePanel.style.maxWidth = "0px";
    } else {
      thePanel.style.maxHeight = this.attributes.originalHeight;
      thePanel.style.maxWidth = this.attributes.originalWidth;
    }
  }

  setPanelContent(input) {
    let thePanel = document.getElementById(this.attributes.panelId);
    thePanel.style.maxHeight = "";
    thePanel.style.maxWidth = "";
    thePanel.innerHTML = input;
    let originalHeight = window.getComputedStyle(thePanel).height;
    let originalWidth = window.getComputedStyle(thePanel).width;
    this.container.setAttribute("originalHeight", originalHeight);
    this.container.setAttribute("originalWidth", originalWidth);
  }

  setPanelLabel(input) {
    let panelLabel = document.getElementById(this.attributes.panelLabelId);
    panelLabel.innerHTML = input;
  }

  initialize(
    /** @type {boolean} */
    resetPanel,
  ) {
    if (this.container === null || this.container === undefined || this.container === "") {
      return this;
    }
    if (typeof this.container === "string") {
      this.container = document.getElementById(this.container);
    }
    this.containerId = this.container.id;
    if (resetPanel !== true) {
      for (let label in this.attributes) {
        let incoming = this.container.getAttribute(label);
        this.attributes[label] = incoming;
        if (incoming === null || incoming === undefined || incoming === undefined) {
          resetPanel = true;
          break;
        }
      }
    }
    if (resetPanel === true) {
      numberOfButtonToggleProgressReport++;
      for (let label in this.attributes) {
        this.attributes[label] = `progressReport${label}${numberOfButtonToggleProgressReport}`;
      }
      this.attributes.panelStatus = "collapsed";
      this.initializeProgressPanel();
    }
  }

  setStatusToBeCalledThroughTimeout() {
    let panel = document.getElementById(this.attributes.panelId);
    if (this.attributes.panelStatus === "expanded") {
      panel.style.maxHeight = this.attributes.originalHeight;
      panel.style.maxWidth = this.attributes.originalWidth;
    } else {
      panel.style.maxHeight = "0px";
      panel.style.maxWidth = "0px";
    }
  }

  doToggleContent() {
    if (this.attributes.panelStatus === "collapsed") {
      document.getElementById(this.attributes.expandedMarkerId).innerHTML = "&#9662;";
      this.attributes.panelStatus = "expanded";
    } else {
      document.getElementById(this.attributes.expandedMarkerId).innerHTML = "&#9666;";
      this.attributes.panelStatus = "collapsed";
    }
    this.container.setAttribute("panelStatus", this.attributes.panelStatus);
    setTimeout(this.setStatusToBeCalledThroughTimeout.bind(this), 0);
  }

  initializeProgressPanel() {
    for (let label in this.attributes) {
      this.container.setAttribute(label, this.attributes[label]);
    }
    let spanContainer = document.createElement("span");
    let spanContent = "";
    spanContent += `<span id = "${this.attributes.panelLabelId}"></span><button id = "${this.attributes.buttonId}" `;
    spanContent += `class = "buttonProgress accordionLikeIndividual" `;
    spanContent += `onclick = "window.calculator.panels.doToggleContent('${this.containerId}');">`;
    spanContent += `<span id = "${this.attributes.expandedMarkerId}">&#9666;</span>`;
    spanContent += "</button>";
    spanContent += `<div id = "${this.attributes.panelId}" class = "PanelExpandable"></div>`;
    spanContainer.innerHTML = spanContent;
    while (this.container.firstChild) {
      this.container.removeChild(this.container.firstChild);
    }
    this.container.appendChild(spanContainer);
  }
}

function makePanelFromData(
  /** @type {PanelExpandableData} */
  data
) {
  let doCreatePanel = false;
  if (data.content.length > data.minimalCharacterLengthForPanel) {
    doCreatePanel = true;
  }
  if (doCreatePanel) {
    let inputPanel = new PanelExpandable(data.id);
    inputPanel.initialize(true);
    inputPanel.setPanelContent(data.content);
    inputPanel.setPanelLabel(data.label);
    if (!data.startHidden) {
      inputPanel.doToggleContent();
    }
    inputPanel.matchPanelStatus();
  } else {
    let element = document.getElementById(data.id);
    if (element != null) {
      element.innerHTML = data.content;
    }
  }
}

function doToggleContent(progressId) {
  let thePanel = new PanelExpandable(progressId);
  thePanel.initialize(false);
  thePanel.doToggleContent();
}

let numberOfButtonToggleProgressReport = 0;

if (window.calculator === undefined) {
  window.calculator = {};
}

if (window.calculator.panels === undefined) {
  window.calculator.panels = {};
}

if (window.calculator.panels.doToggleContent === undefined) {
  window.calculator.panels.doToggleContent = doToggleContent;
}

module.exports = {
  modifyHeightForTimeout,
  toggleMenu,
  doToggleContent,
  PanelExpandable,
  PanelExpandableData,
  makePanelFromData,
  toggleHeight
};