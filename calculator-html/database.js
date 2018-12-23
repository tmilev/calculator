"use strict";
const submitRequests = require('./submit_requests');
const ids = require('./ids_dom_elements');
const pathnames = require('./pathnames');
const jsonToHtml = require('./json_to_html');
const miscellaneous = require('./miscellaneous');
const panels = require('./panels');

function clickDatabaseTable(currentCollection) {
  window.calculator.mainPage.storage.variables.database.labels.setAndStore(JSON.stringify(currentCollection)); 
  updateDatabasePage();
}

function callbackFetchProblemData(button, input, output) {
  var inputParsed = JSON.parse(input);
  var panelId = button.getAttribute("panelId");
  var thePanel = new panels.PanelExpandable(panelId, false);
  var transformer = new jsonToHtml.JSONToHTML();
  var problemData = [];
  try {
    var inputParsed = JSON.parse(input);
    var problemDataRaw = inputParsed.rows[0].problemDataJSON;
    for (var label in problemDataRaw) {
      var incomingProblem = {
        problemName: label,
        problemInfo: problemDataRaw[label],
      }
      problemData.push(incomingProblem)
    }
  } catch (e) {
    problemData = {
      error: e,
      input: input,
    };
  }
  var resultHTML = transformer.getTableFromObject(problemData);
  thePanel.setPanelContent(resultHTML);
  transformer.bindButtons();
  thePanel.attributes.panelStatus = "collapsed";
  thePanel.matchPanelStatus();
  setTimeout(thePanel.doToggleContent.bind(thePanel), 0);
  console.log("DEBUG: parsed input: " + JSON.stringify(inputParsed));
}

function fetchProblemData() {
  var labelsString = this.getAttribute("labels");
  //var labels = JSON.parse(labelsString);
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  theURL += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  theURL += `${pathnames.urlFields.database.labels}=${labelsString}&`;
  console.log("DEBUG: labelString: " + labelsString);
  console.log(" URL: " + theURL);
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: callbackFetchProblemData.bind(null, this),
  });  
}

function deleteDatabaseItemCallback(
  /**@type {JSONToHTML} */
  transformer, 
  deletedItem, 
  labels,
  input,
  output,
) {
  document.getElementById(deletedItem).remove();
}

function deleteDatabaseItem(
  /**@type {JSONToHTML} */
  transformer, 
  input, 
  labels,
) {
  var finalSelector = {
    fields: labels
  };
  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=databaseDeleteOneEntry&item=${escape(JSON.stringify(finalSelector))}`;
  submitRequests.submitGET({
    url: theURL,
    callback: deleteDatabaseItemCallback.bind(null, transformer, input, labels),
    progress: ids.domElements.spanProgressReportGeneral
  });  
}

function bounceString(input) {
  return input;
}

var transformersDatabase = {
  deleteProblemDataItem: {
    clickHandler: deleteDatabaseItem,
  },
  fetchProblemData: {
    clickHandler: fetchProblemData,
    transformer: bounceString.bind(null, "problem data"),
  },
};

var optionsDatabase = {
  transformers: {
    "users.${number}.problemDataJSON": transformersDatabase.fetchProblemData,
    "users.${number}.activationToken" : jsonToHtml.transformersStandard.shortener,
    "users.${number}.authenticationToken": jsonToHtml.transformersStandard.shortener,
    "users.${number}.password": jsonToHtml.transformersStandard.shortener,
    "users.${number}._id": jsonToHtml.transformersStandard.shortener,
  },
};

function updateDatabasePageCallback(incoming, output) {
  var thePage = window.calculator.mainPage;
  var labelString = thePage.storage.variables.database.labels.getValue();
  var labels = [];
  try {
    labels = JSON.parse(labelString);
  } catch (e) {
    labels = [""];
  }
  var theParsed = JSON.parse(incoming);
  var theOutput = document.getElementById(ids.domElements.divDatabaseOutput);
  if ("rows" in theParsed) {
    var transformer = new jsonToHtml.JSONToHTML();
    for (var i = 0; i < theParsed.rows.length; i ++) {
      theParsed.rows[i]["problemDataJSON"] = "";
    }
    document.getElementById(ids.domElements.spanDatabaseComments).innerHTML = `${theParsed.rows.length} out of ${theParsed.totalRows} rows displayed.<br> `;
    theOutput.innerHTML = transformer.getTableFromObject(theParsed.rows, optionsDatabase, {table: labels[0]});
    transformer.bindButtons();
  } else {
    for (var counterCollection = 0; counterCollection < theParsed.collections.length; counterCollection ++) {
      var currentCollection = theParsed.collections[counterCollection]; 
      var linkHTML = "";
      var urlObjectIncoming = miscellaneous.deepCopy(thePage.storage.urlObject);
      urlObjectIncoming.databaseLabels = currentCollection;
      linkHTML += `<a href = '#${JSON.stringify(urlObjectIncoming)}' onclick = 'window.calculator.database.clickDatabaseTable(["${currentCollection}"]);'>`;
      linkHTML += `${currentCollection}</a>`;
      theParsed.collections[counterCollection] = linkHTML;
    }
    var transformer = new jsonToHtml.JSONToHTML();
    theOutput.innerHTML = transformer.getTableFromObject(theParsed.collections);
    transformer.bindButtons();
  }
}

function updateDatabasePageResetCurrentTable() {
  var thePage = window.calculator.mainPage;
  thePage.storage.variables.database.labels.setAndStore("[]");
  updateDatabasePage();
}

function updateDatabasePage() {
  var thePage = window.calculator.mainPage;
  if (!thePage.isLoggedIn()) {
    document.getElementById(ids.domElements.divDatabaseOutput).innerHTML = "<b>Not logged-in.</b>";
    return;
  }
  var labels = thePage.storage.variables.database.labels.getValue();
  var theUrl = "";
  theUrl += `${pathnames.urls.calculatorAPI}?`;
  theUrl += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  theUrl += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  theUrl += `${pathnames.urlFields.database.labels}=${labels}&`;
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