"use strict";
const submitRequests = require("./submit_requests");
const pathnames = require("./pathnames");
const ids = require("./ids_dom_elements");

function updateServerStatus() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.serverStatusJSON}`,
    result: ids.domElements.serverStatus,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports = {
  updateServerStatus,
};