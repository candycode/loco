////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////
try {
  Loco.console.println("LoCO - Loosely Coupled Objects");
  var app = Loco.ctx.appInfo();
  for( p in app ) {
    if( p !== null && app[ p ].length > 0 ) { 
      Loco.console.println( p + ": " + app[ p ] );
    }     	
  }
  Loco.console.println("OS: " + Loco.ctx.os());
  Loco.console.println("ENV: " );
  var e = Loco.sys.env();
  for( v in e ) {
    Loco.console.print( v );
    Loco.console.print( " = " );
    Loco.console.println( e[ v ] );
  }
  Loco.console.println("QT VERSION: " + Loco.ctx.qtVersion());
  Loco.console.println("Javascript interpreter: " + Loco.ctx.jsInterpreterName());
  Loco.ctx.exit( 0 );
} catch( e ) {
  Loco.console.printerrln( e );
  Loco.ctx.exit( -1 );
}
