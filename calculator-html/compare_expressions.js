"use strict";
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const storage = require("./storage").storage;
const miscellaneous = require("./miscellaneous_frontend");
const equationEditor = require("./equation_editor");
const submit = require("./submit_requests");
const jsonToHtml = require("./json_to_html");

class CompareExpressions {
  constructor() {
    /**@type{boolean} */
    this.initialized = false;
    /**@type{InputPanelData|null} */
    this.givenPanel = null;
    /**@type{InputPanelData|null} */
    this.desiredPanel = null;
    /**@type{HTMLElement} */
    this.resultUserFriendly = document.getElementById(ids.domElements.pages.compareExpressions.resultUserFriendly);
    /**@type{HTMLElement} */
    this.resultBoxRaw = document.getElementById(ids.domElements.pages.compareExpressions.resultRaw);
    /**@type{HTMLElement} */
    this.resultBoxFormatted = document.getElementById(ids.domElements.pages.compareExpressions.resultFormatted);
    /**@type{HTMLElement} */
    this.calculatorLink = document.getElementById(ids.domElements.pages.compareExpressions.calculatorLink);
    /**@type{string} */
    this.givenData = "";
    /**@type{string} */
    this.desiredData = "";
  }

  selectPage() {
    if (this.initialized) {
      return;
    }
    let button = document.getElementById(ids.domElements.pages.compareExpressions.buttonCompare);
    if (button === null) {
      return;
    }
    button.addEventListener("click", () => {
      this.compare();
    });

    this.initialized = true;
    this.givenPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressions.givenEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressions.givenPanel,
      idPureLatex: ids.domElements.pages.compareExpressions.givenRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.givenPanel.initialize();

    this.desiredPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressions.desiredEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressions.desiredPanel,
      idPureLatex: ids.domElements.pages.compareExpressions.desiredRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.desiredPanel.initialize();

    if (storage.variables.compare.given.getValue() !== "") {
      this.givenPanel.equationEditor.writeLatex(storage.variables.compare.given.getValue());
      this.givenPanel.equationEditor.writeLatexToInput();
    }
    if (storage.variables.compare.desired.getValue() !== "") {
      this.desiredPanel.equationEditor.writeLatex(storage.variables.compare.desired.getValue());
      this.desiredPanel.equationEditor.writeLatexToInput();
    }
  }

  storeData() {
    let givenContainer = document.getElementById(ids.domElements.pages.compareExpressions.givenRawInput);
    this.givenData = givenContainer.value;
    let desiredContainer = document.getElementById(ids.domElements.pages.compareExpressions.desiredRawInput);
    this.desiredData = desiredContainer.value;
  }

  compare() {
    this.storeData();
    storage.variables.compare.given.setAndStore(this.givenData, true);
    storage.variables.compare.desired.setAndStore(this.desiredData, true);
    this.doCompare();
  }

  doCompare(
  ) {
    let debug = storage.variables.flagDebug.isTrue();
    submit.submitGET({
      url: pathnames.addresses.compareExpressions(this.givenData, this.desiredData, debug),
      callback: (input) => {
        this.writeResult(input);
      },
      progress: ids.domElements.pages.compareExpressions.progress,
      panelOptions: {
        dontCollapsePanel: true,
      },
    });
  }

  writeResult(
    /**@type{string} */
    input,
  ) {
    try {
      let result = miscellaneous.jsonUnescapeParse(input);
      let resultHTML = this.htmlUserFriendlyResult(result);
      this.resultUserFriendly.innerHTML = resultHTML;
      this.resultBoxRaw.textContent = JSON.stringify(result);
      jsonToHtml.writeJSONtoDOMComponent(result, this.resultBoxFormatted);
      equationEditor.typeset(this.resultBoxFormatted);
    } catch (e) {
      this.resultBoxRaw.innerHTML = `<b style='color:red'>${e}</b><br>${input}`;
    }
  }

  writeCalculatorLink() {
    let url = pathnames.addresses.calculatorComputation(
      `CompareExpressionsJSON(${this.givenData},${this.desiredData})`,
    );
    this.calculatorLink.innerHTML = `<a href=${url} target="_blank">calculator link</a>`;
  }

  getFirstValueFromKeys(
    input,
    /**@type{string[]} */
    keys,
  ) {
    for (let i = 0; i < keys.length; i++) {
      let candidate = input[keys[i]];
      if (candidate !== undefined && candidate !== null && candidate !== "") {
        return candidate;
      }
    }
    return "";
  }

  getAllValuesOfGivenKeys(
    input,
    /**@type{string[]} */
    labels,
  ) {
    let result = [];
    for (let i = 0; i < labels.length; i++) {
      let candidate = input[labels[i]];
      if (candidate !== undefined && candidate !== null && candidate !== "") {
        result.push(candidate);
      }
    }
    return result.join("<br>");
  }

  htmlUserFriendlyResult(input) {
    let syntaxErrors = this.getFirstValueFromKeys(input, [
      pathnames.urlFields.result.syntaxErrors,
      pathnames.urlFields.result.syntaxErrorsExtra
    ]);
    if (syntaxErrors !== "") {
      return `<b style='color:red'>?</b><br>${syntaxErrors}`;
    }
    this.writeCalculatorLink();
    let errorTags = [
      pathnames.urlFields.result.error,
      pathnames.urlFields.result.comparison.errorEvaluation,
      pathnames.urlFields.result.comparison.errorInAnswer,
    ];
    let errorsEvaluation = this.getAllValuesOfGivenKeys(input, errorTags);
    if (errorsEvaluation !== "") {
      return `<b style='color:red'>?</b><br>${errorsEvaluation}`;
    }
    let resultHTML = this.getAllValuesOfGivenKeys(input, [pathnames.urlFields.result.resultHtml]);
    let commentsAndCrashes = [
      pathnames.urlFields.result.comments,
      pathnames.urlFields.result.commentsGlobal,
      pathnames.urlFields.result.crashReport,
    ];
    resultHTML += this.getAllValuesOfGivenKeys(input, commentsAndCrashes);
    return resultHTML;
  }
}

const compareExpressions = new CompareExpressions();

module.exports = {
  compareExpressions,
};
