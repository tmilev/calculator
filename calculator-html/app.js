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

//initialize everything not initilized while loading:
window.calculator.mainPage.initializeCalculatorPage();
