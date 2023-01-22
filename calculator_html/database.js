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
  /** @type{string} */
  input,
  /** @type{HTMLElement} */
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
  /** @type{HTMLElement} */
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
  let parsed = miscellaneous.jsonUnescapeParse(incoming);
  let output = document.getElementById(ids.domElements.divDatabaseOutput);
  output.textContent = "";
  miscellaneous.writeHtmlElementsFromCommentsAndErrors(parsed, output);
  if (
    parsed.error !== undefined &&
    parsed.error != null &&
    parsed.error != ""
  ) {
    output.innerHTML = miscellaneous.jsonParseGetHtmlStandard(incoming);
  } else if ("rows" in parsed) {
    let transformer = new jsonToHtml.JSONToHTML();
    for (let i = 0; i < parsed.rows.length; i++) {
      parsed.rows[i]["problemDataJSON"] = "";
    }
    document.getElementById(
      ids.domElements.spanDatabaseComments
    ).innerHTML = `${parsed.rows.length} out of ${parsed.totalRows} rows displayed.<br> `;
    output.appendChild(
      transformer.getTableFromObject(
        parsed.rows, optionsDatabase, {
        table: labels[0]
      })
    );
  } else {
    let table = document.createElement("table");
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
    table.className = "tableJSON";
    output.appendChild(table);
  }
}

function updateDatabasePageResetCurrentTable() {
  storage.storage.variables.database.labels.setAndStore("[]");
  updateDatabasePage();
}

function updateDatabasePage() {
  let page = window.calculator.mainPage;
  if (!page.isLoggedIn()) {
    document.getElementById(ids.domElements.divDatabaseOutput).innerHTML = "<b>Not logged-in.</b>";
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
  transformersDatabase,
  optionsDatabase,
  updateDatabasePage,
  clickDatabaseTable,
  updateDatabasePageResetCurrentTable,
};