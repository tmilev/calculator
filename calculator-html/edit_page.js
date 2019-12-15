"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
// const calculatorPage = require('./calculator_page');
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous");

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

/**@returns {HTMLElement[]} */
function getClonePanel(
  /**@type{string} */
  fileNameSource, 
  /**@type{string} */
  fileNameTarget
) {
  if (fileNameSource === "" || fileNameSource === undefined || fileNameSource === null) {
    return [];
  }
  var idCloneInput = encodeURIComponent(`cloneButton${fileNameSource}`);
  var idSpanClonePageReport = encodeURIComponent(`cloneButtonReport${fileNameSource}`);
  var result = [];
  var table = document.createElement("table");
  var row = table.insertRow(- 1);
  var cellClone = row.insertCell(- 1);
  
  // result += "<table><tr><td>";
  var buttonClone = document.createElement("button");
  buttonClone.className = "buttonClone";
  buttonClone.style.width = "50px";
  buttonClone.addEventListener("click", window.calculator.editPage.handleClone.bind(null,fileNameSource, idCloneInput, idSpanClonePageReport));
  buttonClone.innerHTML = "Clone";
  cellClone.appendChild(buttonClone)
  var cellFileInfo = row.insertCell(- 1);
  var cellFileInfoTable = document.createElement("table");
  var cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  var nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = "from:";
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = `<b style = 'color: green'>${fileNameSource}</b></td>`;
  var sizeFile = fileNameTarget.length;
  cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = "to:";
  nextCell = cellFileInfoTableRow.insertCell( - 1);
  nextCell.innerHTML = `<input type = "text" value = '${fileNameTarget}' size = '${sizeFile}' id = '${idCloneInput}'></input>`;
  cellFileInfo.append(cellFileInfoTable);
  result.push(table);
  var cloneReport = document.createElement("div");
  cloneReport.id = idSpanClonePageReport;
  result.push(cloneReport);
  return result;
}

/**@returns {HTMLElement} */
function getEditPanel(fileName) {
  var thePage = window.calculator.mainPage;
  var doGenerate = !thePage.flagProblemPageOnly;

  if (!thePage.flagProblemPageOnly) {
    if (!thePage.user.hasProblemEditRights() || thePage.studentView()) {
      doGenerate = false;
    }
  }
  if (!doGenerate) {
    return document.createTextNode("");
  }
  if (fileName === "" || fileName === undefined || fileName === null) {
    return document.createTextNode("");
  }
  var result = document.createElement("DIV");
  result.className = "spanFileInfo";
  var saveEdit = document.createElement("BUTTON");
  saveEdit.className = "buttonSaveEdit";
  saveEdit.innerHTML = "Edit";
  saveEdit.style.width = "50px";
  saveEdit.addEventListener('click', window.calculator.editPage.selectEditPage.bind(null, fileName));
  result.appendChild(saveEdit);
  result.appendChild(document.createTextNode(` ${fileName} `));
  var clonePanel = document.createElement("BUTTON");
  clonePanel.className = "accordionLike";
  clonePanel.innerHTML = "Clone panel &#9666;";
  clonePanel.addEventListener('click', window.calculator.editPage.toggleClonePanel.bind(null, clonePanel));
  result.appendChild(clonePanel);
  var panelElement = document.createElement("span");
  panelElement.className = "panelDeadlines";
  var panelContent = getClonePanel(fileName, fileName);
  for (var i = 0; i < panelContent.length; i ++) {
    panelElement.appendChild(panelContent[i]);
  }
  result.appendChild(panelElement);
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
  var content = encodeURIComponent(editor.getValue());
  queryParameters += `&${pathnames.urlFields.requests.fileContent}=${content}`;
  queryParameters += `&${pathnames.urlFields.problem.fileName}=${thePage.storage.variables.editor.currentlyEditedPage.getValue()}`;
  thePage.pages.problemPage.flagLoaded = false;
  submitRequests.submitPOST({
    url: theURL,
    parameters: queryParameters,
    result: ids.domElements.spanSubmitEditPageReport,
  });
}

function initEditorAce() {
  var thePage = window.calculator.mainPage;
  var saveButton = document.getElementById(ids.domElements.buttonSaveEdit);
  saveButton.className = "buttonSaveEdit";
  saveButton.addEventListener("click", window.calculator.editPage.storeEditedPage);
  saveButton.innerHTML = "Save";
  var editor = document.getElementById(ids.domElements.divEditorAce);
  editor.addEventListener("keydown", window.calculator.editPage.ctrlSPressAceEditorHandler);
  thePage.pages.editPage.editor = ace.edit(ids.domElements.divEditorAce);
}

function selectEditPageCallback(input, outputComponent) {
  var thePage = window.calculator.mainPage;
  try {
    var parsedInput = miscellaneous.jsonUnescapeParse(input);
    ace.require("ace/ext/language_tools");
    if (thePage.pages.editPage.editor === null) {
      initEditorAce();
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
    var problemIdURLed = thePage.storage.variables.editor.currentlyEditedPage.getValue();
    thePage.storage.variables.currentCourse.currentProblemId.setAndStore(problemIdURLed);
    thePage.storage.variables.currentCourse.problemFileName.setAndStore(decodeURIComponent(problemIdURLed));
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

/**@returns {HTMLElement} */
function getNextEditButton(problemId) {
  return getNavigationEditButton(problemId, "&#8594;");
}

/** @returns {HTMLElement} */
function getPreviousEditButton(problemId) {
  return getNavigationEditButton(problemId, "&#8592;");
}

/** @returns {HTMLElement} */
function getNavigationEditButton(problemId, contentHTML) {
  if (
    problemId === null || 
    problemId === "" || 
    problemId === undefined
  ) {
    return document.createTextNode("");
  }
  var navigationButton = document.createElement("button");
  navigationButton.addEventListener(
    "click", selectEditPage.bind(null, problemId)
  );
  navigationButton.className = "buttonNavigationStandard";
  navigationButton.innerHTML = contentHTML; 
  return navigationButton;
}

function writeNextPreviousEditButton(currentlyEditedPage) {
  var thePage = window.calculator.mainPage;
  if (!(currentlyEditedPage in window.calculator.problemPage.allProblems.allProblems)) {
    return;
  }
  var problem = thePage.getProblemById(currentlyEditedPage);
  var previousButtonSpan = document.getElementById(ids.domElements.spanButtonPreviousEdit);
  var nextButtonSpan = document.getElementById(ids.domElements.spanButtonNextEdit);
  if (previousButtonSpan !== null) {
    previousButtonSpan.innerHTML = "";
    previousButtonSpan.appendChild(getPreviousEditButton(problem.previousProblemId));
  }
  if (nextButtonSpan !== null) {
    nextButtonSpan.innerHTML = "";
    nextButtonSpan.appendChild(getNextEditButton(problem.nextProblemId));
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
  if (!thePage.flagProblemPageOnly) {
    if (thePage.storage.variables.currentPage.getValue() !== thePage.pages.editPage.name) {
      thePage.selectPage(thePage.pages.editPage.name);
      return;
    }
  }
  writeNextPreviousEditButton(currentlyEditedPage);
  var theTopicTextArea = document.getElementById(ids.domElements.textAreaTopicListEntry);
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