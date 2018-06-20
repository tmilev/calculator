"use strict";

function updateServerStatusCallback(input, span) {
  document.getElementById("idServerStatus").innerHTML = input;
}

function updateServerStatus() {
  submitGET({
    url: `${pathnames.calculatorAPI}?request=serverStatusJSON`,
    callback: updateServerStatusCallback,
    progress: "spanProgressReportGeneral"
  });
}