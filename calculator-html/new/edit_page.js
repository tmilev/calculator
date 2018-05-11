"use strict";

function getEditPagePanel(filesToEdit) { 
  var result = "";
  result += "<editPagePanel>";
  result += "</editPagePanel>";
  return result;
}

var AceEditorAutoCompletionWordList = [];

var staticWordCompleter = {
  getCompletions: function(editor, session, pos, prefix, callback) {
    callback(null, AceEditorAutoCompletionWordList.map(function(word) {
      return {
        caption: word,
        value: word,
        meta: "static"
      };
    }));
  }
}

function ctrlSPressAceEditorHandler(event){
  if (event.ctrlKey != true){
    return;
  }
  if (event.keyCode != 83){
    return;
  }
  console.log("not implemented yet");
  event.preventDefault();
  submitStringAsMainInput(editor.getValue(), 'spanSubmitReport', 'modifyPage', null, 'spanSubmitReport');
}

function selectEditPageCallback(input, outputComponent) {
  try {
    var parsedInput = JSON.parse(input);
    document.getElementById('editor').textContent = decodeURIComponent(parsedInput.content);

    ace.require("ace/ext/language_tools");
    var editor = ace.edit("editor");
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
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = thePage.pages.editPage.storage.currentlyEditedPage;
  }
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = "/coursesavailable/default.txt";
  }
  var theURL = `${pathnames.calculatorAPI}?${pathnames.requestEditPage}&fileName=${currentlyEditedPage}`;
  submitGET({
    url: theURL,
    callback: selectEditPageCallback
  });
}