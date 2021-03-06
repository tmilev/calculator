"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");

function getAccountsTable(inputAccounts) {
  var result = "";
  result += "<table><tr><th>username</th><th>Email</th><th>Activated?</th><th>Course</th><th>Section</th><th>Semester</th></tr>";
  for (var counterAccounts = 0; counterAccounts < inputAccounts.length; counterAccounts++) {
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
    parsedUserInfo = miscellaneous.jsonUnescapeParse(input);
    var admins = parsedUserInfo["admins"];
    var students = parsedUserInfo["students"];
    if (parsedUserInfo.error !== undefined && parsedUserInfo.error !== "") {
      outputComponentAdmin.innerHTML = parsedUserInfo.error;
    } else {
      outputComponentAdmin.innerHTML = getAccountsTable(admins);
      outputComponentStudents.innerHTML = getAccountsTable(students);
    }
  } catch (e) {
    outputComponentStudents.innerHTML = e;
    console.log(e);
  }
}

function callbackAddEmailsOrUsers(input, outputComponent) {
  if (typeof outputComponent == "string") {
    outputComponent = document.getElementById(outputComponent);
  }
  outputComponent.innerHTML = miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input));
}

function addEmailsOrUsers(
  idEmailList,
  problemCollectionName,
  idOutput,
  userRole,
  idUserGroup,
  idPasswords,
  requestType
) {
  var spanEmailList = document.getElementById(idEmailList);
  var spanUserGroup = document.getElementById(idUserGroup);
  var spanPasswords = document.getElementById(idPasswords);
  var theURL = "";
  theURL += `${pathnames.urls.calculatorAPI}?`;
  theURL += `${pathnames.urlFields.request}=${requestType}&`;
  theURL += `${pathnames.urlFields.userRole}=${userRole}&`;
  theURL += `userList=${encodeURIComponent(spanEmailList.value)}&`;
  theURL += `studentSection=${encodeURIComponent(spanUserGroup.value)}&`;
  theURL += `passwordList=${encodeURIComponent(spanPasswords.value)}&`;
  theURL += `filterAccounts=&`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    result: idOutput,
    callback: callbackAddEmailsOrUsers
  });
}

function getTeachersStudentsCallback(input, output) {
  document.getElementById(output).innerHTML = miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input));
}

function getTeachersStudents() {
  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.setTeacher}&`;
  var inputSections = document.getElementById('inputSections').value;
  var inputTeachers = document.getElementById('inputSetTeacher').value;
  var teachersAndSections = {
    teachers: encodeURIComponent(inputTeachers),
    students: encodeURIComponent(inputSections)
  }
  theURL += `${pathnames.urlFields.teachersAndSections}=${encodeURIComponent(JSON.stringify(teachersAndSections))}&`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    result: "idOutputSections",
    callback: getTeachersStudentsCallback
  });

}

function updateAccountsPage() {
  var theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.accountsJSON}`;

  submitRequests.submitGET({
    url: theURL,
    callback: updateAccountsPageCallback,
    progress: ids.domElements.spanProgressReportGeneral,
  });

}

module.exports = {
  addEmailsOrUsers,
  updateAccountsPage,
  getTeachersStudents
};