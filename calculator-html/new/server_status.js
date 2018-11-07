"use strict";
const submitRequests = require('./submit_requests');
const pathnames = require('./pathnames');

function updateServerStatusCallback(input, span) {
  document.getElementById("idServerStatus").innerHTML = input;
}

function updateServerStatus() {
  submitRequests.submitGET({
    url: `${pathnames.urls.calculatorAPI}?request=serverStatusJSON`,
    callback: updateServerStatusCallback,
    progress: "spanProgressReportGeneral"
  });
}

module.exports = {
  updateServerStatus
}