"use strict";
const mainPage = require('./page_navigation');

//declare globals:

function initializeGlobals() {
  var incomingGlobals = {
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
    globalMQ: MathQuill.getInterface(2),
    equationEditor: null,
    MathQuillHasMatrixSupport: true,
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
  window.calculator.login = require('./login');
  window.calculator.signUp = require('./signup').signUp;
  window.calculator.forgotLogin = require('./forgot_password').forgotLogin;
  window.calculator.calculator = require('./calculator_page').calculator;
  window.calculator.autocomplete = require('./autocomplete');
  window.calculator.initializeButtons = require('./initialize_buttons');
  window.calculator.selectCourse = require('./select_course');
  window.calculator.coursePage = require('./course_page');
  window.calculator.problemPage = require('./problem_page');
  window.calculator.standAloneProblem = require('./stand_alone_problem');
  window.calculator.submitRequests = require("./submit_requests");
  window.calculator.editPage = require("./edit_page");
  window.calculator.drawing = require('./three-d').drawing;
  window.calculator.miscellaneousFrontend = require('./miscellaneous_frontend');
  window.calculator.accountActivation = require('./account_activation');
  window.calculator.manageAccounts = require('./manage_accounts');
  window.calculator.account = require('./account');
  window.calculator.panels = require('./panels');
  window.calculator.database = require('./database');
  window.calculator.themes = require('./themes');
  // window.calculator.mathjax = require('./mathjax-calculator-setup');
  window.calculator.graphicsNDimensions = require('./graphics_n_dimensions');
  window.calculator.processMonitoring = require('./process_monitoring');
  window.calculator.pageNavigation = require('./page_navigation');
  window.calculator.equationEditor = require('./equation_editor');
  //initialize everything not initilized while loading:
  window.calculator.crypto = require('./crypto');
  require('./external/build/output-min');
  window.calculator.mainPage = new mainPage.Page();
  if (window.calculator.flagRunMainPage) {
    window.calculator.mainPage.initializeCalculatorPage();
  }
}

initializeGlobals();
loadGlobals();