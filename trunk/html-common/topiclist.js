"use strict";


function getProblemPointsFromIndex(theIndex)
{ var currentItem=listTopics[theIndex];
}

function getProblemDeadlineFromIndex(theIndex, inputSection)
{ var currentItem=listTopics[theIndex];
  var result=  currentItem.deadlines[inputSection];
  return result;
}

function populateTopicList(inputSection)
{ //var theRows= document.getElementsByClassName("calculatorProblem");
  for (var i=0; i<listTopics.length; i++)
  { var currentElement= document.getElementById(listTopics[i].id);
    if (currentElement.cells!==undefined)
      currentElement.cells[3].innerHTML=getProblemDeadlineFromIndex(i,inputSection);
  }
}
