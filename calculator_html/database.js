"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const jsonToHtml = require('./json_to_html');
const CompositeDataKey = require('./json_to_html').CompositeDataKey;
const DataProcessor = require('./json_to_html').DataProcessor;
const DataProcessorCollection = require('./json_to_html').DataProcessorCollection;
const DataTransformer = require('./json_to_html').DataTransformer;
const miscellaneous = require("./miscellaneous_frontend");
const panels = require('./panels');
const storage = require('./storage');

class DatabasePage {
  constructor() {

  }
  initialize() {
    let tableButtons = document.getElementById(
      ids.domElements.pages.database.buttonTables
    );
    tableButtons.addEventListener('click', () => {
      updateDatabasePageResetCurrentTable();
    });
  }
}

function clickDatabaseTable(currentCollection) {
  window.calculator.mainPage.storage.variables.database.labels.setAndStore(
    JSON.stringify(currentCollection)
  );
  updateDatabasePage();
}

function callbackFetchProblemData(
  /** @type {string} */
  input,
  /** @type {HTMLElement} */
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
  /** @type {HTMLElement} */
  output,
) {
  //let labels = JSON.parse(labelsString);
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  url += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  url += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  let labelsString = JSON.stringify(labels);
  url += `${pathnames.urlFields.database.labels}=${labelsString}&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: (input, _) => {
      callbackFetchProblemData(input, output);
    },
  });
}

function deleteDatabaseItemCallback(
  deletedItem
) {
  document.getElementById(deletedItem).remove();
}

function deleteDatabaseItem(
  _,
  input,
  labels,
) {
  let finalSelector = {
    fields: labels
  };
  let item = encodeURIComponent(JSON.stringify(finalSelector));
  let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=databaseDeleteOneEntry&item=${item}`;
  submitRequests.submitGET({
    url: url,
    callback: () => {
      deleteDatabaseItemCallback(input);
    },
    progress: ids.domElements.spanProgressReportGeneral
  });
}

function shortenString(input) {
  return miscellaneous.shortenString(8, input);
}

let shortener = new DataTransformer(null, shortenString);
let problemFetcher = new DataTransformer(
  fetchProblemData,
  (_) => {
    return "problem data";
  }
);

let optionsDatabase = new DataProcessorCollection([
  new DataProcessor(
    new CompositeDataKey("users", ["problemDataJSON"]),
    problemFetcher,
  ),
  new DataProcessor(
    new CompositeDataKey("users", ["activationToken"]),
    shortener,
  ),
  new DataProcessor(
    new CompositeDataKey("users", ["activationToken"]),
    shortener,
  ),
  new DataProcessor(
    new CompositeDataKey("users", ["authenticationToken"]),
    shortener,
  ),
  new DataProcessor(
    new CompositeDataKey("users", ["_id", "$oid"]),
    shortener,
  ),
  new DataProcessor(
    new CompositeDataKey("users", ["password"]),
    shortener,
  ),
]);

function updateDatabasePageCallback(incoming, unused) {
  let labelString = storage.storage.variables.database.labels.getValue();
  let labels = [];
  try {
    labels = JSON.parse(labelString);
  } catch (e) {
    labels = [""];
  }
  let parsed = miscellaneous.jsonUnescapeParse(incoming);
  let output = document.getElementById(ids.domElements.divDatabaseOutput);
  output.textContent = "";
  miscellaneous.writeHtmlElementsFromCommentsAndErrors(parsed, output);
  if (
    parsed.error !== undefined &&
    parsed.error != null &&
    parsed.error != ""
  ) {
    miscellaneous.writeHTML(output, miscellaneous.jsonParseGetHtmlStandard(incoming));
  } else if ("rows" in parsed) {
    let transformer = new jsonToHtml.JSONToHTML();
    for (let i = 0; i < parsed.rows.length; i++) {
      parsed.rows[i]["problemDataJSON"] = "";
    }
    let comments =
      document.getElementById(
        ids.domElements.spanDatabaseComments
      );
    miscellaneous.writeHTML(
      comments,
      `${parsed.rows.length} out of ${parsed.totalRows} rows displayed.<br> `,
    );
    output.appendChild(
      transformer.getTableFromObject(
        parsed.rows, optionsDatabase, {
        table: labels[0]
      })
    );
  } else {
    updateTables(parsed, output);
  }
}

function updateTables(parsed, /** @type {HTMLElement} */ output) {
  if (parsed.collections === null || parsed.collections === undefined) {
    let errorDiv = document.createElement("b");
    errorDiv.style.color = "red";
    errorDiv.textContent = "Missing collections.";
    output.appendChild(errorDiv);
    return;
  }
  let table = document.createElement("table");
  table.className = "tableJSON";
  for (let i = 0; i < parsed.collections.length; i++) {
    let currentCollection = parsed.collections[i];
    let anchor = document.createElement("a");
    anchor.textContent = currentCollection;
    let urlObjectIncoming = miscellaneous.deepCopy(storage.storage.urlObject);
    urlObjectIncoming.databaseLabels = currentCollection;
    anchor.href = `#${JSON.stringify(urlObjectIncoming)}`;
    anchor.addEventListener("click", () => {
      clickDatabaseTable([currentCollection]);
    });
    table.insertRow().insertCell().appendChild(anchor);
  }
  output.appendChild(table);
}


function updateDatabasePageResetCurrentTable() {
  storage.storage.variables.database.labels.setAndStore("[]");
  updateDatabasePage();
}

function updateDatabasePage() {
  let page = window.calculator.mainPage;
  if (!page.isLoggedIn()) {
    let element = document.getElementById(ids.domElements.divDatabaseOutput);
    miscellaneous.writeHTML(element, "<b>Not logged-in.</b>");
    return;
  }
  let labels = page.storage.variables.database.labels.getValue();
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  url += `${pathnames.urlFields.request}=${pathnames.urlFields.requests.database}&`;
  url += `${pathnames.urlFields.database.operation}=${pathnames.urlFields.database.fetch}&`;
  url += `${pathnames.urlFields.database.labels}=${labels}&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: updateDatabasePageCallback,
  });
}

let databasePage = new DatabasePage();
databasePage.initialize();

module.exports = {
  databasePage,
  optionsDatabase,
  updateDatabasePage,
  clickDatabaseTable,
  updateDatabasePageResetCurrentTable,
};