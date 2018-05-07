"use strict";

function getEditPagePanel(filesToEdit) { 
  var result = "";
  result += "<editPagePanel>";
  result += "</editPagePanel>";
  return result;
}

function selectEditPageCallback(input, outputComponent) {
  document.getElementById("divEditPageContainer").innerHTML = input;
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