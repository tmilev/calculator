"use strict";

function updateDatabasePageCallback(incoming, output) {
  try {
    var currentTable = thePage.storage.database.currentTable.getValue();
    var theParsed = JSON.parse(incoming);
    //console.log("DEBUG: incoming: " + JSON.stringify(theParsed));
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
  var theUrl = `${pathnames.calculatorAPI}?${pathnames.requestDatabase}&${pathnames.databaseTable}=${currentTable}`;
  submitGET({
    url: theUrl,
    progress: "spanProgressReportGeneral",
    callback: updateDatabasePageCallback
  });
}