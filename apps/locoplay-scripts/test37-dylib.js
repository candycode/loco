try {
  Loco.ctx.onError.connect( function( err ) { throw err; } );
  var DYLIB_PATH = ""
  var DYLIB_NAME = "SampleDyLib.qtdylib"; 
  if( Loco.ctx.args.length < 3 ) {
    var BUILD_DIR = Loco.fs.home() + '/build/loco';
    DYLIB_PATH = BUILD_DIR + '/modules/dylib/sample';
  } else {
    DYLIB_PATH = Loco.ctx.args[ 2 ];
  }
  var creatorFunName = 'Create';//name of creator function
  var creationParams = { helloMessage : 'Hello from a humble dynamic library' };
  var dyobj = Loco.ctx.loadQObjectFromDyLib( DYLIB_PATH + '/' + DYLIB_NAME,
                                             creatorFunName, creationParams );
  if( dyobj === null ) throw "Cannot create object from " + DYLIB_PATH + '/' + DYLIB_NAME;
  dyobj.hello();
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.println( e );
  Loco.console.println( "Make sure the sample dylib is built and the path is correct" );
  Loco.ctx.exit( -1 );   
}


