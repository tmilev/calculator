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
  }

  initialize() {
    this.calculatorPanel = new InputPanelData({
      idEquationEditorElement: "mainInputEditorField",
      idEditorComments: "mqPanelComments",
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

  getSemiColumnEnclosure() {
    let startPos = this.selectionEnd;
    let calculatorInput = this.calculatorPanel.getPureLatexElement();
    for (; startPos > 0 && startPos < calculatorInput.value.length; startPos--) {
      if (isSeparatorCharacter(calculatorInput.value[startPos])) {
        break;
      }
    }
    if (startPos >= calculatorInput.value.length) {
      startPos = calculatorInput.value.length - 1;
    }
    let rightPos = startPos;
    let lastSeparator = startPos;
    let lastWord = '';
    let currentChar = 'a';
    for (; rightPos < calculatorInput.value.length - 1; rightPos++) {
      currentChar = calculatorInput.value[rightPos];
      if (currentChar === ';') {
        if (rightPos > 0) {
          rightPos--;
        }
        break;
      }
      if (isSeparatorCharacter(currentChar)) {
        lastWord = '';
        lastSeparator = rightPos;
      } else {
        lastWord += currentChar;
      }
      if (lastWord.length > 3) {
        if (!isKeyWordStartKnownToMathQuill(lastWord)) {
          rightPos = lastSeparator;
          break;
        }
      }
    }
    let calculatorSeparatorCounts = {
      leftSeparators: 0,
      rightSeparators: 0
    };
    let leftPos = rightPos - 1;
    lastWord = '';
    lastSeparator = rightPos;
    for (; leftPos > 0; leftPos--) {
      currentChar = calculatorInput.value[leftPos];
      if (currentChar === ';') {
        leftPos++;
        break;
      }
      if (accountCalculatorDelimiterReturnMustEndSelection(calculatorInput.value[leftPos], calculatorSeparatorCounts)) {
        leftPos++;
        break;
      }
      if (isSeparatorCharacter(currentChar)) {
        lastWord = '';
        lastSeparator = leftPos;
      } else {
        lastWord = currentChar + lastWord;
      }
      if (lastWord.length > 3) {
        if (!isKeyWordEndKnownToMathQuill(lastWord)) {
          leftPos = lastSeparator;
          break;
        }
      }
    }
    if (leftPos > rightPos) {
      leftPos = rightPos;
    }
    if (rightPos - leftPos > 1000) {
      console.log(`Latex may be too large for the editor, ${rightPos - leftPos} characters.`);
    }
    let startingCharacterSectionUnderMathQuillEdit = '';
    if (calculatorInput.value[leftPos] === '\n' || calculatorInput.value[leftPos] === ' ' ||
      calculatorInput.value[leftPos] === '\t') {
      startingCharacterSectionUnderMathQuillEdit = calculatorInput.value[leftPos];
    }
    this.chopStrings(leftPos, rightPos);
  }

  chopStrings(
    /**@type{number} */
    calculatorLeftPosition,
    /**@type{number} */
    calculatorRightPosition,
  ) {
    let mqCommentsSpan = document.getElementById(this.calculatorPanel.idEditorComments);
    if (calculatorRightPosition - calculatorLeftPosition > 1000) {
      this.flagCalculatorInputOK = false;
      mqCommentsSpan.innerHTML = "<b style ='color:red'>Formula too big </b>";
      return;
    }
    this.flagCalculatorInputOK = true;
    mqCommentsSpan.innerHTML = "Equation assistant";
    this.calculatorPanel.getEditorContainer().style.visibility = "visible";
    let calculatorInput = this.calculatorPanel.getPureLatexElement();
    this.theLaTeXString = calculatorInput.value.substring(calculatorLeftPosition, calculatorRightPosition + 1);
    this.calculatorLeftString = calculatorInput.value.substring(0, calculatorLeftPosition);
    this.calculatorRightString = calculatorInput.value.substring(calculatorRightPosition + 1);
  }

  writeIntoEquationEditor() {
    this.getSemiColumnEnclosure();
    if (this.calculatorPanel.equationEditor === null) {
      return;
    }
    this.ignoreNextEditorEvent = true;
    if (this.flagCalculatorInputOK) {
      this.calculatorPanel.equationEditor.writeLatex(this.theLaTeXString);
    }
    this.ignoreNextEditorEvent = false;
  }

  equationEditorChangeCallback() {
    if (!this.calculatorPanel.flagCalculatorInputOK) {
      return;
    }
    let content = this.calculatorPanel.equationEditor.rootNode.toLatex();
    this.middleEditedString = initializeButtons.processMathQuillLatex(content);
    this.inputMainTextbox().value = this.leftString + this.middleEditedString + this.rightString;
  }

  inputMainTextbox() {
    return document.getElementById(ids.domElements.pages.calculator.inputMain);
  }
}

module.exports = {
  CalculatorEquationEditor,
};