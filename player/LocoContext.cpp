//#SCRHEADER

#include <QTimer>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

#include "LocoContext.h"

#include "LocoDefaultJSInit.h"


namespace loco {


Context::Context( Context* parent ) : jsContext_( new JSContext( *this ) ), jsInterpreter_( 0 ),
                     app_( 0 ), parent_( 0 ), globalContextJSName_( "Loco" ),
                     jsInitGenerator_( 0 ), netAccessMgr_( 0 ),
                     readNetworkTimeout_( 10000 ), maxNetRedirections_( 0 ),
                     autoMapFilters_( false )  {
    connect( this, SIGNAL( onError( const QString& ) ), 
             this, SLOT( OnSelfError( const QString& ) ) );

} 


Context::Context( IJSInterpreter* wf, LocoQtApp* app, const QStringList& cmdLine,
                  Context* parent)
:   jsContext_( new JSContext( *this ) ),
    jsInterpreter_( wf ), app_( app ), parent_( parent ), cmdLine_( cmdLine ),
    globalContextJSName_( "Loco" ), jsInitGenerator_( 0 ), netAccessMgr_( 0 ),
    readNetworkTimeout_( 10000 ), maxNetRedirections_( 0 ),
    autoMapFilters_( false )  {
    connect( this, SIGNAL( onError( const QString& ) ), 
             this, SLOT( OnSelfError( const QString& ) ) );
    Init( wf, app, cmdLine, parent );
}


void Context::AddContextToJS() { AddJSStdObject( jsContext_.data() ); }

void Context::ConnectErrCBack( Object* obj ) {
    if( obj == jsContext_ ) return;  
    connect( obj, SIGNAL( onError( const QString& ) ), this, SLOT( OnObjectError( const QString& ) ) );
}



void Context::Init( IJSInterpreter* wf, LocoQtApp* app, const QStringList& cmdLine,
                    Context* parent ) {

    jsInterpreter_ = wf,
    app_ = app;
    parent_ = parent,
    cmdLine_ = cmdLine;
    
    connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ),
         this, SLOT( RemoveInstanceObjects() ) );
    connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ),
         this, SLOT( RemoveFilters() ) );
    connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ),
         this, SLOT( AddJavaScriptObjects() ) );
    connect( jsInterpreter_, SIGNAL( JavaScriptContextCleared() ),
         this, SLOT( InitJScript() ) );

    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveInstanceObjects() ) );
    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveStdObjects() ) );

    connect( this, SIGNAL( destroyed() ), this, SLOT( RemoveFilters() ) );
    
    //allow js context to receive errors from context and emit signals
    connect( this, SIGNAL( onError( const QString&) ), jsContext_.data(), SLOT( ForwardError( const QString& ) ) );

    jsInitGenerator_ = QSharedPointer< IJavaScriptInit >( new DefaultJSInit( this ) );

	jsInterpreter_->Init();
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
    
 QVariant Context::LoadObject( const QString& uri,  //used as a regular file path for now
                               bool persistent ) { 
    if( uriObjectMap_[ persistent ].find( uri ) != uriObjectMap_[ persistent ].end() ) {
        Object* obj = uriObjectMap_[ persistent ][ uri ];
        return jsInterpreter_->EvaluateJavaScript( obj->jsInstanceName() );
    }
    QPluginLoader* pl = new QPluginLoader( uri );
		if( !pl->load() ) {
			delete pl;
			error( "Cannot load " + uri );
			jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
			return QVariant();
		} 			
    Object* obj = qobject_cast< ::loco::Object* >( pl->instance() );
    if( !obj ) {
			delete pl;
			error( "Wrong object type " + uri );
			jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
			return QVariant();
    }
	obj->SetContext( this );
    obj->SetPluginLoader( pl );
    connect( obj, SIGNAL( onError( const QString& ) ), this, SLOT( OnObjectError( const QString& ) ) );
		if( persistent ) {
			jscriptStdObjects_.push_back( obj );
			stdPluginLoaders_.push_back( pl );
		} else {
			jscriptCtxInstances_.push_back( obj );
			ctxPluginLoaders_.push_back( pl );
		}
    uriObjectMap_[ persistent ][ uri ] = obj;
    jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj );
    return jsInterpreter_->EvaluateJavaScript( obj->jsInstanceName() );
}


QByteArray Context::ReadUrl( const QString& url, QSet< QUrl > redirects ) {
    if( netAccessMgr_ == 0 ) return QByteArray();
    if( redirects.size() > maxNetRedirections_ ) {
        error( QString( "Max number of redirections(%1) excedeed" ).arg( maxNetRedirections_ ) );
        return QByteArray();
    }
    if( redirects.find( QUrl( url ) ) != redirects.end() ) {
        error( "Circular redirection: " + url );
        return QByteArray();
    }
    QNetworkReply* reply = netAccessMgr_->get( QNetworkRequest( QUrl( url ) ) );
    QEventLoop loop;
    QObject::connect( reply, SIGNAL( readyRead() ), &loop, SLOT( quit() ) );
    // soft real-time guarantee: kill network request if the total time is >= timeout
    QTimer::singleShot( readNetworkTimeout_, &loop, SLOT( quit() ) );
    // Execute the event loop here, now we will wait here until readyRead() signal is emitted
    // which in turn will trigger event loop quit.
    loop.exec();
    if( reply->isRunning() ) {
        reply->close();
        error( "Network request is taking too long to complete" );
        return QByteArray();
    }
    QVariant possibleRedirectUrl =  reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    if( !possibleRedirectUrl.toUrl().isEmpty()  ) {
        redirects.insert( QUrl( url ) );
        return ReadUrl( possibleRedirectUrl.toString(), redirects );
    }
    else return reply->readAll(); 
}

QByteArray Context::ReadFile( const QString& f ) {
    if( !fileAccessMgr_->CheckAccess( f ) ) {
        error( "Not authorized to access file " + f );
        return QByteArray();
    }
    if( !QFile::exists( f ) ) {
        error( "File " + f + " does not exists" );
        return QByteArray();
    } 
    QFile file( f );
    if( !file.open( QIODevice::ReadOnly ) ) {
        error( "Cannot open file " + f );
        return QByteArray();
    }
    QByteArray b = file.readAll();
    if( file.error() != QFile::NoError ) {
        error( file.errorString() );
        return QByteArray();
    }
    return b;
}      

}