"use srict";
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const ids = require("./ids_dom_elements");
const panels = require('./panels');

function recordProgressDone(
  /**@type{HTMLElement|string} */
  progress,
  /**@type{number} */
  timeFinished,
  /**@type{{dontCollapsePanel:boolean, width:number}|null} */
  panelOptions,
) {
  if (progress === null || progress === undefined || progress === "") {
    return;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  if (progress === null) {
    return;
  }
  let timeTotal = timeFinished - progress.getAttribute("timeStarted");
  let panelLabel = `<b style = 'color:green'>Received</b> ${timeTotal} ms`;
  let panel = new panels.PanelExpandable(progress);
  panel.initialize(false);
  panel.setPanelLabel(panelLabel);
  if (panelOptions !== null) {
    if (panelOptions.dontCollapsePanel === true) {
      panel.doToggleContent();
    }
  }
}

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

function recordProgressStarted(
  /**@returns{HTMLElement} */
  progress,
  address,
  isPost,
  timeStarted,
) {
  if (progress === "" || progress === null || progress === undefined) {
    return;
  }
  if (isPost === undefined || isPost === null) {
    isPost = false;
  }
  if (typeof progress === "string") {
    progress = document.getElementById(progress);
  }
  if (progress === null) {
    return;
  }
  let panelWithButton = new panels.PanelExpandable(progress);
  panelWithButton.initialize(true);
  panelWithButton.initialize
  progress.setAttribute("timeStarted", timeStarted);
  panelWithButton.setPanelLabel(`<b style = "color:orange">Sent</b>`)

  censoredAddressStarts = [
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
  if (!isPost) {
    content += `<a href='${address}' target ='_blank' class = 'linkProgressReport'>${convertStringToHtml(address)}</a>`;
  } else {
    content += address;
  }
  if (window.calculator.mainPage.storage.variables.flagDebug.isTrue()) {
    content += `<br> <b>LaTeX link:</b> `;
    content += getLatexLink();
  }
  panelWithButton.setPanelContent(content);
  panelWithButton.matchPanelStatus();
}

function recordResult(resultText, resultSpan) {
  if (resultSpan === null || resultSpan === undefined) {
    return;
  }
  if (typeof resultSpan === "string") {
    resultSpan = document.getElementById(resultSpan);
  }
  resultSpan.innerHTML = miscellaneous.jsonParseGetHtmlStandard(resultText);
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
  let xhr = new XMLHttpRequest();
  recordProgressStarted(progress, theAddress, false, (new Date()).getTime());
  xhr.open('GET', theAddress, true);
  xhr.setRequestHeader('Accept', 'text/html');
  xhr.onload = responseStandard.bind(null, xhr, callback, result, progress, panelOptions);
  xhr.send();
}

function responseStandard(
  /**@type {XMLHttpRequest} */
  request,
  /**@type {Function} */
  callback,
  /**@type {String|HTMLElement} */
  result,
  /**@type {String|HTMLElement} */
  progress,
  /**@type{{dontCollapsePanel:boolean, width:number}} */
  panelOptions,
) {
  recordProgressDone(progress, (new Date()).getTime(), panelOptions);
  if (callback !== undefined && callback !== null) {
    callback(request.responseText, result);
  } else {
    recordResult(request.responseText, result);
  }
}

function submitPOST(
  /** @type {{url: string, parameters: string, callback: Function, progress: string, result: string}}*/
  inputObject,
) {
  let theAddress = correctAddress(inputObject.url);
  let progress = inputObject.progress;
  let result = inputObject.result;
  let callback = inputObject.callback;
  let https = new XMLHttpRequest();
  let parameters = inputObject.parameters;
  https.open("POST", theAddress, true);
  https.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
  if (progress === undefined) {
    progress = ids.domElements.spanProgressReportGeneral;
  }
  timeOutCounter = 0;

  let postRequest = `POST ${pathnames.urls.calculatorAPI}<br>message: ${miscellaneous.shortenString(200, parameters)}`;
  recordProgressStarted(progress, postRequest, true, (new Date()).getTime());

  https.onload = responseStandard.bind(null, https, callback, result, progress);
  ////////////////////////////////////////////
  https.send(parameters);
  ////////////////////////////////////////////
}

module.exports = {
  submitGET,
  submitPOST,
};