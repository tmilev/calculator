"use strict";

function getProblemPointsFromIndex(theIndex)
{ var currentItem=listTopics[theIndex];
}

function getProblemDeadlineFromIndex(theIndex, inputSection)
{ var currentItem=listTopics[theIndex];
  var result = currentItem.deadlinesFormatted[inputSection];
  return result;
}

function populateTopicList(inputSection)
{ //var theRows= document.getElementsByClassName("calculatorProblem");
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
