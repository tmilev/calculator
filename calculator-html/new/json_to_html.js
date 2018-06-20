"use srict";

function writeJSONtoDOMComponent(inputJSON, theDomComponent) {
  if (typeof theDomComponent === "string") {
    theDomComponent = document.getElementById(theDomComponent);
  }
  theDomComponent.innerHTML = getHtmlFromArrayOfObjects(inputJSON);
}

function deleteDatabaseItemCallback(input, output) {
  console.log (`Debug: result: ${input}`);
}

function deleteDatabaseItem(labels, selector) {
  var finalSelector = {
    table: selector.table,
    object: selector.object,
    fields: labels
  }
  var theURL = `${pathnames.calculatorAPI}?request=databaseDeleteOneEntry&item=${escape(JSON.stringify(finalSelector))}`;
  console.log("DEBUG: final selector: " + JSON.stringify(finalSelector));
  submitGET({
    url: theURL,
    callback: deleteDatabaseItemCallback,
    progress: "spanProgressReportGeneral"
  });  
}

function matchesPattern(currentLabels, selector, pattern) {
  if (currentLabels.length !== pattern.length - 1) {
    return false;
  }
  //console.log(`DEBUG: Does ${currentLabels.join(", ")} match ${pattern.join(", ")}`);
  if (pattern.length < 1) {
    return false;
  }
  if (selector.table !== pattern[0]) {
    return false;
  }
  for (var counterLabel = 1; counterLabel < pattern.length; counterLabel ++) {
    if (pattern[counterLabel] === modifiableDatabaseData.universalSelector) {
      continue;
    }
    if (pattern[counterLabel] !== currentLabels[counterLabel - 1]) {
      return false;
    }
  }
  return true;
}

function isDeleteable(currentLabels, selector) {
  if (currentLabels === null || currentLabels === undefined || selector === null || selector === undefined) {
    return false;
  }
  for (var counterModifiable = 0; counterModifiable < modifiableDatabaseData.modifiableFields.length; counterModifiable ++) {
    if (matchesPattern(currentLabels, selector, modifiableDatabaseData.modifiableFields[counterModifiable])) {
      return true;
    }
  }
  return false;
}

function getDeleteButtonFromLabels(theLabels, selector) {
  return `<button onclick='deleteDatabaseItem(${JSON.stringify(theLabels)}, ${JSON.stringify(selector)})'>Delete</button>`;
}

function getTableHorizontallyLaidFromJSON(input, currentLabels, selector) {
  var inputType = typeof input; 
  if (inputType === "string" || inputType === "number" || inputType === "boolean") {
    if (isDeleteable(currentLabels, selector)) {
      return `${input} ${getDeleteButtonFromLabels(currentLabels, selector)}`;
    }
    return input;
  }  
  var hasLabels = (currentLabels !== undefined && currentLabels !== null);
  if (Array.isArray(input)) {
    var result = "";
    result += "<table class='tableJSONItem'>";
    var newLabels = null;
    if (hasLabels) {
      newLabels = currentLabels.slice();
      newLabels.push("");
    }
    for (var counterInput = 0; counterInput < input.length; counterInput ++) {
      if (hasLabels) {
        newLabels[newLabels.length - 1] = `${counterInput}`;
      }
      var item = input[counterInput];
      result += `<tr><td> <tiny>${counterInput}</tiny></td><td>${getTableHorizontallyLaidFromJSON(item, newLabels, selector)}</td></tr>`; 
    }
    result += "</table>";
    return result;
  }
  if (typeof input === "object") {
    var result = "";
    result += "<table class='tableJSONItem'>";
    var newLabels = null;
    if (hasLabels) {
      newLabels = currentLabels.slice();
      newLabels.push("");
    }
    for (item in input) {
      if (hasLabels) {
        newLabels[newLabels.length - 1] = item;
      }
      result += `<tr>`;
      result += `<td>${item}</td><td>${getTableHorizontallyLaidFromJSON(input[item], newLabels, selector)}</td>`;
      if (isDeleteable(newLabels, selector)) {
        result += `<td>${getDeleteButtonFromLabels(newLabels, selector)}</td>`;
      }
      result += `</tr>`; 
    }
    result += "</table>";
    return result;
  }  
  return typeof input;
}

function getLabelsRows(input) {
  var result = {
    labels: [],
    rows: [],
    idRow: - 1
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

function getHtmlFromArrayOfObjects(input, selector) {
  var inputJSON = input;
  var hasLabels = (selector !== null && selector !== undefined);

  if (typeof inputJSON === "string") {
    inputJSON = input.replace(/[\r\n]/g, " "); 
    if (inputJSON[0] !== "{" && inputJSON[0] !== "[" && input[0] !== "\"") {
      inputJSON = `"${inputJSON}"`;
    }
    try {
      inputJSON = JSON.parse(inputJSON);
    } catch (e) {
      return `<error>Error while parsing ${escape(inputJSON)}: ${e}</error>`;
    }
  }
  var result = "";
  if (typeof inputJSON === "object" && !Array.isArray(inputJSON)) {
    inputJSON = [inputJSON];
  }
  if (Array.isArray(inputJSON)) {
    var newLabel = null;
    var labelsRows = getLabelsRows(inputJSON);
    if (labelsRows !== null) { 
      result += "<table class='tableJSON'>";
      result += "<tr>";
      for (var counterColumn = 0; counterColumn < labelsRows.labels.length; counterColumn ++) {
        result += `<th>${labelsRows.labels[counterColumn]}</th>`;
      }
      for (var counterRow = 0; counterRow < labelsRows.rows.length; counterRow ++) {
        if (hasLabels) {
          if (labelsRows.idRow != - 1) {
            selector.object = labelsRows.rows[counterRow][labelsRows.idRow]["$oid"];
          } 
        }
        result += "<tr>";
        for (var counterColumn = 0; counterColumn < labelsRows.labels.length; counterColumn ++) {
          if (hasLabels) {
            newLabel = [ labelsRows.labels[counterColumn] ];
          }
          result += `<td>${getTableHorizontallyLaidFromJSON(labelsRows.rows[counterRow][counterColumn], newLabel, selector)}</td>`;
        }
        result += "</tr>";
      }
      result += "</tr>";
      result += "</table>";
    } else {
      result += getTableHorizontallyLaidFromJSON(inputJSON);
    }
  } else {
    result += inputJSON + "<br>";
  }
  return result;
}