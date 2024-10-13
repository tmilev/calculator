"use strict";
const submitRequests = require("./submit_requests");
const ids = require("./ids_dom_elements");
const pathnames = require("./pathnames");
const miscellaneous = require("./miscellaneous_frontend");
const { storage } = require("./storage");

class ManageAccountsPage {
  initialize() {
    const buttonSetTeacher = document.getElementById(
      ids.domElements.pages.manageAccounts.buttonSetTeacher
    );
    if (buttonSetTeacher === null || buttonSetTeacher === undefined) {
      // Did not find buttonSetTeacher, not initializing accounts page.
      return;
    }
    buttonSetTeacher.addEventListener('click', () => {
      this.getTeachersStudents();
    });
    document.getElementById(
      ids.domElements.pages.manageAccounts.buttonAddUsersTeachers
    ).addEventListener('click', () => {
      this.addEmailsOrUsers(
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
      this.addEmailsOrUsers(
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

  addEmailsOrUsers(
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
      callback: (input, idOutput) => {
        this.callbackAddEmailsOrUsers(input, idOutput);
      }
    });
  }

  callbackAddEmailsOrUsers(input, outputComponent) {
    if (typeof outputComponent == "string") {
      outputComponent = document.getElementById(
        outputComponent
      );
    }
    miscellaneous.writeHTML(
      outputComponent,
      miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input)),
    );
  }

  updateAccountsPage() {
    const api = pathnames.urls.calculatorAPI;
    const fields = pathnames.urlFields;
    let url = `${api}?${fields.request}=${fields.accountsJSON}`;
    let courseHome = storage.variables.currentCourse.courseHome.getValue();
    let elements = document.getElementsByClassName("currentCourseIndicator");
    for (const element of elements) {
      element.textContent = courseHome;
    }
    submitRequests.submitGET({
      url: url,
      callback: (input, output) => {
        this.updateAccountsPageCallback(input, output);
      },
      progress: ids.domElements.spanProgressReportGeneral,
    });
  }

  updateAccountsPageCallback(input, notUsed) {
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
        const adminsTable = this.getAccountsTable(admins);
        const studentsTable = this.getAccountsTable(students);
        outputComponentAdmin.textContent = '';
        outputComponentStudents.textContent = '';
        outputComponentAdmin.appendChild(adminsTable);
        outputComponentStudents.appendChild(studentsTable);
      }
    } catch (e) {
      miscellaneous.writeHTML(outputComponentStudents, e);
      console.log(e);
    }
  }

  /** 
   * Gets a table with input accounts. 
   * @return {HTMLElement} 
   */
  getAccountsTable(inputAccounts) {
    let result = document.createElement("table");
    let headerRow = result.insertRow();
    const username = document.createElement("th");
    const email = document.createElement("th");
    const activated = document.createElement("th");
    const course = document.createElement("th");
    const section = document.createElement("th");
    const semester = document.createElement("th");
    const instructorHeader = document.createElement("th");
    username.textContent = "username";
    email.textContent = "email";
    activated.textContent = "activated";
    course.textContent = "course";
    section.textContent = "section";
    semester.textContent = "semester";
    instructorHeader.textContent = "instructor";
    headerRow.appendChild(username);
    headerRow.appendChild(email);
    headerRow.appendChild(activated);
    headerRow.appendChild(course);
    headerRow.appendChild(section);
    headerRow.appendChild(semester);
    headerRow.appendChild(instructorHeader);
    for (let i = 0; i < inputAccounts.length; i++) {
      const row = result.insertRow();
      let currentUser = inputAccounts[i];
      row.insertCell().textContent = currentUser.username;
      const currentEmail = row.insertCell();
      if (currentUser.email !== undefined) {
        currentEmail.textContent = currentUser.email;
      } else {
        currentEmail.textContent = "-";
      }
      const currentActivated = row.insertCell();
      if (currentUser.activationToken === "activated") {
        currentActivated.style.color = "green";
        currentActivated.textContent = currentUser.activationToken;
      } else {
        currentActivated.textContent = "Not activated";
        currentActivated.style.color = "red";
      }
      const currentCourses = row.insertCell();
      if (currentUser.currentCourses !== undefined) {
        currentCourses.textContent = currentUser.currentCourses;
      } else {
        currentCourses.textContent = "-";
      }
      const currentSection = row.insertCell();
      if (currentUser.studentSection !== undefined) {
        currentSection.textContent = currentUser.studentSection;
      } else {
        currentSection.textContent = "-";
      }
      const semesterCell = row.insertCell();
      if (currentUser.semester !== undefined) {
        semesterCell.textContent = currentUser.semester;
      } else {
        semesterCell.textContent = "-";
      }
      const currentInstructor = row.insertCell();
      if (currentUser.instructor !== undefined) {
        currentInstructor.textContent = currentUser.instructor;
      } else {
        currentInstructor.textContent = "-";
      }
    }
    return result;
  }

  getTeachersStudents() {
    const api = pathnames.urls.calculatorAPI;
    const fields = pathnames.urlFields;
    let url = `${api}?${fields.request}=${fields.requests.setTeacher}&`;
    let inputSections = document.getElementById('inputSections').value;
    let inputTeachers = document.getElementById('inputSetTeacher').value;
    let teachersAndSections = {};
    teachersAndSections[
      fields.requests.teachers
    ] = encodeURIComponent(inputTeachers);
    teachersAndSections[
      fields.requests.sections
    ] = encodeURIComponent(inputSections);
    const json = JSON.stringify(teachersAndSections);
    url += `${fields.teachersAndSections}=${encodeURIComponent(json)}&`;
    submitRequests.submitGET({
      url: url,
      progress: ids.domElements.spanProgressReportGeneral,
      result: "idOutputSections",
      callback: (input, output) => {
        this.getTeachersStudentsCallback(
          input, output
        );
      }
    });
  }

  getTeachersStudentsCallback(input, output) {
    let element = document.getElementById(output);
    miscellaneous.writeHTML(
      element,
      miscellaneous.jsonParseGetHtmlStandard(decodeURIComponent(input)),
    );
  }
}

function updateAccountsPage() {
  manageAccountsPage.updateAccountsPage();
}

let manageAccountsPage = new ManageAccountsPage();
manageAccountsPage.initialize();

module.exports = {
  manageAccountsPage,
  updateAccountsPage,
};