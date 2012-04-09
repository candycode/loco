cmdLineToJSON = function( cmdLine ) {
 // cmdLine: param1 param2...
  var jsonCmdLine = {};
  var i = 0;
  var param = "";
  var values = [];
  while( i < cmdLine.length ) {
    if( cmdLine[ i ][ 0 ] === '-' ) {
      param = cmdLine[ i ];
      ++i;
      values = []; 
      while( i < cmdLine.length && (cmdLine[ i ][ 0 ] !== '-') ) {
        values.push( cmdLine[ i ] );
        ++i;
      } 
      if( !jsonCmdLine[ param ] ) jsonCmdLine[ param ] = [];
      jsonCmdLine[ param ] = jsonCmdLine[ param ].concat( values );  
    } else ++i;
  }
  return jsonCmdLine; 
};
