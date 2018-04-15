"use strict";

function updateServerStatusCallback(input, span){
  document.getElementById("idServerStatus").innerHTML = input;
}

function updateServerStatus(){
  submitGET({
    url: `${pathnames.calculator}?request=serverStatusJSON`,
    callback: updateServerStatusCallback,
    progress: "spanProgressReportGeneral"
  });
}