var ss = SpreadsheetApp.openById('SpreadsheetID');
var sheet = ss.getSheetByName('Sheet1');

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
function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}
