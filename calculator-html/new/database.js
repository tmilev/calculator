"use strict";

function updateDatabasePageCallback(incoming, output){
  console.log(incoming);
}

function updateDatabasePage(){
  var theUrl = `${pathnames.calculator}?${pathnames.requestDatabase}&${pathnames.databaseTable}=${thePage.pages.database.storage.currentTable}`;
  submitGET({
    url: theUrl,
    callback: updateDatabasePageCallback
  });
}