"use strict";
const ids = require("./ids_dom_elements");
const InputPanelData = require("./initialize_buttons").InputPanelData;
const storage = require("./storage").storage;
const submit = require("./submit_requests");
const miscellaneous = require("./miscellaneous_frontend");

class CompareExpressionsSocratic {
  constructor() {
    /**@type{boolean} */
    this.initialized = false;
    /**@type{InputPanelData|null} */
    this.givenPanel = null;
    /**@type{InputPanelData|null} */
    this.desiredPanel = null;
    /**@type{HTMLElement} */
    this.resultUserFriendly = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.resultUserFriendly);
    /**@type{HTMLElement} */
    this.resultBoxRaw = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.resultRaw);
    /**@type{HTMLElement} */
    this.resultBoxFormatted = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.resultFormatted);
    /**@type{HTMLElement} */
    this.calculatorLink = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.calculatorLink);
    /**@type{string} */
    this.givenData = "";
    /**@type{string} */
    this.desiredData = "";
    /** @type{string} 
     * We store our socratic key with byte entries shifted so it does not appear in the javascript as plain text. 
     * Of course, this is not intended to be secure, 
     * but a protection that the api key pop up easily in searches.
     */
    this.keyShifted = "FNfX~H|TUY^t{\\y~8INq[OWRMf~5h_YY66gunl";
  }

  /** @return{string} */
  getKey() {
    let result = "";
    for (let i = 0; i < this.keyShifted.length; i++) {
      result += String.fromCharCode(this.keyShifted.charCodeAt(i) - 5);
    }
    return result;
  }

  selectPage() {
    if (this.initialized) {
      return;
    }
    let button = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.buttonCompare);
    if (button === null) {
      return;
    }
    button.addEventListener("click", () => {
      this.compare();
    });

    this.initialized = true;
    this.givenPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressionsSocratic.givenEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressionsSocratic.givenPanel,
      idPureLatex: ids.domElements.pages.compareExpressionsSocratic.givenRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.givenPanel.initialize();

    this.desiredPanel = new InputPanelData({
      idEquationEditorElement: ids.domElements.pages.compareExpressionsSocratic.desiredEquationEditor,
      problemId: "",
      idButtonContainer: ids.domElements.pages.compareExpressionsSocratic.desiredPanel,
      idPureLatex: ids.domElements.pages.compareExpressionsSocratic.desiredRawInput,
      flagCalculatorPanel: false,
      flagAnswerPanel: false,
      buttonsPerLine: 8,
    });
    this.desiredPanel.initialize();

    if (storage.variables.compareSocratic.given.getValue() !== "") {
      this.givenPanel.equationEditor.writeLatex(storage.variables.compareSocratic.given.getValue());
      this.givenPanel.equationEditor.writeLatexToInput();
    }
    if (storage.variables.compareSocratic.desired.getValue() !== "") {
      this.desiredPanel.equationEditor.writeLatex(storage.variables.compareSocratic.desired.getValue());
      this.desiredPanel.equationEditor.writeLatexToInput();
    }
  }

  storeData() {
    let givenContainer = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.givenRawInput);
    this.givenData = givenContainer.value;
    let desiredContainer = document.getElementById(ids.domElements.pages.compareExpressionsSocratic.desiredRawInput);
    this.desiredData = desiredContainer.value;
  }

  compare() {
    this.storeData();
    storage.variables.compareSocratic.given.setAndStore(this.givenData, true);
    storage.variables.compareSocratic.desired.setAndStore(this.desiredData, true);
    this.doCompare();
  }

  doCompare() {
    let url = `https://bloom-pa.googleapis.com/v1/math:grade?key=${this.getKey()}`;
    let payload = JSON.stringify({
      "given_answer": {
        "text": "x",
      },
    });
    submit.submitPOST({
      url: url,
      callback: (input) => {
        this.writeResult(input);
      },
      parameters: payload,
      progress: ids.domElements.pages.compareExpressionsSocratic.progress,
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
      this.resultBoxRaw.textContent = JSON.stringify(result);
      this.resultBoxFormatted.textContent = "";
      jsonToHtml.writeJSONtoDOMComponent(result, this.resultBoxFormatted);
    } catch (e) {
      this.resultBoxRaw.innerHTML = `<b style='color:red'>${e}</b><br>${input}`;
    }
  }

}

const compareExpressionsSocratic = new CompareExpressionsSocratic();

module.exports = {
  compareExpressionsSocratic,
};
