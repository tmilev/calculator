var CurrentProblemLinkRow=null;
var CurrentProblemTableElement=null;
var CurrentProblemIframeRow=null;
var CurrentProblemRowNumber=-1;
var DesiredLinkStyle="sameWindow";

function accordionExpansionOfProblem(theAnchor)
{ if (CurrentProblemIframeRow!==null && CurrentProblemRowNumber!==-1 &&
      CurrentProblemTableElement!==null)
  { CurrentProblemTableElement.deleteRow(CurrentProblemRowNumber);
    CurrentProblemTableElement=null;
    CurrentProblemIframeRow=null;
    CurrentProblemRowNumber=-1;
  }
  CurrentProblemLinkRow=theAnchor;
  while (CurrentProblemLinkRow.nodeName!=="TR" &&
         CurrentProblemLinkRow.nodeName!=="tr" &&
         CurrentProblemLinkRow!==null)
    CurrentProblemLinkRow=CurrentProblemLinkRow.parentElement;
  CurrentProblemTableElement=CurrentProblemLinkRow;
  while (CurrentProblemTableElement.nodeName!=="TABLE" &&
         CurrentProblemTableElement.nodeName!=="table" &&
         CurrentProblemTableElement!==null)
    CurrentProblemTableElement=CurrentProblemTableElement.parentElement;
  if (CurrentProblemLinkRow===null || CurrentProblemTableElement===null)
  { alert("Could not find containing table/row for element: " + theAnchor);
    return;
  }
  CurrentProblemRowNumber=CurrentProblemLinkRow.rowIndex+1;
  CurrentProblemIframeRow= CurrentProblemTableElement.insertRow(CurrentProblemRowNumber);
  CurrentProblemIframeRow.className="problemContainerRow";
  var newCell=CurrentProblemIframeRow.insertCell(0);
  newCell.className="problemContainerCell";
  newCell.colSpan=4;
  newCell.theIframeId=theAnchor.href;
  newCell.innerHTML=
    "<div class=\"CurrentProblemIframeContainer\"> <iframe style=\"background:white; width:100%\" onload=\"resizeIFrameToFitContent(this);\" id=\"" +
    newCell.theIframeId + "\" src=\""+
    theAnchor.href + "&navigationBar=false"+
    "\" "+
    "> It appears your web browser doesn't support iframes ...</iframe></div>";
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

function resizeIFrameToFitContent(iFrame)
{ var iFrameBody=iFrame.contentWindow.document.body;
  var iFrameHtml=iFrame.contentWindow.document.documentElement;
  var desiredHeight= Math.max(
    iFrameBody.scrollHeight, iFrameBody.offsetHeight,
    iFrameHtml.clientHeight, iFrameHtml.scrollHeight, iFrameHtml.offsetHeight );
  desiredHeight+=200;
  iFrame.height=desiredHeight;
}
