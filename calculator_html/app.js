"use strict";

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
    lieAlgebras: null,
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

// Loads all modules using require statements and 
// initializes the calculator page or web worker.
function loadGlobals() {
  let calculator = window.calculator;
  calculator.login = require("./login");
  calculator.lieAlgebras = require("./lie_algebras");
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
  calculator.drawing = require("./graphics").drawing;
  calculator.miscellaneousFrontend = require("./miscellaneous_frontend");
  calculator.accountActivation = require("./account_activation");
  calculator.manageAccounts = require("./manage_accounts");
  calculator.hardCodedProblems = require("./hard_coded_problems");
  calculator.account = require("./account");
  calculator.panels = require("./panels");
  calculator.database = require("./database");
  calculator.themes = require("./themes");
  calculator.graphicsNDimensions = require("./graphics_n_dimensions");
  calculator.dynamicJavascript = require("./dynamic_javascript");
  calculator.processMonitoring = require("./process_monitoring");
  calculator.equationEditor = require("./equation_editor/src/equation_editor");
  calculator.crypto = require("./crypto");
  // External dependencies:
  require("./external/build/output-min");
  calculator.pageNavigation = require("./page_navigation");
}

function initializeAndLoad() {
  initializeGlobals();
  loadGlobals();
}

function run() {
  document.onreadystatechange = () => {
    if (document.readyState !== 'complete') {
      return;
    }
    initializeAndLoad();
  }
}

run();


