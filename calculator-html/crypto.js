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

AbstractSyntaxOne.prototype.appendAnnotation = function(
  /**@type {HTMLElement} */ 
  container, 
  currentInterpretation,
) {
  var offset = currentInterpretation.offset;
  var currentElement = document.createElement("SPAN");
  currentElement.classList.add("abstractSyntaxOneElement");

  var elementLeadingByte = document.createElement("SPAN");
  elementLeadingByte.classList.add("abstractSyntaxOneLeadingByte"); 
  elementLeadingByte.innerHTML = currentInterpretation.startByteOriginal;
  var tooltipLeadingByte =     `Type: ${currentInterpretation.type}`;
  tooltipLeadingByte += `<br>Leading byte: ${currentInterpretation.startByteOriginal}`;
  tooltipLeadingByte += `<br>Leading bits: ${currentInterpretation.leadingBits}`;
  tooltipLeadingByte += `<br>Offset: ${offset}`;
  if (currentInterpretation.numberOfElements) {
    tooltipLeadingByte += `<br># of sub-elements: ${currentInterpretation.numberOfElements}`;
  }
  attachTooltip(
    elementLeadingByte, 
    tooltipLeadingByte,
  );

  var elementLength = document.createElement("SPAN");
  elementLength.classList.add("abstractSyntaxOneLength");
  elementLength.innerHTML = currentInterpretation.lengthEncoding;
  var lengthTooltipContent = `Length: ${currentInterpretation.length}`;
  attachTooltip(elementLength, lengthTooltipContent);

  var elementHeader = document.createElement("SPAN");
  elementHeader.classList.add("abstractSyntaxOneHeader");
  elementHeader.appendChild(elementLeadingByte);
  elementHeader.appendChild(elementLength);

  currentElement.appendChild(elementHeader);
  /**@type {Array} */
  var subList = null;
  if (Array.isArray(currentInterpretation.value)) {
    subList = currentInterpretation.value;
  } else if (Array.isArray(currentInterpretation.set)) {
    subList = currentInterpretation.set;
  }
  
  var foundContent = false;
  if (subList !== null) {
    foundContent = true;
    for (var i = 0; i < subList.length; i ++) {
      var interpretation = subList[i];
      this.appendAnnotation(currentElement, interpretation);
    }  
  } else if (currentInterpretation.body !== undefined && currentInterpretation.body !== null) {
    var elementHex = document.createElement("SPAN");
    elementHex.innerHTML = currentInterpretation.body;
    elementHex.classList.add("abstractSyntaxOneContent");
    var tooltipBody = "";
    if (currentInterpretation.value !== undefined) {
      tooltipBody += `Value: ${currentInterpretation.value}`;
    }
    if (currentInterpretation.interpretation !== undefined) {
      if (tooltipBody !== "") {
        tooltipBody += "<br>";
      }
      tooltipBody += `Interpretation: ${currentInterpretation.interpretation}`;
    }
    if (tooltipBody !== "") {
      attachTooltip(elementHex, tooltipBody);
    }
    currentElement.appendChild(elementHex);
    foundContent = true;
  } 
  if (!foundContent) {
    var noContent = document.createElement("SPAN");
    noContent.innerHTML = `[no content] ${JSON.stringify(currentInterpretation)}`;
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
  // debugElement.innerHTML = JSON.stringify(this.parsed);
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