"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");

class ManageAccountsPage {
  initialize() {
    document.getElementById(
      ids.domElements.pages.manageAccounts.buttonSetTeacher
    ).addEventListener('click', () => {
      getTeachersStudents();
    });
    document.getElementById(
      ids.domElements.pages.manageAccounts.buttonAddUsersTeachers
    ).addEventListener('click', () => {
      addEmailsOrUsers(
        'inputAddUsersadmin',
        '',
        'idOutputAdmins',
        'admin',
        'inputAddExtraInfoAdmin',
        'inputAddDefaultPasswordsAdmin',
        'addUsers'
      );
    });
    document.getElementById(
      ids.domElements.pages.manageAccounts.buttonAddUsersStudent
    ).addEventListener('click', () => {
      addEmailsOrUsers(
        'inputAddUsersStudent',
        '',
        'idOutputStudents',
        'student',
        'inputAddExtraInfostudent',
        'inputAddDefaultPasswordsStudent',
        'addUsers'
      );
    });    
  }
}

function getAccountsTable(inputAccounts) {
  let result = "";
  result += "<table><tr>";
  result += "<th>username</th>";
  result += "<th>Email</th>";
  result += "<th>Activated?</th>";
  result += "<th>Course</th>";
  result += "<th>Section</th>";
  result += "<th>Semester</th>";
  result += "</tr>";
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
      miscellaneous.writeHTML(
        outputComponentStudents,
        "<b style='color:red'>Not logged in</b>",
      );
      return;
    }
    let admins = parsedUserInfo["admins"];
    let students = parsedUserInfo["students"];
    if (
      parsedUserInfo.error !== undefined &&
      parsedUserInfo.error !== ""
    ) {
      miscellaneous.writeHTML(outputComponentAdmin, parsedUserInfo.error);
    } else {
      miscellaneous.writeHTML(outputComponentAdmin, getAccountsTable(admins));
      miscellaneous.writeHTML(outputComponentStudents, getAccountsTable(students));
    }
  } catch (e) {
    miscellaneous.writeHTML(outputComponentStudents, e);
    console.log(e);
  }
}

function callbackAddEmailsOrUsers(input, outputComponent) {
  if (typeof outputComponent == "string") {
    outputComponent = document.getElementById(outputComponent);
  }
  miscellaneous.writeHTML(
    outputComponent,
    miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input)),
  );
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
  let url = "";
  url += `${pathnames.urls.calculatorAPI}?`;
  url += `${pathnames.urlFields.request}=${requestType}&`;
  url += `${pathnames.urlFields.userRole}=${userRole}&`;
  url += `userList=${encodeURIComponent(spanEmailList.value)}&`;
  url += `studentSection=${encodeURIComponent(spanUserGroup.value)}&`;
  url += `passwordList=${encodeURIComponent(spanPasswords.value)}&`;
  url += `filterAccounts=&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    result: idOutput,
    callback: callbackAddEmailsOrUsers
  });
}

function getTeachersStudentsCallback(input, output) {
  let element = document.getElementById(output);
  miscellaneous.writeHTML(
    element,
    miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input)),
  );
}

function getTeachersStudents() {
  let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.requests.setTeacher}&`;
  let inputSections = document.getElementById('inputSections').value;
  let inputTeachers = document.getElementById('inputSetTeacher').value;
  let teachersAndSections = {};
  teachersAndSections[
    pathnames.urlFields.requests.teachers
  ] = encodeURIComponent(inputTeachers);
  teachersAndSections[
    pathnames.urlFields.requests.sections
  ] = encodeURIComponent(inputSections);
  url += `${pathnames.urlFields.teachersAndSections}=${encodeURIComponent(JSON.stringify(teachersAndSections))}&`;
  submitRequests.submitGET({
    url: url,
    progress: ids.domElements.spanProgressReportGeneral,
    result: "idOutputSections",
    callback: getTeachersStudentsCallback
  });

}

function updateAccountsPage() {
  let url = `${pathnames.urls.calculatorAPI}?${pathnames.urlFields.request}=${pathnames.urlFields.accountsJSON}`;

  submitRequests.submitGET({
    url: url,
    callback: updateAccountsPageCallback,
    progress: ids.domElements.spanProgressReportGeneral,
  });

}

let manageAccountsPage = new ManageAccountsPage();
manageAccountsPage.initialize();

module.exports = {
  manageAccountsPage,
  addEmailsOrUsers,
  updateAccountsPage,
  getTeachersStudents
};