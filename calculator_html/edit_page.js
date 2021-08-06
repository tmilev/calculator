"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
// const calculatorPage = require('./calculator_page');
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage");

let editorAce = null;
let aceEditorAutoCompletionWordList = {};

let staticWordCompleter = {
  getCompletions: function (editor, session, pos, prefix, callback) {
    callback(null, aceEditorAutoCompletionWordList.map(function (word) {
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
  let idCloneInput = encodeURIComponent(`cloneButton${fileNameSource}`);
  let idSpanClonePageReport = encodeURIComponent(`cloneButtonReport${fileNameSource}`);
  let result = [];
  let table = document.createElement("table");
  let row = table.insertRow(- 1);
  let cellClone = row.insertCell(- 1);

  // result += "<table><tr><td>";
  let buttonClone = document.createElement("button");
  buttonClone.className = "buttonClone";
  buttonClone.style.width = "50px";
  buttonClone.addEventListener("click", window.calculator.editPage.handleClone.bind(null, fileNameSource, idCloneInput, idSpanClonePageReport));
  buttonClone.innerHTML = "Clone";
  cellClone.appendChild(buttonClone)
  let cellFileInfo = row.insertCell(- 1);
  let cellFileInfoTable = document.createElement("table");
  let cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  let nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = "from:";
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = `<b style = 'color: green'>${fileNameSource}</b></td>`;
  let sizeFile = fileNameTarget.length;
  cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = "to:";
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.innerHTML = `<input type = "text" value = '${fileNameTarget}' size = '${sizeFile}' id = '${idCloneInput}'></input>`;
  cellFileInfo.append(cellFileInfoTable);
  result.push(table);
  let cloneReport = document.createElement("div");
  cloneReport.id = idSpanClonePageReport;
  result.push(cloneReport);
  return result;
}

/**@returns {HTMLElement} */
function getEditPanel(
  /**@type{string} */
  fileName,
  /** @type{boolean} Whether online edit is allowed. */
  withInstructorRights,
  /** @type{boolean} Show a source button without full admin rights. */
  showSource,
) {
  if (withInstructorRights === undefined) {
    withInstructorRights = false;
  }
  if (showSource === undefined) {
    showSource = false;
  }
  if (!withInstructorRights && !showSource) {
    return document.createTextNode("");
  }
  if (fileName === "" || fileName === undefined || fileName === null) {
    return document.createTextNode("");
  }
  let result = document.createElement("DIV");
  result.className = "spanFileInfo";
  let saveEdit = document.createElement("BUTTON");
  saveEdit.className = "buttonSaveEdit";
  saveEdit.style.width = "";
  if (withInstructorRights) {
    saveEdit.textContent = "Edit";
  } else {
    saveEdit.style.background = "#fed8b1";
    saveEdit.textContent = "Problem source code";
  }
  saveEdit.addEventListener('click', () => {
    selectEditPage(fileName, withInstructorRights);
  });
  result.appendChild(saveEdit);
  if (!withInstructorRights) {
    return result;
  }
  result.appendChild(document.createTextNode(` ${fileName} `));
  let clonePanel = document.createElement("BUTTON");
  clonePanel.className = "accordionLike";
  clonePanel.innerHTML = "Clone panel &#9666;";
  clonePanel.addEventListener('click', window.calculator.editPage.toggleClonePanel.bind(null, clonePanel));
  result.appendChild(clonePanel);
  let panelElement = document.createElement("span");
  panelElement.className = "panelDeadlines";
  let panelContent = getClonePanel(fileName, fileName);
  for (let i = 0; i < panelContent.length; i++) {
    panelElement.appendChild(panelContent[i]);
  }
  result.appendChild(panelElement);
  return result;
}

function callbackClone(input, output) {
  if (typeof output === "string") {
    output = document.getElementById(output);
  }
  let inputParsed = JSON.parse(input);
  let errorFound = false;
  let result = "";
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
  let newFileName = document.getElementById(idCloneInput).value;
  let theURL = "";
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
  let thePage = window.calculator.mainPage;
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  let queryParameters = "";
  queryParameters += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.modifyPage}`;
  let content = encodeURIComponent(editorAce.getValue());
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
  let saveButton = document.getElementById(ids.domElements.buttonSaveEdit);
  saveButton.className = "buttonSaveEdit";
  saveButton.addEventListener("click", () => {
    storeEditedPage();
  });
  saveButton.innerHTML = "Save";
  let editorElement = document.getElementById(ids.domElements.divEditorAce);
  editorElement.addEventListener("keydown", window.calculator.editPage.ctrlSPressAceEditorHandler);
  editorAce = ace.edit(ids.domElements.divEditorAce);
}

function selectEditPageCallback(input, outputComponent) {
  try {
    let parsedInput = miscellaneous.jsonUnescapeParse(input);
    ace.require("ace/ext/language_tools");
    if (editorAce === null) {
      initEditorAce();
    }
    if (parsedInput.autoComplete !== null && parsedInput.autoComplete !== undefined) {
      aceEditorAutoCompletionWordList = parsedInput.autoComplete;
    }
    editorAce.$blockScrolling = Infinity;
    let incomingContent = "";
    let errorSpan = document.getElementById(ids.domElements.spanErrorsEditPage);
    if (parsedInput.error !== null && parsedInput.error !== undefined) {
      errorSpan.innerHTML = `<br>${parsedInput.error}`;
    } else {
      errorSpan.innerHTML = "";
    }
    if (parsedInput.content !== null && parsedInput.content !== undefined) {
      incomingContent = decodeURIComponent(parsedInput.content);
    }
    let problemIdURLed = storage.storage.variables.editor.currentlyEditedPage.getValue();
    storage.storage.variables.currentCourse.fileName.setAndStore(decodeURIComponent(problemIdURLed));
    editorAce.getSession().setValue(incomingContent);
    editorAce.setTheme("ace/theme/chrome");
    editorAce.getSession().setMode("ace/mode/xml");
    editorAce.setOptions({
      enableBasicAutocompletion: true,
      enableLiveAutocompletion: true,
    });
    editorAce.completers = [staticWordCompleter];
    editorAce.$blockScrolling = Infinity;
    editorAce.resize();
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
  let navigationButton = document.createElement("button");
  navigationButton.addEventListener(
    "click", selectEditPage.bind(null, problemId)
  );
  navigationButton.className = "buttonNavigationStandard";
  navigationButton.innerHTML = contentHTML;
  return navigationButton;
}

function writeNextPreviousEditButton(
  currentlyEditedPage,
) {
  let thePage = window.calculator.mainPage;
  let problem = thePage.getProblemByIdOrNull(currentlyEditedPage);
  if (problem === null) {
    return;
  }
  let previousButtonSpan = document.getElementById(ids.domElements.spanButtonPreviousEdit);
  let nextButtonSpan = document.getElementById(ids.domElements.spanButtonNextEdit);
  if (previousButtonSpan !== null) {
    previousButtonSpan.innerHTML = "";
    previousButtonSpan.appendChild(getPreviousEditButton(problem.previousProblemId));
  }
  if (nextButtonSpan !== null) {
    nextButtonSpan.innerHTML = "";
    nextButtonSpan.appendChild(getNextEditButton(problem.nextProblemId));
  }
}

function selectEditPage(
  /**@type{string} */
  currentlyEditedPage,
  /** @type{boolean} Whether online edit is allowed. */
  withInstructorRights,
) {
  if (withInstructorRights === undefined) {
    withInstructorRights = false;
  }
  let saveButton = document.getElementById(ids.domElements.buttonSaveEdit);
  if (withInstructorRights) {
    saveButton.style.visibility = "";
  } else {
    saveButton.style.visibility = "hidden";
  }
  let thePage = window.calculator.mainPage;
  let storageVariables = thePage.storage.variables;
  let fileNameSources = [
    storageVariables.editor.currentlyEditedPage,
    storageVariables.currentCourse.problemFileName,
    storageVariables.currentCourse.fileName,
    storageVariables.currentCourse.courseHome,
    storageVariables.currentCourse.topicList,
  ];
  for (let i = 0; i < fileNameSources.length; i++) {
    if (
      (typeof currentlyEditedPage) === "string" &&
      currentlyEditedPage !== ""
    ) {
      break;
    }
    currentlyEditedPage = fileNameSources[i].getValue();
  }
  if (
    currentlyEditedPage === undefined ||
    currentlyEditedPage === null ||
    currentlyEditedPage === ""
  ) {
    currentlyEditedPage = "/coursesavailable/default.txt";
  }
  storageVariables.editor.currentlyEditedPage.setAndStore(currentlyEditedPage);
  if (!thePage.flagProblemPageOnly) {
    if (thePage.storage.variables.currentPage.getValue() !== thePage.pages.editPage.name) {
      thePage.selectPage(thePage.pages.editPage.name);
      return;
    }
  }
  writeNextPreviousEditButton(currentlyEditedPage);
  let theTopicTextArea = document.getElementById(ids.domElements.textAreaTopicListEntry);
  theTopicTextArea.value = `Title: ${currentlyEditedPage}\nProblem: ${currentlyEditedPage}`;
  theTopicTextArea.cols = currentlyEditedPage.length + 15;

  let theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requestEditPage}&`;
  theURL += `${pathnames.urlFields.problem.fileName}=${thePage.storage.variables.editor.currentlyEditedPage.getValue()}`;
  submitRequests.submitGET({
    url: theURL,
    callback: selectEditPageCallback,
    progress: ids.domElements.spanProgressReportGeneral,
  });
}

function toggleClonePanel(button) {
  let thePanel = button.nextElementSibling;
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
  aceEditorAutoCompletionWordList,
  editorAce,
  storeEditedPage,
  toggleClonePanel,
  selectEditPage,
  getEditPanel,
  getClonePanel,
  handleClone,
  ctrlSPressAceEditorHandler,
  staticWordCompleter,
};