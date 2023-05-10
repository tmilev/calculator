"use strict";
const miscellaneous = require("./miscellaneous_frontend");
const styles = require('./styles');
const jsonToHtml = require('./json_to_html');

function attachTooltip(
  /** @type {HTMLElement} */
  element,
  /**@string {string} */
  tooltipHTML,
) {
  let tooltip = document.createElement("SPAN");
  miscellaneous.writeHTML(tooltip, tooltipHTML);
  tooltip.classList.add("tooltiptext");
  element.appendChild(tooltip);
  element.addEventListener("mouseover", showToolTip.bind(element, tooltip));
  element.addEventListener("mouseout", hideToolTip.bind(element, tooltip));
}

function showToolTip(
  /** @type {HTMLElement} */
  tooltipElement,
) {
  tooltipElement.style.visibility = "visible";
}

function hideToolTip(
  /** @type {HTMLElement} */
  tooltipElement,
) {
  tooltipElement.style.visibility = "hidden";
}


function abstractSyntaxNotationAnnotate(binaryHex, interpretation, id) {
  let annotation = new AbstractSyntaxOne();
  annotation.initializeAnnotation(binaryHex, interpretation, id);
  annotation.annotate();
}

function displayTransportLayerSecurity(
  /** @type {string} */
  outputId,
  input,
) {
  let theServer = new TransportLayerSecurityServer();
  theServer.displayMessages(outputId, input);
}

function writeSessionToDOM(
  session,
  /** @type {HTMLElement} */
  outputElement,
) {
  let htmlContent = "";
  htmlContent += `<table class = '${styles.classNames.table.borderStandard}'>`;
  let labelsToIgnore = {
    "cipherSuites": true,
    "algorithmSpecifications": true,
  };
  for (let label in session) {
    if (label in labelsToIgnore) {
      continue;
    }
    htmlContent += `<tr><td>${label}</td><td>${session[label]}</td></tr>`;
  }
  htmlContent += "</table>";
  htmlContent += `<table class = '${styles.classNames.table.borderStandard}'><tr><th>id</th><th>interpretation</th></tr>`;
  for (let label in session.cipherSuites) {
    htmlContent += `<tr>`;
    htmlContent += `<td>${label}</td>`;
    htmlContent += `<td>${session.cipherSuites[label]}</td>`;
    htmlContent += `</tr>`;
  }
  htmlContent += "</table>";
  let newChild = document.createElement("SPAN");
  newChild.innerHTML = htmlContent;
  outputElement.appendChild(newChild);
  let algorithmSpecs = document.createElement("SPAN");
  jsonToHtml.writeJSONtoDOMComponent(session.algorithmSpecifications, algorithmSpecs);
  outputElement.appendChild(algorithmSpecs);
}

function displaySSLRecord(
  /** @type {string} */
  outputId,
  input,
) {
  let outputElement = document.getElementById(outputId);
  let annotation = new AnnotatedBytes();
  outputElement.className = styles.classNames.containers.hexStandardWidth;
  annotation.writeMessageToDOM(input, outputElement);
  let extraAnnotation = "";
  let content = input.content;
  writeSessionToDOM(input.session, outputElement);
  let flagNames = ["renegotiate", "OCSPrequest", "signedCertificateTimestampRequest"];
  for (let counter = 0; counter < flagNames.length; counter++) {
    let flagName = flagNames[counter];
    if (content[flagName] !== undefined) {
      extraAnnotation += `<br>${flagName}: ${content[flagName]}`;
    }
  }
  extraAnnotation += `<br>Session id: ${content.sessionId}`;
  extraAnnotation += `<br>Cipher spec length: ${content.cipherSpecLength}`;
  extraAnnotation += `<br>Renegotiation characters: ${content.renegotiationCharacters}`;
  extraAnnotation += `<table class = '${styles.classNames.table.borderStandard}'><tr><th>type</th><th>name</th><th width = '50%'>data</th></tr>`;
  for (let counter = 0; counter < content.extensions.length; counter++) {
    let label = content.extensions[counter].name;
    let type = content.extensions[counter].type;
    let data = content.extensions[counter].data;
    extraAnnotation += `<tr>`;
    extraAnnotation += `<td>${type}</td>`;
    extraAnnotation += `<td>${label}</td>`;
    extraAnnotation += `<td>${data}</td>`;
    extraAnnotation += `</tr>`;
  }
  extraAnnotation += "</table>";
  // extraAnnotation += JSON.stringify(content);
  let annotationAnnotation = document.createElement("SPAN");
  annotationAnnotation.innerHTML += extraAnnotation;
  outputElement.appendChild(annotationAnnotation);
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
 *  offsetLastRead: number,
 *  offsetLastWrite: number,
 *  offset: number,
 * }} ASNElement
*/

class AbstractSyntaxOne {
  constructor() {
    /** @type {string} */
    this.idAnnotation = null;
    /** @type {HTMLElement} */
    this.DOMElementAnnotationContainer = null;
    /** @type {HTMLElement} */
    this.DOMElementAnnotation = null;
    /** @type {HTMLElement} */
    this.DOMElementAnnotationTree = null;
    /** @type {string} */
    this.binaryHex = "";
    /** @type {Object} */
    this.interpretation = null;
    this.positionInBinary = 0;
  }

  initializeAnnotation(
    inputBinaryHex,
    inputInterpretation,
    /** @type {string} */
    inputIdAnnotation,
  ) {
    this.idAnnotation = inputIdAnnotation;
    this.binaryHex = inputBinaryHex;
    this.interpretation = inputInterpretation;
    this.DOMElementAnnotationContainer = document.getElementById(inputIdAnnotation);
    if (this.DOMElementAnnotationContainer === null) {
      throw (`Element of id ${inputIdAnnotation} is missing. `);
    }
  }

  appendAnnotation(
    /** @type {HTMLElement} */
    container,
    /** @type  {ASNElement} */
    currentInterpretation,
  ) {
    let currentElement = document.createElement("SPAN");
    currentInterpretation.dom = {};
    currentInterpretation.dom.element = currentElement;
    currentElement.classList.add("abstractSyntaxOneElement");
    let elementLeadingByte = document.createElement("SPAN");
    currentInterpretation.dom.leadingByte = elementLeadingByte;
    elementLeadingByte.classList.add("abstractSyntaxOneLeadingByte");
    elementLeadingByte.innerHTML = currentInterpretation.startByteOriginal;
    let tooltipLeadingByte = `Type: ${currentInterpretation.type}`;
    tooltipLeadingByte += `<br>Leading byte: ${currentInterpretation.startByteOriginal}`;
    // offsetLastWrite is the offset in the recoded stream.
    // offsetLastRead is the offset in the original stream.
    // offset: general offset, not clear whether in the original or recoded stream.
    let offset = currentInterpretation.offsetLastWrite;
    if (offset === undefined) {
      offset = currentInterpretation.offset;
    }
    if (offset !== undefined) {
      tooltipLeadingByte += `<br>Offset: ${offset}`;
    }
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

    let elementLength = document.createElement("SPAN");
    currentInterpretation.dom.length = elementLength;
    elementLength.classList.add("abstractSyntaxOneLength");
    elementLength.innerHTML = currentInterpretation.lengthEncoding;
    let lengthTooltipContent = `Length: ${currentInterpretation.lengthPromised}`;
    attachTooltip(elementLength, lengthTooltipContent);

    let elementHeader = document.createElement("SPAN");
    currentInterpretation.dom.header = elementHeader;
    elementHeader.classList.add("abstractSyntaxOneHeader");
    elementHeader.appendChild(elementLeadingByte);
    elementHeader.appendChild(elementLength);
    currentElement.appendChild(elementHeader);
    let elementBody = document.createElement("SPAN");
    currentInterpretation.dom.body = elementBody;
    let foundContent = false;
    if (currentInterpretation.children !== undefined) {
      foundContent = true;
      for (let i = 0; i < currentInterpretation.children.length; i++) {
        let interpretation = currentInterpretation.children[i];
        this.appendAnnotation(elementBody, interpretation);
      }
    } else if (currentInterpretation.body !== undefined && currentInterpretation.body !== null) {
      let elementHex = document.createElement("SPAN");
      elementHex.innerHTML = currentInterpretation.body;
      elementHex.classList.add("abstractSyntaxOneContent");
      elementBody.appendChild(elementHex);
      foundContent = true;
      let tooltipBody = "";
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
      let errorElement = document.createElement("SPAN");
      errorElement.style.color = "red";
      errorElement.innerHTML = currentInterpretation.error;
      elementBody.appendChild(noContent);
    }
    if (!foundContent) {
      let errorElement = document.createElement("SPAN");
      errorElement.innerHTML = `[no content] ${JSON.stringify(currentInterpretation)}`;
      elementBody.appendChild(noContent);
    }
    currentElement.appendChild(elementBody);
    container.appendChild(currentElement);
  }

  mouseOverAbstractSyntaxOneElement(
    annotation,
    /** @type {HTMLElement} */
    annotationTreeElement,
  ) {
    annotation.leadingByte.style.backgroundColor = "gray";
    annotation.length.style.backgroundColor = "lightskyblue";
    annotation.body.style.backgroundColor = "lightgreen";
    annotationTreeElement.style.backgroundColor = "lightgray";
  }

  mouseOutAbstractSyntaxOneElement(
    annotation,
    /** @type {HTMLElement} */
    annotationTreeElement,
  ) {
    annotation.leadingByte.style.backgroundColor = "";
    annotation.length.style.backgroundColor = "";
    annotation.body.style.backgroundColor = "";
    annotationTreeElement.style.backgroundColor = "";
  }

  appendAnnotationTree(
    /** @type {HTMLElement} */
    container,
    /** @type  {ASNElement} */
    currentInterpretation,
  ) {
    let currentElement = document.createElement("DIV");
    currentElement.classList.add("abstractSyntaxOneElementTree");
    let currentHead = document.createElement("SPAN");
    let headHTML = `<b>${currentInterpretation.type}</b>`;
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
    let annotationElementPeer = currentInterpretation.dom;
    let elementsToAttachTo = [currentHead, annotationElementPeer.header];
    if (currentInterpretation.children !== undefined) {
      for (let counter = 0; counter < currentInterpretation.children.length; counter++) {
        this.appendAnnotationTree(currentElement, currentInterpretation.children[counter]);
      }
    } else {
      elementsToAttachTo.push(annotationElementPeer.body);
    }
    for (let counter = 0; counter < elementsToAttachTo.length; counter++) {
      let theElement = elementsToAttachTo[counter];
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

  annotate() {
    this.DOMElementAnnotationContainer.innerHTML = "";
    this.DOMElementAnnotation = document.createElement("SPAN");
    this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotation");
    this.DOMElementAnnotation.innerHTML = "";
    this.DOMElementAnnotationTree = document.createElement("SPAN");
    this.DOMElementAnnotation.classList.add("abstractSyntaxOneAnnotationTree");
    this.DOMElementAnnotation.innerHTML = "";
    let theTable = document.createElement("TABLE");
    let row = theTable.insertRow(0);
    let cellLeft = row.insertCell(- 1);
    let cellRight = row.insertCell(- 1);
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
}

class TransportLayerSecurityServer {
  constructor() {
  }

  displayMessages(
    /** @type {string} */
    outputId,
    input,
  ) {
    let outputElement = document.getElementById(outputId);
    outputElement.className = "hexContainerStandardWidth";
    writeSessionToDOM(input.session, outputElement);
    let inputHeader = document.createElement("span");
    inputHeader.innerHTML = "<br>";
    outputElement.appendChild(inputHeader);
    for (let i = 0; i < input.spoofer.inputMessages.length; i++) {
      let currentStringHeader = document.createElement("SPAN");
      currentStringHeader.innerHTML = `<br><b>Input ${i + 1}:</b><br>`;
      outputElement.appendChild(currentStringHeader);
      let nextInput = document.createElement("span");
      nextInput.className = "hexContainerStandard";
      let annotation = new AnnotatedBytes();
      annotation.writeMessageToDOM(input.spoofer.inputMessages[i], nextInput);
      outputElement.appendChild(nextInput);
    }
    if (input.spoofer.errorsOnInput.length > 0) {
      let inputErrors = document.createElement("span");
      let errorHTML = "<br><b style = 'color:red'>Input errors:</b>";
      for (let i = 0; i < input.spoofer.errorsOnInput.length; i++) {
        errorHTML += "<br>" + input.spoofer.errorsOnInput[i];
      }
      inputErrors.innerHTML = errorHTML;
      outputElement.appendChild(inputErrors);
    }
    let outputHeader = document.createElement("span");
    outputHeader.innerHTML = "<br>";
    outputElement.appendChild(outputHeader);
    for (let i = 0; i < input.spoofer.outputMessages.length; i++) {
      let outputHeader = document.createElement("span");
      outputHeader.innerHTML = `<br><b>Output ${i + 1}:</b><br>`;
      outputElement.appendChild(outputHeader);
      let currentOutputContainer = document.createElement("span");
      currentOutputContainer.className = "hexContainerStandard";
      for (let j = 0; j < input.spoofer.outputMessages[i].length; j++) {
        let nextOutput = document.createElement("span");
        let annotation = new AnnotatedBytes();
        annotation.writeMessageToDOM(input.spoofer.outputMessages[i][j], nextOutput);
        currentOutputContainer.appendChild(nextOutput);
        if (j < input.spoofer.outputMessages[i].length - 1) {
          let separator = document.createElement("span");
          separator.innerHTML = "<span style = 'background-color:red;display:inline-block;'>&nbsp;</span>";
          currentOutputContainer.appendChild(separator);
        }
      }
      outputElement.appendChild(currentOutputContainer);
    }
    if (input.spoofer.errorsOnOutput.length > 0) {
      let outputErrors = document.createElement("span");
      let errorHTML = "Output errors:<br>";
      for (let i = 0; i < input.spoofer.errorsOnOutput.length; i++) {
        errorHTML += input.spoofer.errorsOnOutput[i] + "<br>";
      }
      outputErrors.innerHTML = errorHTML;
      outputElement.appendChild(outputErrors);
    }
    if (input.spoofer.errorsOnOutput.length > 0) {
      let inputErrors = document.createElement("span");
      let errorHTML = "<br><b style = 'color:red'>Output errors:</b><br>";
      for (let i = 0; i < input.spoofer.errorsOnOutput.length; i++) {
        errorHTML += "<br>" + input.spoofer.errorsOnOutput[i];
      }
      inputErrors.innerHTML = errorHTML;
      outputElement.appendChild(inputErrors);
    }
    let sessionStatusElement = document.createElement("SPAN");
    if (outputElement.style.height < 800) {
      outputElement.style.height = 800;
    }
    if (outputElement.style.width < 1000) {
      outputElement.style.width = 1000;
    }
  }
}

class AnnotatedBytes {
  constructor() {

  }

  writeMessageToDOM(
    input,
    /** @type {HTMLElement} */
    outputComponent,
  ) {
    let serialization = input.serialization;
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
      this.nextByteOffset++;
    }
    if (this.top.currentBody.length > 0) {
      this.top.flushBody();
    }
    let inputStringifiedElement = document.createElement("SPAN");
    // inputStringifiedElement.innerHTML = "<br>" + JSON.stringify(input);
    outputComponent.appendChild(inputStringifiedElement);
  }

  getStackTop() {
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

  stackOnTop() {
    while (this.nextMarkerOffset < this.markers.length) {
      let nextMarker = this.markers[this.nextMarkerOffset];
      if (nextMarker.offset > this.nextByteOffset) {
        break;
      }
      if (nextMarker.offset == this.nextByteOffset) {
        this.top.flushBody();
        let newElement = document.createElement("SPAN");
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
      this.nextMarkerOffset++;
    }
  }
}

class StackElement {
  constructor(
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

  flushBody() {
    if (this.currentBody === null) {
      return;
    }
    if (this.currentBody.length <= 0) {
      return;
    }
    let bodyElement = document.createElement("SPAN");
    bodyElement.innerHTML = this.currentBody.join("");
    bodyElement.className = "byteAnnotationBody";
    let tooltipContent = "";
    tooltipContent += `Offset: ${this.offset}, length: ${this.length}`
    if (this.label !== "" && this.label !== undefined) {
      tooltipContent += `<br>label: ${this.label}`;
    }
    attachTooltip(bodyElement, tooltipContent);
    this.dom.appendChild(bodyElement);
  }
}

module.exports = {
  abstractSyntaxNotationAnnotate,
  displayTransportLayerSecurity,
  displaySSLRecord,
};