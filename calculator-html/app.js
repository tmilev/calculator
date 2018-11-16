"use strict";
const mainPage = require('./page_navigation');

//declare globals:
window.calculator = {
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
  submitRequests: null,
  editPage: null,
  drawing: null,
  hideHtmlWithTags: null,
  accountActivation: null,
  manageAccounts: null,
  panels: null,
  database: null,
  mathJaxSetup: null,
  storageVariables: null,
  graphicsNDimension: null,
  account: null,
};

//load globals:
window.calculator.mainPage = new mainPage.Page();
window.calculator.login = require('./login');
window.calculator.signUp = require('./signup').signUp;
window.calculator.forgotLogin = require('./forgot_password').forgotLogin;
window.calculator.calculator = require('./calculator_page').calculator;
window.calculator.autocomplete = require('./autocomplete');
window.calculator.initializeButtons = require('./initialize_buttons');
window.calculator.selectCourse = require('./select_course');
window.calculator.coursePage = require('./course_page');
window.calculator.problemPage = require('./problem_page');
window.calculator.submitRequests = require('./submit_requests');
window.calculator.editPage = require('./edit_page');
window.calculator.drawing = require('./three-d').drawing;
window.calculator.hideHtmlWithTags = require('./hide_html_with_tags');
window.calculator.accountActivation = require('./account_activation');
window.calculator.manageAccounts = require('./manage_accounts');
window.calculator.account = require('./account');
window.calculator.panels = require('./panels');
window.calculator.database = require('./database');
window.calculator.mathJaxSetup = require('./mathjax-calculator-setup');
window.calculator.graphicsNDimension = require('./graphics_n_dimensions');
//initialize everything not initilized while loading:
window.calculator.mainPage.initializeCalculatorPage();
window.calculator.storageVariables = window.calculator.mainPage.storage.variables;
