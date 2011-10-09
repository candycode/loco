//#SCRHEADER

#include <QTimer>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

#include "LocoContext.h"

#include "LocoDefaultJSInit.h"


namespace loco {


Context::Context( Context* parent ) : jsContext_( new JSContext( this ) ),
                     jsInterpreter_( 0 ), jsInitGenerator_( 0 ),
                     app_( 0 ), parent_( 0 ), globalContextJSName_( "Loco" ), netAccessMgr_( 0 ),
                     readNetworkTimeout_( 10000 ), maxNetRedirections_( 0 ),
                     autoMapFilters_( false ), addParentObjs_( false ), appInfo_( 0 )  {
    connect( this, SIGNAL( onError( const QString& ) ), 
             this, SLOT( OnSelfError( const QString& ) ) );
    jsContext_->setParent( this );
    includePath_ << "." << QCoreApplication::applicationDirPath() + "/include"
                 << QCoreApplication::applicationDirPath() + "/scripts";
} 


Context::Context( IJSInterpreter* jsi, LocoQtApp* app, const QStringList& cmdLine,
                  Context* parent)
:   jsContext_( new JSContext( this ) ), jsInitGenerator_( 0 ),
    jsInterpreter_( jsi ), app_( app ), parent_( parent ), cmdLine_( cmdLine ),
    globalContextJSName_( "Loco" ), netAccessMgr_( 0 ),
    readNetworkTimeout_( 10000 ), maxNetRedirections_( 0 ),
    autoMapFilters_( false ), addParentObjs_( false ), appInfo_( 0 )  {
    connect( this, SIGNAL( onError( const QString& ) ), 
             this, SLOT( OnSelfError( const QString& ) ) );
    Init( jsi, app, cmdLine, parent );
    jsContext_->setParent( this );
    includePath_ << "." << QCoreApplication::applicationDirPath() + "/include"
                 << QCoreApplication::applicationDirPath() + "/scripts";
}


void Context::AddContextToJS() { AddJSStdObject( jsContext_ ); }

void Context::ConnectErrCBack( Object* obj ) {
    if( obj == jsContext_ ) return;  
    connect( obj, SIGNAL( onError( const QString& ) ), this, SLOT( OnObjectError( const QString& ) ) );
}

void Context::Init( IJSInterpreter* jsi, LocoQtApp* app, const QStringList& cmdLine,
                    Context* parent ) {

    jsInterpreter_ = jsi,
    app_ = app;
    parent_ = parent,
    cmdLine_ = cmdLine;
    if( jsInitGenerator_ != 0 ) jsInitGenerator_->deleteLater();
    jsInitGenerator_ =  new DefaultJSInit( this );
    jsInitGenerator_->setParent( this );
    
    
    connect( jsInterpreter_.data(), SIGNAL( JavaScriptContextCleared() ),
         this, SLOT( OnJSContextCleared() ) );

    // all objects are set as children of this context and will therefore be
    // automatically deleted when the context is destroyed
       
    //allow js context to receive errors from context and emit signals
    connect( this, SIGNAL( onError( const QString&) ), jsContext_, SLOT( ForwardError( const QString& ) ) );

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
		const QString err = pl->errorString();
		delete pl;
		error( "Cannot load " + uri + " - " + err );
		jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
		return QVariant();
	}
	QObject* qobj = pl->instance();
	if( !qobj ) {
	    delete pl;
		error( "NULL instance - " + uri );
		jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
		return QVariant();
	}
	bool foreign = false;
    Object* obj = qobject_cast< ::loco::Object* >( pl->instance() );
	if( !obj ) {
		foreign = true;
		obj = new Object;
		obj->SetPluginLoader( pl );
		connect( qobj, SIGNAL( destroyed() ), obj, SLOT( deleteLater() ) );
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
	if( foreign ) jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), qobj );
	else jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj );
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
    QString filePath = f;    
    if( !QDir::isAbsolutePath( f ) ) {
        QStringList::const_iterator i = includePath_.begin();
        for( ; i != includePath_.end(); ++i ) {
            if( !QFile::exists( *i + f ) ) continue;
            else { filePath = *i + f; break; }
        }
        if( i == includePath_.end() ) {
            error( "File " + f + " does not exists" );
            return QByteArray();
        }
    } 
                
    if( !fileAccessMgr_->CheckAccess( filePath ) ) {
        error( "Not authorized to access file " + filePath );
        return QByteArray();
    }    
    QFile file( filePath );
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

void Context::SetJSContextName( const QString& n ) { jsContext_->setName( n ); }

}
