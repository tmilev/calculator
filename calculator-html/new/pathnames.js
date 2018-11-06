"use strict";

var urls = {
  calculatorAPI: "/cgi-bin/calculator",
  appNoCache: "/appNoCache",
  appWithCache: "/app",
  app: "/app",
  request: "request",
};

var urlFields = {
  mainInput: "mainInput",
  fileName: "fileName",
  requestDatabase: "databaseJSON",
  requestEditPage: "editPageJSON", 
  requestModifyPage: "modifyPage",
  requestClonePage: "clonePageJSON",
  databaseTable: "currentDatabaseTable",
  setProblemData: "setProblemData",
}

module.exports = {
  urls,
  urlFields,
}