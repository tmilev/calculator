"use strict";
const mainPage = require("./page_navigation");

//declare globals:

function initializeGlobals() {
  let incomingGlobals = {
    mainPage: null,
    login: null,
    signUp: null,
    forgotLogin: null,
    calculator: null,
    autocomplete: null,
    initializeButtons: null,
    selectCourse: null,
    coursePage: null,
    problemPage: null,
    standAloneProblem: null,
    hardCodedProblems: null,
    submitRequests: null,
    editPage: null,
    drawing: null,
    miscellaneousFrontend: null,
    accountActivation: null,
    manageAccounts: null,
    panels: null,
    database: null,
    mathjax: null,
    storageVariables: null,
    graphicsNDimensions: null,
    account: null,
    jwt: null,
    processMonitoring: null,
    crypto: null,
    themes: null,
    pageNavigation: null,
    equationEditor: null,
    hardCodedServerAddress: "",
  };
  if (window.calculator === undefined || window.calculator === null) {
    window.calculator = {};
  }
  window.calculator = Object.assign(window.calculator, incomingGlobals);
  if (window.calculator.flagRunMainPage === undefined) {
    window.calculator.flagRunMainPage = true;
  }
}

function loadGlobals() {
  let calculator = window.calculator;
  calculator.login = require("./login");
  calculator.signUp = require("./signup").signUp;
  calculator.forgotLogin = require("./forgot_password").forgotLogin;
  calculator.calculator = require("./calculator_page").calculator;
  calculator.autocomplete = require("./autocomplete");
  calculator.initializeButtons = require("./initialize_buttons");
  calculator.selectCourse = require("./select_course");
  calculator.coursePage = require("./course_page");
  calculator.problemPage = require("./problem_page");
  calculator.submitRequests = require("./submit_requests");
  calculator.editPage = require("./edit_page");
  calculator.drawing = require("./three-d").drawing;
  calculator.miscellaneousFrontend = require("./miscellaneous_frontend");
  calculator.accountActivation = require("./account_activation");
  calculator.manageAccounts = require("./manage_accounts");
  calculator.hardCodedProblems = require("./hard_coded_problems");
  calculator.account = require("./account");
  calculator.panels = require("./panels");
  calculator.database = require("./database");
  calculator.themes = require("./themes");
  //calculator.mathjax = require("./mathjax-calculator-setup");
  calculator.graphicsNDimensions = require("./graphics_n_dimensions");
  calculator.processMonitoring = require("./process_monitoring");
  calculator.pageNavigation = require("./page_navigation");
  calculator.equationEditor = require("./equation_editor");
  //initialize everything not initilized while loading:
  calculator.crypto = require("./crypto");
  require("./external/build/output-min");
  calculator.mainPage = new mainPage.Page();
  if (calculator.flagRunMainPage) {
    calculator.mainPage.initializeCalculatorPage();
  }
}

initializeGlobals();
loadGlobals();