//#SCRHEADER

#include "LocoContext.h"

#include "LocoDefaultJSInit.h"


namespace loco {

Context::Context( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
                  Context* parent)
: Object( 0, "LocoContext", "Loco/Context" ),
    webFrame_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ),
    globalContextJSName_( "Loco" ), jsInitGenerator_( 0 ) {
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( RemoveInstanceObjects() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( RemoveFilters() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( AddJavaScriptObjects() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( InitJScript() ) );

    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveInstanceObjects() ) );
    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveStdObjects() ) );
    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveFilters() ) );
    
  	jsInitGenerator_ = new DefaultJSInit( this );
}

void Context::Init( QWebFrame* wf, QApplication* app, const CMDLine& cmdLine,
                    Context* parent ) {

    webFrame_ = wf,
    app_ = app;
    parent_ = parent,
    cmdLine_ = cmdLine;
    
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( RemoveInstanceObjects() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( RemoveFilters() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( AddJavaScriptObjects() ) );
    connect( webFrame_, SIGNAL( javaScriptWindowObjectCleared() ),
         this, SLOT( InitJScript() ) );

    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveInstanceObjects() ) );
    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveStdObjects() ) );
    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveFilters() ) );
    
    jsInitGenerator_ = new DefaultJSInit( this );
}

Object* Context::Find( const QString& jsInstanceName ) const {
    for( JScriptObjCtxInstances::const_iterator i = instanceObjs_.begin();
         i != instanceObjs_.end(); ++i ) {
        if( *i && (*i)->jsInstanceName() == jsInstanceName ) return *i;
    }
    for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
         i != jscriptStdObjects_.end(); ++i ) {
        if( *i && (*i)->jsInstanceName() == jsInstanceName ) return *i;
    }
    for( JScriptObjCtxInstances::const_iterator i = jscriptCtxInstances_.begin();
         i != jscriptCtxInstances_.end(); ++i ) {
        if( *i && (*i)->jsInstanceName() == jsInstanceName ) return *i;
    }
    return 0;
}
    
 QVariant Context::loadObject( const QString& uri,  //used as a regular file path for now
             bool persistent ) { 
    if( uriObjectMap_[ persistent ].find( uri ) != uriObjectMap_[ persistent ].end() ) {
        Object* obj = uriObjectMap_[ persistent ][ uri ];
        return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
    }
    QPluginLoader* pl = new QPluginLoader( uri );
		if( !pl->load() ) {
			delete pl;
			error( "Cannot load " + uri );
			webFrame_->evaluateJavaScript( jsErrCBack_ );
			return QVariant();
		} 			
    Object* obj = qobject_cast< ::loco::Object* >( pl->instance() );
    if( !obj ) {
			delete pl;
			error( "Wrong object type " + uri );
			webFrame_->evaluateJavaScript( jsErrCBack_ );
			return QVariant();
		}
		obj->SetContext( this );
    obj->SetPluginLoader( pl );
    connect( obj, SIGNAL( error( const QString& ) ), this, SLOT( OnError( const QString& ) ) );
		if( persistent ) {
			jscriptStdObjects_.push_back( obj );
			stdPluginLoaders_.push_back( pl );
		} else {
			jscriptCtxInstances_.push_back( obj );
			ctxPluginLoaders_.push_back( pl );
		}
    uriObjectMap_[ persistent ][ uri ] = obj;
    webFrame_->addToJavaScriptWindowObject( obj->jsInstanceName(), obj );
    return webFrame_->evaluateJavaScript( obj->jsInstanceName() );
}      



}
