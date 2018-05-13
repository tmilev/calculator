"use strict";

function getEditPagePanel(filesToEdit) { 
  var result = "";
  result += "<editPagePanel>";
  result += "</editPagePanel>";
  return result;
}

var staticWordCompleter = {
  getCompletions: function(editor, session, pos, prefix, callback) {
    callback(null, thePage.pages.editPage.storage.AceEditorAutoCompletionWordList.map(function(word) {
      return {
        caption: word,
        value: word,
        meta: "static"
      };
    }));
  }
}

function ctrlSPressAceEditorHandler(event) {
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

function storeEditedPage() {

}

function selectEditPageCallback(input, outputComponent) {
  try {
    var parsedInput = JSON.parse(input);
    //document.getElementById('divEditorAce').textContent = decodeURIComponent(parsedInput.content);
    ace.require("ace/ext/language_tools");
    var editor = ace.edit("divEditorAce");
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
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = thePage.pages.editPage.storage.currentlyEditedPage;
  }
  if (currentlyEditedPage === undefined || currentlyEditedPage === null) { 
    currentlyEditedPage = "/coursesavailable/default.txt";
  }
  thePage.pages.editPage.storage.currentlyEditedPage = currentlyEditedPage;
  if (thePage.currentPage !== "editPage") {
    thePage.selectPage("editPage");
    return;
  }
  document.getElementById("spanLabelEditedPageFileName").innerHTML = currentlyEditedPage;
  document.getElementById("spanLabelEditedPageComments").classList.remove("divInvisible");
  var theTopicTextArea = document.getElementById("textareaTopicListEntry");
  theTopicTextArea.value  = `Title: ${currentlyEditedPage}\nProblem: ${currentlyEditedPage}`;
  theTopicTextArea.cols = currentlyEditedPage.length + 15;

  var theURL = `${pathnames.calculatorAPI}?${pathnames.requestEditPage}&fileName=${thePage.pages.editPage.storage.currentlyEditedPage}`;
  submitGET({
    url: theURL,
    callback: selectEditPageCallback
  });
}