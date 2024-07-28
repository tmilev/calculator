"use strict";
const ids = require("./ids_dom_elements");
const storage = require("./storage");

class Theme {
  constructor() {
    let themeIds = ids.domElements.themeRadioButtons;
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
    this.buttonRadioThemeDefault =
      document.getElementById(
        ids.domElements.pages.themes.radioThemeDefault
      );
    this.buttonRadioThemeExperimental = document.getElementById(
      ids.domElements.pages.themes.radioThemeExperimental
    )
    if (this.buttonRadioThemeDefault === null) {
      // Theme buttons not found.
      return;
    }
    this.buttonRadioThemeDefault.addEventListener("click", () => {
      this.setFromRadioButton();
    });
    this.buttonRadioThemeExperimental.addEventListener("click", () => {
      this.setFromRadioButton();
    });
  }

  initializeThemes(themeId) {
    let currentElement = document.getElementById(themeId);
    if (currentElement !== null) {
      currentElement.checked = true;
    }
    this.setFromRadioButton();
  }

  setFromRadioButton() {
    let value = null;
    for (let label in ids.domElements.themeRadioButtons) {
      let currentId = ids.domElements.themeRadioButtons[label];
      let currentElement = document.getElementById(currentId);
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
    storage.storage.variables.theme.setAndStore(value);
    this.currentTheme = value;
    let currentSpec = this.themes[this.currentTheme];
    let root = document.documentElement;
    for (let label in currentSpec) {
      root.style.setProperty(label, currentSpec[label]);
    }
  }
}

let theme = new Theme();

module.exports = {
  theme,
};