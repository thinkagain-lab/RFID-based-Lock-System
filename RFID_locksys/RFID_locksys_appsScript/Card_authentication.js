/*
 * Copyright 2024 Rohit Paul and Think Again Lab
 *
 * Licensed under the MIT License.
 * See the LICENSE file in the project root for full license information.
 */

var ss = SpreadsheetApp.openById('SpreadsheetID');
var sheet = ss.getSheetByName('Sheet1');
//   Get the last row with data in column A (assuming your data starts in A2)
function authenticateCard(cardId) {
  var dataRange = sheet.getRange('A2:A100'); // Assuming card IDs are stored in column A
  var values = dataRange.getValues();
  var flag = 0;
  
  for (var i = 0; i < values.length; i++) {
    if (values[i][0] == cardId) {
      return "true";
      flag = 1;
    }
  }
  if (flag == 0){
    return "false";
  }
  
}
//   This is the function that will be called when a door opens or closes. It checks whether to allow access based on the card ID.

// This function is called by the client-side code, not by Google Apps Script
function doGet(e) {
  Logger.log( JSON.stringify(e) );
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }
  var cardId = stripQuotes(e.parameters.cardId);
  //var cardId = e.parameter.cardId;
  var result = authenticateCard(cardId);
  return ContentService.createTextOutput(result);
}
//   Strip quotes from the parameter value, if any
function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}
