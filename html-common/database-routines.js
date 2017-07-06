var extraDatabaseRequestParameters;

function fetchDBentry(dbRow, dbColumn, idOutput)
{ var theMainInput="currentDatabaseRow=" + dbRow+"&currentDatabaseDesiredColumn="+dbColumn+"&";
  submitStringAsMainInput(extraDatabaseRequestParameters+theMainInput, idOutput, 'databaseOneEntry');
}

function modifyGetUI(dbRow, dbColumn, idOutput)
{ var theElement= document.getElementById(idOutput);
  var theModLink=theElement.previousSibling.previousSibling;
  var theContent=theElement.innerHTML;
  //var newElt = document.createElement('span');
  theModLink.innerHTML="<textarea>" + theContent +"</textArea><button onclick=\"" +
  "modifyDBentry('" + dbRow + "', '" + dbColumn + "', '" + idOutput + "')\">Modify</button>";
  //theModLink.parentElement.replaceChild(newElt, theModLink);
}

function modifyDBentry(dbRow, dbColumn, idOutput)
{ var theMainInput="currentDatabaseRow=" + dbRow+"&currentDatabaseDesiredColumn="+dbColumn+"&";
  submitStringAsMainInput(extraDatabaseRequestParameters+theMainInput, idOutput, 'databaseModifyEntry');
}
