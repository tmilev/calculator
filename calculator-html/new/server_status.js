"use strict";
const submitRequests = require('./submit_requests');

function updateServerStatusCallback(input, span) {
  document.getElementById("idServerStatus").innerHTML = input;
}

function updateServerStatus() {
  submitRequests.submitGET({
    url: `${pathnames.calculatorAPI}?request=serverStatusJSON`,
    callback: updateServerStatusCallback,
    progress: "spanProgressReportGeneral"
  });
}

module.exports = {
  updateServerStatus
}