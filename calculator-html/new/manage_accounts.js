"use strict";
const submitRequests = require('./submit_requests');

function getAccountsTable(inputAccounts) {
  var result = "";
  result += "<table><tr><th>username</th><th>Email</th><th>Activated?</th><th>Course</th><th>Section</th><th>Semester</th></tr>";
  for (var counterAccounts = 0; counterAccounts < inputAccounts.length; counterAccounts ++) {
    result += "<tr>";
    var currentUser = inputAccounts[counterAccounts];
    //console.log("Current user: " + JSON.stringify(currentUser));
    result += `<td>${currentUser.username}</td>`;
    if (currentUser.email !== undefined) {
      result += `<td>${currentUser.email}</td>`;
    } else {
      result += `<td>-</td>`;
    }
    if (currentUser.activationToken === "activated") {
      result += `<td><span style = 'color:green'>${currentUser.activationToken}</span></td>`;
    } else {
      result += "<td><span style = 'color:red'>Not activated</span></td>";
    }
    if (currentUser.currentCourses !== undefined) {
      result += `<td>${currentUser.currentCourses}</td>`;
    } else {
      result += "<td>-</td>";
    }
    if (currentUser.studentSection !== undefined) {
      result += `<td>${currentUser.studentSection}</td>`;
    } else {
      result += "<td>-</td>";
    }
    if (currentUser.semester !== undefined) {
      result += `<td>${currentUser.semester}</td>`;
    } else {
      result += "<td>-</td>";
    }
    result += "</tr>";
  }
  result += "</table>";
  return result;
}

function updateAccountsPageCallback(input, notUsed) {
  var outputComponentAdmin = document.getElementById("idOutputAdmins");
  var outputComponentStudents = document.getElementById("idOutputStudents");
  var parsedUserInfo = null;
  try {
    parsedUserInfo = JSON.parse(input);
    var admins = parsedUserInfo["admins"];
    var students = parsedUserInfo["students"];
    outputComponentAdmin.innerHTML = getAccountsTable(admins);
    outputComponentStudents.innerHTML = getAccountsTable(students);
  } catch (e) {
    outputComponentStudents.innerHTML = e;
    console.log(e);
  }
}

function callbackAddEmailsOrUsers(input, outputComponent) {
  if (typeof outputComponent == "string") {
    outputComponent = document.getElementById(outputComponent);
  }
  outputComponent.innerHTML = decodeURIComponent(input);
}

function addEmailsOrUsers(
  idEmailList, problemCollectionName, 
  idOutput, userRole, idUserGroup, 
  idPasswords, requestType
) {
  var spanOutput = document.getElementById(idOutput);
  var spanEmailList = document.getElementById(idEmailList);
  var spanUserGroup = document.getElementById(idUserGroup);
  var spanPasswords = document.getElementById(idPasswords);
  var theURL = "";
  theURL += `${pathnames.calculatorAPI}?`;
  theURL += `request=${requestType}&`;
  theURL += `userRole=${userRole}&`;
  theURL += `userList=${encodeURIComponent(spanEmailList.value)}&`;
  theURL += `studentSection=${encodeURIComponent(spanUserGroup.value)}&`;
  theURL += `passwordList=${encodeURIComponent(spanPasswords.value)}&`;
  theURL += `filterAccounts=&`;
  submitRequests.submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    result: idOutput,
    callback: callbackAddEmailsOrUsers
  });
}

function getTeachersStudentsCallback(input, output) {
  document.getElementById(output).innerHTML = input; 
}

function getTeachersStudents() { 
  var theURL = `${pathnames.calculatorAPI}?request=setTeacher&`;
  var inputSections = document.getElementById('inputSections').value;
  var inputTeachers = document.getElementById('inputSetTeacher').value;
  var teachersAndSections = {
    teachers: encodeURIComponent(inputTeachers),
    students: encodeURIComponent(inputSections)
  }
  theURL += `teachersAndSections=${encodeURIComponent(JSON.stringify(teachersAndSections))}&`;
  submitRequests.submitGET({
    url: theURL,
    progress: "spanProgressReportGeneral",
    result: "idOutputSections",
    callback: getTeachersStudentsCallback
  });

}

function updateAccountsPage() {
  var theURL = `${pathnames.calculatorAPI}?request=accountsJSON`;

  submitRequests.submitGET({
    url: theURL,
    callback: updateAccountsPageCallback,
    progress: "spanProgressReportGeneral"
  });

}

module.exports = {
  updateAccountsPage
}