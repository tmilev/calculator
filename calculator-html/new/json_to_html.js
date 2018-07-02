"use srict";

function writeJSONtoDOMComponent(inputJSON, theDomComponent) {
  if (typeof theDomComponent === "string") {
    theDomComponent = document.getElementById(theDomComponent);
  }
  theDomComponent.innerHTML = getHtmlFromArrayOfObjects(inputJSON);
}

function deleteDatabaseItemCallback(input, output) {
  document.getElementById(this).remove();
  console.log (`Debug: result: ${input}`);
  //this.parentElement.innerHTML = "";
}

function deleteDatabaseItem(containerLabel, labels, selector) {
  var finalSelector = {
    table: selector.table,
    object: selector.object,
    fields: labels
  }
  var theURL = `${pathnames.calculatorAPI}?request=databaseDeleteOneEntry&item=${escape(JSON.stringify(finalSelector))}`;
  //console.log("DEBUG: final selector: " + JSON.stringify(finalSelector));
  submitGET({
    url: theURL,
    callback: deleteDatabaseItemCallback.bind(containerLabel),
    progress: "spanProgressReportGeneral"
  });  
}

var minimizableDatabase = [
  ["users", "problemDataJSON"]
];

var abbreviatedDatabase = [
  ["users", "activationToken"],
  ["users", "authenticationToken"],
  ["users", "password"]
];

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
  return fitsPattern(currentLabels, selector, modifiableDatabaseData.modifiableFields);
}

function isMinimizable(currentLabels, selector) {
  return fitsPattern(currentLabels, selector, minimizableDatabase);
}

function shouldBeAbbreviated(currentLabels, selector) {
  return fitsPattern(currentLabels, selector, abbreviatedDatabase);
}

function fitsPattern(currentLabels, selector, pattern) {
  if (currentLabels === null || currentLabels === undefined || selector === null || selector === undefined) {
    return false;
  }
  for (var counterMinimizing = 0; counterMinimizing < pattern.length; counterMinimizing ++) {
    if (matchesPattern(currentLabels, selector, pattern[counterMinimizing])) {
      return true;
    }
  }
  return false;
}

function getDeleteButtonFromLabels(theLabels, selector, containerLabel) {
  return `<button onclick='deleteDatabaseItem("${containerLabel}", ${JSON.stringify(theLabels)}, ${JSON.stringify(selector)})'>Delete</button>`;
}

var counterDatabaseTables = 0;

function getTableHorizontallyLaidFromJSON(input, currentLabels, selector) {
  var inputType = typeof input; 
  if (inputType === "string" || inputType === "number" || inputType === "boolean") {
    if (shouldBeAbbreviated(currentLabels, selector)) {
      label = shortenString(input, 4);
      input = getToggleButton(input, label);
    }
    if (isDeleteable(currentLabels, selector)) {
      return `${input} ${getDeleteButtonFromLabels(currentLabels, selector)}`;
    }
    return input;
  }  
  var hasLabels = (currentLabels !== undefined && currentLabels !== null);
  if (Array.isArray(input)) {
    var result = "";
    var newLabels = null;
    if (hasLabels) {
      newLabels = currentLabels.slice();
      newLabels.push("");
    }
    //if (isMinimizable(newLabels, selector)) {
    //  result += `min`;
    //}
    result += "<table class='tableJSONItem'>";
    for (var counterInput = 0; counterInput < input.length; counterInput ++) {
      if (hasLabels) {
        newLabels[newLabels.length - 1] = `${counterInput}`;
      }
      var item = input[counterInput];
      result += `<tr><td><tiny>${counterInput}</tiny></td><td>${getTableHorizontallyLaidFromJSON(item, newLabels, selector)}</td></tr>`; 
    }
    result += "</table>";
    return result;
  }
  if (typeof input === "object") {
    var result = "";
    var newLabels = null;
    if (hasLabels) {
      newLabels = currentLabels.slice();
      newLabels.push("");
    }
    var flagIsDeleteable = false;
    result += "<table class='tableJSONItem'>"; 
    if (isDeleteable(newLabels, selector)) {
      flagIsDeleteable = true;
    }
    for (item in input) {
      if (hasLabels) {
        newLabels[newLabels.length - 1] = item;
      }
      var tableLabel = "";
      if (flagIsDeleteable) {
        counterDatabaseTables ++;
        tableLabel += `databaseItem${counterDatabaseTables}`;
        result += `<tr id='${tableLabel}'>`;
      } else {
        result += `<tr>`;
      }
      result += `<td>${item}</td><td>${getTableHorizontallyLaidFromJSON(input[item], newLabels, selector)}</td>`;
      if (flagIsDeleteable) {
        result += `<td>${getDeleteButtonFromLabels(newLabels, selector, tableLabel)}</td>`;
      }
      result += `</tr>`; 
    }
    result += "</table>";
    if (isMinimizable(currentLabels, selector)) {
      result = getToggleButton(result, "show");
    }

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
      result += "</tr>";
      for (var counterRow = 0; counterRow < labelsRows.rows.length; counterRow ++) {
        var currentIsDeleteable = false; 
        if (hasLabels) {
          if (labelsRows.idRow != - 1) {
            selector.object = labelsRows.rows[counterRow][labelsRows.idRow]["$oid"];
            currentIsDeleteable = isDeleteable([], selector);
          } 
        }
        var trIdIfNeeded = "";
        if (currentIsDeleteable) {
          trIdIfNeeded = `trOid${selector.object}`;
          result += `<tr id = "${trIdIfNeeded}">`;
        } else {
          result += "<tr>";
        }
        for (var counterColumn = 0; counterColumn < labelsRows.labels.length; counterColumn ++) {
          if (hasLabels) {
            newLabel = [labelsRows.labels[counterColumn]];
          }
          result += "<td>";
          result += getTableHorizontallyLaidFromJSON(labelsRows.rows[counterRow][counterColumn], newLabel, selector);
          if (currentIsDeleteable && counterColumn === labelsRows.idRow) {
            result += getDeleteButtonFromLabels([], selector, trIdIfNeeded);
          }
          result += "</td>";
        }
        result += "</tr>";
      }
      result += "</table>";
    } else {
      result += getTableHorizontallyLaidFromJSON(inputJSON);
    }
  } else {
    result += inputJSON + "<br>";
  }
  return result;
}