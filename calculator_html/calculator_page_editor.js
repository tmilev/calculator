"use strict";
const InputPanelData = require("./initialize_buttons").InputPanelData;
const autocompleter = require("./autocomplete").autocompleter;
const initializeButtons = require("./initialize_buttons");
const ids = require("./ids_dom_elements");
const processMonitoring = require("./process_monitoring");

class CalculatorEquationEditor {
  constructor(
    /** @type{Function} */
    keyPressHandler,
  ) {
    /** @type{InputPanelData|null} */
    this.calculatorPanel = null;
    this.flagCalculatorInputOK = true;
    this.keyPressHandler = keyPressHandler;
    this.theLaTeXString = null;
    this.extractor = new EditorInputExtractor();
    this.slider = document.getElementById(ids.domElements.pages.calculator.equationEditorFontSizeSlider);
  }

  initialize() {
    this.calculatorPanel = new InputPanelData({
      idEquationEditorElement: "mainInputEditorField",
      problemId: "",
      idPureLatex: ids.domElements.pages.calculator.inputMain,
      idButtonContainer: ids.domElements.pages.calculator.divEquationEditorButtons,
      flagCalculatorPanel: true,
      valueChangeHandler: () => {
        this.equationEditorChangeCallback();
      },
    });
    this.calculatorPanel.initialize();
    let pauseButton = document.getElementById(ids.domElements.pages.calculator.monitoring.buttonPauseToggle);
    pauseButton.addEventListener(
      "click", () => {
        processMonitoring.monitor.togglePause();
      }
    )
    pauseButton.style.display = "none";

    let inputMain = this.inputMainTextbox();
    inputMain.addEventListener("keypress", (e) => {
      this.keyPressHandler(e);
    });
    inputMain.addEventListener("keyup", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
    });
    inputMain.addEventListener("keydown", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
      autocompleter.arrowAction(e);
    });
    inputMain.addEventListener("mouseup", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
    });
    inputMain.addEventListener("input", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
    });
    this.slider.addEventListener("change", () => {
      this.clickSlider();
    });
    this.slider.addEventListener("input", () => {
      this.clickSlider();
    });
  }

  clickSlider() {
    let editor = this.calculatorPanel.equationEditor;
    this.calculatorPanel.equationEditorContainer.style.fontSize = `${this.slider.value}px`;
    editor.updateDOM();
    editor.rootNode.focusRestore(1);
    editor.writeDebugInfo(null);
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
    return document.getElementById(ids.domElements.pages.calculator.editorCommentsDebug);
  }

  writeIntoEquationEditor() {
    if (this.calculatorPanel.equationEditor === null) {
      return;
    }
    this.ignoreNextEditorEvent = true;
    let textBox = this.inputMainTextbox();
    if (textBox.value.length > 10000) {
      this.flagCalculatorInputOK = false;
      this.commentsElement().innerHTML = "<b style ='color:red'>Formula too big </b>";
      return;
    }
    this.commentsElement().textContent = "";
    this.extractor.extract(textBox.value, textBox.selectionEnd);
    this.flagCalculatorInputOK = true;
    this.calculatorPanel.equationEditor.writeLatex(this.extractor.middleEditedString);
    this.ignoreNextEditorEvent = false;
  }

  equationEditorChangeCallback() {
    if (!this.flagCalculatorInputOK) {
      return;
    }
    let content = this.calculatorPanel.equationEditor.rootNode.toLatex();
    this.extractor.middleEditedString = initializeButtons.processMathQuillLatex(content);
    this.inputMainTextbox().value = this.extractor.leftString + this.extractor.middleEditedString + this.extractor.rightString;
  }

  /** @return{HTMLTextAreaElement} */
  inputMainTextbox() {
    return this.calculatorPanel.getPureLatexElement();
  }
}

class EditorInputExtractor {
  constructor() {
    /** @type{string} */
    this.rawInput = "";
    /** @type{string} */
    this.leftString = "";
    /** @type{string} */
    this.middleEditedString = "";
    /** @type{string} */
    this.rightString = "";
    /** @type{number} */
    this.caretPosition = 0;

    /** @type{number} The right end of the selection, equal to the index of the right caret needed to enclose our selection.*/
    this.rightIndex = - 1;
    /** @type{number} The left end of the selection, equal to index of the left caret needed to enclose our selection. */
    this.leftIndex = - 1;
    /** @type{number}*/
    this.openLeftDelimiters = 0;
    /** @type{number}*/
    this.openRightDelimiters = 0;
    /** @type{boolean}*/
    this.foundSemiColumn = false;
    this.leftDelimiters = {
      "(": true,
      "{": true,
    };
    this.rightDelimiters = {
      ")": true,
      "}": true,
    }
  }

  extract(
    /** @type{string}*/
    inputLatex,
    /** @type{number}*/
    caretPosition,
  ) {
    this.rawInput = inputLatex;
    this.caretPosition = caretPosition;
    this.doExtract();
  }

  commentsElement() {
    return document.getElementById(ids.domElements.pages.calculator.editorCommentsDebug);
  }

  toStringDebug() {
    return `caret: ${this.caretPosition}, left: ${this.leftString}, leftIndex: ${this.leftIndex} middle: ${this.middleEditedString}, right: ${this.rightString}, rightIndex: ${this.rightIndex}`;
  }

  initializeIndices() {
    this.leftIndex = this.caretPosition;
    this.rightIndex = this.caretPosition;
    this.openLeftDelimiters = 0;
    this.openRightDelimiters = 0;
    this.foundRightEnd = false;
  }

  doExtract() {
    this.initializeIndices();
    while (this.growReturnFalseWhenDone()) {
    }
    this.leftString = this.rawInput.substr(0, this.leftIndex);
    this.middleEditedString = this.rawInput.substr(this.leftIndex, this.rightIndex - this.leftIndex);
    this.rightString = this.rawInput.substr(this.rightIndex);
    this.commentsElement().innerHTML = ""; //this.toStringDebug();
  }

  growReturnFalseWhenDone() {
    if (this.openLeftDelimiters > 0) {
      // More open right delimiters than left: we must expand to the left.
      if (this.expandRight()) {
        return true;
      }
    }
    if (this.openRightDelimiters > 0) {
      // More open left delimiters than right: we must expand to the right.
      if (this.expandLeft()) {
        return true;
      }
    }
    if (this.expandRight()) {
      return true;
    }
    if (this.expandLeft()) {
      return true;
    }
    return false;
  }

  expandRight() {
    if (this.rightIndex >= this.rawInput.length) {
      this.foundRightEnd = true
      return false;
    }
    if (this.foundRightEnd && this.openLeftDelimiters <= 0) {
      return false;
    }
    let rightCharacter = this.rawInput[this.rightIndex];
    if (rightCharacter === ";") {
      this.foundRightEnd = true;
      this.rightIndex++;
      return true;
    }
    if (rightCharacter in this.rightDelimiters) {
      if (this.openLeftDelimiters > 0) {
        this.openLeftDelimiters--;
      } else {
        this.openRightDelimiters++;
      }
      this.rightIndex++;
      return true;
    }
    if (rightCharacter in this.leftDelimiters) {
      this.openLeftDelimiters++;
      this.rightIndex++;
      return true;
    }
    this.rightIndex++;
    return true;
  }

  expandLeft() {
    if (this.leftIndex - 1 < 0) {
      return false;
    }
    let nextCharacter = this.rawInput[this.leftIndex - 1];
    if (nextCharacter in this.leftDelimiters) {
      if (this.foundRightEnd && this.openLeftDelimiters === 0 && this.openRightDelimiters === 0) {
        return false;
      }
      if (this.openRightDelimiters > 0) {
        this.openRightDelimiters--;
        this.leftIndex--;
        return true;
      }
      this.leftIndex--;
      this.openLeftDelimiters++;
      return true;
    }
    if (nextCharacter in this.rightDelimiters) {
      this.leftIndex--;
      this.openRightDelimiters++;
      return true;
    }
    if (nextCharacter === ";") {
      if (this.foundRightEnd && this.openLeftDelimiters === 0 && this.openRightDelimiters === 0) {
        return false;
      }
    }
    this.leftIndex--;
    return true;
  }
}

module.exports = {
  CalculatorEquationEditor,
};