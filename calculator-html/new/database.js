"use strict";

function updateDatabasePageCallback(incoming, output){
  try {
    var theParsed = JSON.parse(incoming);
    console.log(JSON.stringify(theParsed));
    var theOutput = document.getElementById("divDatabaseOutput");
    if ("rows" in theParsed) {
      theOutput.innerHTML = getHtmlFromArrayOfObjects(theParsed.rows);
    } else {
      theOutput.innerHTML = getHtmlFromArrayOfObjects(theParsed.collections);
    }
  } catch (e) {
    console.log(`Error parsing calculator output. ${e}`);
  }
}

function updateDatabasePage(){
  var theUrl = `${pathnames.calculator}?${pathnames.requestDatabase}&${pathnames.databaseTable}=${thePage.pages.database.storage.currentTable}`;
  submitGET({
    url: theUrl,
    callback: updateDatabasePageCallback
  });
}