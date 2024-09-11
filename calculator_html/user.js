"use strict";
const ids = require("./ids_dom_elements");
const storage = require("./storage").storage;
const miscellaneous = require("./miscellaneous_frontend");

/** 
 * A mutable object that stores the state of the current user, 
 * most importantly whether the user is logged in or not. 
 */
class User {
  constructor() {
    this.loginSequenceInProgress = false;
    this.flagLoggedIn = false;
    this.googleProfile = null;
    this.sectionsTaught = [];
    this.instructor = "";
    this.sectionInDB = "";
    this.deadlineSchema = "";
    this.sectionComputed = "";
    this.flagDatabaseInactiveEveryoneIsAdmin = false;
    this.debugLoginProcess = false;
    this.debugLogin = false;
  }

  isLoggedIn() {
    return this.flagLoggedIn;
  }

  debugLoginIsOn() {
    return (
      this.flagDatabaseInactiveEveryoneIsAdmin === true ||
      this.debugLogin === true
    );
  }

  getRole() {
    return storage.variables.user.role.getValue();
  }

  hasAdminRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hasProblemEditRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hasInstructorRights() {
    return this.getRole() === "admin" && this.isLoggedIn();
  }

  hideProfilePicture() {
    document.getElementById(ids.domElements.divProfilePicture).classList.add("divInvisible");
    document.getElementById(ids.domElements.divProfilePicture).classList.remove("divVisible");
  }

  makeFromUserInfo(inputData) {
    // Please note: the authentication token is
    // silently set through the cookie headers.
    // Please do not take explicit action as
    // inputdata.authenticationToken may not 
    // contain the authentication token.
    // not ok: 
    // storage.variables.user.authenticationToken.
    // setAndStore(inputData.authenticationToken);
    storage.variables.user.name.setAndStore(inputData.username);
    storage.variables.user.role.setAndStore(inputData.userRole);
    this.flagLoggedIn = true;
    this.sectionsTaught = inputData.sectionsTaught;
    this.instructor = inputData.instructor;
    this.sectionInDB = inputData.studentSection;
    this.sectionComputed = inputData.studentSection;
    this.deadlineSchema = inputData.deadlineSchema;
    let spanUser = document.getElementById(
      ids.domElements.spanUserIdInAccountsPage
    );
    miscellaneous.writeHTML(spanUser, storage.variables.user.name.value);
    let userName = document.getElementById(
      ids.domElements.inputUsername
    );
    userName.value = storage.variables.user.name.value;
  }

  studentView() {
    return storage.variables.flagStudentView.isTrue();
  }

  hasInstructorRightsNotViewingAsStudent() {
    return this.hasInstructorRights() && !this.studentView();
  }
}

const globalUser = new User();
module.exports = {
  globalUser,
}