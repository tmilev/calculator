var extraDatabaseRequestParameters;

function fetchDBentry(dbRow, dbColumn, idOutput)
{ var theMainInput="currentDatabaseRow=" + dbRow+"&currentDatabaseDesiredColumn="+dbColumn+"&";
  submitStringAsMainInput(extraDatabaseRequestParameters+theMainInput, idOutput, 'databaseOneEntry');
}

function modifyGetUI(dbRow, dbColumn, idOutput)
{ var theElement= document.getElementById(idOutput);
  var theModLink=theElement.previousSibling.previousSibling;
  var theContent=theElement.innerHTML;
  var textAreaId= "textAreaId"+dbRow+dbColumn;
  var newElt = document.createElement('span');
  newElt.innerHTML="<textarea id='"+ textAreaId +"'>" + theContent +"</textArea><button onclick=\"" +
  "modifyDBentry('" + dbRow + "', '" + dbColumn + "', '" + idOutput + "','" + textAreaId + "' )\">Modify</button>";
  theModLink.parentElement.replaceChild(newElt, theModLink);
}

function modifyDBentry(dbRow, dbColumn, idOutput, textAreaId)
{ var theMainInput=extraDatabaseRequestParameters;
  theMainInput+="currentDatabaseRow=" + dbRow+"&currentDatabaseDesiredColumn="+dbColumn+"&";
  theMainInput+= "desiredContent="+encodeURIComponent(document.getElementById(textAreaId).value) + "&";
  submitStringAsMainInput(theMainInput, idOutput, 'databaseModifyEntry');
}
