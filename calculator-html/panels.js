/**
 * File needs to function as a stand-alone javascript 
 * as well be used as a commonJS module included via
 * require('panels.js')).
 * Please don't require(...) any modules from this file.
 **/


"use strict";

function modifyHeightForTimeout(currentButtonPanel, newHeight) { 
  currentButtonPanel.style.maxHeight = newHeight;
  currentButtonPanel.style.height    = newHeight;
}

function toggleHeight(currentButton, currentPanelID) { 
  var currentPanel = document.getElementById(currentPanelID);
  if (currentPanel === null) {
    return;
  }
  currentPanel.buttonThatModifiesMe = currentButton;
  if (currentPanel.startingMaxHeight === undefined || currentPanel.startingMaxHeight === null) { 
    currentPanel.startingMaxHeight = window.getComputedStyle(currentPanel).height;
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
  }
  var currentContainer = currentPanel;
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
    function() { 
      toggleHeightForTimeout(currentPanel);
    }, 0
  );
}

function transitionDone(event) { 
  //console.log("CAlled transitionDone");
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

var mainMenuIsHidden = false;
var mainMenuExpandedLength = null;
function toggleMenu() {
  var theMenuDiv = document.getElementById("divTheMainMenuPanel");
  var theToggleButton = document.getElementById("buttonToggleTheMainMenu");
  if (!mainMenuIsHidden) {
    if (mainMenuExpandedLength === null) {
      mainMenuExpandedLength = theMenuDiv.style.width;
    }
    for (var counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv ++) {
      var currentNode = theMenuDiv.childNodes[counterDiv];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div") {
        currentNode.style.display = "none";
      }
    }
    theMenuDiv.classList.add("divMainMenuCollapsed");
    mainMenuIsHidden = true;
    theToggleButton.innerHTML = "&#9656;";
    document.getElementById("divProfilePicture").classList.add("divProfilePictureContainerCollapsed");
  } else {
    for (var counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv ++) {
      var currentNode = theMenuDiv.childNodes[counterDiv];
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

function PanelExpandable(
  container, 
  /** @type {Boolean} */ 
  resetPanel,
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
  this.container = null;
  this.initialize(container, resetPanel);
}

PanelExpandable.prototype.matchPanelStatus = function() {
  var thePanel = document.getElementById(this.attributes.panelId);
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

PanelExpandable.prototype.setPanelContent = function(input) {
  var thePanel = document.getElementById(this.attributes.panelId);
  thePanel.style.maxHeight = "";
  thePanel.style.maxWidth = "";
  thePanel.innerHTML = input;
  var originalHeight = window.getComputedStyle(thePanel).height;
  var originalWidth = window.getComputedStyle(thePanel).width;
  this.container.setAttribute("originalHeight", originalHeight);
  this.container.setAttribute("originalWidth", originalWidth);
}

PanelExpandable.prototype.setPanelLabel = function(input) {
  var panelLabel = document.getElementById(this.attributes.panelLabelId);
  panelLabel.innerHTML = input;
}

PanelExpandable.prototype.initialize = function(container, resetPanel) {
  if (container === null || container === undefined || container === "") {
    return this;
  }
  this.container = container;
  if (typeof this.container === "string") {
    this.container = document.getElementById(this.container);
  }
  this.containerId = this.container.id;
  if (resetPanel !== true) { 
    for (var label in this.attributes) {
      var incoming = this.container.getAttribute(label);
      this.attributes[label] = incoming;
      if (incoming === null || incoming === undefined || incoming === undefined) {
        resetPanel = true;
        break;
      }
    }
  }
  if (resetPanel === true) {
    numberOfButtonToggleProgressReport ++;
    for (var label in this.attributes) {
      this.attributes[label] = `progressReport${label}${numberOfButtonToggleProgressReport}`;
    }
    this.attributes.panelStatus = "collapsed";
    this.initializeProgressPanel();
  }
}

PanelExpandable.prototype.setStatusToBeCalledThroughTimeout = function() {
  var panel = document.getElementById(this.attributes.panelId);
  if (this.attributes.panelStatus === "expanded") {
    panel.style.maxHeight = this.attributes.originalHeight;
    panel.style.maxWidth = this.attributes.originalWidth;
  } else {
    panel.style.maxHeight = "0px";
    panel.style.maxWidth = "0px";
  }
}

function doToggleContent(progressId) {
  var thePanel = new PanelExpandable(progressId); 
  thePanel.doToggleContent();
}

PanelExpandable.prototype.doToggleContent = function() {
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

var numberOfButtonToggleProgressReport = 0;

PanelExpandable.prototype.initializeProgressPanel = function() {
  for (var label in this.attributes) {
    this.container.setAttribute(label, this.attributes[label]);
  }  
  var spanContainer = document.createElement("span");
  var spanContent = "";
  spanContent += `<span id = "${this.attributes.panelLabelId}"></span><button id = "${this.attributes.buttonId}" `; 
  spanContent += `class = "buttonProgress accordionLikeIndividual" `;
  spanContent += `onclick = "window.calculator.panels.doToggleContent('${this.containerId}');">`;
  spanContent += `<span id = "${this.attributes.expandedMarkerId}">&#9666;</span>`;
  spanContent += "</button>";
  spanContent += `<div id = "${this.attributes.panelId}" class = "panelExpandable"></div>`;
  spanContainer.innerHTML = spanContent;
  while (this.container.firstChild) {
    this.container.removeChild(this.container.firstChild);
  }
  this.container.appendChild(spanContainer);
}

var module;
if (module === undefined) {
  module = {};
}

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
};