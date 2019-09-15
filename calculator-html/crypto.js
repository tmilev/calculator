"use strict";

function AbstractSyntaxOne() {
  /**@type {string} */
  this.idAnnotation = null;
  /**@type {HTMLElement} */
  this.DOMElementAnnotation = null;
  /**@type {string} */
  this.annotationHTML = "";
  /**@type {string} */
  this.binaryHex = "";
  /**@type {Object} */
  this.interpretation = null;
  /**@type {parsed} */
  this.parsed = null;
}

AbstractSyntaxOne.prototype.initializeAnnotation = function (
  inputBinaryHex, 
  inputInterpretation, 
  inputParsed, 
  /**@type {string} */ inputIdAnnotation,
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

AbstractSyntaxOne.prototype.annotate = function () {
  this.annotationHTML = this.binaryHex;
  this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotation");
  this.DOMElementAnnotation.innerHTML = this.annotationHTML;
}

function abstractSyntaxNotationAnnotate(binaryHex, interpretation, parsed, id) {
  var annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, parsed, id); 
  annotation.annotate();
}

module.exports =  {
  abstractSyntaxNotationAnnotate,
};