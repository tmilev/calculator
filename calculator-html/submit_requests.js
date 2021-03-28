"use srict";
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const ids = require("./ids_dom_elements");
const panels = require('./panels');

function convertStringToHtml(input) {
  let result = "";
  for (let counter = 0; counter < input.length; counter++) {
    if (input[counter] === '&') {
      result += '&amp;';
      continue;
    }
    result += input[counter];
  }
  return result;
}

/**@returns {String} */
getLatexLink = function () {
  let firstPart = window.location.href.split("#")[0];
  let hash = window.location.hash;
  if (hash.startsWith('#')) {
    hash = hash.substring(1);
  }
  if (hash.indexOf("%22") !== - 1) {
    hash = decodeURIComponent(hash);
  }
  if (hash.indexOf("{") !== - 1) {
    hash = encodeURIComponent(hash);
  }
  let theURL = `${firstPart}#${hash}`;
  return latexifyLink(theURL);
}

/**@returns {String} */
latexifyLink = function (inputURL) {
  let result = "";
  result += "\\href{";
  for (let i = 0; i < inputURL.length; i++) {
    if (inputURL[i] == '%') {
      result += "\\%";
    } else if (inputURL[i] == '_') {
      result += "\\_";
    } else {
      result += inputURL[i];
    }
  }
  result += "}{Click-able sample algebraic computation}";
  return result;
}

function addressToHtml(
  /**@type{string} */
  address,
) {
  let censoredAddressStarts = [
    pathnames.urlFields.newPassword,
    pathnames.urlFields.reenteredPassword,
    pathnames.urlFields.password,
  ];
  for (let i = 0; i < censoredAddressStarts.length; i++) {
    let addressPassSplit = address.split(censoredAddressStarts[i]);
    if (addressPassSplit.length > 1) {
      let indexAmpersand = addressPassSplit[1].search("&");
      addressPassSplit[1] = addressPassSplit[1].substr(indexAmpersand);
      addressPassSplit[1] = "***" + addressPassSplit[1];
      address = addressPassSplit.join(censoredAddressStarts[i] + "=");
    }
  }
  let content = "";
  content += `<a href='${address}' target ='_blank' class = 'linkProgressReport'>${convertStringToHtml(address)}</a>`;
  if (window.calculator.mainPage.storage.variables.flagDebug.isTrue()) {
    content += `<br> <b>LaTeX link:</b> `;
    content += getLatexLink();
  }
  return content;
}

function correctAddress(inputURL) {
  if (
    inputURL.startsWith("http://") ||
    inputURL.startsWith("https://")
  ) {
    return inputURL;
  }
  if (window.calculator.hardCodedServerAddress !== "") {
    if (window.calculator.hardCodedServerAddress.endsWith("/") && inputURL.startsWith("/")) {
      return window.calculator.hardCodedServerAddress + inputURL.slice(1);
    } else {
      return window.calculator.hardCodedServerAddress + inputURL;
    }
  }
  return inputURL;
}

class RequestWithProgress {
  constructor(
    /**@type{string} */
    address,
    /**@type{HTMLElement|string} */
    output,
    /**@type{boolean} */
    isPost,
    /**@type{HtmlElement|string|null} */
    progress,
    callback,
    /**@type{{dontCollapsePanel:boolean, width:number}} */
    panelOptions,
  ) {
    /**@type{XMLHttpRequest|null} */
    this.xhr = null;
    this.address = address;
    this.isPost = isPost;
    this.progress = progress;
    if (this.progress === null || this.progress === undefined) {
      this.progress = document.getElementById(ids.domElements.spanProgressReportGeneral);
    }
    this.output = output;
    // this.progress = progress;
    this.callback = callback;
    /**@type{{dontCollapsePanel:boolean, width:number}} */
    this.panelOptions = panelOptions;
    /**@type{panels.PanelExpandable|null} */
    this.panel = new panels.PanelExpandable(this.progress);
    this.startTime = 0;
    this.details = "";
  }

  sendPrepare() {
    this.xhr = new XMLHttpRequest();
    this.xhr.onload = () => {
      this.responseStandard();
    }
  }

  sendGET() {
    this.sendPrepare();
    this.xhr.open("GET", this.address, true);
    this.xhr.setRequestHeader('Accept', 'text/html');
    this.details = addressToHtml(this.address);
    this.recordProgressStarted();
    this.xhr.send();
  }

  sendPOST(parameters) {
    this.sendPrepare();
    this.xhr.open("POST", this.address, true);
    this.xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    this.details = `POST ${this.address}<br>message: ${miscellaneous.shortenString(200, parameters)}`
    this.recordProgressStarted();
    this.xhr.send(parameters);
  }

  recordProgressStarted() {
    this.panel.initialize(true);
    this.startTime = new Date().getTime();
    this.panel.setPanelLabel(`<b style = "color:orange">Sent</b>`)
    this.panel.setPanelContent(this.details);
  }

  responseStandard() {
    this.recordProgressDone();
    if (this.callback !== undefined && this.callback !== null) {
      this.callback(this.xhr.responseText, this.output);
    } else {
      this.recordResult(this.xhr.responseText);
    }
  }

  recordProgressDone() {
    if (this.progress === null || this.progress === undefined || this.progress === "") {
      return;
    }
    let timeTotal = new Date().getTime() - this.startTime;
    let panelLabel = `<b style = 'color:green'>Received</b> ${timeTotal} ms`;
    this.panel.setPanelLabel(panelLabel);
  }

  recordResult(
    /**@type{string} */
    resultText,
  ) {
    if (this.output === null || this.output === undefined) {
      return;
    }
    if (typeof this.output === "string") {
      this.output = document.getElementById(this.output);
    }
    this.output.innerHTML = miscellaneous.jsonParseGetHtmlStandard(resultText);
  }
}

/**
 * Fires up a get requests.
 * Expected input: an object with the following fields filled in.
 *
 * inputObject.url: url of the address to get. If omitted, service must be specified.
 *
 * inputObject.callback: function to callback. The function will be passed on
 *   as arguments the received result.
 *   The result may in addition be displayed in the component inputObject.result, should
 *   this object be provided.
 *   The function will be called only if the get operation was successful.
 *
 * inputObject.progress: id or handle of an object to display the progress of the operation.
 *   Indended for developer use.
 *   Will create a button whose label shows progress of the operation and
 *   clicking which will show/hide the address.
 *   Pass null or undefined if you don't want any progress report.
 *
 * inputObject.result: id or handle of an object to dump the html-escaped
 *   but otherwise non-processed final result.
 *   Pass null or undefined if you don't want to show the result.
 */
function submitGET(
  /** @type {{url: string, callback: Function, progress: string, result: HTMLElement|string, panelOptions:{dontCollapsePanel:boolean, width:number}}}*/
  inputObject,
) {
  let theAddress = correctAddress(inputObject.url);
  let progress = inputObject.progress;
  let result = inputObject.result;
  let callback = inputObject.callback;
  let panelOptions = null;
  if (inputObject.panelOptions !== undefined) {
    panelOptions = inputObject.panelOptions;
  }
  let request = new RequestWithProgress(theAddress, result, false, progress, callback, panelOptions);
  request.sendGET();
}

function submitPOST(
  /** @type {{url: string, parameters: string, callback: Function, progress: string, result: string}}*/
  inputObject,
) {
  let theAddress = correctAddress(inputObject.url);
  let progress = inputObject.progress;
  let result = inputObject.result;
  let callback = inputObject.callback;
  let parameters = inputObject.parameters;
  let panelOptions = null;
  if (inputObject.panelOptions !== undefined) {
    panelOptions = inputObject.panelOptions;
  }
  let request = new RequestWithProgress(theAddress, result, true, progress, callback, panelOptions);
  request.sendPOST(parameters);
}

module.exports = {
  submitGET,
  submitPOST,
};