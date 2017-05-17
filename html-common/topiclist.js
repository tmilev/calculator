"use strict";
var CurrentProblemRowElement=null;
var DesiredLinkStyle="sameWindow";
//var currentIframe=null;

function resizeIFrameToFitContent(iFrame)
{ var iFrameBody=iFrame.contentWindow.document.body;
  var iFrameHtml=iFrame.contentWindow.document.documentElement;
  var desiredHeight= Math.max(
    iFrameBody.scrollHeight, iFrameBody.offsetHeight,
    iFrameHtml.clientHeight, iFrameHtml.scrollHeight, iFrameHtml.offsetHeight );
  desiredHeight+=200;
  iFrame.height=desiredHeight;
}

function getProblemPointsFromIndex(theIndex)
{ var currentItem=listTopics[theIndex];
}

function getProblemDeadlineFromIndex(theIndex, inputSection)
{ var currentItem=listTopics[theIndex];
  var result = currentItem.deadlinesFormatted[inputSection];
  return result;
}

function accordionExpansionOfProblem(theAnchor)
{ var currentRow=theAnchor;
  while (currentRow.nodeName!=="TR" && currentRow.nodeName!=="tr" && currentRow!==null)
    currentRow=currentRow.parentElement;
  var currentTable=currentRow;
  while (currentTable.nodeName!=="TABLE" && currentTable.nodeName!=="table" && currentTable!==null)
    currentTable=currentTable.parentElement;
  if (currentRow===null || currentTable===null)
  { alert("Could not find containing table/row for element: " + theAnchor);
    return;
  }
  var newRow= currentTable.insertRow(currentRow.rowIndex+1);
  newRow.className="problemContainerRow";
  var newCell=newRow.insertCell(0);
  newCell.className="problemContainerCell";
  newCell.colSpan=4;
  newCell.theIframeId=theAnchor.href;
  newCell.innerHTML="<iframe onload=\"resizeIFrameToFitContent(this);\" id=\"" + newCell.theIframeId + "\" src=\""+ theAnchor.href +"\" "+
  "style=\"width:100%\"> It appears your web browser doesn't support iframes ...</iframe>";
  //currentIframe=document.getElementById(newCell.theIframeId);
  //theIframe.style.height=document.style.height/0.8;
  CurrentProblemRowElement=newRow;
}

function setProblemLinkStyle(desiredStyle)
{ var theLinks=document.getElementsByClassName("problemLink");
  DesiredLinkStyle=desiredStyle;
  for (var i=0; i<theLinks.length; i++)
  { if (desiredStyle==="newWindow")
    { theLinks[i].target="_blank";
    }
    if (desiredStyle==="sameWindow")
    { theLinks[i].target="";
    }
    if (desiredStyle==="accordion")
    { theLinks[i].onclick=
      function (event)
      { if (DesiredLinkStyle==="accordion")
        { event.preventDefault();
          accordionExpansionOfProblem(this);
        }
      };
    }
  }
}

function populateTopicList(inputSection)
{ //var theRows= document.getElementsByClassName("calculatorProblem");
  currentStudentSection=inputSection;
  for (var i=0; i<listTopics.length; i++)
  { var currentButton=document.getElementById(listTopics[i].idButton);
    currentButton.innerHTML=getProblemDeadlineFromIndex(i, inputSection);
  }
}

function updateDeadlines(idRowUpdated, idDeadlineTableUpdated)
{ var currentRow=document.getElementById(idRowUpdated);
  var startingElement=null;
  for (var i=0; i<listTopics.length; i++)
  { startingElement=listTopics[i];
    if (startingElement.id===idRowUpdated)
      break;
  }
  var currentDeadlineTable=document.getElementById(idDeadlineTableUpdated);
  for (i=1; i<currentDeadlineTable.rows.length-2; i++)
  { var groupCell=currentDeadlineTable.rows[i].cells[0];
    var deadlineCell=currentDeadlineTable.rows[i].cells[1];
    var currentGroup= groupCell.childNodes[0].textContent;
    var currentDeadline=deadlineCell.firstElementChild.value;
    startingElement.deadlinesFormatted[currentGroup]="Deadline: " +currentDeadline;
    startingElement.deadlines[currentGroup]=currentDeadline;
    startingElement.isInherited[currentGroup]=false;
    var processedTopics=[startingElement];
    for (var j=0; j< processedTopics.length; j++)
    { var currentTopicElement=processedTopics[j];
      for (var k=0; k<currentTopicElement.immediateChildren.length; k++)
        processedTopics.push(listTopics[currentTopicElement.immediateChildren[k]]);
      if (currentTopicElement.isInherited[currentGroup])
      { currentTopicElement.deadlinesFormatted[currentGroup]="Deadline: " +currentDeadline;
        if (currentGroup===currentStudentSection)
          document.getElementById(currentTopicElement.idButton).
            innerHTML="Inherited: " + currentDeadline;
      }
    }
  }
}

var StudentScoresAreBeingDisplayed=false;
function toggleStudentScores
(studentScoreLoadRep, scoresInCoursePage, studentScoresLoadReport)
{ if (!StudentScoresAreBeingDisplayed)
  { submitStringAsMainInput(currentStudentSection, studentScoreLoadRep,
    scoresInCoursePage, updateStudentScores, studentScoresLoadReport);
    StudentScoresAreBeingDisplayed=true;
    var theScores=document.getElementsByClassName('studentScoresContent');
    for (var i=0; i<theScores.length; i++)
    { //theScores[i].style.transition='opacity 1s linear';
      theScores[i].style.opacity='1';
    }
    var theButtons=document.getElementsByClassName('studentScoresButton');
    for (i=0; i<theButtons.length; i++)
      theButtons[i].innerHTML='Hide';
  } else
  { StudentScoresAreBeingDisplayed=false;
    theScores=document.getElementsByClassName('studentScoresContent');
    for (i=0; i<theScores.length; i++)
    { //theScores[i].style.transition='opacity 1s linear';
      theScores[i].style.opacity='0';
    }
    theButtons=document.getElementsByClassName('studentScoresButton');
    for (i=0; i<theButtons.length; i++)
    { theButtons[i].innerHTML='Scores';
    }
  }
}

var problemWeightsVisible=false;

function toggleProblemWeights
(studentScoreLoadRep, scoresInCoursePage, studentScoresLoadReport)
{ var theWeights=document.getElementsByClassName('panelProblemWeights');
  if (!problemWeightsVisible)
  { for (var i=0; i<theWeights.length; i++)
    { //theScores[i].style.transition='opacity 1s linear';
      theWeights[i].style.opacity='1';
      theWeights[i].style.maxHeight='200px';
    }
  } else
  { for (i=0; i<theWeights.length; i++)
    { //theScores[i].style.transition='opacity 1s linear';
      theWeights[i].style.opacity='0';
      theWeights[i].style.maxHeight='0';
    }
  }
  problemWeightsVisible=!problemWeightsVisible;
}

function updateStudentScores(idElement)
{ onLoadDefaultFunction(idElement);
  for (var i=0; i<studentScoresInHomePage.length; i++)
  { var currentElt=document.getElementById(studentScoresInHomePage[i].theId);
    if (currentElt===null || currentElt===undefined)
      continue;
    currentElt.innerHTML=
    "<b><span style=\"color:green\">" +
    studentScoresInHomePage[i].numSolvedAll+
    " solved all; </span> <span style=\"color:orange\">" +
    studentScoresInHomePage[i].numSolvedPart+
    " solved some; </span>"+
    "<span style=\"color:Red\"> " +studentScoresInHomePage[i].numSolvedNone+
    " solved none.</span></b>";
    if (!studentScoresInHomePage[i].weightsOK)
      currentElt.innerHTML+= " Not all problems have weights. ";
  }
}

