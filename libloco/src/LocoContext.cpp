//#SCRHEADER

#include <QTimer>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QMetaMethod>

#include "LocoContext.h"
#include "LocoJSContext.h"
#include "LocoDefaultJSInit.h"
#include "LocoScriptNetworkRequestHandler.h"
#include "LocoContextThread.h"
#include "LocoContextThreadLoop.h"
#include "Arrays/LocoFloat64Array.h"
#include "Arrays/LocoFloat32Array.h"
#include "Arrays/LocoIntArray.h"
#include "Arrays/LocoUIntArray.h"
#include "Arrays/LocoShortArray.h"
#include "Arrays/LocoUShortArray.h"
#include "Arrays/LocoByteArray.h"

#ifdef LOCO_WKIT
#include "LocoWebKitJSCore.h"
#endif
#ifdef LOCO_SCRIPT
#include "LocoQScriptInterpreter.h"
#endif
#include "LocoDataType.h"


namespace loco {
namespace {
template < typename T >
QVariant CreateArray( Context& ctx, const QVariantMap& init ) {
    typename T::Allocator a;
    if( init.contains( "align" ) ) a.set_alignment( init[ "align" ].toUInt() );
	bool ok = false;
	T* array = new T( a );
	if( init.contains( "size" ) ) {
		array->resize( init[ "size" ].toULongLong( &ok ) );
	    if( !ok ) {
		    delete array;
		    ctx.error( "Invalid unsigned long value");
		    return QVariant();
	    }
	}
	return ctx.AddObjToJSContext( array );
}
}

Context::Context( Context* parent ) : jsContext_( new JSContext( this ) ),
                     jsInterpreter_( 0 ), jsInitGenerator_( 0 ),
                     app_( 0 ), parent_( 0 ), globalContextJSName_( "Loco" ), netAccessMgr_( 0 ),
                     readNetworkTimeout_( 10000 ), maxNetRedirections_( 2 ),
                     autoMapFilters_( false ), addParentObjs_( false ), appInfo_( 0 ) {
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
    readNetworkTimeout_( 10000 ), maxNetRedirections_( 2 ),
    autoMapFilters_( false ), addParentObjs_( false ), appInfo_( 0 )  {
    connect( this, SIGNAL( onError( const QString& ) ), 
             this, SLOT( OnSelfError( const QString& ) ) );
    Init( jsi, app, cmdLine, parent );
    jsContext_->setParent( this );
    includePath_ << "." << QCoreApplication::applicationDirPath() + "/include"
                 << QCoreApplication::applicationDirPath() + "/scripts";
	connect( jsInterpreter_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
		     this, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
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
    connect( jsInterpreter_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
		     this, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ) );
	jsInterpreter_->Init();
}
    
 QVariant Context::LoadObject( const QString& uri,  //used as a regular file path for now
                               bool persistent ) { 
    if( uriObjectMap_.find( uri ) != uriObjectMap_.end() ) {
        Object* obj = uriObjectMap_[ uri ];
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
	Object* obj = qobject_cast< ::loco::Object* >( qobj );
	if( !obj ) {
	    error( "Not a loco::Object, use loco::Context::LoadQtPlugin() method instead" );
	    return QVariant();
	}
	obj->SetContext( this );
    obj->SetPluginLoader( pl );
    obj->setParent( pl );
    connect( obj, SIGNAL( onError( const QString& ) ), this, SLOT( OnObjectError( const QString& ) ) );
    jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj ); //lifetime managed by parent object/Qt
    if( persistent ) {
		pl->setParent( this );
		jscriptStdObjects_.push_back( obj );
		uriObjectMap_[ uri ] = obj;
	} else {
		//adding the plugin loader as a javascript managed object with the created instance as a child
		jsInterpreter_->AddObjectToJS( obj->jsInstanceName() + "__PluginLoader",
				                       pl, QScriptEngine::ScriptOwnership );
	}
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
    QObject::connect( netAccessMgr_, SIGNAL( finished( QNetworkReply* ) ), &loop, SLOT( quit() ) );
    if( !reply->isFinished() ) {
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
    }
    QVariant possibleRedirectUrl =  reply->attribute( QNetworkRequest::RedirectionTargetAttribute );
    if( !possibleRedirectUrl.toUrl().isEmpty()  ) {
        redirects.insert( QUrl( url ) );
        return ReadUrl( possibleRedirectUrl.toString(), redirects );
    }
    else {
    	lastReadURI_ = reply->url().toString();
        return reply->readAll();
    }
}

QByteArray Context::ReadFile( QString filePath ) {
    if( filePath.startsWith( "~/" ) ) filePath = QDir::homePath() + filePath.remove( 0, 1 );
    else if( !filePath.contains( ":/" ) && !QDir::isAbsolutePath( filePath ) ) {
        QStringList::const_iterator i = includePath_.begin();
        for( ; i != includePath_.end(); ++i ) {
            if( !QFile::exists( *i + "/" + filePath ) ) continue;
            else { filePath = *i + "/" + filePath; break; }
        }
        if( i == includePath_.end() ) {
            error( "File " + filePath + " does not exists" );
            return QByteArray();
        }
    }
                
    if( !fileAccessMgr_->CheckAccess( filePath ) ) {
        error( "Not authorized to access file " + filePath );
        return QByteArray();
    }    
    QFile file( filePath );
    if( !file.open( QIODevice::ReadOnly ) ) {
        error( "Cannot open file " + filePath );
        return QByteArray();
    }
    QByteArray b;
    while( file.bytesAvailable() ) b.append( file.readAll() );
    file.close();
    if( file.error() != QFile::NoError ) {
        error( file.errorString() );
        return QByteArray();
    }
    lastReadURI_ = filePath;
    return b;
}      

void Context::SetJSContextName( const QString& n ) { jsContext_->setName( n ); }

void Context::RemoveNameFilterMapping( const QRegExp& rx ) {
    for( NameFilterMap::iterator i = nameFilterMap_.begin();
         i != nameFilterMap_.end(); ++i ) {
        if( i->first == rx ) {
            nameFilterMap_.erase( i );
            break;
        }
    } 
}

void Context::AddJSStdObject( Object* obj, bool immediateAdd ) { 
    jscriptStdObjects_.push_back( obj );
    if( obj->GetContext() == 0 ) obj->SetContext( this );
    if( obj->GetPluginLoader() == 0 && obj->parent() == 0 ) obj->setParent( this );
    ConnectErrCBack( obj );
    connect( obj, SIGNAL( destroyed( QObject* ) ), this, SLOT( RemoveStdObject( QObject* ) ) );
    if( immediateAdd ) {
        jsInterpreter_->AddObjectToJS( obj->jsInstanceName(), obj );
    }
}

QVariant Context::AddObjToJSContext( Object* obj, bool ownedByJavascript ) {
    jsInterpreter_->AddObjectToJS( obj->jsInstanceName(),
                                   obj,
                                   ownedByJavascript ? QScriptEngine::ScriptOwnership :
                                     QScriptEngine::QtOwnership );
    obj->SetContext( this );
    ConnectErrCBack( obj );
    return jsInterpreter_->EvaluateJavaScript( obj->jsInstanceName() );
}

void Context::AddQObjectToJSContext( QObject* obj, const QString& name, bool ownedByJavascript ) {
    jsInterpreter_->AddObjectToJS( name,
                                   obj,
                                   ownedByJavascript ? QScriptEngine::ScriptOwnership :
                                     QScriptEngine::QtOwnership );
}

void Context::SetNetworkAccessManager( NetworkAccessManager* nam ) { 
    if( nam == netAccessMgr_ && nam != 0 ) return;
    if( netAccessMgr_ != 0 ) {
        disconnect( netAccessMgr_, SIGNAL( UrlAccessDenied( QString ) ),
                    this, SLOT( OnNetworkRequestDenied( QString ) ) );
        disconnect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                    this, SLOT( OnUnauthorizedNetworkAccess() ) );
    }
    netAccessMgr_ = nam;
    if( netAccessMgr_ ) {
        connect( netAccessMgr_, SIGNAL( UrlAccessDenied( QString ) ),
                 this, SLOT( OnNetworkRequestDenied( QString ) ) );
        connect( netAccessMgr_, SIGNAL( UnauthorizedNetworkAccessAttempt() ),
                 this, SLOT( OnUnauthorizedNetworkAccess() ) ); 
    }
}

void Context::RemoveStdObject( QObject* o ) {
    Object* obj = qobject_cast< Object* >( o );
    JScriptObjCtxInstances::iterator i = 
        std::find( jscriptStdObjects_.begin(), jscriptStdObjects_.end(), obj );
    if( i != jscriptStdObjects_.end() ) jscriptStdObjects_.erase( i );
}

void Context::InitJScript() {
	if( addParentObjs_ && parent_ != 0 ) {
	    jsInterpreter_->EvaluateJavaScript( parent_->GetJSInitCode() );
		const bool APPEND_TO_GLOBAL_LOCO_OBJECT = true;
		jsInterpreter_->EvaluateJavaScript( jsInitGenerator_->GenerateCode( APPEND_TO_GLOBAL_LOCO_OBJECT ) );
	} else {
	    jsInterpreter_->EvaluateJavaScript( jsInitGenerator_->GenerateCode() );
	}
}

void Context::RemoveStdObjects() {
    for( JScriptObjCtxInstances::iterator i = jscriptStdObjects_.begin();
        i != jscriptStdObjects_.end(); ++i ) {
        if( (*i)->GetContext() == this && (*i)->GetPluginLoader() == 0 && 
            ( (*i)->parent() == this || (*i)->parent() == 0 ) ) (*i)->destroy();
    }      
    jscriptStdObjects_.clear();
    uriObjectMap_.clear();
}    

void Context::RemoveFilters() {
    filters_.clear();
    ResetNameFilterMap(); 
}
	
void Context::OnJSContextCleared() {
   	AddJavaScriptObjects();
   	InitJScript();
   	emit JSContextCleared();
}
//public slots:
void Context::AddJSStdObjects( IJSInterpreter* jsi ) {
    for( JScriptObjCtxInstances::const_iterator i = jscriptStdObjects_.begin();
         i != jscriptStdObjects_.end(); ++i ) {
        if( (*i)->GetContext() == 0 ) (*i)->SetContext( this );      
        ConnectErrCBack( *i );
        if( (*i)->GetPluginLoader() == 0 && (*i)->parent() == 0 ) (*i)->setParent( this );
        jsi->AddObjectToJS( (*i)->jsInstanceName(), *i );  
    } 
}

QVariant Context::Eval( QString code, const QStringList& filters ) { 
   if( !filters.isEmpty() ) code = ApplyFilter( code, filters );
   if( storeCode_ ) code_ = code;
   if( !error() ) {
       return jsInterpreter_->EvaluateJavaScript( code );
   }
   else {
       jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
       return QVariant();
   } 
}

void Context::LoadFilter( const QString& id, const QString& uri ) {
    if( !fileAccessMgr_->CheckAccess( uri ) ) {
        error( "Access to " + uri + " not allowed" );
        return;
    }
    if( uriFilterMap_.find( uri ) != uriFilterMap_.end() ) return;
    QPluginLoader* pl = new QPluginLoader( uri );
    if( !pl->load() ) {
        error( pl->errorString() );
        jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
        delete pl;
    } else {
        loco::Filter* lf = qobject_cast< loco::Filter* >( pl->instance() );
        if( lf == 0 ) {
            error( "Wrong filter type" );
            jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
            delete pl;    
        }
        filters_[ id ] = lf;
        connect( lf, SIGNAL( onError( const QString& ) ), 
                 this, SLOT( OnFilterError( const QString& ) ) );
        lf->setParent( pl );
        pl->setParent( this );
        uriFilterMap_[ uri ] = lf;
    }      
}

QVariant Context::Insert( const QString& uri, const QStringList& filters ) {
    QString code;
	bool match = false;
    if( filters.isEmpty() && autoMapFilters_ ) {
        for( NameFilterMap::iterator i = nameFilterMap_.begin();
             i != nameFilterMap_.end(); ++i ) {
            if( i->first.exactMatch( uri ) ) {
			    code = Read( uri, i->second );
                match = true;
				break;
            }
        }    
    }
    if( !match ) code = Read( uri, filters );
    if( code.isEmpty() ) return QVariant();
    return Eval( code );
}

QVariant Context::LoadQtPlugin( QString filePath,
		                        QString jsInstanceName,
		                        const QString& initMethodName,
		                        const QVariantMap& params ) {
	if( filePath.startsWith( "~/" ) ) filePath = QDir::homePath() + filePath.remove( 0, 1 );
	QPluginLoader* pluginLoader = new QPluginLoader( filePath );
	if( !pluginLoader ) {
		error( "Cannot load plugin from " + filePath );
		return QVariant();
	}
	QObject* obj = pluginLoader->instance();
	if( !obj ) {
		delete pluginLoader;
		error( "Cannot create object instance from plugin " + filePath );
		return QVariant();
	}
	obj->setParent( pluginLoader );
	const QMetaObject* mo = obj->metaObject();
	const QString initMethodSignature = "QString " + initMethodName + "(QVariantMap)";
	if( mo->indexOfMethod( initMethodSignature.toAscii().constData() ) >= 0 ) {
	    QMetaMethod method = mo->method( mo->indexOfMethod( initMethodSignature.toAscii().constData() ) );
	    QString ret;
	    method.invoke( obj, Qt::DirectConnection,
	    		            Q_RETURN_ARG( QString, ret ),
	          			    Q_ARG( QVariantMap, params ) );
        if( !ret.isEmpty() && ret.toLower() != "ok" ) {
        	pluginLoader->deleteLater();
        	error( ret );
        	return QVariant();
        }
	}
	if( jsInstanceName.isEmpty() ) {
		Object obj;
		jsInstanceName = obj.jsInstanceName();
	}
	AddQObjectToJSContext( pluginLoader, jsInstanceName + "__QPluginLoader", true );
    AddQObjectToJSContext( obj, jsInstanceName, false );
    return jsInterpreter_->EvaluateJavaScript( jsInstanceName );
}

QVariant Context::WrapQObject( QObject* qobj, bool takeOwnership ) {
	Object obj;
	AddQObjectToJSContext( qobj, obj.jsInstanceName(), takeOwnership );
	return jsInterpreter_->EvaluateJavaScript( obj.jsInstanceName() );
}

QVariant Context::Create( const QString& className, const QVariantMap& init ) {
	if( className == "ProtocolHandler" ) {
		return AddObjToJSContext( new ScriptNetworkRequestHandler ); //lifetime managed by Javascript
	}
#ifdef LOCO_WKIT
	else if( className == "JavaScriptCoreContext" ) {
		Context* ctx = new Context();
		JSContext* jsCtx = new JSContext( ctx );
		ctx->setParent( jsCtx ); // javascript interpreter OWNS jsCtx
		                         // jsCtx OWNS ctx
		ctx->Init( new WebKitJSCore(), app_, cmdLine_ );
		return AddObjToJSContext( jsCtx );
	}
#endif
#ifdef LOCO_SCRIPT
	else if( className == "QtScriptContext" ) {
		Context* ctx = new Context();
		JSContext* jsCtx = new JSContext( ctx );
		ctx->setParent( jsCtx ); // javascript interpreter OWNS jsCtx
		                         // jsCtx OWNS ctx
		ctx->Init( new QScriptInterpreter(), app_, cmdLine_ );
		return AddObjToJSContext( jsCtx );
	}
#endif
	else if( className == "ContextThread" ) {
		ContextThread* ct = new ContextThread();
		AddQObjectToJSContext( ct, ct->JSInstanceName() );
		return jsInterpreter_->EvaluateJavaScript( ct->JSInstanceName() );
	} else if( className == "ContextThreadLoop" ) {
		ContextThreadLoop* ct = new ContextThreadLoop();
		AddQObjectToJSContext( ct, ct->JSInstanceName() );
		return jsInterpreter_->EvaluateJavaScript( ct->JSInstanceName() );
	} else if( className == "Float64Array" ) {
		return CreateArray< Float64Array >( *this, init );
	} else if( className == "Float32Array" ) {
		return CreateArray< Float32Array >( *this, init );
	} else if( className == "IntArray" ) {
		return CreateArray< IntArray >( *this, init );
	} else if( className == "UIntArray" ) {
		return CreateArray< UIntArray >( *this, init );
	} else if( className == "ShortArray" ) {
		return CreateArray< ShortArray >( *this, init );
	} else if( className == "UShortArray" ) {
		return CreateArray< UShortArray >( *this, init );
	} else if( className == "ByteArray" ) {
		return CreateArray< ByteArray >( *this, init );
	} else {
		error( "Cannot create object of type " + className );
		return QVariant();
	}
}

void Context::AddNetworkRequestHandler( const QString& scheme, QObject* handler ) {
	INetworkRequestHandler* nrh = dynamic_cast< INetworkRequestHandler*  >( handler );
	if( !nrh ) {
		error( "Invalid request handler type" );
		return;
	} else {
	    netAccessMgr_->AddNetworkRequestHandler( scheme, nrh );
	}
}

void Context::LoadScriptFilter( const QString& id,
		                        const QString& uri,
						        const QString& jfun,
						        const QString& jcode,
                                const QString& jerrfun,
                                const QString& codePlaceHolder ) {
	if( !fileAccessMgr_->CheckAccess( uri ) ) {
		error( "Access to " + uri + " not allowed" );
		return;
	}
	Include( uri );
	Filter* lf = new ScriptFilter( jsInterpreter_, jfun, jcode, jerrfun, codePlaceHolder );
	connect( lf, SIGNAL( onError( const QString& ) ),
			 this, SLOT( OnFilterError( const QString& ) ) );
	filters_[ id ] = lf;
}

QVariant Context::Data( const QVariant& data ) {
	return AddObjToJSContext( new DataType( data ) );
}


}
