"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const calculatorPage = require('./calculator_page');
const ids = require('./ids_dom_elements');

var staticWordCompleter = {
  getCompletions: function(editor, session, pos, prefix, callback) {
    var thePage = window.calculator.mainPage;
    callback(null, thePage.aceEditorAutoCompletionWordList.map(function(word) {
      return {
        caption: word,
        value: word,
        meta: "static"
      };
    }));
  }
}

function ctrlSPressAceEditorHandler(event) {
  if (event.ctrlKey !== true) {
    return;
  }
  if (event.keyCode !== 83) {
    return;
  }
  event.preventDefault();
  storeEditedPage();
}

/**@returns {string} */
function getClonePanel(
  /**@type{string} */
  fileNameSource, 
  /**@type{string} */
  fileNameTarget
) {
  if (fileNameSource === "" || fileNameSource === undefined || fileNameSource === null) {
    return "";
  }
  var result = "";
  var idCloneInput = encodeURIComponent(`cloneButton${fileNameSource}`);
  var idSpanClonePageReport = encodeURIComponent(`cloneButtonReport${fileNameSource}`);
  result += "<table><tr><td>";
  result += `<button class = 'buttonClone' style = 'width:50px' onclick = `;
  result += `"window.calculator.editPage.handleClone('${fileNameSource}', '${idCloneInput}', '${idSpanClonePageReport}')">Clone</button>`;
  result += "</td>";
  result += "<td>";
  var sizeFile = fileNameTarget.length;
  result += "<table><tr>";
  result += "<td>from:</td>";
  result += `<td><b style = 'color: green'>${fileNameSource}</b></td>`;
  result += "</tr>";
  result += "<tr>";
  result += "<td>to:</td>";
  result += `<td><input type = "text" value = '${fileNameTarget}' size = '${sizeFile}' id = '${idCloneInput}'></input></td>`;
  result += "</tr></table>";

  result += "</td>";
  result += "</tr></table>";
  result += `<div id = "${idSpanClonePageReport}"></div>`;
  return result;
}

/**@returns {string} */
function getEditPanel(fileName) {
  var thePage = window.calculator.mainPage;
  if (!thePage.user.hasProblemEditRights() || thePage.studentView()) {
    return "";
  }
  if (fileName === "" || fileName === undefined || fileName === null) {
    return "";
  }
  var result = "";
  result += `<div class = 'spanFileInfo'>`;
  result += `<button class = "buttonSaveEdit" onclick = "window.calculator.editPage.selectEditPage('${fileName}');" style = 'width:50px'>Edit</button> ${fileName} `;
  result += `<button class = "accordionLike" onclick = "window.calculator.editPage.toggleClonePanel(this)">Clone panel &#9666;</button>`;
  result += `<span class = "panelDeadlines">`;
  result += getClonePanel(fileName, fileName);
  result += "</span>";
  result += `</div>`;
  return result;
}

function callbackClone(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  var inputParsed = JSON.parse(input);
  var errorFound = false;
  var result = "";
  if (inputParsed.error !== undefined && inputParsed.error !== "" && inputParsed !== null) {
    errorFound = true;
    result += `<b style = 'color:red'>Error.</b> ${inputParsed.error}`;
  }
  if (inputParsed.comments !== undefined && inputParsed.comments !== "") {
    result += inputParsed.comments;
  }
  if (!errorFound) {
    result += `<button class = 'buttonStandard' `;
    result += `onclick = "window.calculator.mainPage.pages.problemPage.flagLoaded=false; window.calculator.mainPage.selectPage('problemPage')"`;
    result += `>Force-reload problem</button>`;
  }
  output.innerHTML = result;
}

function handleClone(fileName, idCloneInput, idSpanClonePageReport) {
  var newFileName = document.getElementById(idCloneInput).value;
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.clonePage}`;
  theURL += `&${pathnames.urlFields.problem.fileNameTarget}=${newFileName}`;
  theURL += `&${pathnames.urlFields.problem.fileName}=${fileName}&`;
  submitRequests.submitGET({
    url: theURL,
    result: idSpanClonePageReport,
    callback: callbackClone
  });
}

function storeEditedPage() {
  var thePage = window.calculator.mainPage;
  var editor = thePage.pages.editPage.editor;
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  var queryParameters = "";
  queryParameters += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.modifyPage}`;
  queryParameters += `&${pathnames.urlFields.requests.fileContent}=${editor.getValue()}`;
  queryParameters += `&${pathnames.urlFields.problem.fileName}=${thePage.storage.variables.editor.currentlyEditedPage.getValue()}`;
  thePage.pages.problemPage.flagLoaded = false;
  submitRequests.submitPOST({
    url: theURL,
    parameters: queryParameters,
    result: "spanSubmitEditPageReport"
  });
}

function selectEditPageCallback(input, outputComponent) {
  var thePage = window.calculator.mainPage;
  try {
    var parsedInput = JSON.parse(input);
    ace.require("ace/ext/language_tools");
    if (thePage.pages.editPage.editor === null) {
      thePage.pages.editPage.editor = ace.edit("divEditorAce");
    }
    if (parsedInput.autoComplete !== null && parsedInput.autoComplete !== undefined) {
      thePage.aceEditorAutoCompletionWordList = parsedInput.autoComplete;
    }
    var editor = thePage.pages.editPage.editor;
    editor.$blockScrolling = Infinity;
    var incomingContent = "";
    var errorSpan = document.getElementById(ids.domElements.spanErrorsEditPage);
    if (parsedInput.error !== null && parsedInput.error !== undefined) {
      errorSpan.innerHTML = `<br>${parsedInput.error}`;
    } else {
      errorSpan.innerHTML = "";
    }
    if (parsedInput.content !== null && parsedInput.content !== undefined) {
      incomingContent = decodeURIComponent(parsedInput.content);
    }
    editor.getSession().setValue(incomingContent);
    editor.setTheme("ace/theme/chrome");
    editor.getSession().setMode("ace/mode/xml");
    editor.setOptions({
      enableBasicAutocompletion: true,
      enableLiveAutocompletion: true,
    });
    editor.completers = [staticWordCompleter];
    editor.$blockScrolling = Infinity;
    editor.resize();
  } catch (e) {
    console.log(`Error: ${e}`);
  }
}

function selectEditPage(currentlyEditedPage) {
  var thePage = window.calculator.mainPage;
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = thePage.storage.variables.editor.currentlyEditedPage.getValue();
  }
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = "/coursesavailable/default.txt";
  }
  thePage.storage.variables.editor.currentlyEditedPage.setAndStore(currentlyEditedPage);
  if (thePage.storage.variables.currentPage.getValue() !== thePage.pages.editPage.name) {
    thePage.selectPage(thePage.pages.editPage.name);
    return;
  }
  var theTopicTextArea = document.getElementById("textareaTopicListEntry");
  theTopicTextArea.value = `Title: ${currentlyEditedPage}\nProblem: ${currentlyEditedPage}`;
  theTopicTextArea.cols = currentlyEditedPage.length + 15;

  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requestEditPage}&`;
  theURL += `fileName=${thePage.storage.variables.editor.currentlyEditedPage.getValue()}`;
  submitRequests.submitGET({
    url: theURL,
    callback: selectEditPageCallback,
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

function toggleClonePanel(button) {
  var thePanel = button.nextElementSibling; 
  if (thePanel.style.maxHeight === '200px') {
    thePanel.style.opacity = '0';
    thePanel.style.maxHeight = '0';
    button.innerHTML = `Clone panel &#9666;`;
  } else {
    thePanel.style.opacity = '1';
    thePanel.style.maxHeight = '200px';
    button.innerHTML = `Clone panel &#9660;`;
  }
}

module.exports = {
  storeEditedPage,
  toggleClonePanel,
  selectEditPage, 
  getEditPanel,
  getClonePanel,
  handleClone,
  ctrlSPressAceEditorHandler
};