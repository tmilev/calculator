var CurrentProblemLinkRow=null;
var CurrentProblemTableElement=null;
var CurrentProblemIframeRow=null;
var CurrentProblemRowNumber=-1;

var CurrentVideoLinkRow=null;
var CurrentVideoTableElement=null;
var CurrentVideoIframeRow=null;
var CurrentVideoRowNumber=-1;

var DesiredLinkStyle="sameWindow";

function accordionExpansionOfVideo(theAnchor)
{ if (CurrentVideoIframeRow!==null && CurrentVideoRowNumber!==-1 &&
      CurrentVideoTableElement!==null)
  { if (CurrentVideoTableElement===CurrentProblemTableElement &&
        CurrentProblemRowNumber>CurrentVideoRowNumber)
      CurrentProblemRowNumber--;
    CurrentVideoTableElement.deleteRow(CurrentVideoRowNumber);
    CurrentVideoTableElement=null;
    CurrentVideoIframeRow=null;
    CurrentVideoRowNumber=-1;
  }
  CurrentVideoLinkRow=theAnchor;
  while (CurrentVideoLinkRow.nodeName!=="TR" &&
         CurrentVideoLinkRow.nodeName!=="tr" &&
         CurrentVideoLinkRow!==null)
    CurrentVideoLinkRow=CurrentVideoLinkRow.parentElement;
  CurrentVideoTableElement=CurrentVideoLinkRow;
  while (CurrentVideoTableElement.nodeName!=="TABLE" &&
         CurrentVideoTableElement.nodeName!=="table" &&
         CurrentVideoTableElement!==null)
    CurrentVideoTableElement=CurrentVideoTableElement.parentElement;
  if (CurrentVideoLinkRow===null || CurrentVideoTableElement===null)
  { alert("Could not find containing table/row for element: " + theAnchor);
    return;
  }
  CurrentVideoRowNumber=CurrentVideoLinkRow.rowIndex+1;
  if (CurrentVideoTableElement===CurrentProblemTableElement &&
      CurrentProblemRowNumber>=CurrentVideoRowNumber)
    CurrentProblemRowNumber++;
  CurrentVideoIframeRow= CurrentVideoTableElement.insertRow(CurrentVideoRowNumber);
  CurrentVideoIframeRow.className="videoContainerRow";
  var newCell=CurrentVideoIframeRow.insertCell(0);
  newCell.className="videoContainerRow";
  newCell.colSpan=5;
  newCell.theIframeId=theAnchor.href;
  var newHref=theAnchor.href;
  if (newHref.startsWith("https://youtu.be/"))
    newHref="https://www.youtube.com/embed/"+newHref.substr(17);

  newCell.innerHTML=
    "<div class=\"CurrentVideoIframeContainer\"> <iframe width=\"560\" height=\"315\" onload=\"\" id=\"" +
    newCell.theIframeId + "\" src=\""+
    newHref +
    "\" "+
    "frameborder=\"0\" allowfullscreen> It appears your web browser doesn't support iframes ...</iframe></div>";
}

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
  newCell.colSpan=5;
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
  var theVideoLinks=document.getElementsByClassName("videoLink");
  for (i=0; i<theVideoLinks.length; i++)
    theVideoLinks[i].onclick=
      function (event)
      { event.preventDefault();
        accordionExpansionOfVideo(this);
      };
  addCookie("problemLinkStyle", desiredStyle, 100);
}

function resizeIFrameToFitContent(iFrame)
{ var iFrameBody=iFrame.contentWindow.document.body;
  var iFrameHtml=iFrame.contentWindow.document.documentElement;
  var desiredHeight= Math.max(
    iFrameBody.scrollHeight, iFrameBody.offsetHeight,
    iFrameHtml.clientHeight, iFrameHtml.scrollHeight, iFrameHtml.offsetHeight );
  desiredHeight=Math.min(desiredHeight, 600);
  desiredHeight+=200;
  iFrame.height=desiredHeight;
}
