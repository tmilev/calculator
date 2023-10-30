"use strict";
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
    if (this.suggestions.length > 0) {
      this.autocompleteHintsElement.style.visibility = "visible";
    }
    this.indexInAutocomplete = 0;
    this.displaySuggestions();
  }

  arrowAction(event) {
    if (event.ctrlKey !== true) {
      return;
    }
    if (event.keyCode === 32) {//32-space bar, 13-enter key, 9-tab key
      this.replaceLastWord();
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
    let caretPosition = this.inputElement.selectionEnd;
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
  }

  setCursorPosition(caretPosition) {
    if (this.inputElement.createTextRange) {
      let range = this.inputElement.createTextRange();
      range.move('character', caretPosition);
      range.select();
    } else {
      if (this.inputElement.selectionStart) {
        this.inputElement.focus();
        this.inputElement.setSelectionRange(caretPosition, caretPosition);
      } else {
        this.inputElement.focus();
      }
    }
  }
}

let autocompleter = new Autocompleter();

module.exports = {
  autocompleter,
};