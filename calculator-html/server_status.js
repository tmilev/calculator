"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements'); 

function updateServerStatusCallback(input, span) {
  var inputParsed = JSON.parse(input);
  document.getElementById("idServerStatus").innerHTML = inputParsed.result;
}

function updateServerStatus() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.serverStatusJSON}`,
    callback: updateServerStatusCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports = {
  updateServerStatus
};