"use strict";

function AbstractSyntaxOne() {
  /**@type {string} */
  this.idAnnotation = null;
  /**@type {HTMLElement} */
  this.DOMElementAnnotation = null;
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
  this.DOMElementAnnotation = document.getElementById(inputIdAnnotation);
  if (this.DOMElementAnnotation === null) {
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
  currentElement.classList.add("abstractSyntaxOneElement");

  var elementLeadingByte = document.createElement("SPAN");
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
  attachTooltip(
    elementLeadingByte, 
    tooltipLeadingByte,
  );

  var elementLength = document.createElement("SPAN");
  elementLength.classList.add("abstractSyntaxOneLength");
  elementLength.innerHTML = currentInterpretation.lengthEncoding;
  var lengthTooltipContent = `Length: ${currentInterpretation.lengthPromised}`;
  attachTooltip(elementLength, lengthTooltipContent);

  var elementHeader = document.createElement("SPAN");
  elementHeader.classList.add("abstractSyntaxOneHeader");
  elementHeader.appendChild(elementLeadingByte);
  elementHeader.appendChild(elementLength);

  currentElement.appendChild(elementHeader);  
  var foundContent = false;
  if (currentInterpretation.children !== undefined) {
    foundContent = true;
    for (var i = 0; i < currentInterpretation.children.length; i ++) {
      var interpretation = currentInterpretation.children[i];
      this.appendAnnotation(currentElement, interpretation);
    }  
  } else if (currentInterpretation.body !== undefined && currentInterpretation.body !== null) {
    var elementHex = document.createElement("SPAN");
    elementHex.innerHTML = currentInterpretation.body;
    elementHex.classList.add("abstractSyntaxOneContent");
    var tooltipBody = "";
    if (currentInterpretation.interpretation !== undefined) {
      tooltipBody += `Interpretation: ${currentInterpretation.interpretation}`;
    }
    if (tooltipBody !== "") {
      attachTooltip(elementHex, tooltipBody);
    }
    currentElement.appendChild(elementHex);
    foundContent = true;
  } 
  if (
    currentInterpretation.error !== null && 
    currentInterpretation.error !== undefined
  ) {
    var errorElement = document.createElement("SPAN");
    errorElement.style.color = "red";
    errorElement.innerHTML = currentInterpretation.error;
    currentElement.appendChild(noContent);
  }
  if (!foundContent) {
    var errorElement = document.createElement("SPAN");
    errorElement.innerHTML = `[no content] ${JSON.stringify(currentInterpretation)}`;
    currentElement.appendChild(noContent);
  }
  container.appendChild(currentElement);
}

AbstractSyntaxOne.prototype.annotate = function() {
  this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotation");
  this.DOMElementAnnotation.innerHTML = "";
  this.positionInBinary = 0;
  this.appendAnnotation(this.DOMElementAnnotation, this.interpretation);
  var debugElement = document.createElement("SPAN");
  this.DOMElementAnnotation.appendChild(debugElement);
}

function abstractSyntaxNotationAnnotate(binaryHex, interpretation, id) {
  var annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, id); 
  annotation.annotate();
}

module.exports =  {
  abstractSyntaxNotationAnnotate,
};