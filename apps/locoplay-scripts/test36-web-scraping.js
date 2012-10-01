try {
  if( Loco.ctx.args.length < 3 ) {
    Loco.console.println( 'usage: ' + Loco.ctx.args[0] + 
                          ' ' + Loco.ctx.args[1] + ' sticker simbol e.g. "AAPL"');
    Loco.ctx.exit( 0 );
  }
  var sticker = Loco.ctx.args[2];
  var url = 'http://financials.morningstar.com/ratios/r.html?t=' + sticker 
            + '&region=USA&culture=en-US'; 
  // create main window  
  var ww = Loco.gui.create( "WebWindow" );
  // setup main window 
  if( !ww.syncLoad( url, 10000 ) ) throw "Cannot load url " + url;

  // 1) extract row from exact position
  var element = ww.findFirstElement( "#financials table tbody tr:nth-child(20)");
  if( !element ) Loco.console.println( 'Not found' );
  else Loco.console.println(element.toPlainText());

  // 2) more robust: search for row with first cell containing 'book value'
  element = ww.findFirstElement( "#financials table tbody" );
  // get non-empty rows
  var rows = element.findElements( 'tr' );
  // for each row check if first cell contains 'book value'
  for( var i = 0; i !== rows.length; i++) {
    var th = rows[i].findFirstElement('th');
    if( th === null ) continue;
    if( th.toPlainText().toLowerCase().indexOf('book value') >= 0 ) {
      Loco.console.println( rows[i].toPlainText() );
      break;
    }  
  }
  Loco.ctx.exit(0);
  //ww.show();
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.exit( -1 );
}

