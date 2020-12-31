"use strict";
var ids = require("./ids_dom_elements");

class Theme {
  constructor() {
    var themeIds = ids.domElements.themeRadioButtons;
    this.currentTheme = themeIds.radioThemeDefault;
    this.themes = {};
    this.themes[themeIds.radioThemeDefault] = {
      "--main-border-radius": "3px",
      "--main-background-color": "white",
      "--box-shadow-buttonSelectPage": "none",
      "--box-shadow-tableCalculatorOutputs": "none",
      "--box-shadow-buttonPreview": "none",
      "--box-shadow-buttonDefault": "none",
      "--box-shadow-buttonAnswer": "none",
      "--box-shadow-buttonMQ": "none",
    };
    this.themes[themeIds.radioThemeExperimental] = {
      "--main-border-radius": "0px",
      "--main-background-color": "white",
      "--box-shadow-buttonSelectPage": "5px 5px 5px grey",
      "--box-shadow-tableCalculatorOutputs": "none",
      "--box-shadow-buttonPreview": "5px 5px 5px grey",
      "--box-shadow-buttonDefault": "5px 5px 5px grey",
      "--box-shadow-buttonAnswer": "5px 5px 5px grey",
      "--box-shadow-buttonMQ": "3px 3px 3px grey",
    };
  }

  setFromRadioButton() {
    var value = null;
    for (var label in ids.domElements.themeRadioButtons) {
      var currentId = ids.domElements.themeRadioButtons[label];
      var currentElement = document.getElementById(currentId);
      if (currentElement.checked) {
        value = currentId;
      }
    }
    if (value === null) {
      return;
    }
    if (value === this.currentTheme) {
      return;
    }
    var variables = window.calculator.mainPage.storage.variables;
    variables.theme.setAndStore(value);
  }

  doChangeTheme(themeId) {
    if (themeId === this.currentTheme) {
      return;
    }
    this.currentTheme = themeId;
    var currentSpec = this.themes[this.currentTheme];
    var root = document.documentElement;
    for (var label in currentSpec) {
      root.style.setProperty(label, currentSpec[label]);
    }
    document.getElementById(this.currentTheme).checked = true;
  }
}

let theme = new Theme();

module.exports = {
  theme,
};