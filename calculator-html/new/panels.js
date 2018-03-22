"use strict";

function modifyHeightForTimeout(currentButtonPanel, newHeight){ 
  currentButtonPanel.style.maxHeight = newHeight;
  currentButtonPanel.style.height    = newHeight;
}

function toggleHeight(currentButton, currentPanelID){ 
  var currentPanel = document.getElementById(currentPanelID);
  if (currentPanel === null){
    return;
  }
  currentPanel.buttonThatModifiesMe = currentButton;
  if (currentPanel.startingMaxHeight === undefined || currentPanel.startingMaxHeight === null){ 
    currentPanel.startingMaxHeight = window.getComputedStyle(currentPanel).height;
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
  }
  var currentContainer = currentPanel;
  while (true){ 
    currentContainer = currentContainer.parentNode;
    //console.log("processing "+ currentContainer.tagName+"."+ currentContainer.className);
    if (!currentContainer.className.startsWith("body")){
      break;
    }
    currentContainer.style.maxHeight = "";
    currentContainer.style.height = "";
    currentContainer.startingMaxHeight = null;
  }
  if (currentPanel.style.display === "none" || currentPanel.transitionState === "collapsed"){
    currentPanel.style.display = "";
  }
  if (currentPanel.transitionState === "collapsing" ||
      currentPanel.transitionState === "collapsed"){ 
    currentPanel.transitionState = "expanding";
    currentButton.innerHTML = "&#9660;";
  } else if (
    currentPanel.transitionState === null || currentPanel.transitionState === undefined ||
    currentPanel.transitionState === "expanding" || currentPanel.transitionState === "expanded"
  ){ 
    currentPanel.transitionState = "collapsing";
    currentButton.innerHTML = "&#9656;";
  }
  currentPanel.addEventListener("transitionend", transitionDone);
  setTimeout(function(){ 
    toggleHeightForTimeout(currentPanel);
  },
    0
  );
}

function transitionDone(event){ 
  //console.log("CAlled transitionDone");
  this.removeEventListener("transitionend", transitionDone);
  if (this.transitionState === "collapsing"){ 
    this.style.display = "none";
    this.transitionState = "collapsed";
    window.frontend.thePage.storeOneCollapsiblePanelSettings(this, true)
  } else if (this.transitionState === "expanding"){ 
    this.style.display = "";
    this.transitionState = "expanded";
    window.frontend.thePage.storeOneCollapsiblePanelSettings(this, false)
  }
}

function toggleHeightForTimeout(currentPanel){ 
  if (currentPanel.transitionState === "expanding"){ 
    currentPanel.style.maxHeight = currentPanel.startingMaxHeight;
    currentPanel.style.height = currentPanel.startingMaxHeight;
    currentPanel.style.opacity = "1";
  } else if (currentPanel.transitionState === "collapsing"){ 
    currentPanel.style.opacity = "0";
    currentPanel.style.maxHeight = "0px";
    currentPanel.style.height = "0px";
  }
}

var mainMenuIsHidden = false;
var mainMenuExpandedLength = null;
function toggleMenu(){
  var theMenuDiv = document.getElementById("divTheMainMenuPanel");
  var theToggleButton = document.getElementById("buttonToggleTheMainMenu");
  if (!mainMenuIsHidden){
    if (mainMenuExpandedLength === null){
      mainMenuExpandedLength = theMenuDiv.style.width;
    }
    for (var counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv ++){
      var currentNode = theMenuDiv.childNodes[counterDiv];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div"){
        currentNode.style.display = "none";
      }
    }
    theMenuDiv.classList.add("divMainMenuCollapsed");
    mainMenuIsHidden = true;
    theToggleButton.innerHTML = "&#9656;";
    document.getElementById("divProfilePicture").classList.add("divProfilePictureContainerCollapsed");
  } else {
    for (var counterDiv = 3; counterDiv < theMenuDiv.childNodes.length; counterDiv ++){
      var currentNode = theMenuDiv.childNodes[counterDiv];
      if (currentNode.nodeName === "DIV" || currentNode.nodeName === "div"){
        currentNode.style.display = "";
      }
    }
    theMenuDiv.classList.remove("divMainMenuCollapsed");
    mainMenuIsHidden = false;
    theToggleButton.innerHTML = "&#9660;";
    document.getElementById("divProfilePicture").classList.remove("divProfilePictureContainerCollapsed");
  }
}
