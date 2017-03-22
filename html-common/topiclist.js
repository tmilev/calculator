"use strict";

function getProblemPointsFromIndex(theIndex)
{ var currentItem=listTopics[theIndex];
}

function getProblemDeadlineFromIndex(theIndex, inputSection)
{ var currentItem=listTopics[theIndex];
  var result = currentItem.deadlinesFormatted[inputSection];
  return result;
}

var currentStudentSection=null;

function populateTopicList(inputSection)
{ //var theRows= document.getElementsByClassName("calculatorProblem");
  currentStudentSection=inputSection;
  for (var i=0; i<listTopics.length; i++)
  { var currentElement= document.getElementById(listTopics[i].id);
    if (currentElement.cells!==undefined)
    { var currentDeadlineButton=currentElement.cells[3].childNodes[0];
      if (currentDeadlineButton.className!=="accordion")
        currentDeadlineButton=currentElement.cells[3].childNodes[1];
      currentDeadlineButton.innerHTML=getProblemDeadlineFromIndex(i, inputSection);
    }
  }
}

function updateDeadlines(idRowUpdated, idDeadlineTableUpdated)
{ var currentRow=document.getElementById(idRowUpdated);
  var currentTopicElement=null;
  for (var i=0; i<listTopics.length; i++)
  { currentTopicElement=listTopics[i];
    if (currentTopicElement.id===idRowUpdated)
      break;
  }
  var currentDeadlineTable=document.getElementById(idDeadlineTableUpdated);
  for (i=1; i<currentDeadlineTable.rows.length-2; i++)
  { var groupCell=currentDeadlineTable.rows[i].cells[0];
    var deadlineCell=currentDeadlineTable.rows[i].cells[1];
    var currentGroup= groupCell.childNodes[0].textContent;
    var currentDeadline=deadlineCell.firstElementChild.value;
    currentTopicElement.deadlinesFormatted[currentGroup]="Deadline: " +currentDeadline;
    currentTopicElement.deadlines[currentGroup]=currentDeadline;
  }
}

var StudentScoresAreBeingDisplayed=false;
function toggleStudentScores
(studentScoreLoadRep, scoresInCoursePage, studentScoresLoadReport)
{ if (!StudentScoresAreBeingDisplayed)
  { submitStringAsMainInput('', studentScoreLoadRep,
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

