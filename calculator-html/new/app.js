"use strict";
const mainPage = require('./page_navigation');

window.calculator = {
  mainPage: new mainPage.Page(),
};

window.calculator.mainPage.initializeCalculatorPage();
