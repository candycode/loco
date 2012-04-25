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

#include <iostream>
#include <stdexcept>

#include <LocoApp.h>

#ifdef LOCOPLAY_CONSOLE
#include <LocoConsole.h>
#endif
#ifdef LOCOPLAY_FSYSTEM
#include <LocoFileSystem.h>
#endif
#ifdef LOCOPLAY_SYSTEM
#include <LocoSystem.h>
#endif
#include <LocoObjectInfo.h>
#ifdef LOCOPLAY_WKIT
#include <LocoWebKitJSCore.h>
#else
#include <LocoQScriptInterpreter.h>
#endif
#ifdef LOCOPLAY_GUI
#include <LocoGUI.h>
#endif
#include <LocoQtApp.h>
#ifdef LOCOPLAY_JS_SYNTAX_CHECKER
#include <LocoJSSyntaxChecker.h>
#endif
#include <LocoNetwork.h>

#include <LocoAppConfig.h>

#include <LocoFilter.h>

// remove first line of script if it starts with !#
class SystemScriptFilter : public loco::Filter {
public:
    virtual QString Apply( const QString& code ) const {
        if( code.startsWith( "#!" ) ) {
            QString c = code;    
            return c.remove( 0, code.indexOf( "\n" ) );
        } else return code;
    }
    virtual QByteArray Apply( const QByteArray& ba ) const { 
        return Apply( QString( ba ) ).toAscii();
    }
};

using namespace loco;
 
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int ret = -1;
    try {
        LocoQtApp qtApp( argc, argv );
        loco::ObjectInfo* i = new loco::ObjectInfo;
        QStringList sl = LOCO_APP_VERSION.split(".");
        i->SetName( LOCO_APP_NAME )
          .SetVersion( sl )
          .SetLicense( LOCO_APP_LICENSE )
          .SetAuthor( LOCO_APP_AUTHOR )
          .SetCopyright( LOCO_APP_COPYRIGHT )
          .SetVendor( LOCO_APP_VENDOR )
          .SetUrl( LOCO_APP_URL );
        loco::App app( qtApp, argc, argv, i );
        //WARNING: interpreter MUST BE CREATED AFTER APP if not
        //AN ERROR IS REPORTED ("QWidgets must be created after QApplication")
        //and in the case of the QtWebKit JavaScriptCore interpreter the interpreter
        //is inside a class that has a dependency on QWidget.
#ifdef LOCOPLAY_WKIT
        app.SetInterpreter( new WebKitJSCore ); // app owns interpreter
#else
        app.SetInterpreter( new QScriptInterpreter );
#endif

#ifdef LOCOPLAY_CONSOLE
        app.AddModuleToJS( new Console );
#endif
#ifdef LOCOPLAY_FSYSTEM
        app.AddModuleToJS( new FileSystem );
        app.SetAllowFileAccess( true );
        app.SetFilterFileAccess( false );
#endif
#ifdef LOCOPLAY_SYSTEM
        app.AddModuleToJS( new System );
#endif
#ifdef LOCOPLAY_GUI
        app.AddModuleToJS( new GUI );
        QObject::connect( &qtApp, SIGNAL( lastWindowClosed() ), &qtApp, SLOT( quit() ) ); 
#endif
#ifdef LOCOPLAY_CONTEXT
        app.AddContextToJS();
#endif
#ifdef LOCOPLAY_JS_SYNTAX_CHECKER
        app.AddModuleToJS( new SyntaxChecker );
#endif
#ifdef LOCOPLAY_NETWORK
        app.SetAllowNetAccess( true );
        app.SetFilterNetRequests( false );
        app.AddModuleToJS( new Network );
#endif
        const QString& CS = ":/coffee-script-1.2.js";
        app.InitContext();
        
        SystemScriptFilter f;
        app.PreloadFilter( "script", &f );

        app.PreloadScriptFilter( "coffee", CS,
                                 "loco_coffeeCompile_",
                                 "function loco_coffeeCompile_( coffeeCode ) {"
                                 "  return CoffeeScript.compile( coffeeCode, {bare: true} );"
                                 "}" );
        app.PreloadScriptFilter( "skulpt", ":/skulpt/skulpt-filter.js",
                                 "loco_skulptCompile_",
                                 "function loco_skulptCompile_( code ) {"
                                 "  var s = Sk.compile( code, '', 'single' );"
                                 "  return s.code + s.funcname + '()';"
                                 "}" );
#ifdef LOCO_WKIT //issues in making BiwaSchem work properly with QtScript
        app.PreloadScriptFilter( "biwascheme", ":/biwascheme/biwascheme-filter.js",
                                 "loco_schemeCompile_",
                                 "function loco_schemeCompile_( code ) {"
                                 " return (new BiwaScheme.Interpreter(locoHandleBiwaSchemeError_)).evaluate( code );"
                                 "}" );
        app.MapToFilters( QRegExp( ".+\\.+scm$" ), QStringList() << "script" << "biwascheme" );
#endif
        app.MapToFilters( QRegExp( ".+\\.+coffee$" ), QStringList() << "script" << "coffee" );
        app.MapToFilters( QRegExp( ".+\\.+py$" ), QStringList() << "script" << "skulpt" );
        app.MapToFilters( QRegExp( ".+\\.js$|.+\\.ljs$" ), QStringList() << "script" ); 

        app.SetMapFiltersToFileNames( true );
        // tell the application what is considered a valid file name for a script
        // to execute
        app.SetScriptFileNameMatchingExpression( QRegExp( ".+\\.ljs$|.+\\.js$|.+\\.coffee$|.+\\.py$|.+\\.scm$" ) );
        app.SetDocHandler( QRegExp( ".+\\.html$|.+\\.htm$|^http(s)?://.+" ), ":/browser.js" );
        app.SetDocHandler( QRegExp( ".+\\.rcc$|.+\\.lrcc$" ), ":/rccexec.js" );
        app.ParseCommandLine();
        ret = app.Execute();
    } catch( const std::exception& e ) {
        std::cerr << LOCO_APP_NAME.toStdString() << ": " << e.what() << std::endl;
    }
    return ret;
}
