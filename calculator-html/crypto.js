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
  /**@type {parsed} */
  this.parsed = null;
  this.positionInBinary = 0;
}

AbstractSyntaxOne.prototype.initializeAnnotation = function(
  inputBinaryHex, 
  inputInterpretation, 
  inputParsed, 
  /**@type {string} */ 
  inputIdAnnotation,
) {
  this.idAnnotation = inputIdAnnotation;
  this.binaryHex = inputBinaryHex;
  this.interpretation = inputInterpretation;
  this.parsed = inputParsed;
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

AbstractSyntaxOne.prototype.computeAnnotation = function(
  /**@type {HTMLElement} */ 
  outputDOMElements, 
  currentInterpretation,
) {
  var offsetHex = currentInterpretation.offset * 2;
  var lengthBytes = currentInterpretation.numberOfLengthBytes * 2;
  if (currentInterpretation.length >= 128) {
    lengthBytes += 2;
  }
  var elementLeadingByte = document.createElement("SPAN");
  elementLeadingByte.classList.add("abstractSyntaxOneLeadingByte"); 
  elementLeadingByte.innerHTML = currentInterpretation.startByteOriginal;
  attachTooltip(elementLeadingByte, `Leading byte: ${currentInterpretation.startByteOriginal}`);

  var elementLength = document.createElement("SPAN");
  elementLength.classList.add("abstractSyntaxOneLength");
  elementLength.innerHTML = currentInterpretation.lengthEncoding;
  attachTooltip(elementLength, `Length:`);

  var elementHeader = document.createElement("SPAN");
  elementHeader.classList.add("abstractSyntaxOneHeader");
  elementHeader.appendChild(elementLeadingByte);
  elementHeader.appendChild(elementLength);

  outputDOMElements.appendChild(elementHeader);
  var foundContent = false;
  if (Array.isArray(currentInterpretation.value)) {
    foundContent = true;
    for (var i = 0; i < currentInterpretation.value.length; i ++) {
      var interpretation = currentInterpretation.value[i];
      var domElement = document.createElement("SPAN");
      domElement.classList.add("abstractSyntaxOneElement");
      this.computeAnnotation(domElement, interpretation);
      outputDOMElements.appendChild(domElement);
    }  
  } else if (currentInterpretation.body !== undefined && currentInterpretation.body !== null) {
    var elementHex = document.createElement("SPAN");
    elementHex.innerHTML = currentInterpretation.body;
    elementHex.classList.add("abstractSyntaxOneContent");
    outputDOMElements.appendChild(elementHex);
    foundContent = true;
  } 
  if (!foundContent) {
    var noContent = document.createElement("SPAN");
    noContent.innerHTML = `[no content] ${JSON.stringify(currentInterpretation)}`;
    outputDOMElements.appendChild(noContent);
  }
}

AbstractSyntaxOne.prototype.annotate = function() {
  this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotation");
  this.DOMElementAnnotation.innerHTML = "";
  this.positionInBinary = 0;
  this.computeAnnotation(this.DOMElementAnnotation, this.interpretation);
  var debugElement = document.createElement("SPAN");
  debugElement.innerHTML = JSON.stringify(this.parsed);
  this.DOMElementAnnotation.appendChild(debugElement);
}

function abstractSyntaxNotationAnnotate(binaryHex, interpretation, parsed, id) {
  var annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, parsed, id); 
  annotation.annotate();
}

module.exports =  {
  abstractSyntaxNotationAnnotate,
};