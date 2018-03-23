"use strict";
var minLengthAutocompleteTrigger = 2;
var DebugCounter = 0;
var AutocompleteIsInitialized = false;

var theAutocompleteDictionaryLowerCase;
var theAutocompleteDictionaryByKey;
var theText;
var theSuggestions;
var theSuggestionsHighlighted;
var theAutoComplete;
var theAutoCompleteHints;
var indexInAutocomplete;
var autocompleteDebugWindow;
var theLastWord;
var oldLastWord;
var lastWordStart;
var ignoreOninput;

function initializeAutocomplete()
{ if (AutocompleteIsInitialized)
    return true;
  if (typeof( theAutocompleteDictionary) == 'undefined')
    return false;
  AutocompleteIsInitialized = true;
  theAutocompleteDictionaryLowerCase = new Array;
  theAutocompleteDictionaryByKey = new Array;
  theText = document.getElementById('mainInputID');
  theSuggestions= new Array;
  theSuggestionsHighlighted= new Array;
  theAutoComplete=document.getElementById('idAutocompleteSpan');
  theAutoCompleteHints=document.getElementById('idAutocompleteHints');
  indexInAutocomplete=-1;
  autocompleteDebugWindow=document.getElementById('idAutocompleteDebug');
  theLastWord = new String;
  ignoreOninput=false;
  DebugCounter=0;
  //theAutoCompleteHints.style.visibility="hidden";
  for (var i=0; i<theAutocompleteDictionary.length; i++)
    theAutocompleteDictionaryLowerCase.push(theAutocompleteDictionary[i].toLowerCase());
  for (i=0; i<theAutocompleteDictionary.length; i++)
    theAutocompleteDictionaryByKey[theAutocompleteDictionary[i]]=1;
  return true;
}

//displayDictionary();
function displayDictionary()
{ var result;
  result="";
  counter=0;
  for (var i in theAutocompleteDictionaryByKey)
  { result+=i+" "+ theAutocompleteDictionaryByKey[i]+ " <br />";
    counter++;
    if (counter>= 15)
    { result+="... displaying first 15 out of a total of "+ Object.keys(theAutocompleteDictionaryByKey).length + " words in the dictionary.";
      break;
    }
  }
  autocompleteDebugWindow.innerHTML=result;
}

function isSeparator(theChar)
{ switch(theChar)
  { case '+':
    case ' ':
    case ',':
    case '(':
    case ')':
    case '%':
    case '\n':
    case '\r':
    case '\"':
    case '=':
      return true;
    default: return false;
  }
}

function getLastWord()
{ var theLastWordReversed;
  theLastWordReversed="";
  var cursorPosition=theText.selectionEnd;
  lastWordStart=-1;
  for (var i=cursorPosition-1; i>=0; i--)
  { if (isSeparator(theText.value[i]))
      break;
    theLastWordReversed+=(theText.value[i]);
  }
  theLastWord="";
  lastWordStart=i;
  for (i=theLastWordReversed.length-1; i>=0; i--)
    theLastWord+=theLastWordReversed[i];
  for (i=cursorPosition; i<theText.value.length; i++)
    if (isSeparator(theText.value[i]))
      break;
    else
      theLastWord+=theText.value[i];
  DebugCounter++;
//  autocompleteDebugWindow.innerHTML=theLastWord+ "<br>cursorPosition="+ cursorPosition+ "<br>theText.selectionEnd=" +theText.selectionEnd+  "theText.selectionStart="+theText.selectionStart+ "<br>DebugCounter="+DebugCounter;
}

function clearAutocompleteTab()
{ theAutoComplete.innerHTML="";
//  theAutoCompleteHints.style.visibility="hidden";  
  indexInAutocomplete=-1;
  theSuggestions.length=0;
  theSuggestionsHighlighted.length=0;
}

function suggestWord()
{ if (!initializeAutocomplete())
    return;
  getLastWord();
  if (oldLastWord===theLastWord)
    return;
  oldLastWord=theLastWord;
  theSuggestions.length=0;
  theSuggestionsHighlighted.length=0;
  if (theLastWord=="")
  { clearAutocompleteTab();
    return;
  }
  if (theLastWord.length< minLengthAutocompleteTrigger)
  { clearAutocompleteTab();
    return;
  }  
  for (var i=0; i<theAutocompleteDictionary.length; i++)
  { if (theLastWord==theAutocompleteDictionary[i])
    { clearAutocompleteTab();
      return;
    }
    var lastWordLowerCase=theLastWord.toLowerCase();
    var startpos=theAutocompleteDictionaryLowerCase[i].indexOf(lastWordLowerCase);
    if (startpos===-1)
      continue;
    var currentString=theAutocompleteDictionary[i].substr(0, startpos);
    currentString+="<b>" + theLastWord+ "</b>"+ theAutocompleteDictionary[i].substr(startpos+theLastWord.length);
    theSuggestions.push(theAutocompleteDictionary[i]);
    theSuggestionsHighlighted.push(currentString);
  }      
  if (theSuggestions.length > 0)
    theAutoCompleteHints.style.visibility = "visible";
  indexInAutocomplete = 0;
  displaySuggestions();
}

function displaySuggestions()
{ theAutoComplete.innerHTML="";
  if (theSuggestionsHighlighted.length>0)
  { if (indexInAutocomplete<0) 
      indexInAutocomplete=theSuggestionsHighlighted.length-1;
    if (indexInAutocomplete>=theSuggestionsHighlighted.length)
      indexInAutocomplete=0;
  }
  for (var i=0; i<theSuggestionsHighlighted.length; i++)
    if (i=== indexInAutocomplete)
      theAutoComplete.innerHTML+="<span style='background:silver'>"+theSuggestionsHighlighted[i]+"</span><br>";
    else  
      theAutoComplete.innerHTML+=theSuggestionsHighlighted[i] + "<br>";
  //theAutoComplete.innerHTML+="theindex: " + indexInAutocomplete;
}

function replaceLastWord()
{ if(theSuggestions.length<=0)
    return;
  if (indexInAutocomplete<0 || indexInAutocomplete>=theSuggestions.length)
    return;
  theText.value=theText.value.substring(0, lastWordStart+1)+ theSuggestions[indexInAutocomplete]+
  theText.value.substring(1+lastWordStart+theLastWord.length);
  setCursorPosition(lastWordStart+theSuggestions[indexInAutocomplete].length+1);
}

function arrowAction(event)
{ if (event.ctrlKey!==true)
    return;
  if (event.keyCode===32)//32-space bar, 13-enter key, 9-tab key
    replaceLastWord();
  if (event.keyCode==38)//up arrow
    indexInAutocomplete--;
  else if (event.keyCode==40)//down arrow
    indexInAutocomplete++;
  else 
    return;
  displaySuggestions();
//  event.stopPropagation();
  event.preventDefault();
//  event.cancelBubble = true;
}

function accountWordAtCursor(amount)
{ getLastWord();
  if (theLastWord==="" || theLastWord===null)
    return;
  theAutoComplete.textContent=theLastWord;
  if (typeof(theAutocompleteDictionary[theLastWord])!="number")
    theAutocompleteDictionary[theLastWord]=amount;
  else
    theAutocompleteDictionary[theLastWord]+=amount;
  displayDictionary();
}

function setCursorPosition(caretPos) 
{ if(theText.createTextRange) 
  { var range = theText.createTextRange();
    range.move('character', caretPos);
    range.select();
  } else 
  { if(theText.selectionStart) 
    { theText.focus();
      theText.setSelectionRange(caretPos, caretPos);
    } else
      theText.focus();
  }
}
