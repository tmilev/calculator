"use strict";
let ids = require("./ids_dom_elements");

class Configuration {
  constructor() {
    /**@type{boolean} */
    this.calculatorDefaultsEnabled = false;
    if (document.getElementById(ids.domElements.calculatorDefaultsEnabled) !== null) {
      this.calculatorDefaultsEnabled = true;
    }
  }
}

let configuration = new Configuration();
module.exports = {
  configuration,
}