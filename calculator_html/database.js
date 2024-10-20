"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const jsonToHtml = require('./json_to_html');
const CompositeDataKey = require('./json_to_html').CompositeDataKey;
const CompositeDataKeyAndValue = require('./json_to_html').CompositeDataKeyAndValue;
const DataProcessor = require('./json_to_html').DataProcessor;
const DataProcessorCollection = require('./json_to_html').DataProcessorCollection;
const DataTransformer = require('./json_to_html').DataTransformer;
const miscellaneous = require("./miscellaneous_frontend");
const panels = require('./panels');
const storage = require('./storage').storage;

class DatabasePage {
  constructor() {
    this.mainPage = null;
  }
  initialize(mainPage) {
    this.mainPage = mainPage;
    let tableButtons = document.getElementById(
      ids.domElements.pages.database.buttonTables
    );
    if (tableButtons === null) {
      // Database page not found.
      return;
    }
    tableButtons.addEventListener('click', () => {
      updateDatabasePageResetCurrentTable();
    });
  }
  updateDatabasePage(/** @type{string?} */ requestedOperation) {
    if (!this.mainPage.isLoggedIn()) {
      let element = document.getElementById(ids.domElements.divDatabaseOutput);
      miscellaneous.writeHTML(element, "<b>Not logged-in.</b>");
      return;
    }
    const urlFields = pathnames.urlFields;
    if (requestedOperation === undefined || requestedOperation === null) {
      requestedOperation = urlFields.database.fetch;
    }
    let findQuery = storage.variables.database.findQuery.getValue();
    console.log("DEBUG: and the findQuery is: ", findQuery);
    let url = "";
    url += `${pathnames.urls.calculatorAPI}?`;
    url += `${urlFields.request}=${urlFields.requests.database}&`;
    url += `${urlFields.database.operation}=${requestedOperation}&`;
    url += `${urlFields.database.findQuery}=${findQuery}&`;
    submitRequests.submitGET({
      url: url,
      progress: ids.domElements.spanProgressReportGeneral,
      callback: updateDatabasePageCallback,
    });
  }
}

function clickDatabaseTable(findQuery) {
  storage.variables.database.findQuery.setAndStore(
    JSON.stringify(findQuery)
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
  let commentDiv = document.createElement("div");
  try {
    let inputParsed = JSON.parse(input);
    miscellaneous.writeHtmlFromCommentsAndErrors(inputParsed, commentDiv);
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
  let resultWithCommments = document.createElement("div");
  resultWithCommments.appendChild(commentDiv);
  resultWithCommments.appendChild(resultHTML);
  outputPanel.initialize(false);
  outputPanel.setPanelContent(resultWithCommments);
}

function fetchProblemData(
  /** @type {CompositeDataKey} */
  key,
  /** @type {CompositeDataKeyAndValue|null} */
  ambientRowSelector,
  /** @type {HTMLElement} */
  output,
) {
  if (ambientRowSelector === null) {
    let errorMessage = document.createElement("b");
    errorMessage.textContent = "Missing row selector.";
    errorMessage.style.color = "red";
    output.textContent = "";
    output.appendChild(errorMessage);
    return;
  }
  let url = "";
  const urlFields = pathnames.urlFields;
  const fetchCommand = urlFields.database.fetch;
  const operation = urlFields.database.operation;
  const api = pathnames.urls.calculatorAPI;
  const request = urlFields.request;
  const database = urlFields.requests.database;
  const find = urlFields.database.findQuery;
  url += `${api}?`;
  url += `${request}=${database}&`;
  url += `${operation}=${fetchCommand}&`;
  const query = JSON.stringify(ambientRowSelector.toObject());
  url += `${find}=${query}&`;
  url += `${pathnames.urlFields.database.projector}="${key.toStringLabels()}"&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    callback: (input, _) => {
      callbackFetchProblemData(input, output);
    },
  });
}

function queryFromCollection(
  /** @type {string} */
  collection,
) {
  let result = {};
  result[pathnames.urlFields.database.collection] = collection;
  return result;
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
  const api = pathnames.urls.calculatorAPI;
  let item = encodeURIComponent(JSON.stringify(finalSelector));
  let url = `${api}?${pathnames.urlFields.request}=databaseDeleteOneEntry&item=${item}`;
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
  let findQueryString = storage.variables.database.findQuery.getValue();
  let findQuery = {};
  try {
    findQuery = JSON.parse(findQueryString);
  } catch (e) {
    findQuery = {};
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
        parsed.rows,
        optionsDatabase, {
        collection: findQuery[pathnames.urlFields.database.collection],
      })
    );
  } else {
    updateTables(parsed, output);
  }
}

function updateTables(
  parsed,
  /** @type {HTMLElement} */
  output,
) {
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
    let findQuery = queryFromCollection(currentCollection);
    anchor.href = `#${JSON.stringify(findQuery)}`;
    anchor.addEventListener("click", () => {
      clickDatabaseTable(findQuery);
    });
    table.insertRow().insertCell().appendChild(anchor);
  }
  output.appendChild(table);
}

function updateDatabasePageResetCurrentTable() {
  storage.variables.database.findQuery.setAndStore("{}");
  updateDatabasePage(pathnames.urlFields.database.allTables);
}

function updateDatabasePage(/** @type{string?} */ requestedOperation) {
  databasePage.updateDatabasePage(requestedOperation);
}

let databasePage = new DatabasePage();

module.exports = {
  databasePage,
  optionsDatabase,
  updateDatabasePage,
  clickDatabaseTable,
  updateDatabasePageResetCurrentTable,
};