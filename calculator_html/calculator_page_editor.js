"use strict";
const InputPanelData = require("./initialize_buttons").InputPanelData;
const autocompleter = require("./autocomplete").autocompleter;
const initializeButtons = require("./initialize_buttons");
const ids = require("./ids_dom_elements");
const processMonitoring = require("./process_monitoring");
const miscellaneous = require("./miscellaneous_frontend");
const equationEditor = require("./equation_editor/src/equation_editor");

class CalculatorEquationEditor {
  constructor(
    /** @type {Function} */
    keyPressHandler,
  ) {
    /** @type {InputPanelData|null} */
    this.calculatorPanel = null;
    this.flagCalculatorInputOK = true;
    this.keyPressHandler = keyPressHandler;
    this.theLaTeXString = null;
    this.extractor = new EditorInputExtractor();
    this.slider = document.getElementById(
      ids.domElements.pages.calculator.equationEditorFontSizeSlider
    );
    /** @type {HTMLDivElement|null} */
    this.textBox = null;
    /** @type {HTMLElement|null} */
    this.commentsElement = null;
    // Set this to true to disable the action of writeComments().
    this.ignoreComments = true;
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
    let pauseButton = document.getElementById(
      ids.domElements.pages.calculator.monitoring.buttonPauseToggle
    );
    pauseButton.addEventListener(
      "click", () => {
        processMonitoring.monitor.togglePause();
      }
    )
    pauseButton.style.display = "none";

    this.textBox = document.getElementById(ids.domElements.pages.calculator.inputMain);
    const isChrome = navigator.userAgent.includes("Chrome");
    if (!isChrome) {
      this.textBox.contentEditable = true;
    }

    this.commentsElement = document.getElementById(
      ids.domElements.pages.calculator.editorCommentsDebug,
    );
    this.textBox.addEventListener("keypress", (e) => {
      this.keyPressHandler(e);
    });
    this.textBox.addEventListener("keyup", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
    });
    this.textBox.addEventListener("keydown", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
      autocompleter.arrowAction(e);
    });
    this.textBox.addEventListener("mouseup", (e) => {
      autocompleter.suggestWord();
      this.writeIntoEquationEditor();
    });
    this.textBox.addEventListener("input", (e) => {
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

  isSeparatorCharacter(inputCharacter) {
    if (inputCharacter[0] >= 'a' && inputCharacter[0] <= 'z') {
      return false;
    }
    if (inputCharacter[0] >= 'A' && inputCharacter[0] <= 'Z') {
      return false;
    }
    return true;
  }

  accountCalculatorDelimiterReturnMustEndSelection(
    character, calculatorSeparatorCounts,
  ) {
    if (character in this.calculatorSeparatorLeftDelimiters) {
      calculatorSeparatorCounts.leftSeparators++;
    }
    if (character in this.calculatorSeparatorLeftDelimiters) {
      calculatorSeparatorCounts.rightSeparators++;
    }
    return calculatorSeparatorCounts.leftSeparators > calculatorSeparatorCounts.rightSeparators;
  }

  writeIntoEquationEditor() {
    if (this.calculatorPanel.equationEditor === null) {
      return;
    }
    this.ignoreNextEditorEvent = true;
    if (this.textBox.textContent.length > 10000) {
      this.flagCalculatorInputOK = false;
      miscellaneous.writeHTML(
        this.commentsElement,
        "<b style ='color:red'>Formula too big </b>",
      );
      return;
    }
    this.commentsElement.textContent = "";
    const cursorInformation = new equationEditor.CursorInformation(this.textBox);
    this.extractor.extract(this.textBox.textContent, cursorInformation.positionCursor);
    this.flagCalculatorInputOK = true;
    this.calculatorPanel.equationEditor.writeLatex(this.extractor.middleEditedString);
    this.ignoreNextEditorEvent = false;
    this.writeComments();
  }

  equationEditorChangeCallback() {
    if (!this.flagCalculatorInputOK) {
      return;
    }
    let content = this.calculatorPanel.equationEditor.rootNode.toLatex();
    this.extractor.middleEditedString = content;
    this.textBox.textContent =
      this.extractor.leftString +
      this.extractor.middleEditedString +
      this.extractor.rightString;
    this.writeComments();
  }

  writeComments() {
    if (this.ignoreComments) {
      return;
    }
    this.commentsElement.textContent = "";
    this.commentsElement.appendChild(this.extractor.getCommentHTML());
  }
}

class EditorInputExtractor {
  constructor() {
    /** @type {string} */
    this.rawInput = "";
    /** @type {string} */
    this.leftString = "";
    /** @type {string} */
    this.middleEditedString = "";
    /** @type {string} */
    this.rightString = "";
    /** @type {number} */
    this.cursorPosition = 0;

    /** 
     * @type {number} 
     * The right end of the selection, equal to the index of the right 
     * cursor needed to enclose our selection. Note that the 
     * largest possible value for the index of the cursor is
     * equal to the length of the string, so 1 larger than 
     * the largest index of a character in the string.
     */
    this.rightIndex = - 1;
    /** 
     * @type {number} 
     * The left end of the selection, equal to index of the left 
     * cursor needed to enclose our selection. 
     */
    this.leftIndex = - 1;
    /** @type {number} */
    this.openLeftDelimiters = 0;
    /** @type {number} */
    this.openRightDelimiters = 0;
    /** @type {boolean} */
    this.foundSemiColumn = false;
    this.leftDelimiters = {
      "(": true,
      "{": true,
    };
    this.rightDelimiters = {
      ")": true,
      "}": true,
    };
    this.commentsElement = document.getElementById(
      ids.domElements.pages.calculator.editorCommentsDebug
    );
  }

  extract(
    /** @type {string} */
    inputLatex,
    /** @type {number} */
    cursorPosition,
  ) {
    if (
      cursorPosition === null ||
      cursorPosition === undefined ||
      cursorPosition < 0 ||
      cursorPosition > inputLatex.length + 1
    ) {
      console.log("Invalid cursorPosition.");
      return;
    }
    this.rawInput = inputLatex;
    this.cursorPosition = cursorPosition;
    this.doExtract();
  }

  toStringDebug() {
    return `cursor: ${this.cursorPosition}, ` +
      `left: ${this.leftString}, leftIndex: ${this.leftIndex}, ` +
      `middle: ${this.middleEditedString}, right: ${this.rightString}, ` +
      `rightIndex: ${this.rightIndex}`;
  }

  toStringDebugShort() {
    return `cursor: ${this.cursorPosition}, ` +
      `leftIndex: ${this.leftIndex}, ` +
      `rightIndex: ${this.rightIndex}`;
  }

  initializeIndices() {
    this.leftIndex = this.cursorPosition;
    this.rightIndex = this.cursorPosition;
    this.openLeftDelimiters = 0;
    this.openRightDelimiters = 0;
    this.foundRightEnd = false;
  }

  doExtract() {
    this.initializeIndices();
    while (this.growReturnFalseWhenDone()) {
    }
    this.leftString = this.rawInput.substring(0, this.leftIndex);
    this.middleEditedString = this.rawInput.substring(
      this.leftIndex,
      this.rightIndex,
    );
    this.rightString = this.rawInput.substring(this.rightIndex);
    this.commentsElement.textContent = "";
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
      this.foundRightEnd = true;
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
      if (
        this.foundRightEnd &&
        this.openLeftDelimiters === 0 &&
        this.openRightDelimiters === 0
      ) {
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
      if (
        this.foundRightEnd &&
        this.openLeftDelimiters === 0 &&
        this.openRightDelimiters === 0
      ) {
        return false;
      }
    }
    this.leftIndex--;
    return true;
  }

  getCommentHTML() {
    let result = document.createElement("div");
    let left = document.createElement("span");
    left.textContent = this.leftString;
    let selected = document.createElement("b");
    selected.style.color = "red";
    selected.textContent = this.middleEditedString;
    let right = document.createElement("span");
    right.textContent = this.rightString;
    result.appendChild(left);
    result.appendChild(selected);
    result.appendChild(right);
    let separator = document.createElement("hr");
    let fullText = document.createElement("span");
    fullText.textContent = this.rawInput;
    result.appendChild(separator);
    result.appendChild(fullText);
    let anotherSeparator = document.createElement("hr");
    let debugString = document.createElement("span");
    debugString.textContent = this.toStringDebugShort();
    result.appendChild(anotherSeparator);
    result.appendChild(debugString);
    return result;
  }
}

module.exports = {
  CalculatorEquationEditor,
};