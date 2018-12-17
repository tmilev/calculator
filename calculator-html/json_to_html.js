"use srict";
const submitRequests = require('./submit_requests');
const miscellaneous = require('./miscellaneous');
const ids = require('./ids_dom_elements');
const modifiableDatabaseData = require('./modifiable_database_fields').modifiableDatabaseData;
const panels = require('./panels');
const BufferCalculator = require('./buffer').BufferCalculator;

var modifiableFields = {};
for (var i = 0; i < modifiableDatabaseData.modifiableFields.length; i ++) {
  var incomingLabel = modifiableDatabaseData.modifiableFields[i].join("");
  modifiableFields[incomingLabel] = true;
}

var transformersStandard = {
  shortener: {
    transformer: miscellaneous.shortenString.bind(null, 4),
  },
};

function JSONToHTML() {
  /** @type {Array.<{panelId: string, label: string, content: string}>} */
  this.panelInformation = [];
  /** @type {String} */
  this.tableName = "";
  /**@type {Object.<string,{clickHandler: Function, labels: String[]}>} */
  this.buttonBindings = {};
  /**@type {String[]} */
  this.buttonIdsInOrderOfCreation = [];
  this.ambientObject = null;
  this.labelsRows = null;
  this.optionsConstant = {};
  this.inputParsed = null;
}

function writeJSONtoDOMComponent(inputJSON, theDomComponent) {
  if (typeof theDomComponent === "string") {
    theDomComponent = document.getElementById(theDomComponent);
  }
  theDomComponent.innerHTML = jsonToHtml.getHtmlFromArrayOfObjects(inputJSON);
}

var counterToggleButtons = 0;

function getToggleButton(
  /**@type {JSONToHTML} */
  transformer, 
  input, 
  label,
) {
  counterToggleButtons ++;
  var panelId = `panelFromJSONFormatter${counterToggleButtons}`;
  transformer.panelInformation.push({
    panelId: panelId,
    label: label,
    content: input,
  });
  return `<div id = "${panelId}"></div>`;
}

function getDeleteButtonFromLabels(theLabels, selector, containerLabel) {
  return `<button onclick = 'window.calculator.database.deleteDatabaseItem("${containerLabel}", ${JSON.stringify(theLabels)}, ${JSON.stringify(selector)})'>Delete</button>`;
}

var counterDatabaseTables = 0;

JSONToHTML.prototype.getOptionFromLabel = function(
  /**@type {String[]} */
  currentLabelsGeneralized,
) {
  if (this.optionsConstant.transformers === null || this.optionsConstant.transformers === undefined) {
    return null;
  }
  for (var i = - 1; i < currentLabelsGeneralized.length; i ++) {
    var labelTry = currentLabelsGeneralized.slice();
    if (i >= 0) {
      labelTry[i] = "${any}";
    }
    var currentLabel = labelTry.join(".");
    if (currentLabel in this.optionsConstant.transformers) {
      return this.optionsConstant.transformers[currentLabel];
    }
  }
}

var numberOfButtonsGenerated = 0;

JSONToHTML.prototype.getButtonFromLabels = function(
  input, 
  inputTransformed, 
  /**@type {String[]} */
  currentLabels, 
  /**@type {Function} */
  clickHandler,
) {
  numberOfButtonsGenerated ++;
  var result = "";
  var id = `buttonClickHandler${numberOfButtonsGenerated}`;
  result += `<button id = "${id}">${inputTransformed}</button>`;
  this.buttonBindings[id] = {
    clickHandler : clickHandler,
    labels: currentLabels.slice()
  };
  this.buttonIdsInOrderOfCreation.push(id);
  return result;
}

JSONToHTML.prototype.getSingleEntry = function(
  input, 
  /**@type {String[]} */
  currentLabels,
  /**@type {String[]} */
  currentLabelsGeneralized,
) {
  var currentOption = this.getOptionFromLabel(currentLabelsGeneralized);
  if (currentOption === null || currentOption === undefined) {
    return input;
  }
  var inputTransformed = input;
  if (typeof currentOption.transformer === "function") {
    inputTransformed = currentOption.transformer(input);
  }
  if (typeof currentOption.clickHandler === "function") {
    inputTransformed = this.getButtonFromLabels(input, inputTransformed, currentLabels, currentOption.clickHandler);
  }
  if (inputTransformed === input) {
    return input;
  }
  return getToggleButton(this, input, inputTransformed);
}

JSONToHTML.prototype.getTableHorizontallyLaidFromJSON = function(
  input, 
  /**@type {String[]} */
  currentLabels, 
  /**@type {String[]} */
  currentLabelsGeneralized, 
  /**@type {BufferCalculator} */
  output,
) {
  var inputType = typeof input; 
  if (inputType === "string" || inputType === "number" || inputType === "boolean") {
    output.write(this.getSingleEntry(input, currentLabels, currentLabelsGeneralized));
    return;
  }  
  if (Array.isArray(input)) {
    this.getTableHorizontallyLaidFromArray(input, currentLabels, currentLabelsGeneralized, output);
    return;
  } if (inputType === "object") {
    this.getTableHorizontallyLaidFromObject(input, currentLabels, currentLabelsGeneralized, output);
    return;
  } else {
    output.write(typeof input);
    return;
  }
}

JSONToHTML.prototype.getTableHorizontallyLaidFromArray = function(  
  input, 
  /**@type {String[]} */ 
  currentLabels, 
  /**@type {String[]} */
  currentLabelsGeneralized, 
  /**@type {BufferCalculator} */
  output,
) {
  if (!Array.isArray(input)) {
    return;
  }
  var hasLabels = (currentLabels !== undefined && currentLabels !== null);
  var newLabels = null;
  var newLabelsGeneralized = null;
  if (hasLabels) {
    newLabels = currentLabels.slice();
    newLabelsGeneralized = currentLabelsGeneralized.slice();
    newLabels.push("");
    newLabelsGeneralized.push("");
  }
  output.write("<table class = 'tableJSONItem'>");
  for (var counterInput = 0; counterInput < input.length; counterInput ++) {
    if (hasLabels) {
      newLabels[newLabels.length - 1] = "${number}";
    }
    var item = input[counterInput];
    output.write(`<tr><td><tiny>${counterInput}</tiny></td><td>`);
    this.getTableHorizontallyLaidFromJSON(item, newLabels, newLabelsGeneralized, output);
    output.write(`</td></tr>`); 
  }
  output.write("</table>");
}

JSONToHTML.prototype.getTableHorizontallyLaidFromObject = function(  
  input, 
  /**@type {String[]} */
  currentLabels, 
  /**@type {String[]} */
  currentLabelsGeneralized, 
  /**@type {BufferCalculator} */
  output,
) {
  if (typeof input !== "object") {
    return;
  }
  var hasLabels = (currentLabels !== undefined && currentLabels !== null);
  var newLabels = null;
  var newLabelsGeneralized = null;
  if (hasLabels) {
    newLabels = currentLabels.slice();
    newLabelsGeneralized = currentLabelsGeneralized.slice();
    newLabels.push("");
  }
  output.write("<table class = 'tableJSONItem'>"); 
  for (item in input) {
    if (hasLabels) {
      newLabels[newLabels.length - 1] = item;
    }
    var flagIsDeleteable = false;
    if (flagIsDeleteable) {
      counterDatabaseTables ++;
      var tableLabel = `databaseItem${counterDatabaseTables}`;
      output.write(`<tr id='${tableLabel}'>`);
    } else {
      output.write(`<tr>`)
    }
    output.write(`<td>${item}</td><td>`)
    this.getTableHorizontallyLaidFromJSON(input[item], newLabels, newLabelsGeneralized, output);
    output.write(`</td>`);
    if (flagIsDeleteable) {
      output.write(`<td>`);
      output.write(getDeleteButtonFromLabels(newLabels, tableName, tableLabel));
      output.write(`</td>`);
    }
    output.write(`</tr>`); 
  }
  output.write("</table>");
}

function getLabelsRows(input) {
  var result = {
    labels: [],
    rows: [],
    idRow: - 1,
  };
  var labelFinder = {};
  for (var counterRow = 0; counterRow < input.length; counterRow ++) {
    if (typeof input[counterRow] !== "object") {
      return null;
    }
    for (var label in input[counterRow]) {
      labelFinder[label] = true;
    }
  }
  result.labels = Object.keys(labelFinder).sort();
  for (var counterLabel = 0; counterLabel < result.labels.length; counterLabel ++) {
    if (result.labels[counterLabel] === "_id") {
      result.idRow = counterLabel;
      break;
    }
  }
  for (var counterRow = 0; counterRow < input.length; counterRow ++) {
    var currentInputItem = input[counterRow];
    result.rows.push([]);
    var currentOutputItem = result.rows[result.rows.length - 1];
    for (var counterLabel = 0; counterLabel < result.labels.length; counterLabel ++) {
      var label = result.labels[counterLabel];
      if (label in currentInputItem) {
        currentOutputItem.push(currentInputItem[label]);
      } else {
        currentOutputItem.push("");
      }
    }
  }
  return result;
}

JSONToHTML.prototype.bindButtons = function() {
  for (var i = this.panelInformation.length - 1; i >= 0; i--) {
    var currentInfo = this.panelInformation[i];
    var currentPanel = new panels.PanelExpandable(currentInfo.panelId, true);
    currentPanel.setPanelContent(currentInfo.content);
    currentPanel.setPanelLabel(currentInfo.label);
  }
  for (var i = this.buttonIdsInOrderOfCreation.length - 1; i >= 0; i --) {
    var currentId = this.buttonIdsInOrderOfCreation[i];
    var currentBinding = this.buttonBindings[currentId];
    var currentButton = document.getElementById(currentId);
    currentButton.addEventListener('click', currentBinding.clickHandler);
    currentButton.setAttribute("labels", JSON.stringify(currentBinding.labels));
  }
  for (var i = 0; i < this.panelInformation.length; i ++) {
    var currentInfo = this.panelInformation[i];
    var currentPanel = new panels.PanelExpandable(currentInfo.panelId, false);
    currentPanel.matchPanelStatus();  
  }
}

JSONToHTML.prototype.getHtmlFromArrayOfObjects = function(input, optionsConstant, optionsModified) {
  this.inputParsed = input;
  this.optionsConstant = {};
  if ((typeof optionsConstant) === "object") {
    this.optionsConstant = optionsConstant;
  } 
  if (typeof optionsModified !== "object") {
    optionsModified = {};
  }
  if (typeof optionsModified.table === "string") {
    this.tableName = optionsModified.table;
  }
  if (typeof this.inputParsed === "string") {
    this.inputParsed = input.replace(/[\r\n]/g, " "); 
    if (this.inputParsed[0] !== "{" && this.inputParsed[0] !== "[" && input[0] !== "\"") {
      this.inputParsed = `"${this.inputParsed}"`;
    }
    try {
      this.inputParsed = JSON.parse(this.inputParsed);
    } catch (e) {
      return `<b style = 'color:red'>Error while parsing ${escape(this.inputParsed)}: ${e}</b>`;
    }
  }
  var resultBuffer = new BufferCalculator();
  if (typeof this.inputParsed === "object" && !Array.isArray(this.inputParsed)) {
    this.inputParsed = [this.inputParsed];
  }
  if (Array.isArray(this.inputParsed)) {
    this.getHtmlFromArray(this.inputParsed, resultBuffer);
  } else {
    resultBuffer.write(this.inputParsed + "<br>");
  }
  return resultBuffer.toString();
}

JSONToHTML.prototype.getHtmlFromArray = function(
  inputJSON, 
  /**@type {BufferCalculator} */
  output,
) {
  if (!Array.isArray(inputJSON)) {
    return;
  }
  this.labelsRows = getLabelsRows(inputJSON);
  if (this.labelsRows === null) { 
    this.getTableHorizontallyLaidFromJSON(inputJSON, [], [], output);
    return;
  }
  output.write("<table class = 'tableJSON'>");
  output.write("<tr>");
  output.write("<th></th>");
  for (var counterColumn = 0; counterColumn < this.labelsRows.labels.length; counterColumn ++) {
    output.write(`<th>${this.labelsRows.labels[counterColumn]}</th>`);
  }
  output.write("</tr>");
  var id = "";
  for (var counterRow = 0; counterRow < this.labelsRows.rows.length; counterRow ++) {
    if (this.labelsRows.idRow != - 1) {
      id = this.labelsRows.rows[counterRow][this.labelsRows.idRow]["$oid"];
      output.write(`<tr id = "trOid${id}">`);
    } else {
      output.write("<tr>");
      id = counterRow;
    }
    output.write(`<td><tiny>${counterRow}</tiny></td>`);
    for (var counterColumn = 0; counterColumn < this.labelsRows.labels.length; counterColumn ++) {
      var labelsGeneralized = []; 
      var labels = [];
      if (this.tableName !== "") { 
        labels.push(this.tableName);
        labelsGeneralized.push(this.tableName);
      }
      labels.push(id, this.labelsRows.labels[counterColumn]);
      labelsGeneralized.push("${number}", this.labelsRows.labels[counterColumn]);
      output.write("<td>");
      this.getTableHorizontallyLaidFromJSON(this.labelsRows.rows[counterRow][counterColumn], labels, labelsGeneralized, output);
      output.write("</td>");
    }
    output.write("</tr>");
  }
  output.write("</table>");
}

module.exports = {
  JSONToHTML,
  getToggleButton,
  transformersStandard,
}