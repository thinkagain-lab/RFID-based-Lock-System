

var ss = SpreadsheetApp.openById('SpreadsheetId');
var sheet = ss.getSheetByName('sheet1');
var timeZone = "Asia/Kolkata"; // change according your timezone
//var currentTime = new Date().toLocaleString("en-US", {timeZone: indianTimezone});
//console.log(currentTime);


function doGet(e){
  Logger.log( JSON.stringify(e) );
  //----------------------------------------------------------------------------------
  //write_google_sheet() function in esp32 sketch, is send data to this code block
  //----------------------------------------------------------------------------------
  //get gps data from ESP32
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }
  //----------------------------------------------------------------------------------
  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timeZone, 'HH:mm:ss');
  var name = stripQuotes(e.parameters.name);
  //Logger.log('name=' + name);
  //----------------------------------------------------------------------------------
  var nextRow = sheet.getLastRow() + 1;
  sheet.getRange("A" + nextRow).setValue(Curr_Date);
  sheet.getRange("B" + nextRow).setValue(Curr_Time);
  sheet.getRange("C" + nextRow).setValue(name);
  //----------------------------------------------------------------------------------

  //returns response back to ESP32
  return ContentService.createTextOutput("Card holder name is stored in column C");
  //----------------------------------------------------------------------------------
}


function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}

//Extra Function. Not used in this project.
//planning to use in future projects.
//this function is used to handle POST request
function doPost(e) {
  var val = e.parameter.value;
  
  if (e.parameter.value !== undefined){
    var range = sheet.getRange('A2');
    range.setValue(val);
  }
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM