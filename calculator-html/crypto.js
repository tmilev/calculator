"use strict";
const miscellaneous = require('./miscellaneous');
const styles = require('./styles');
const jsonToHtml = require('./json_to_html');

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
  var annotationElementPeer = currentInterpretation.dom;
  var elementsToAttachTo = [currentHead, annotationElementPeer.header];
  if (currentInterpretation.children !== undefined) {
    for (var counter = 0; counter < currentInterpretation.children.length; counter ++) {
      this.appendAnnotationTree(currentElement, currentInterpretation.children[counter]);
    }
  } else {
    elementsToAttachTo.push(annotationElementPeer.body);
  }
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

function TransportLayerSecurityServer() {
}

function AnnotatedBytes() {

}

AnnotatedBytes.prototype.getStackTop = function() {
  while (true) {
    if (this.stack.length == 0) {
      this.top = null;
      return;
    }
    this.top = this.stack[this.stack.length - 1];
    if (this.nextByteOffset < this.top.offset + this.top.length) {
      return;
    }
    this.top.flushBody();
    this.stack.pop();
  }
  // return null;
}

AnnotatedBytes.prototype.stackOnTop = function() {
  while (this.nextMarkerOffset < this.markers.length) {
    var nextMarker = this.markers[this.nextMarkerOffset];
    if (nextMarker.offset > this.nextByteOffset) {
      break;
    }
    if (nextMarker.offset == this.nextByteOffset) {
      this.top.flushBody();
      var newElement = document.createElement("SPAN");
      this.top.dom.appendChild(newElement);
      this.stack.push(
        new StackElement(
          this.nextByteOffset,
          nextMarker.length,
          nextMarker.label,
          newElement,
          [],
        )
      );
      this.top = this.stack[this.stack.length - 1];
    }
    this.nextMarkerOffset ++;
  }
}

function StackElement(
  /** @type {number} */
  offset, 
  /** @type {number} */
  length, 
  /** @type {string} */
  label, 
  /** @type {HTMLElement} */
  dom, 
  /** @type {Array<string>} */
  currentBody
) {
  this.offset = offset;
  this.length = length;
  this.label = label;
  this.dom = dom;
  this.currentBody = currentBody;
}

StackElement.prototype.flushBody = function() {
  if (this.currentBody === null) {
    return;
  }
  if (this.currentBody.length <= 0) {
    return;
  }
  var bodyElement = document.createElement("SPAN");
  bodyElement.innerHTML = this.currentBody.join("");
  bodyElement.className = "byteAnnotationBody";
  var tooltipContent = "";
  tooltipContent += `Offset: ${this.offset}, length: ${this.length}`
  if (this.label !== "" && this.label !== undefined) {
    tooltipContent += `<br>label: ${this.label}`;
  }
  attachTooltip(bodyElement, tooltipContent);
  this.dom.appendChild(bodyElement);
}

AnnotatedBytes.prototype.writeMessageToDOM = function(
  input,
  /**@type {HTMLElement} */
  outputComponent,
) {
  var serialization = input.serialization;
  this.bodyHex = serialization.body;
  this.byteLength = this.bodyHex.length / 2;
  this.stack = [new StackElement(
    0,
    this.byteLength,
    "allMessages",
    outputComponent,
    [],
  )];
  this.nextByteOffset = 0;
  this.nextMarkerOffset = 0;
  this.markers = serialization.markers;
  this.top = this.stack[0];
  while (this.nextByteOffset < this.byteLength) {
    this.getStackTop();
    if (this.top == null) {
      break;
    }
    this.stackOnTop();
    if (this.top == null) {
      break;
    }
    this.top.currentBody.push(this.bodyHex.slice(this.nextByteOffset * 2, this.nextByteOffset * 2 + 2));
    this.nextByteOffset ++;
  }
  if (this.top.currentBody.length > 0) {
    this.top.flushBody();
  }
  var inputStringifiedElement = document.createElement("SPAN");
  // inputStringifiedElement.innerHTML = "<br>" + JSON.stringify(input);
  outputComponent.appendChild(inputStringifiedElement);
}

TransportLayerSecurityServer.prototype.displayMessages = function(
  /**@type {string} */
  outputId, 
  input,
) {
  var outputElement = document.getElementById(outputId);
  outputElement.className = "hexContainerStandardWidth";
  writeSessionToDOM(input.session, outputElement);
  var inputHeader = document.createElement("span");
  inputHeader.innerHTML = "<br>";
  outputElement.appendChild(inputHeader);
  for (var i = 0; i < input.spoofer.inputMessages.length; i ++) {
    var currentStringHeader = document.createElement("SPAN");
    currentStringHeader.innerHTML = `<br><b>Input ${i + 1}:</b><br>`;
    outputElement.appendChild(currentStringHeader);
    var nextInput = document.createElement("span");
    nextInput.className = "hexContainerStandard";
    var annotation = new AnnotatedBytes();
    annotation.writeMessageToDOM(input.spoofer.inputMessages[i], nextInput);
    outputElement.appendChild(nextInput);
  }
  if (input.spoofer.errorsOnInput.length > 0) {
    var inputErrors = document.createElement("span");
    var errorHTML = "<br><b style = 'color:red'>Input errors:</b>";
    for (var i = 0; i < input.spoofer.errorsOnInput.length; i ++) {
      errorHTML += "<br>" + input.spoofer.errorsOnInput[i];
    }
    inputErrors.innerHTML = errorHTML;
    outputElement.appendChild(inputErrors);
  }
  var outputHeader = document.createElement("span");
  outputHeader.innerHTML = "<br>";
  outputElement.appendChild(outputHeader);
  for (var i = 0; i < input.spoofer.outputMessages.length; i ++) {
    var outputHeader = document.createElement("span");
    outputHeader.innerHTML = `<br><b>Output ${i+1}:</b><br>`;
    outputElement.appendChild(outputHeader);
    var currentOutputContainer = document.createElement("span");
    currentOutputContainer.className = "hexContainerStandard";
    for (var j = 0; j < input.spoofer.outputMessages[i].length; j ++) {
      var nextOutput = document.createElement("span");
      var annotation = new AnnotatedBytes();
      annotation.writeMessageToDOM(input.spoofer.outputMessages[i][j], nextOutput);
      currentOutputContainer.appendChild(nextOutput);
      if (j < input.spoofer.outputMessages[i].length - 1) {
        var separator = document.createElement("span");
        separator.innerHTML = "<span style = 'background-color:red;display:inline-block;'>&nbsp;</span>";
        currentOutputContainer.appendChild(separator);
      }
    }
    outputElement.appendChild(currentOutputContainer);
  }
  if (input.spoofer.errorsOnOutput.length > 0) {
    var outputErrors = document.createElement("span");
    var errorHTML = "Output errors:<br>";
    for (var i = 0; i < input.spoofer.errorsOnOutput.length; i ++) {
      errorHTML += input.spoofer.errorsOnOutput[i] + "<br>";
    }
    outputErrors.innerHTML = errorHTML;
    outputElement.appendChild(outputErrors);
  }
  if (input.spoofer.errorsOnOutput.length > 0) {
    var inputErrors = document.createElement("span");
    var errorHTML = "<br><b style = 'color:red'>Output errors:</b><br>";
    for (var i = 0; i < input.spoofer.errorsOnOutput.length; i ++) {
      errorHTML += "<br>" + input.spoofer.errorsOnOutput[i];
    }
    inputErrors.innerHTML = errorHTML;
    outputElement.appendChild(inputErrors);
  }
  var sessionStatusElement = document.createElement("SPAN");
  if (outputElement.style.height < 800) {
    outputElement.style.height = 800;
  }
  if (outputElement.style.width < 1000) {
    outputElement.style.width = 1000;
  }
}

function abstractSyntaxNotationAnnotate(binaryHex, interpretation, id) {
  var annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, id); 
  annotation.annotate();
}

function displayTransportLayerSecurity(  
  /**@type {string} */
  outputId, 
  input,
) {
  var theServer = new TransportLayerSecurityServer();
  theServer.displayMessages(outputId, input);
}

function writeSessionToDOM( 
  session,
  /**@type {HTMLElement} */
  outputElement,
) {
  var htmlContent = "";
  htmlContent += `<table class = '${styles.classNames.table.borderStandard}'>`;
  var labelsToIgnore = {
    "cipherSuites" : true,
    "algorithmSpecifications": true,
  };
  for (var label in session) {
    if (label in labelsToIgnore) {
      continue;
    }
    htmlContent += `<tr><td>${label}</td><td>${session[label]}</td></tr>`;
  }
  htmlContent += "</table>";
  htmlContent += `<table class = '${styles.classNames.table.borderStandard}'><tr><th>id</th><th>interpretation</th></tr>`;
  for (var label in session.cipherSuites) {
    htmlContent += `<tr>`;
    htmlContent += `<td>${label}</td>`;
    htmlContent += `<td>${session.cipherSuites[label]}</td>`;
    htmlContent += `</tr>`;
  }
  htmlContent += "</table>";
  var newChild = document.createElement("SPAN");
  newChild.innerHTML = htmlContent;
  outputElement.appendChild(newChild);
  var algorithmSpecs = document.createElement("SPAN");
  jsonToHtml.writeJSONtoDOMComponent(session.algorithmSpecifications, algorithmSpecs);  
  outputElement.appendChild(algorithmSpecs);  
}

function displaySSLRecord(
  /**@type {string} */
  outputId, 
  input,
) {
  var outputElement = document.getElementById(outputId);
  var annotation = new AnnotatedBytes();
  outputElement.className = styles.classNames.containers.hexStandardWidth;
  annotation.writeMessageToDOM(input, outputElement);
  var extraAnnotation = "";
  var content = input.content;
  writeSessionToDOM(input.session, outputElement);
  var flagNames = ["renegotiate", "OCSPrequest", "signedCertificateTimestampRequest"];
  for (var counter = 0; counter < flagNames.length; counter ++) {
    var flagName = flagNames[counter];
    if (content[flagName] !== undefined) {
      extraAnnotation += `<br>${flagName}: ${content[flagName]}`;
    }
  }
  extraAnnotation += `<br>Session id: ${content.sessionId}`;
  extraAnnotation += `<br>Cipher spec length: ${content.cipherSpecLength}`;
  extraAnnotation += `<br>Renegotiation characters: ${content.renegotiationCharacters}`;
  extraAnnotation += `<table class = '${styles.classNames.table.borderStandard}'><tr><th>type</th><th>name</th><th width = '50%'>data</th></tr>`;
  for (var counter = 0; counter < content.extensions.length; counter ++) {
    var label = content.extensions[counter].name;
    var type = content.extensions[counter].type;
    var data = content.extensions[counter].data;
    extraAnnotation += `<tr>`;
    extraAnnotation += `<td>${type}</td>`;
    extraAnnotation += `<td>${label}</td>`;
    extraAnnotation += `<td>${data}</td>`;
    extraAnnotation += `</tr>`;
  }
  extraAnnotation += "</table>";
  // extraAnnotation += JSON.stringify(content);
  var annotationAnnotation = document.createElement("SPAN");
  annotationAnnotation.innerHTML += extraAnnotation;
  outputElement.appendChild(annotationAnnotation);
}

module.exports =  {
  abstractSyntaxNotationAnnotate,
  displayTransportLayerSecurity,
  displaySSLRecord,
};