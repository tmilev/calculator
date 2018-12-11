"use strict";
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');
const pathnames = require('./pathnames');
const jsonToHtml = require('./json_to_html');

function clickDatabaseTable(currentCollection) {
  window.calculator.mainPage.storage.variables.database.currentTable.setAndStore(currentCollection); 
  updateDatabasePage();
}

function updateDatabasePageCallback(incoming, output) {
  //try {
    var thePage = window.calculator.mainPage;
    var currentTable = thePage.storage.variables.database.currentTable.getValue();
    var theParsed = JSON.parse(incoming);
    var theOutput = document.getElementById(ids.domElements.divDatabaseOutput);
    if ("rows" in theParsed) {
      var transformer = new jsonToHtml.JSONToHTML();
      document.getElementById(ids.domElements.spanDatabaseComments).innerHTML = `${theParsed.rows.length} out of ${theParsed.totalRows} rows displayed.<br> `;
      theOutput.innerHTML = transformer.getHtmlFromArrayOfObjects(theParsed.rows, {table: currentTable});
      transformer.bindButtons();
    } else {
      for (var counterCollection = 0; counterCollection < theParsed.collections.length; counterCollection ++) {
        var currentCollection = theParsed.collections[counterCollection]; 
        var linkHTML = "";
        linkHTML += `<a href = "#" onclick = window.calculator.database.clickDatabaseTable('${currentCollection}')>`;
        linkHTML += `${currentCollection}</a>`;
        theParsed.collections[counterCollection] = linkHTML;
      }
      var transformer = new jsonToHtml.JSONToHTML();
      theOutput.innerHTML = transformer.getHtmlFromArrayOfObjects(theParsed.collections);
      transformer.bindButtons();
    }
  //} catch (e) {
  //  console.log(`Error parsing calculator output. ${e}`);
  //}
}

function updateDatabasePageResetCurrentTable() {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.database.currentTable.setAndStore("");
  updateDatabasePage();
}

function updateDatabasePage() {
  var thePage = window.calculator.mainPage;
  if (!thePage.isLoggedIn()) {
    document.getElementById(ids.domElements.divDatabaseOutput).innerHTML = "<b>Not logged-in.</b>";
    return;
  }
  var currentTable = thePage.storage.variables.database.currentTable.getValue();
  var theUrl = "";
  theUrl += `${pathnames.urls.calculatorAPI}?`;
  theUrl += `${pathnames.urlFields.request}=${pathnames.urlFields.requestDatabase}&`;
  theUrl += `${pathnames.urlFields.databaseTable}=${currentTable}&`;
  submitRequests.submitGET({
    url: theUrl,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: updateDatabasePageCallback
  });
}

module.exports = {
  updateDatabasePage,
  clickDatabaseTable,
  updateDatabasePageResetCurrentTable
}