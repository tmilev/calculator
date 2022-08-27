"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");

function getAccountsTable(inputAccounts) {
  let result = "";
  result += "<table><tr><th>username</th><th>Email</th><th>Activated?</th><th>Course</th><th>Section</th><th>Semester</th></tr>";
  for (let counterAccounts = 0; counterAccounts < inputAccounts.length; counterAccounts++) {
    result += "<tr>";
    let currentUser = inputAccounts[counterAccounts];
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
  let outputComponentAdmin = document.getElementById("idOutputAdmins");
  let outputComponentStudents = document.getElementById("idOutputStudents");
  let parsedUserInfo = null;
  try {
    parsedUserInfo = miscellaneous.jsonUnescapeParse(input);
    if (pathnames.standardResponses.isNotLoggedInResponse(parsedUserInfo)) {
      outputComponentStudents.innerHTML = "<b style='color:red'>Not logged in</b>";
      return;
    }
    let admins = parsedUserInfo["admins"];
    let students = parsedUserInfo["students"];
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
  let spanEmailList = document.getElementById(idEmailList);
  let spanUserGroup = document.getElementById(idUserGroup);
  let spanPasswords = document.getElementById(idPasswords);
  let theURL = "";
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
  let theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.setTeacher}&`;
  let inputSections = document.getElementById('inputSections').value;
  let inputTeachers = document.getElementById('inputSetTeacher').value;
  let teachersAndSections = {};
  teachersAndSections[
    pathnames.urlFields.requests.teachers
  ] = encodeURIComponent(inputTeachers);
  teachersAndSections[
    pathnames.urlFields.requests.sections
  ] = encodeURIComponent(inputSections);
  theURL += `${pathnames.urlFields.teachersAndSections}=${encodeURIComponent(JSON.stringify(teachersAndSections))}&`;
  submitRequests.submitGET({
    url: theURL,
    progress: ids.domElements.spanProgressReportGeneral,
    result: "idOutputSections",
    callback: getTeachersStudentsCallback
  });

}

function updateAccountsPage() {
  let theURL = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.accountsJSON}`;

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