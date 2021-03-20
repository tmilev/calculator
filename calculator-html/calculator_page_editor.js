"use strict";
const InputPanelData = require("./initialize_buttons").InputPanelData;
const autocomplete = require("./autocomplete");
const initializeButtons = require("./initialize_buttons");
const ids = require("./ids_dom_elements");

class CalculatorEquationEditor {
  constructor(
    /**@type{Function} */
    keyPressHandler,
  ) {
    /**@type{InputPanelData|null} */
    this.calculatorPanel = null;
    this.leftString = "";
    this.middleEditedString = "";
    this.rightString = "";
    this.flagCalculatorInputOK = true;
    this.keyPressHandler = keyPressHandler;
    this.theLaTeXString = null;
    this.selectionEnd = 0;
    this.calculatorSeparatorLeftDelimiters = {
      '(': true,
      '{': true
    };
  }

  initialize() {
    this.calculatorPanel = new InputPanelData({
      idEquationEditorElement: "mainInputEditorField",
      problemId: "",
      idPureLatex: ids.domElements.pages.calculator.inputMain,
      idButtonContainer: 'mainInputEditorFieldButtons',
      flagCalculatorPanel: true,
      valueChangeHandler: () => {
        this.equationEditorChangeCallback();
      },
    });
    this.calculatorPanel.initialize();
    document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle).addEventListener(
      "click", () => {
        processMonitoring.monitor.togglePause();
      }
    )
    let inputMain = this.inputMainTextbox();
    inputMain.addEventListener("keypress", (e) => {
      this.keyPressHandler(e);
    });
    inputMain.addEventListener("keyup", (e) => {
      autocomplete.suggestWord();
      this.writeIntoEquationEditor();
    });
    inputMain.addEventListener("keydown", (e) => {
      autocomplete.suggestWord();
      this.writeIntoEquationEditor();
      autocomplete.arrowAction(e);
    });
    inputMain.addEventListener("mouseup", (e) => {
      autocomplete.suggestWord();
      this.writeIntoEquationEditor();
    });
    inputMain.addEventListener("input", (e) => {
      autocomplete.suggestWord();
      this.writeIntoEquationEditor();
    });
  }

  isSeparatorCharacter(theChar) {
    if (theChar[0] >= 'a' && theChar[0] <= 'z') {
      return false;
    }
    if (theChar[0] >= 'A' && theChar[0] <= 'Z') {
      return false;
    }
    return true;
  }

  isKeyWordStartKnownToMathQuill(input) {
    for (let i = 0; i < keyWordsKnownToMathQuill.length; i++) {
      if (keyWordsKnownToMathQuill[i].startsWith(input)) {
        return true;
      }
    }
    return false;
  }

  isKeyWordEndKnownToMathQuill(input) {
    for (let i = 0; i < keyWordsKnownToMathQuill.length; i++) {
      if (keyWordsKnownToMathQuill[i].endsWith(input)) {
        return true;
      }
    }
    return false;
  }

  accountCalculatorDelimiterReturnMustEndSelection(character, calculatorSeparatorCounts) {
    if (character in this.calculatorSeparatorLeftDelimiters) {
      calculatorSeparatorCounts.leftSeparators++;
    }
    if (character in this.calculatorSeparatorLeftDelimiters) {
      calculatorSeparatorCounts.rightSeparators++;
    }
    return calculatorSeparatorCounts.leftSeparators > calculatorSeparatorCounts.rightSeparators;
  }

  commentsElement() {
    return document.getElementById(ids.domElements.pages.calculator.editorComments);
  }

  writeIntoEquationEditor() {
    if (this.calculatorPanel.equationEditor === null) {
      return;
    }
    this.ignoreNextEditorEvent = true;
    let textBox = this.inputMainTextbox();
    this.selectionEnd = textBox.selectionEnd;
    this.theLaTeXString = textBox.value;
    if (this.theLaTeXString.length > 10000) {
      this.flagCalculatorInputOK = false;
      this.commentsElement().innerHTML = "<b style ='color:red'>Formula too big </b>";
      return;
    }
    this.flagCalculatorInputOK = true;
    this.calculatorPanel.equationEditor.writeLatex(this.theLaTeXString);
    this.ignoreNextEditorEvent = false;
  }

  equationEditorChangeCallback() {
    if (!this.flagCalculatorInputOK) {
      return;
    }
    let content = this.calculatorPanel.equationEditor.rootNode.toLatex();
    this.middleEditedString = initializeButtons.processMathQuillLatex(content);
    this.inputMainTextbox().value = this.leftString + this.middleEditedString + this.rightString;
  }

  /**@returns{HTMLTextAreaElement} */
  inputMainTextbox() {
    return this.calculatorPanel.getPureLatexElement();
  }
}

module.exports = {
  CalculatorEquationEditor,
};