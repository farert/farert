/**
 * Adds a custom menu
 *
 * @param {Object} e The event parameter for a simple onOpen trigger.
 */
 function onOpen(e) {
    SpreadsheetApp.getUi()
        .createMenu('Custom')
        .addItem('Download as TSV', 'downloadTSV_GUI')
        .addToUi();
  }
  
  
  /**
   * Display a modal dialog with a single download link.
   *
   * From: http://stackoverflow.com/a/37336778/1677912
   */
  function downloadTSV_GUI() {
    // Get current spreadsheet's ID, place in download URL
    var ssID = SpreadsheetApp.getActive().getId();
    var URL = 'https://docs.google.com/spreadsheets/d/'+ssID+'/export?format=tsv';
  
    // Display a modal dialog box with download link.
    var htmlOutput = HtmlService
                    .createHtmlOutput('<a href="'+URL+'">Click to download</a>')
                    .setSandboxMode(HtmlService.SandboxMode.IFRAME)
                    .setWidth(80)
                    .setHeight(60);
    SpreadsheetApp.getUi().showModalDialog(htmlOutput, 'Download TSV');
  }
  
  