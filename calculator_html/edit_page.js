"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");
const miscellaneous = require("./miscellaneous_frontend");
const storage = require("./storage").storage;
const globalUser = require("./user").globalUser;

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

/** @return {HTMLElement[]} */
function getClonePanel(
  /** @type {string} */
  fileNameSource,
  /** @type {string} */
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
  buttonClone.addEventListener("click", () => {
    handleClone(
      fileNameSource,
      idCloneInput,
      idSpanClonePageReport,
    );
  });
  buttonClone.textContent = "Clone";
  cellClone.appendChild(buttonClone)
  let cellFileInfo = row.insertCell(- 1);
  let cellFileInfoTable = document.createElement("table");
  let cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  let nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.textContent = "from:";
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  miscellaneous.writeHTML(nextCell, `<b style = 'color: green'>${fileNameSource}</b>`);
  let sizeFile = fileNameTarget.length;
  cellFileInfoTableRow = cellFileInfoTable.insertRow(- 1);
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  nextCell.textContent = "to:";
  nextCell = cellFileInfoTableRow.insertCell(- 1);
  miscellaneous.writeHTML(
    nextCell,
    `<input type="text" value='${fileNameTarget}' size='${sizeFile}' id='${idCloneInput}'></input>`,
  );
  cellFileInfo.append(cellFileInfoTable);
  result.push(table);
  let cloneReport = document.createElement("div");
  cloneReport.id = idSpanClonePageReport;
  result.push(cloneReport);
  return result;
}

/** @return {HTMLElement} */
function getEditPanel(
  /** @type {string} */
  fileName,
  /** @type {boolean} Whether online edit is allowed. */
  withInstructorRights,
  /** @type {boolean} Show a source button without full admin rights. */
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
  let result = document.createElement("div");
  result.className = "spanFileInfo";
  let saveEdit = document.createElement("button");
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
  miscellaneous.writeHTML(clonePanel, "Clone panel &#9666;");
  clonePanel.addEventListener('click', (ev) => {
    toggleClonePanel(clonePanel);
  });
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
  output.textContent = "";
  if (
    inputParsed.error !== undefined &&
    inputParsed.error !== "" &&
    inputParsed !== null
  ) {
    errorFound = true;
    let errorMessage = document.createElement("b");
    errorMessage.style.color = "red";
    errorMessage.textContent = "Error.";
    output.appendChild(document.createTextNode(` ${inputParsed.error})`));
  }
  if (inputParsed.comments !== undefined && inputParsed.comments !== "") {
    output.appendChild(document.createTextNode(inputParsed.comments));
  }
  if (!errorFound) {
    let button = document.createElement("button");
    button.textContent = "Force-reload problem";
    button.className = "buttonStandard";
    button.addEventListener("click", () => {
      problemEditor.mainPage.pages.problemPage.flagLoaded = false;
      problemEditor.mainPage.selectPage('problemPage');
    });
    output.appendChild(button);
  }
}

function handleClone(fileName, idCloneInput, idSpanClonePageReport) {
  let newFileName = document.getElementById(idCloneInput).value;
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  url += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.clonePage}`;
  url += `&${pathnames.urlFields.problem.fileNameTarget}=${newFileName}`;
  url += `&${pathnames.urlFields.problem.fileName}=${fileName}&`;
  submitRequests.submitGET({
    url: url,
    result: idSpanClonePageReport,
    callback: callbackClone
  });
}

function storeEditedPage() {
  let page = problemEditor.mainPage;
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  let queryParameters = "";
  queryParameters += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.modifyPage}`;
  let content = encodeURIComponent(editorAce.getValue());
  queryParameters += `&${pathnames.urlFields.requests.fileContent}=${content}`;
  queryParameters += `&${pathnames.urlFields.problem.fileName}=${storage.variables.editor.currentlyEditedPage.getValue()}`;
  problemEditor.mainPage.pages.problemPage.flagLoaded = false;
  submitRequests.submitPOST({
    url: url,
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
  saveButton.textContent = "Save";
  let editorElement = document.getElementById(ids.domElements.divEditorAce);
  editorElement.addEventListener("keydown", (e) => {
    ctrlSPressAceEditorHandler(e);
  });
  editorAce = ace.edit(ids.domElements.divEditorAce);
}

function selectEditPageCallback(
  input,
  /** @type {boolean} Injects error html from server dangerously.*/
  injectHTMLDangerously,
) {
  if (injectHTMLDangerously === undefined) {
    injectHTMLDangerously = false;
  }
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
    errorSpan.textContent = "";
    if (!injectHTMLDangerously) {
      miscellaneous.writeHtmlElementsFromCommentsAndErrors(parsedInput, errorSpan);
    } else {
      miscellaneous.writeHtmlFromCommentsAndErrors(parsedInput, errorSpan);
    }
    if (parsedInput.content !== null && parsedInput.content !== undefined) {
      incomingContent = decodeURIComponent(parsedInput.content);
    }
    let problemIdURLed = storage.variables.editor.currentlyEditedPage.getValue();
    storage.variables.currentCourse.fileName.setAndStore(decodeURIComponent(problemIdURLed));
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

/** @return {HTMLElement} */
function getNextEditButton(problemId) {
  return getNavigationEditButton(problemId, "&#8594;");
}

/** @return {HTMLElement} */
function getPreviousEditButton(problemId) {
  return getNavigationEditButton(problemId, "&#8592;");
}

/** @return {HTMLElement} */
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
    "click", () => {
      selectEditPage(problemId, null);
    }
  );
  navigationButton.className = "buttonNavigationStandard";
  miscellaneous.writeHTML(navigationButton, contentHTML);
  return navigationButton;
}


class ProblemEditor {
  constructor() {
    this.mainPage = null;
    this.allTopics = null;
  }
  initialize(mainPage, allTopics) {
    this.mainPage = mainPage;
    this.allTopics = allTopics;
  }

  selectEditPage(
    /** @type {string} */
    currentlyEditedPage,
    /** @type {boolean} Whether online edit is allowed. */
    withInstructorRights,
  ) {
    if (withInstructorRights === undefined || withInstructorRights === null) {
      withInstructorRights = globalUser.hasInstructorRightsNotViewingAsStudent();
    }
    let saveButton = document.getElementById(ids.domElements.buttonSaveEdit);
    if (withInstructorRights) {
      saveButton.disabled = false;
    } else {
      saveButton.disabled = true;
    }
    let storageVariables = storage.variables;
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
    if (
      !this.mainPage.flagProblemPageOnly &&
      storage.variables.currentPage.getValue() !== this.mainPage.pages.editPage.name
    ) {
      this.mainPage.selectAndStorePage(this.mainPage.pages.editPage.name);
    }

    this.writeNextPreviousEditButton(currentlyEditedPage);
    let topicTextArea = document.getElementById(ids.domElements.textAreaTopicListEntry);
    topicTextArea.value = `Title: ${currentlyEditedPage}\nProblem: ${currentlyEditedPage}`;
    topicTextArea.cols = currentlyEditedPage.length + 15;

    let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requestEditPage}&`;
    url += `${pathnames.urlFields.problem.fileName}=${storage.variables.editor.currentlyEditedPage.getValue()}`;
    submitRequests.submitGET({
      url: url,
      callback: function (input) {
        selectEditPageCallback(input, withInstructorRights);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  writeNextPreviousEditButton(
    currentlyEditedPage,
  ) {
    const problem = this.allTopics.getTopicElementByIdOrNull(currentlyEditedPage);
    if (problem === null) {
      return;
    }
    let previousButtonSpan = document.getElementById(ids.domElements.spanButtonPreviousEdit);
    let nextButtonSpan = document.getElementById(ids.domElements.spanButtonNextEdit);
    if (previousButtonSpan !== null) {
      previousButtonSpan.textContent = "";
      previousButtonSpan.appendChild(getPreviousEditButton(problem.previousProblemId));
    }
    if (nextButtonSpan !== null) {
      nextButtonSpan.textContent = "";
      nextButtonSpan.appendChild(getNextEditButton(problem.nextProblemId));
    }
  }
}

function selectEditPage(
  /** @type {string} */
  currentlyEditedPage,
  /** @type {boolean} Whether online edit is allowed. */
  withInstructorRights,
) {
  problemEditor.selectEditPage(currentlyEditedPage, withInstructorRights);
}

function toggleClonePanel(button) {
  let panel = button.nextElementSibling;
  if (panel.style.maxHeight === '200px') {
    panel.style.opacity = '0';
    panel.style.maxHeight = '0';
    miscellaneous.writeHTML(button, `Clone panel &#9666;`);
  } else {
    panel.style.opacity = '1';
    panel.style.maxHeight = '200px';
    miscellaneous.writeHTML(button, `Clone panel &#9660;`);
  }
}

const problemEditor = new ProblemEditor();

module.exports = {
  problemEditor,
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