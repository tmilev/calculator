"use strict";
const miscellaneous = require('./miscellaneous');

function AbstractSyntaxOne() {
  /**@type {string} */
  this.idAnnotation = null;
  /**@type {HTMLElement} */
  this.DOMElementAnnotationContainer = null;
  /**@type {HTMLElement} */
  this.DOMElementAnnotation = null;
  /**@type {HTMLElement} */
  this.DOMElementAnnotationTree = null;
  /**@type {string} */
  this.binaryHex = "";
  /**@type {Object} */
  this.interpretation = null;
  this.positionInBinary = 0;
}

AbstractSyntaxOne.prototype.initializeAnnotation = function(
  inputBinaryHex, 
  inputInterpretation, 
  /**@type {string} */ 
  inputIdAnnotation,
) {
  this.idAnnotation = inputIdAnnotation;
  this.binaryHex = inputBinaryHex;
  this.interpretation = inputInterpretation;
  this.DOMElementAnnotationContainer = document.getElementById(inputIdAnnotation);
  if (this.DOMElementAnnotationContainer === null) {
    throw(`Element of id ${inputIdAnnotation} is missing. `);
  }
}

function attachTooltip(
  /**@type {HTMLElement} */
  element, 
  /**@string {string} */
  tooltipHTML,
) {
  var tooltip = document.createElement("SPAN");
  tooltip.innerHTML = tooltipHTML;
  tooltip.classList.add("tooltiptext");
  element.appendChild(tooltip);
  element.addEventListener("mouseover", showToolTip.bind(element, tooltip));
  element.addEventListener("mouseout", hideToolTip.bind(element, tooltip));
}

function showToolTip(
  /**@type {HTMLElement} */
  tooltipElement,
) {
  tooltipElement.style.visibility = "visible";
}

function hideToolTip(  
  /**@type {HTMLElement} */
  tooltipElement,
) {
  tooltipElement.style.visibility = "hidden";  
}

/**
 * @typedef {{
 *  body: string, 
 *  children: Array, 
 *  error: string, 
 *  lengthEncoding: string, 
 *  lengthPromised: number, 
 *  type: string,
 *  tag: string,
 *  startByteOriginal: string,
 *  numberOfChildren: number,
 *  offset: number}} ASNElement
*/

AbstractSyntaxOne.prototype.appendAnnotation = function(
  /**@type {HTMLElement} */ 
  container, 
  /**@type  {ASNElement}*/
  currentInterpretation,
) {
  var currentElement = document.createElement("SPAN");
  currentInterpretation.dom = {};
  currentInterpretation.dom.element = currentElement;
  currentElement.classList.add("abstractSyntaxOneElement");
  var elementLeadingByte = document.createElement("SPAN");
  currentInterpretation.dom.leadingByte = elementLeadingByte;
  elementLeadingByte.classList.add("abstractSyntaxOneLeadingByte"); 
  elementLeadingByte.innerHTML = currentInterpretation.startByteOriginal;
  var tooltipLeadingByte = `Type: ${currentInterpretation.type}`;
  tooltipLeadingByte += `<br>Leading byte: ${currentInterpretation.startByteOriginal}`;
  tooltipLeadingByte += `<br>Offset: ${currentInterpretation.offset}`;
  if (currentInterpretation.numberOfChildren) {
    tooltipLeadingByte += `<br># of sub-elements: ${currentInterpretation.numberOfChildren}`;
  }
  if (currentInterpretation.value !== undefined) {
    tooltipLeadingByte += `<br>Value: ${currentInterpretation.value}`;
  }
  if (currentInterpretation.comment !== undefined) {
    tooltipLeadingByte += "<br>";
    tooltipLeadingByte += `Comment: ${currentInterpretation.comment}`;
  }
  attachTooltip(
    elementLeadingByte, 
    tooltipLeadingByte,
  );

  var elementLength = document.createElement("SPAN");
  currentInterpretation.dom.length = elementLength;
  elementLength.classList.add("abstractSyntaxOneLength");
  elementLength.innerHTML = currentInterpretation.lengthEncoding;
  var lengthTooltipContent = `Length: ${currentInterpretation.lengthPromised}`;
  attachTooltip(elementLength, lengthTooltipContent);

  var elementHeader = document.createElement("SPAN");
  currentInterpretation.dom.header = elementHeader;
  elementHeader.classList.add("abstractSyntaxOneHeader");
  elementHeader.appendChild(elementLeadingByte);
  elementHeader.appendChild(elementLength);
  currentElement.appendChild(elementHeader);
  var elementBody = document.createElement("SPAN");
  currentInterpretation.dom.body = elementBody;
  var foundContent = false;
  if (currentInterpretation.children !== undefined) {
    foundContent = true;
    for (var i = 0; i < currentInterpretation.children.length; i ++) {
      var interpretation = currentInterpretation.children[i];
      this.appendAnnotation(elementBody, interpretation);
    }  
  } else if (currentInterpretation.body !== undefined && currentInterpretation.body !== null) {
    var elementHex = document.createElement("SPAN");
    elementHex.innerHTML = currentInterpretation.body;
    elementHex.classList.add("abstractSyntaxOneContent");
    elementBody.appendChild(elementHex);
    foundContent = true;
    var tooltipBody = "";
    if (currentInterpretation.interpretation !== undefined) {
      tooltipBody += `Interpretation: ${currentInterpretation.interpretation}`;
    }
    if (currentInterpretation.comment !== undefined) {
      if (tooltipBody !== "") {
        tooltipBody += "<br>";
      }
      tooltipBody += `Comment: ${currentInterpretation.comment}`;
    }
  
    if (tooltipBody !== "") {
      attachTooltip(elementBody, tooltipBody);
    }
  } 
  if (
    currentInterpretation.error !== null && 
    currentInterpretation.error !== undefined
  ) {
    var errorElement = document.createElement("SPAN");
    errorElement.style.color = "red";
    errorElement.innerHTML = currentInterpretation.error;
    elementBody.appendChild(noContent);
  }
  if (!foundContent) {
    var errorElement = document.createElement("SPAN");
    errorElement.innerHTML = `[no content] ${JSON.stringify(currentInterpretation)}`;
    elementBody.appendChild(noContent);
  }
  currentElement.appendChild(elementBody);  
  container.appendChild(currentElement);
}

AbstractSyntaxOne.prototype.mouseOverAbstractSyntaxOneElement = function(
  annotation,
  /**@type {HTMLElement} */ 
  annotationTreeElement,
) {
  annotation.leadingByte.style.backgroundColor = "gray";
  annotation.length.style.backgroundColor = "lightskyblue";
  annotation.body.style.backgroundColor = "lightgreen";
  annotationTreeElement.style.backgroundColor = "lightgray";
}

AbstractSyntaxOne.prototype.mouseOutAbstractSyntaxOneElement = function(
  annotation,
  /**@type {HTMLElement} */ 
  annotationTreeElement,
) {
  annotation.leadingByte.style.backgroundColor = "";
  annotation.length.style.backgroundColor = "";
  annotation.body.style.backgroundColor = "";
  annotationTreeElement.style.backgroundColor = "";
}

AbstractSyntaxOne.prototype.appendAnnotationTree = function(  
  /**@type {HTMLElement} */ 
  container, 
  /**@type  {ASNElement}*/
  currentInterpretation,
) {
  var currentElement = document.createElement("DIV");
  currentElement.classList.add("abstractSyntaxOneElementTree");
  var currentHead = document.createElement("SPAN");
  var headHTML = `<b>${currentInterpretation.type}</b>`;
  if (
    currentInterpretation.numberOfChildren !== undefined && 
    currentInterpretation.numberOfChildren !== 0
  ) {
    headHTML += ` [${currentInterpretation.numberOfChildren} elements]`;
  } 
  if (currentInterpretation.interpretation !== undefined) {
    headHTML += " " + miscellaneous.shortenString(15, currentInterpretation.interpretation);
  }
  currentHead.innerHTML = headHTML;
  currentElement.appendChild(currentHead);
  if (currentInterpretation.children !== undefined) {
    for (var counter = 0; counter < currentInterpretation.children.length; counter ++) {
      this.appendAnnotationTree(currentElement, currentInterpretation.children[counter]);
    }
  }
  var annotationElementPeer = currentInterpretation.dom;
  var elementsToAttachTo = [currentHead, annotationElementPeer.header];
  for (var counter = 0; counter < elementsToAttachTo.length; counter ++) {
    var theElement = elementsToAttachTo[counter];
    theElement.addEventListener(
      'mouseover', 
      this.mouseOverAbstractSyntaxOneElement.bind(
        this, 
        annotationElementPeer, 
        currentElement,
      ) 
    );
    theElement.addEventListener(
      'mouseout', 
      this.mouseOutAbstractSyntaxOneElement.bind(
        this, 
        annotationElementPeer, 
        currentElement,
      ) 
    );
  }
  container.appendChild(currentElement);
}

AbstractSyntaxOne.prototype.annotate = function() {
  this.DOMElementAnnotationContainer.innerHTML = "";
  this.DOMElementAnnotation = document.createElement("SPAN");
  this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotation");
  this.DOMElementAnnotation.innerHTML = "";
  this.DOMElementAnnotationTree = document.createElement("SPAN");
  this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotationTree");
  this.DOMElementAnnotation.innerHTML = "";
  var theTable = document.createElement("TABLE");
  var row = theTable.insertRow(0);
  var cellLeft = row.insertCell(- 1);
  var cellRight = row.insertCell(- 1);
  cellLeft.appendChild(this.DOMElementAnnotationTree);
  cellRight.appendChild(this.DOMElementAnnotation);
  this.DOMElementAnnotationContainer.appendChild(theTable);
  this.positionInBinary = 0;
  this.appendAnnotation(
    this.DOMElementAnnotation, 
    this.interpretation
  );
  this.appendAnnotationTree(    
    this.DOMElementAnnotationTree, 
    this.interpretation
  );
}

function abstractSyntaxNotationAnnotate(binaryHex, interpretation, id) {
  var annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, id); 
  annotation.annotate();
}

module.exports =  {
  abstractSyntaxNotationAnnotate,
};