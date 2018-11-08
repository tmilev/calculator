"use strict";
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');

function updateDatabasePageCallback(incoming, output) {
  try {
    var currentTable = thePage.storage.database.currentTable.getValue();
    var theParsed = JSON.parse(incoming);
    var theOutput = document.getElementById("divDatabaseOutput");
    if ("rows" in theParsed) {
      theOutput.innerHTML = getHtmlFromArrayOfObjects(theParsed.rows, {table: currentTable});
    } else {
      for (var counterCollection = 0; counterCollection < theParsed.collections.length; counterCollection ++) {
        var currentCollection = theParsed.collections[counterCollection]; 
        var linkHTML = "";
        linkHTML += `<a href = "#" onclick = `;
        linkHTML += `"thePage.storage.database.currentTable.setAndStore('${currentCollection}'); updateDatabasePage();"`;
        linkHTML += `>${currentCollection}</a>`;
        theParsed.collections[counterCollection] = linkHTML;
      }
      theOutput.innerHTML = getHtmlFromArrayOfObjects(theParsed.collections);
    }
  } catch (e) {
    console.log(`Error parsing calculator output. ${e}`);
  }
}

function updateDatabasePageResetCurrentTable() {
  thePage.storage.database.currentTable.setAndStore("");
  updateDatabasePage();
}

function updateDatabasePage() {
  thePage.storeSettings();
  var currentTable = thePage.storage.database.currentTable.getValue();
  var theUrl = `${pathnames.urls.calculatorAPI}?${pathnames.request}=${pathnames.requestDatabase}&${pathnames.databaseTable}=${currentTable}`;
  submitRequests.submitGET({
    url: theUrl,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: updateDatabasePageCallback
  });
}

module.exports = {
  updateDatabasePage
}