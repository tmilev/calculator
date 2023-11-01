"use strict";
const equationEditor = require("./equation_editor/src/equation_editor");
const ids = require("./ids_dom_elements");
const autoCompleteDictionary = require("./autocomplete_keywords").autoCompleteDictionary;

class WordWithHighlight {
  constructor(
    /** @type {string} */
    left,
    /** @type {string} */
    highlighted,
    /** @type {string} */
    right,
  ) {
    this.left = left;
    this.highlighted = highlighted;
    this.right = right;
    this.autocompleteActive = false;
  }

  /** @return {HTMLElement} */
  toHTML(
    /** @type {boolean} */
    isHighlighted,
    /** @type {Function} */
    clickHandler,
  ) {
    let result = document.createElement("div");
    let leftSpan = document.createElement("span");
    let rightSpan = document.createElement("span");
    let boldSpan = document.createElement("b");
    leftSpan.textContent = this.left;
    rightSpan.textContent = this.right;
    boldSpan.textContent = this.highlighted;
    result.appendChild(leftSpan);
    result.appendChild(boldSpan);
    result.appendChild(rightSpan);
    result.addEventListener('click', clickHandler);
    result.style.cursor = "pointer";
    if (isHighlighted) {
      result.style.background = "silver";
    }
    return result;
  }
}

class Autocompleter {
  constructor() {
    this.initialized = false;
    /** @type {Array.<string>} */
    this.autoCompleteDictionaryLowerCase = [];
    this.minLengthAutocompleteTrigger = 2;
    this.debugCounter = 0;
    /** @type {Array.<string>} */
    this.autoCompleteDictionaryByKey = [];
    /** @type {HTMLDivElement} */
    this.inputElement = null;
    /** @type {Array.<string>} */
    this.suggestions = [];
    /** @type {Array.<WordWithHighlight>} */
    this.suggestionsHighlighted = [];
    /** @type {HTMLElement} */
    this.autocompleteElement = document.getElementById(
      ids.domElements.pages.calculator.inputAutocompleteList
    );
    /** @type {HTMLElement} */
    this.autocompleteHintsElement = document.getElementById(
      ids.domElements.pages.calculator.divAutocompleteHints
    );
    /** @type {HTMLElement} */
    this.autocompletePanel = document.getElementById(
      ids.domElements.pages.calculator.divAutocompletePanel
    );
    const button = document.getElementById(
      ids.domElements.pages.calculator.buttonAutocompleteHide
    );
    button.addEventListener(
      "click", () => {
        this.hideAutocompletePanel();
      }
    );
    this.indexInAutocomplete = - 1;
    this.lastWordStart = - 1;
    this.lastWord = "";
    this.oldLastWord = "";
  }

  initialize() {
    if (this.initialized) {
      return;
    }
    this.initialized = true;
    if (
      autoCompleteDictionary === undefined ||
      autoCompleteDictionary == null
    ) {
      return;
    }
    this.inputElement = document.getElementById(ids.domElements.pages.calculator.inputMain);
    this.indexInAutocomplete = - 1;
    this.lastWord = "";
    for (let i = 0; i < autoCompleteDictionary.length; i++) {
      this.autoCompleteDictionaryLowerCase.push(autoCompleteDictionary[i].toLowerCase());
    }
    for (let i = 0; i < autoCompleteDictionary.length; i++) {
      this.autoCompleteDictionaryByKey[autoCompleteDictionary[i]] = 1;
    }
    return true;
  }

  suggestWord() {
    this.initialize();
    this.getLastWord();
    if (this.oldLastWord === this.lastWord) {
      return;
    }
    this.oldLastWord = this.lastWord;
    this.suggestions = [];
    this.suggestionsHighlighted = [];
    if (this.lastWord == "") {
      this.clearAutocompleteTab();
      return;
    }
    if (this.lastWord.length < this.minLengthAutocompleteTrigger) {
      this.clearAutocompleteTab();
      return;
    }
    for (let i = 0; i < autoCompleteDictionary.length; i++) {
      if (this.lastWord == autoCompleteDictionary[i]) {
        this.clearAutocompleteTab();
        return;
      }
      let lastWordLowerCase = this.lastWord.toLowerCase();
      let startingPosition = this.autoCompleteDictionaryLowerCase[i].indexOf(lastWordLowerCase);
      if (startingPosition === - 1) {
        continue;
      }
      let left = autoCompleteDictionary[i].substring(0, startingPosition);
      let right = autoCompleteDictionary[i].substring(startingPosition + this.lastWord.length);
      let nextWord = new WordWithHighlight(left, this.lastWord, right);
      this.suggestions.push(autoCompleteDictionary[i]);
      this.suggestionsHighlighted.push(nextWord);
    }
    this.indexInAutocomplete = 0;
    this.displaySuggestions();
    const selection = window.getSelection();
    const range = selection.getRangeAt(0);
    const rect = range.getClientRects()[0];
    if (this.suggestions.length > 0) {
      this.autocompletePanel.style.display = "";
      let autoCompleteWidth = this.autocompletePanel.getBoundingClientRect().width;
      let inputWidth = this.inputElement.getBoundingClientRect().width;
      let desiredX = rect.x;
      if (rect.x + autoCompleteWidth > inputWidth) {
        desiredX = inputWidth - autoCompleteWidth;
      }
      this.autocompletePanel.style.left = desiredX;
      this.autocompletePanel.style.top = rect.y + 20;
    } else {
      this.hideAutocompletePanel();
    }
  }

  hideAutocompletePanel() {
    this.autocompletePanel.style.display = "none";
    this.autocompleteActive = false;
  }

  arrowAction(event) {
    if (event.shiftKey === true || !this.autocompleteActive) {
      return;
    }
    // 32-space bar, 13-enter key, 9-tab key
    if (event.keyCode === 13) {
      this.replaceLastWord();
      event.preventDefault();
      event.stopPropagation();
      return;
    }
    if (event.keyCode === 27) {
      // Escape key.
      this.hideAutocompletePanel();
      event.stopPropagation();
      return;
    }
    if (event.keyCode === 38) {//up arrow
      this.indexInAutocomplete--;
    } else if (event.keyCode === 40) {//down arrow
      this.indexInAutocomplete++;
    } else {
      return;
    }
    this.displaySuggestions();
    event.preventDefault();
  }

  isSeparator(theChar) {
    switch (theChar) {
      case '+':
      case ' ':
      case ',':
      case '(':
      case ')':
      case '%':
      case '\n':
      case '\r':
      case '\"':
      case '=':
        return true;
      default: return false;
    }
  }

  getLastWord() {
    let lastWordReversedArray = [];
    let caretPosition = new equationEditor.CursorInformation(this.inputElement).positionCursor;
    let elementText = this.inputElement.textContent;
    this.lastWordStart = - 1;
    let i = caretPosition - 1;
    for (; i >= 0; i--) {
      if (this.isSeparator(elementText[i])) {
        break;
      }
      lastWordReversedArray.push(elementText[i]);
    }
    this.lastWord = lastWordReversedArray.reverse().join("");
    this.lastWordStart = i;
    let suffix = [];
    for (i = caretPosition; i < elementText.length; i++) {
      if (this.isSeparator(elementText[i])) {
        break;
      } else {
        suffix.push(elementText[i]);
      }
    }
    this.lastWord += suffix.join("");
    this.debugCounter++;
  }

  accountWordAtCursor(amount) {
    this.getLastWord();
    if (this.lastWord === "" || this.lastWord === null) {
      return;
    }
    this.autocompleteElement.textContent = this.lastWord;
    if (typeof (autoCompleteDictionary[this.lastWord]) !== "number") {
      autoCompleteDictionary[this.lastWord] = amount;
    } else {
      autoCompleteDictionary[this.lastWord] += amount;
    }
    displayDictionary();
  }

  displayDictionary() {
    let result;
    result = "";
    counter = 0;
    for (let i in this.autoCompleteDictionaryByKey) {
      result += i + " " + this.autoCompleteDictionaryByKey[i] + " <br />";
      counter++;
      let length = Object.keys(this.autoCompleteDictionaryByKey).length;
      if (counter >= 15) {
        result += `... displaying first 15 out of a total of ${length} words in the dictionary.`;
        break;
      }
    }
  }

  clearAutocompleteTab() {
    this.autocompleteElement.textContent = "";
    this.indexInAutocomplete = - 1;
    this.suggestions.length = [];
    this.suggestionsHighlighted = [];
    this.hideAutocompletePanel();
  }

  displaySuggestions() {
    this.autocompleteElement.textContent = '';
    if (this.suggestionsHighlighted.length > 0) {
      if (this.indexInAutocomplete < 0) {
        this.indexInAutocomplete = this.suggestionsHighlighted.length - 1;
      }
      if (this.indexInAutocomplete >= this.suggestionsHighlighted.length) {
        this.indexInAutocomplete = 0;
      }
    }
    for (let i = 0; i < this.suggestionsHighlighted.length; i++) {
      let shouldHighlight = i === this.indexInAutocomplete;
      let next = this.suggestionsHighlighted[i].toHTML(
        shouldHighlight,
        () => {
          this.handleClick(i);
        },
      );
      this.autocompleteElement.appendChild(next);
    }
    this.autocompleteActive = this.suggestionsHighlighted.length > 0;
  }

  handleClick(
    /** @type {number} */ indexClicked
  ) {
    this.indexInAutocomplete = indexClicked;
    this.autocompleteElement.textContent = '';
    this.replaceLastWord();
  }

  replaceLastWord() {
    if (this.suggestions.length <= 0) {
      return;
    }
    if (
      this.indexInAutocomplete < 0 ||
      this.indexInAutocomplete >= this.suggestions.length
    ) {
      return;
    }
    let text = this.inputElement.textContent;
    this.inputElement.textContent = text.substring(0, this.lastWordStart + 1) + this.suggestions[this.indexInAutocomplete] +
      text.substring(1 + this.lastWordStart + this.lastWord.length);
    this.setCursorPosition(this.lastWordStart + this.suggestions[this.indexInAutocomplete].length + 1);
    this.hideAutocompletePanel();
  }

  setCursorPosition(caretPosition) {
    if (this.inputElement.createTextRange) {
      let range = this.inputElement.createTextRange();
      range.move('character', caretPosition);
      range.select();
      return;
    }
    let selection = window.getSelection();
    this.inputElement.focus();
    let range = document.createRange();
    let collapseToStart = false;
    if (caretPosition >= this.inputElement.textContent.length) {
      collapseToStart = false;
      range.selectNodeContents(this.inputElement);
    } else if (caretPosition > 0) {
      collapseToStart = false;
      range.setStart(this.inputElement.childNodes[0], caretPosition)
    } else {
      collapseToStart = true;
      range.setStart(this.inputElement.childNodes[0], 0);
    }
    range.collapse(collapseToStart);
    selection.removeAllRanges();
    selection.addRange(range);
  }
}

let autocompleter = new Autocompleter();

module.exports = {
  autocompleter,
};