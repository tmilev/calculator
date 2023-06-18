"use srict";
const miscellaneous = require("./miscellaneous_frontend");
const modifiableDatabaseData = require('./modifiable_database_fields').modifiableDatabaseData;
const panels = require('./panels');

let modifiableFields = {};
for (let i = 0; i < modifiableDatabaseData.modifiableFields.length; i++) {
  let incomingLabel = modifiableDatabaseData.modifiableFields[i].join("");
  modifiableFields[incomingLabel] = true;
}

let transformersStandard = {
  shortener: {
    transformer: (input) => {
      return miscellaneous.shortenString(30, input);
    },
  },
};

function writeJSONtoDOMComponent(
  /** @type {Object} */
  inputObject,
  /** @type {HTMLElement|string} */
  domComponent,
) {
  if (typeof domComponent === "string") {
    domComponent = document.getElementById(domComponent);
  }
  let transformer = new JSONToHTML();
  let copy = miscellaneous.deepCopy(inputObject);

  domComponent.appendChild(
    transformer.getTableFromObject(
      copy, null, {
      forceRowLayout: true
    })
  );
}

class Statistics {
  constructor() {
    this.toggleButtons = 0;
    this.databaseTables = 0;
    this.buttonsGenerated = 0;
  }
}

let statistics = new Statistics();

class CollectionRowSelector {
  constructor(
    /** @type {string} */
    tableName,
  ) {
    this.tableName = tableName;
    this.labels = [];
    this.exactValue = "";
    this.selectorWithinRow = [];
  }
  copy() {
    const result = new CollectionRowSelector(this.tableName);
    result.labels = this.labels.slice();
    result.exactValue = this.exactValue;
    result.selectorWithinRow = this.selectorWithinRow.slice();
  }
}

class JSONToHTML {
  constructor() {
    /** @type {String} */
    this.tableName = "";
    this.ambientObject = null;
    this.labelsRows = null;
    this.optionsConstant = {};
    this.inputParsed = null;
    this.result = null;
  }

  getOptionFromLabel(
    /** @type {CollectionRowSelector} */
    selector,
  ) {
    if (
      this.optionsConstant.transformers === null ||
      this.optionsConstant.transformers === undefined
    ) {
      return null;
    }
    for (let i = - 1; i < selector.selectorWithinRow.length; i++) {
      let labelTry = selector.selectorWithinRow.slice();
      if (i >= 0) {
        labelTry[i] = "${any}";
      }
      let currentLabel = labelTry.join(".");
      if (currentLabel in this.optionsConstant.transformers) {
        return this.optionsConstant.transformers[currentLabel];
      }
    }
  }

  getButtonFromLabels(
    input,
    inputTransformed,
    /** @type {CollectionRowSelector} */
    selector,
    /** @type {Function} */
    clickHandler,
  ) {
    statistics.buttonsGenerated++;
    let result = document.createElement("span");
    let button = document.createElement("button");
    let panel = document.createElement("div");
    button.textContent = inputTransformed;
    let selectorCopy = selector.copy();
    button.addEventListener("click", () => {
      clickHandler(selectorCopy, panel);
    });
    result.appendChild(button);
    result.appendChild(panel);
    return result;
  }

  getToggleButton(
    input,
    label,
  ) {
    let panelContainer = document.createElement("span");
    let currentPanel = new panels.PanelExpandable(panelContainer);
    currentPanel.initialize(true);
    currentPanel.setPanelLabel(label);
    currentPanel.setPanelContent(input);
    return panelContainer;
  }

  /** @return {HTMLElement} */
  getSingleEntry(
    input,
    /** @type {DatabaseSelector} */
    selector,
  ) {
    let currentOption = this.getOptionFromLabel(selector);
    if (currentOption === null || currentOption === undefined) {
      let result = document.createElement("span");
      miscellaneous.writeHTML(result, input);
      return result;
    }
    let inputTransformed = input;
    if (typeof currentOption.transformer === "function") {
      inputTransformed = currentOption.transformer(input);
    }
    statistics.toggleButtons++;
    if (typeof currentOption.clickHandler === "function") {
      let result = document.createElement("span");
      let button = this.getButtonFromLabels(
        input,
        inputTransformed,
        selector,
        currentOption.clickHandler,
      );
      result.appendChild(button);
      return result;
    }
    if (inputTransformed === input) {
      let result = document.createElement("span");
      miscellaneous.writeHTML(result, input);
      return result;
    }
    return this.getToggleButton(input, inputTransformed);
  }

  /** @return {HTMLElement} */
  getTableHorizontallyLaidFromJSON(
    input,
    /** @type {CollectionRowSelector} */
    selector,
  ) {
    let inputType = typeof input;
    if (
      inputType === "string" ||
      inputType === "number" ||
      inputType === "boolean"
    ) {
      return this.getSingleEntry(input, selector);
    }
    if (Array.isArray(input)) {
      return this.getTableHorizontallyLaidFromArray(
        input, selector,
      );
    }
    if (inputType === "object") {
      return this.getTableHorizontallyLaidFromObject(
        input, selector,
      );
    }
    let result = document.createElement("span");
    result.textContent = typeof input;
    return result;
  }

  /** @return {HTMLElement} */
  getErrorElement(
    /** @type {string} */
    message,
  ) {
    let result = document.createElement("b");
    result.style.color = "red";
    result.textContent = message;
    return result;
  }

  /** @return {HTMLElement} */
  getTableHorizontallyLaidFromArray(
    input,
    /** @type {CollectionRowSelector} */
    selector,
  ) {
    if (!Array.isArray(input)) {
      return this.getErrorElement(
        "Error: bad input to: getTableHorizontallyLaidFromArray"
      );
    }
    let table = document.createElement("table");
    table.className = "tableJSONItem";
    for (let i = 0; i < input.length; i++) {
      selector.selectorWithinRow.push(i)
      let item = input[i];
      let row = table.insertRow();
      row.insertCell().appendChild(this.getTinyLabel(`${i}`));
      let content = this.getTableHorizontallyLaidFromJSON(
        item, selector,
      );
      selector.selectorWithinRow.pop();
      row.insertCell().appendChild(content);
    }
    return table;
  }

  /** @return {HTMLElement} */
  getTableHorizontallyLaidFromObject(
    input,
    /** @type {CollectionRowSelector} */
    selector,
  ) {
    if (typeof input !== "object") {
      return this.getErrorElement("Error: bad input to getTableHorizontallyLaidFromObject.");
    }
    let table = document.createElement("table");
    table.className = "tableJSONItem";
    for (let item in input) {
      selector.selectorWithinRow.push(item);
      let currentRow = table.insertRow();
      currentRow.insertCell().textContent = `${item}`;
      let content = this.getTableHorizontallyLaidFromJSON(
        input[item], selector,
      );
      selector.selectorWithinRow.pop();
      currentRow.insertCell().appendChild(content);
    }
    return table;
  }

  transformObjectToRows(input) {
    let result = [];
    for (let labelRow in input) {
      let currentInputItem = input[labelRow];
      let typeOf = typeof currentInputItem;
      if (typeOf !== "object") {
        input[labelRow] = { "x": currentInputItem };
        currentInputItem = input[labelRow];
      }
      currentInputItem["_rowLabel"] = labelRow;
      result.push(currentInputItem);
    }
    return result;
  }

  /** @return {HtmlElement} */
  getTableFromObject(
    input,
    optionsConstant,
    /** @type {{table: String, forceRowLayout: Boolean}} */
    optionsModified,
  ) {
    this.inputParsed = input;
    this.optionsConstant = {};
    if ((typeof optionsConstant) === "object" && optionsConstant !== null) {
      this.optionsConstant = optionsConstant;
    }
    if (optionsModified === null || typeof optionsModified !== "object") {
      optionsModified = {};
    }
    if (typeof optionsModified.table === "string") {
      this.tableName = optionsModified.table;
    }
    let forceRowLayout = false;
    if (optionsModified.forceRowLayout === true) {
      forceRowLayout = true;
    }
    if (typeof this.inputParsed === "string") {
      this.inputParsed = input.replace(/[\r\n]/g, " ");
      if (this.inputParsed[0] !== "{" && this.inputParsed[0] !== "[" && input[0] !== "\"") {
        this.inputParsed = `"${this.inputParsed}"`;
      }
      try {
        this.inputParsed = JSON.parse(this.inputParsed);
      } catch (e) {
        return this.getErrorElement(`Error while parsing ${escape(this.inputParsed)}: ${e}`)
      }
    }
    if (forceRowLayout && typeof this.inputParsed === "object" && !Array.isArray(this.inputParsed)) {
      this.inputParsed = this.transformObjectToRows(this.inputParsed);
    }
    if (typeof this.inputParsed === "object" && !Array.isArray(this.inputParsed)) {
      this.inputParsed = [this.inputParsed];
    }
    if (Array.isArray(this.inputParsed)) {
      return this.getHtmlElementFromArray(this.inputParsed);
    }
    let result = document.createElement("span");
    miscellaneous.writeHTML(result, this.inputParsed);
    return result;
  }

  /** @return {HTMLElement} */
  getTinyLabel(
    /** @type {string} */
    content,
  ) {
    let result = document.createElement("tiny");
    result.textContent = content;
    return result;
  }

  /** @return {HTMLElement} */
  getHtmlElementFromArray(
    inputJSON,
  ) {
    if (!Array.isArray(inputJSON)) {
      return document.createElement("span");
    }
    /** @type {HTMLTableElement} */
    let table = document.createElement("table");
    table.className = "tableJSON";
    this.labelsRows = getLabelsRows(inputJSON);
    if (this.labelsRows === null) {
      return this.getTableHorizontallyLaidFromJSON(
        inputJSON,
        new CollectionRowSelector(this.tableName),
      );
    }
    let header = table.createTHead();
    let headerRow = header.insertRow();
    headerRow.insertCell();
    for (
      let j = 0;
      j < this.labelsRows.labels.length;
      j++
    ) {
      miscellaneous.writeHTML(
        headerRow.insertCell(),
        this.labelsRows.labels[j],
      );
    }
    for (let i = 0; i < this.labelsRows.rows.length; i++) {
      this.getTableRow(table, i);
    }
    return table;
  }

  getTableRow(
    /** @type {HTMLTableElement} */
    table,
    /** @type {number} */
    index,
  ) {
    let currentRow = table.insertRow();
    currentRow.insertCell().appendChild(this.getTinyLabel(`${index}`));
    const row = this.labelsRows.rows[index];
    let selector = new CollectionRowSelector(this.tableName);
    if (this.tableName !== "" && this.tableName !== undefined) {
      selector.labels.push("_id");
      selector.labels.push("$oid");
    }
    for (let j = 0; j < this.labelsRows.labels.length; j++) {
      let content = this.getTableHorizontallyLaidFromJSON(
        row[j],
        selector
      );
      currentRow.insertCell().appendChild(content);
    }
  }
}

function getLabelsRows(input) {
  let result = {
    labels: [],
    rows: [],
    idRow: - 1,
  };
  let labelFinder = {};
  for (let counterRow = 0; counterRow < input.length; counterRow++) {
    if (typeof input[counterRow] !== "object") {
      return null;
    }
    for (let label in input[counterRow]) {
      labelFinder[label] = true;
    }
  }
  result.labels = Object.keys(labelFinder).sort();
  for (let counterLabel = 0; counterLabel < result.labels.length; counterLabel++) {
    if (result.labels[counterLabel] === "_id") {
      result.idRow = counterLabel;
      break;
    }
  }
  for (let counterRow = 0; counterRow < input.length; counterRow++) {
    let currentInputItem = input[counterRow];
    result.rows.push([]);
    let currentOutputItem = result.rows[result.rows.length - 1];
    for (let counterLabel = 0; counterLabel < result.labels.length; counterLabel++) {
      let label = result.labels[counterLabel];
      if (label in currentInputItem) {
        currentOutputItem.push(currentInputItem[label]);
      } else {
        currentOutputItem.push("");
      }
    }
  }
  return result;
}

module.exports = {
  JSONToHTML,
  writeJSONtoDOMComponent,
  transformersStandard,
  statistics,
};