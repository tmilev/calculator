"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');
const ids = require('./ids_dom_elements'); 

function updateServerStatusCallback(input, span) {
  document.getElementById("idServerStatus").innerHTML = input;
}

function updateServerStatus() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?request=serverStatusJSON`,
    callback: updateServerStatusCallback,
    progress: ids.domElements.spanProgressReportGeneral
  });
}

module.exports = {
  updateServerStatus
}