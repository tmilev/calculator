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
  var idCloneInput = encodeURIComponent(`cloneButton${fileName}`);
  var idSpanClonePageReport = encodeURIComponent(`cloneButtonReport${fileName}`);
  result += `<button class = 'buttonClone' style = 'width:50px' onclick = `;
  result += `"window.calculator.editPage.handleClone('${fileName}', '${idCloneInput}', '${idSpanClonePageReport}')">Clone</button>`;
  var sizeFile = fileName.length;
  result += `<input type = "text" value = '${fileName}' size = '${sizeFile}' id = '${idCloneInput}'></input>`;
  result += `<div id = "${idSpanClonePageReport}"></div>`;
  result += "</span>";
  result += `</div>`;
  return result;
}

function handleClone(fileName, idCloneInput, idSpanClonePageReport) {
  var newFileName = document.getElementById(idCloneInput).value;
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += calculatorPage.calculator.getQueryStringSubmitStringAsMainInput(newFileName, pathnames.urlFields.requests.clonePage);
  theURL += `${pathnames.urlFields.fileName}=${fileName}&`;
  submitRequests.submitGET({
    url: theURL,
    result: idSpanClonePageReport
  });
}

function storeEditedPage() {
  var thePage = window.calculator.mainPage;
  var editor = thePage.pages.editPage.editor;
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  var queryParameters = "";
  queryParameters += calculatorPage.calculator.getQueryStringSubmitStringAsMainInput(editor.getValue(), pathnames.urlFields.requests.modifyPage);
  queryParameters += `${pathnames.urlFields.fileName}=${thePage.storage.variables.editor.currentlyEditedPage.getValue()}&`;
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
    if (input)
    editor.$blockScrolling = Infinity;
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
  handleClone,
  ctrlSPressAceEditorHandler
};