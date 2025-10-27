// This script goes into your google apps script hub. It does not need to be associated with the account that created the attendance sheet since you are going to make anyone able to edit it

// ie. https://docs.google.com/spreadsheets/d/1JGE3v5dyKw6-z0lAmh3VeGk_xUktlv5uun9eu03QN6g/edit?usp=sharing --> Link to Fall 2025 GBM Attendance Sheet

//Script link breaks when you are logged into multiple users so it is best to open an incognito tab if you want to test it out

/*

CTRL + F all "Update" lines to find the values you need to update as you change spreedsheets

*/
var ss = SpreadsheetApp.openById('1JGE3v5dyKw6-z0lAmh3VeGk_xUktlv5uun9eu03QN6g'); //Update your googlesheets URL ID here
var sheet = ss.getSheetByName('Fall \'25'); //Update to the correct tab name
var timezone = "America/Los_Angeles"; //Set your timezone

//Finds the correct row corresponding with the meeting date
function findCol(){
  var currData = new Date()
  var today = Utilities.formatDate(currData, timezone, 'MM/dd/yy') // ie. "03/23/25" is the meeting date format

  var topRow = sheet.getRange('3:3').getValues()  //Update the range to the correct row that the dates are in

  //Loop through the top row to find correct column
  for (var i = 0; i < topRow[0].length; i++){
    var colTitle = topRow[0][i] // ie. "GBM #1 01/23/25" column header format

    //Skip empty cells
    if (!colTitle){
      continue;
    }

    //Search for correct date column matching the current day's date
    var titleStr = Utilities.formatDate(colTitle, timezone, 'MM/dd/yy')
    if (titleStr.includes(today)){
      return(i + 1)
    }
  }
}

function findRow(n){
  var name = n.toLowerCase()

  var nameCol = sheet.getRange('B:B').getValues() //Update to the correct column that the member names are in

  for (var i = 0; i < nameCol.length; i++){
    var cellName = nameCol[i][0].toLowerCase()

    //Skip empty cells
    if (!cellName){
      continue;
    }

    if (cellName.includes(name)){
      return(i + 1)
    }
  }
}

function doGet(e){
  var name = strip(e.parameter.name) //ex. Minh%20Pham-Nguyen
  var row = findRow(name)
  var col = findCol()

  sheet.getRange(row, col).setValue("p") //Update to whatever value is used as present indicator

  return ContentService.createTextOutput(`Brother ${name} has been successfully checked in!`);
}

function strip(value) {
  //Strips quotes & URL encoding (%20)
  return value.toString().replace(/^["']|['"]$/g, "").replace(/%20/g, " ");
}