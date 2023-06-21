"use srict";
const miscellaneous = require("./miscellaneous_frontend");
const modifiableDatabaseData = require('./modifiable_database_fields').modifiableDatabaseData;
const panels = require('./panels');

let modifiableFields = {};
for (let i = 0; i < modifiableDatabaseData.modifiableFields.length; i++) {
  let incomingLabel = modifiableDatabaseData.modifiableFields[i].join("");
  modifiableFields[incomingLabel] = true;
}

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

class CompositeDataKey {
  constructor(
    /** @type {string} */
    tableName,
    /** @type {Array.<Object>} */
    labels,
  ) {
    this.tableName = tableName;
    this.labels = labels;
  }

  /** @return {string} */
  toStringLabels() {
    return this.labels.join(".");
  }

  addLabel(
    /** @type{string|number} */
    label,
  ) {
    this.labels.push(label);
  }

  popLabel() {
    this.labels.pop;
  }

  copy() {
    return new CompositeDataKey(
      this.tableName,
      this.labels.slice(),
    );
  }
}

class CompositeDataKeyAndValue {
  constructor(
    /** @type {CompositeDataKey} */
    key,
    value,
  ) {
    this.key = key;
    this.value = value;
  }

  copy() {
    return new CompositeDataKeyAndValue(this.key.copy(), this.value);
  }

  toObject() {
    return {
      "table": this.key.tableName,
      "key": this.key.toStringLabels(),
      "value": this.value,
    };
  }
}

class DataTransformer {
  constructor(
    /** @type {Function?} */
    clickHandler,
    /** @type {Function?} */
    labelTransformer
  ) {
    this.clickHandler = clickHandler;
    this.labelTransformer = labelTransformer;
  }
}

class DataProcessor {
  constructor(
    /** @type {CompositeDataKey} */
    selector,
    /** @type {DataTransformer} */
    transformer,
  ) {
    this.selector = selector;
    this.transformer = transformer;
  }
}

class DataProcessorCollection {
  constructor(
    /** @type {Array.<DataProcessor>} */
    dataProcessors,
  ) {
    this.dataProcessors = dataProcessors;
  }
  /** @return {DataTransformer|null} */
  getDataTransformer(
    /** @type {CompositeDataKey} */
    selector
  ) {
    for (let i = 0; i < this.dataProcessors.length; i++) {
      let candidate = this.transformerFromSelector(
        selector, this.dataProcessors[i]
      );
      if (candidate !== null) {
        return candidate;
      }
    }
    return null;
  }

  /** @return {DataTransformer|null} */
  transformerFromSelector(
    /** @type {CompositeDataKey} */
    selector,
    /** @type {DataProcessor} */
    processor,
  ) {
    const key = processor.selector;
    if (key.tableName != selector.tableName) {
      return null;
    }
    if (key.labels.length > selector.labels.length) {
      return null;
    }
    for (let i = 0; i < key.labels.length; i++) {
      let filter = key.labels[i];
      let toBeMatched = selector.labels[i];
      if (filter === null) {
        // null matches everything.
        continue;
      }
      if (filter !== toBeMatched) {
        return null;
      }
    }
    return processor.transformer;
  }
}

class JSONToHTML {
  constructor() {
    /** @type {String} */
    this.tableName = "";
    this.ambientObject = null;
    this.labelsRows = null;
    /** @type {DataProcessorCollection?} */
    this.optionsConstant = null;
    this.inputParsed = null;
    this.result = null;
  }

  /** @return {DataTransformer|null} */
  getOptionFromLabel(
    /** @type {CompositeDataKey} */
    selector,
  ) {
    if (this.optionsConstant === null) {
      return null;
    }
    return this.optionsConstant.getDataTransformer(selector);
  }

  getButtonFromLabels(
    input,
    inputTransformed,
    /** @type {CompositeDataKey} */
    selector,
    /** @type {CompositeDataKeyAndValue} */
    ambientRowSelector,
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
      clickHandler(selectorCopy, ambientRowSelector, panel);
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
    currentPanel.initialize(true, false);
    currentPanel.setPanelLabel(label);
    currentPanel.setPanelContent(input);
    return panelContainer;
  }

  /** @return {HTMLElement} */
  getSingleEntry(
    input,
    /** @type {CompositeDataKey} */
    selector,
    /** @type {CompositeDataKeyAndValue|null} */
    ambientRowSelector,
  ) {
    /** @type {DataTransformer} */
    let currentOption = this.getOptionFromLabel(selector);
    if (currentOption === null || currentOption === undefined) {
      let result = document.createElement("span");
      miscellaneous.writeHTML(result, input);
      return result;
    }
    let inputTransformed = input;
    if (typeof currentOption.labelTransformer === "function") {
      inputTransformed = currentOption.labelTransformer(input);
    }
    statistics.toggleButtons++;
    if (typeof currentOption.clickHandler === "function") {
      let result = document.createElement("span");
      let button = this.getButtonFromLabels(
        input,
        inputTransformed,
        selector,
        ambientRowSelector,
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
    /** @type {CompositeDataKey} */
    selector,
    /** @type {CompositeDataKeyAndValue|null} */
    ambientRowSelector,
  ) {
    let inputType = typeof input;
    if (
      inputType === "string" ||
      inputType === "number" ||
      inputType === "boolean"
    ) {
      return this.getSingleEntry(input, selector, ambientRowSelector);
    }
    if (Array.isArray(input)) {
      return this.getTableHorizontallyLaidFromArray(
        input, selector, ambientRowSelector,
      );
    }
    if (inputType === "object") {
      return this.getTableHorizontallyLaidFromObject(
        input, selector, ambientRowSelector,
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
    /** @type {CompositeDataKey} */
    selector,
    /** @type {CompositeDataKeyAndValue|null} */
    ambientRowSelector
  ) {
    if (!Array.isArray(input)) {
      return this.getErrorElement(
        "Error: bad input to: getTableHorizontallyLaidFromArray"
      );
    }
    let table = document.createElement("table");
    table.className = "tableJSONItem";
    for (let i = 0; i < input.length; i++) {
      selector.addLabel(i)
      let item = input[i];
      let row = table.insertRow();
      row.insertCell().appendChild(this.getTinyLabel(`${i}`));
      let content = this.getTableHorizontallyLaidFromJSON(
        item, selector, ambientRowSelector
      );
      selector.popLabel();
      row.insertCell().appendChild(content);
    }
    return table;
  }

  /** @return {HTMLElement} */
  getTableHorizontallyLaidFromObject(
    input,
    /** @type {CompositeDataKey} */
    selector,
    /** @type {CompositeDataKeyAndValue|null} */
    ambientRowSelector
  ) {
    if (typeof input !== "object") {
      return this.getErrorElement("Error: bad input to getTableHorizontallyLaidFromObject.");
    }
    let table = document.createElement("table");
    table.className = "tableJSONItem";
    for (let item in input) {
      selector.addLabel(item);
      let currentRow = table.insertRow();
      currentRow.insertCell().textContent = `${item}`;
      let content = this.getTableHorizontallyLaidFromJSON(
        input[item], selector, ambientRowSelector
      );
      selector.popLabel();
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
    /** @type {DataProcessorCollection} */
    selectors,
    /** @type {{table: String, forceRowLayout: Boolean}} */
    optionsModified,
  ) {
    this.inputParsed = input;
    this.optionsConstant = null;
    if (selectors !== null && selectors != undefined) {
      this.optionsConstant = selectors;
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
      throw "Error: not an array";
    }
    /** @type {HTMLTableElement} */
    let table = document.createElement("table");
    table.className = "tableJSON";
    this.labelsRows = getLabelsRows(inputJSON);
    if (this.labelsRows === null) {
      return this.getTableHorizontallyLaidFromJSON(
        inputJSON,
        new CollectionRowSelector(this.tableName, [], "", []),
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
      this.getTopLevelTableRow(table, i);
    }
    return table;
  }

  getTopLevelTableRow(
    /** @type {HTMLTableElement} */
    table,
    /** @type {number} */
    index,
  ) {
    let currentRow = table.insertRow();
    currentRow.insertCell().appendChild(this.getTinyLabel(`${index}`));
    const row = this.labelsRows.rows[index];
    /** @type {CompositeDataKeyAndValue|null} */
    let ambientRowSelector = null;
    let idColumn = this.labelsRows.idColumn;
    if (
      idColumn !== -1 &&
      this.tableName !== "" &&
      this.tableName !== undefined &&
      row[idColumn] !== undefined &&
      row[idColumn]["$oid"] !== undefined
    ) {
      let rowLabels = new CompositeDataKey(
        this.tableName,
        ["_id", "$oid"],
      );
      ambientRowSelector = new CompositeDataKeyAndValue(
        rowLabels, row[idColumn]["$oid"],
      );
    }
    for (let j = 0; j < this.labelsRows.labels.length; j++) {
      let label = this.labelsRows.labels[j];
      let selector = new CompositeDataKey(this.tableName, [label]);
      let content = this.getTableHorizontallyLaidFromJSON(
        row[j],
        selector,
        ambientRowSelector,
      );
      currentRow.insertCell().appendChild(content);
    }
  }
}

function getLabelsRows(input) {
  let result = {
    labels: [],
    rows: [],
    idColumn: - 1,
  };
  let labelFinder = {};
  for (let i = 0; i < input.length; i++) {
    if (typeof input[i] !== "object") {
      return null;
    }
    for (let label in input[i]) {
      labelFinder[label] = true;
    }
  }
  result.labels = Object.keys(labelFinder).sort();
  for (let counterLabel = 0; counterLabel < result.labels.length; counterLabel++) {
    if (result.labels[counterLabel] === "_id") {
      result.idColumn = counterLabel;
      break;
    }
  }
  for (let i = 0; i < input.length; i++) {
    let currentInputItem = input[i];
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
  DataProcessor,
  DataProcessorCollection,
  DataTransformer,
  CompositeDataKey,
  JSONToHTML,
  writeJSONtoDOMComponent,
  statistics,
};