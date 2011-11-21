#pragma once
//#SRCHEADER
#include "LocoContext.h"

namespace loco {

// wraps Context exposing a subset of the features
class JSContext : public Object {
    Q_OBJECT
    Q_PROPERTY( QString code READ code )
    Q_PROPERTY( bool storeCode WRITE storeCode )
public:
    JSContext( Context* ctx ) : Object( 0, "LocoContext", "Loco/Context" ),
    ctx_( ctx )  {
        SetDestroyable( false );
		connect( ctx_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
			     this, SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ) );
		connect( ctx_, SIGNAL( JSContextCleared() ), this, SIGNAL( javaScriptContextCleared() ) );
    }
    const QString& code() const { return ctx_->Code(); }
    void storeCode( bool on ) { ctx_->SetStoreCode( on ); }
// invocable from javascript
public slots: // js interface
    void kill() {
    	ctx_->Eval( this->jsInstanceName() + "= undefined"  );
    	ctx_->deleteLater();
    	ctx_ = 0;
    	this->deleteLater();
    }
    void addObject( QObject* obj, const QString& jsInstanceName, bool own = false ) {
    	ctx_->AddQObjectToJSContext( obj, jsInstanceName, own );
    }
    void addProtocolHandler( const QString& scheme, QObject* handler ) {
    	ctx_->AddNetworkRequestHandler( scheme, handler );
    }
    void setEnableCustomProtocolHandlers( bool yes ) { ctx_->SetEnableCustomNetRequestHandlers( yes ); }
    QVariant create( const QString& className ) { return ctx_->Create( className ); }
    QVariant loadQtPlugin( QString filePath,
                           const QString& jsInstanceName,
                           const QString& initMethodName = "Init",
                           const QVariantMap& params = QVariantMap() ) {
    	return ctx_->LoadQtPlugin( filePath, jsInstanceName, initMethodName, params );
    }
	bool registerResources( const QString& path, const QString& rootPath = QString() ) {
		return ctx_->RegisterResources( path, rootPath );
	}
	bool unregisterResources( const QString& path, const QString& rootPath = QString() ) {
		return ctx_->UnRegisterResources( path, rootPath );
	}
    void enableAutoMapFilters( bool on ) { ctx_->SetAutoMapFilters( on ); }
    QString jsInterpreterName() const { return ctx_->JSInterpreterName(); }
    bool setNetworkAuthentication( const QString& user, const QString& pwd ) {
    	NetworkAccessManager* nam = qobject_cast< NetworkAccessManager* >( ctx_->GetNetworkAccessManager() );
    	if( !nam ) return false;
    	nam->SetDefaultAuthenticator( user, pwd );
    	return true;
    }
    bool setDefaultSSLExceptionHandler() {
    	NetworkAccessManager* nam = qobject_cast< NetworkAccessManager* >( ctx_->GetNetworkAccessManager() );
    	if( !nam ) return false;
    	nam->SetDefaultSSLExceptionHandler();
    	return true;
    }
    void setIncludePath( const QString& ip ) { setIncludePath( ip.split( ":" ) ); }
    void setIncludePath( const QStringList& ip ) { ctx_->SetIncludePath( ip ); }
    void addToIncludePath( const QString& p ) { ctx_->AddToIncludePath( p ); }
    QStringList includePath() const { return ctx_->GetIncludePath(); }
	void exit( int code ) { ctx_->Exit( code ); }
	QVariant include( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_->Include( path, filters );
    }
    QVariant insert( const QString& path, const QStringList& filters = QStringList() ) {
        return ctx_->Insert( path, filters );
    }
	QString read( const QString& path, const QStringList& filters = QStringList() ) {
	    return ctx_->Read( path, filters );
    }
	QStringList pluginPath() const { return QCoreApplication::libraryPaths(); }
	void setPluginPath( const QStringList& paths ) { QCoreApplication::setLibraryPaths( paths ); }
    void addPluginPath( const QString& p ) { QCoreApplication::addLibraryPath( p ); }
	QString appFilePath() const { return QCoreApplication::applicationFilePath(); }
	QString appDir() const { return QCoreApplication::applicationDirPath(); }
	QString appName() const { return QCoreApplication::applicationName(); }
	QString appVersion() const { return QCoreApplication::applicationVersion(); }
	QString appVendor() const { return QCoreApplication::organizationName(); }
	QString curDir() const { return QDir::current().absolutePath(); }
	QString homeDir() const { return QDir::home().absolutePath(); }
    QVariantMap appInfo() const {
        return ctx_->GetAppInfo() ? ctx_->GetAppInfo()->ToVariantMap()
                                 : QVariantMap();
    }
	void quit() { ctx_->Quit(); }
    // IMPORTANT: this sets the paths associated with a specific resource tag
    // e.g. "images" "$home/images"
    // allows to specify filenames as "images:mypicture.jpg" which gets
    // translated to "$home/images/mypicture.jpg"
    void setSearchPaths( const QString& prefix, const QStringList& paths ) {
        QDir::setSearchPaths( prefix, paths );
    }
    QStringList searchPaths( const QString& prefix ) { return QDir::searchPaths( prefix ); }
    QVariant eval( QString code, const QStringList& filters = QStringList() ) {
        return ctx_->Eval( code, filters );
    }
    QVariant loadObject( const QString& uri,  //used as a regular file path for now
                         bool persistent = false ) { return ctx_->LoadObject( uri, persistent ); }
    QString filter( QString code, const QStringList& filterIds = QStringList() ) {
        return ctx_->ApplyFilter( code, filterIds );
    }
#ifdef LOCO_GUI
    int eventLoop() { return ctx_->Exec(); }
#endif
    QVariant evalFile( const QString& uri, const QStringList& filterIds = QStringList() ) {
		return ctx_->Include( uri, filterIds );
	}
    void loadFilter( const QString& id, const QString& uri ) {
        ctx_->LoadFilter( id, uri );
    }
    bool hasFilter( const QString& id ) { return ctx_->HasFilter( id ); }
    void addScriptFilter( const QString& id,
                          const QString& jfun,
						  const QString& jcode = "",
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        ctx_->AddScriptFilter( id, jfun, jcode, jerrfun, codePlaceHolder );
    }
	void loadScriptFilter( const QString& id,
		                   const QString& uri,
                           const QString& jfun,
                           const QString& jerrfun = "",
                           const QString& codePlaceHolder = "" ) {
        ctx_->LoadScriptFilter( id, uri, jfun, jerrfun, codePlaceHolder );
    }
    QStringList cmdLine() const { return ctx_->CmdLine(); }
    QString env( const QString& envVarName ) const {
#if !defined( Q_WS_WIN )
        return ::getenv( envVarName.toAscii().constData() );
#else
        char* p = 0;
        size_t len = 0;
        errno_t err = _dupenv_s( &p, &len, envVarName.toStdString().c_str() );
        if( err ) return "";
        QString ret( p );
        free( p );
        return ret;
#endif
    }
    void registerErrCBack( const QString& code, const QStringList& filterIds = QStringList() ) {
        ctx_->RegisterJSErrCBack( code, filterIds );
    }
    QString qtVersion() const { return qVersion(); }
    QString os() const {
        #if defined( Q_OS_MAC )
            return "MAC";
        #elif defined( Q_OS_WIN )
            return "WINDOWS";
        #elif defined( Q_OS_LINUX )
            return "LINUX";
        #elif defined( Q_OS_UNIX )
            return "UNIX";
        #else
            return "";
        #endif
    }
    QVariantMap env() const {
        QProcessEnvironment pe = QProcessEnvironment::systemEnvironment();
        QStringList e = pe.toStringList();
        QVariantMap vm;
        for( QStringList::const_iterator i = e.begin(); i != e.end(); ++i ) {
            QStringList kv = i->split( "=" );
            vm[ *kv.begin() ] = *( ++kv.begin() );
        }
        return vm;
    }
signals:
	void javaScriptConsoleMessage( const QString&, int, const QString& );
	void javaScriptContextCleared();
private slots:
    //forward errors received from Context,
    friend class Context;
    void ForwardError( const QString& err ) { error( err ); }


private:
    QPointer< Context > ctx_;

};

}
