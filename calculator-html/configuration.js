"use strict";
let ids = require("./ids_dom_elements");

class Configuration {
  constructor() {
    /**@type{boolean} */
    this.disableStorage = true;
    if (document.getElementById(ids.domElements.calculatorStorageEnabled) !== null) {
      this.disableStorage = false;
    }
  }
}

let configuration = new Configuration();
module.exports = {
  configuration,
}