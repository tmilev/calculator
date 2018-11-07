"use strict";
const submitRequests = require('./submit_requests');

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
  result += `<span class = 'spanFileInfo'>`;
  result += `<button class = "buttonSaveEdit" onclick = "selectEditPage('${fileName}')" style = 'width:50px'>Edit</button>${fileName}&nbsp;`;
  result += `<button class = "accordionLike" onclick = "toggleClonePanel(this)">Clone panel &#9666;</button>`;
  result += `<span class = "panelDeadlines">`;
  var idCloneInput = encodeURIComponent(`cloneButton${fileName}`);
  var idSpanClonePageReport = encodeURIComponent(`cloneButtonReport${fileName}`);
  result += `<button class = 'buttonClone' style = 'width:50px' onclick = `;
  result += `"window.calculator.editPage.handleClone('${fileName}', '${idCloneInput}', '${idSpanClonePageReport}')">Clone</button>`;
  result += `<input type = "text" value = '${fileName}' style = 'width:80%' id = '${idCloneInput}'></input><br>`
  result += `<div id = "${idSpanClonePageReport}" style = 'width:80%'></div>`;
  result += "</span>";
  result += `</span>`;
  return result;
}

function handleClone(fileName, idCloneInput, idSpanClonePageReport) {
  var newFileName = document.getElementById(idCloneInput).value;

  console.log(`DEBUG: handle clone with input: ${fileName}, new file name: ${newFileName}`); 
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += submitRequests.getQueryStringSubmitStringAsMainInput(newFileName, pathnames.requestClonePage);
  theURL += `${pathnames.fileName}=${fileName}&`;
  //console.log("DEBUG: about to submit: " + theURL); 
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
  theURL += submitRequests.getQueryStringSubmitStringAsMainInput(editor.getValue(), pathnames.requestModifyPage);
  theURL += `${pathnames.fileName}=${thePage.storage.editor.currentlyEditedPage.getValue()}&`;
  //console.log("DEBUG: about to submit: " + theURL); 
  submitRequests.submitGET({
    url: theURL,
    result: "spanSubmitEditPageReport"
  });
}

function selectEditPageCallback(input, outputComponent) {
  var thePage = window.calculator.mainPage;
  try {
    var parsedInput = JSON.parse(input);
    //document.getElementById('divEditorAce').textContent = decodeURIComponent(parsedInput.content);
    ace.require("ace/ext/language_tools");
    if (thePage.pages.editPage.editor === null) {
      thePage.pages.editPage.editor = ace.edit("divEditorAce");
    }
    var editor = thePage.pages.editPage.editor;
    editor.$blockScrolling = Infinity;
    editor.getSession().setValue(decodeURIComponent(parsedInput.content));
    editor.setTheme("ace/theme/chrome");
    editor.getSession().setMode("ace/mode/xml");
    editor.setOptions({
      enableBasicAutocompletion: true,
      enableLiveAutocompletion: true
    });
    editor.completers = [staticWordCompleter];    
    editor.$blockScrolling = Infinity;
  } catch (e) {
    console.log("Error: " + e)
  }
}

function selectEditPage(currentlyEditedPage) {
  var thePage = window.calculator.mainPage;
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = thePage.storage.editor.currentlyEditedPage.getValue();
  }
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = "/coursesavailable/default.txt";
  }
  thePage.storage.editor.currentlyEditedPage.setAndStore(currentlyEditedPage);
  if (thePage.storage.currentPage.getValue() !== thePage.pages.editPage.name) {
    thePage.selectPage(thePage.pages.editPage.name);
    return;
  }
  var theTopicTextArea = document.getElementById("textareaTopicListEntry");
  theTopicTextArea.value = `Title: ${currentlyEditedPage}\nProblem: ${currentlyEditedPage}`;
  theTopicTextArea.cols = currentlyEditedPage.length + 15;

  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.request}=${pathnames.requestEditPage}&`;
  theURL += `fileName=${thePage.storage.editor.currentlyEditedPage.getValue()}`;
  submitRequests.submitGET({
    url: theURL,
    callback: selectEditPageCallback
  });
}

module.exports = {
  selectEditPage, 
  getEditPanel,
  handleClone
}