"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const jsonToHtml = require('./json_to_html');
const miscellaneous = require("./miscellaneous_frontend");
const panels = require('./panels');
const storage = require('./storage');

class DatabasePage {
  constructor() {
    
  }
  initialize() {
    let tableButtons = document.getElementById(ids.domElements.pages.database.buttonTables);
    tableButtons.addEventListener('click', () => {
      updateDatabasePageResetCurrentTable();   
    });     
  }
}

function clickDatabaseTable(currentCollection) {
  window.calculator.mainPage.storage.variables.database.labels.setAndStore(JSON.stringify(currentCollection));
  updateDatabasePage();
}

function callbackFetchProblemData(
  /**@type{string} */
  input,
  /**@type{HTMLElement} */
  output,
) {
  let outputPanel = new panels.PanelExpandable(output);
  let problemData = [];
  try {
    let inputParsed = JSON.parse(input);
    let problemDataRaw = inputParsed.rows[0].problemDataJSON;
    for (let label in problemDataRaw) {
      let incomingProblem = {
        name: label,
        problemInformation: problemDataRaw[label],
      }
      problemData.push(incomingProblem)
    }
  } catch (e) {
    problemData = {
      error: e,
      input: input,
    };
  }
  let transformer = new jsonToHtml.JSONToHTML();
  let resultHTML = transformer.getTableFromObject(problemData);
  outputPanel.initialize(false);
  outputPanel.setPanelContent(resultHTML);
}

function fetchProblemData(
  labels,
  /**@type{HTMLElement} */
  output,
) {
  //let labels = JSON.parse(labelsString);
  let theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  theURL += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  let labelsString = JSON.stringify(labels);
  theURL += `${pathnames.urlFields.database.labels}=${labelsString}&`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: (input, _) => {
      callbackFetchProblemData(input, output);
    },
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
  let finalSelector = {
    fields: labels
  };
  let theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=databaseDeleteOneEntry&item=${escape(JSON.stringify(finalSelector))}`;
  submitRequests.submitGET({
    url: theURL,
    callback: deleteDatabaseItemCallback.bind(null, transformer, input, labels),
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function bounceString(input) {
  return input;
}

let transformersDatabase = {
  deleteProblemDataItem: {
    clickHandler: deleteDatabaseItem,
  },
  fetchProblemData: {
    clickHandler: fetchProblemData,
    transformer: bounceString.bind(null, "problem data"),
  },
};

let optionsDatabase = {
  transformers: {
    "users.${number}.problemDataJSON": transformersDatabase.fetchProblemData,
    "users.${number}.activationToken": jsonToHtml.transformersStandard.shortener,
    "users.${number}.authenticationToken": jsonToHtml.transformersStandard.shortener,
    "users.${number}.password": jsonToHtml.transformersStandard.shortener,
    "users.${number}._id": jsonToHtml.transformersStandard.shortener,
  },
};

function updateDatabasePageCallback(incoming, unused) {
  let labelString = storage.storage.variables.database.labels.getValue();
  let labels = [];
  try {
    labels = JSON.parse(labelString);
  } catch (e) {
    labels = [""];
  }
  let theParsed = miscellaneous.jsonUnescapeParse(incoming);
  let output = document.getElementById(ids.domElements.divDatabaseOutput);
  output.textContent = "";
  if (
    theParsed.error !== undefined &&
    theParsed.error != null &&
    theParsed.error != ""
  ) {
    output.innerHTML = miscellaneous.jsonParseGetHtmlStandard(incoming);
  } else if ("rows" in theParsed) {
    let transformer = new jsonToHtml.JSONToHTML();
    for (let i = 0; i < theParsed.rows.length; i++) {
      theParsed.rows[i]["problemDataJSON"] = "";
    }
    document.getElementById(ids.domElements.spanDatabaseComments).innerHTML = `${theParsed.rows.length} out of ${theParsed.totalRows} rows displayed.<br> `;
    output.textContent = "";
    output.appendChild(transformer.getTableFromObject(theParsed.rows, optionsDatabase, { table: labels[0] }));
  } else {
    for (let counterCollection = 0; counterCollection < theParsed.collections.length; counterCollection++) {
      let currentCollection = theParsed.collections[counterCollection];
      let linkHTML = "";
      let urlObjectIncoming = miscellaneous.deepCopy(storage.storage.urlObject);
      urlObjectIncoming.databaseLabels = currentCollection;
      linkHTML += `<a href = '#${JSON.stringify(urlObjectIncoming)}' onclick = 'window.calculator.database.clickDatabaseTable(["${currentCollection}"]);'>`;
      linkHTML += `${currentCollection}</a>`;
      theParsed.collections[counterCollection] = linkHTML;
    }
    let transformer = new jsonToHtml.JSONToHTML();
    output.appendChild(transformer.getTableFromObject(theParsed.collections));
  }
}

function updateDatabasePageResetCurrentTable() {
  storage.storage.variables.database.labels.setAndStore("[]");
  updateDatabasePage();
}

function updateDatabasePage() {
  let thePage = window.calculator.mainPage;
  if (!thePage.isLoggedIn()) {
    document.getElementById(ids.domElements.divDatabaseOutput).innerHTML = "<b>Not logged-in.</b>";
    return;
  }
  let labels = thePage.storage.variables.database.labels.getValue();
  let theUrl = "";
  theUrl += `${pathnames.urls.calculatorAPI}?`;
  theUrl += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  theUrl += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  theUrl += `${pathnames.urlFields.database.labels}=${labels}&`;
  submitRequests.submitGET({
    url: theUrl,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: updateDatabasePageCallback,
  });
}

let databasePage = new DatabasePage();
databasePage.initialize();

module.exports = {
  databasePage,
  transformersDatabase,
  optionsDatabase,
  updateDatabasePage,
  clickDatabaseTable,
  updateDatabasePageResetCurrentTable,
};