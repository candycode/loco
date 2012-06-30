#pragma once
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

#include "LocoContext.h"

namespace loco {

// wraps Context exposing a subset of the features
class JSContext : public Object {
    Q_OBJECT
    Q_PROPERTY( QString code READ code )
    Q_PROPERTY( bool storeCode READ storeCode WRITE storeCode )
    Q_PROPERTY( QStringList commandLine READ cmdLine )
    Q_PROPERTY( QStringList args READ cmdLine )
public:
    JSContext( Context* ctx ) : Object( 0, "LocoContext", "/Loco/Context" ),
    ctx_( ctx )  {
        SetDestroyable( false );
        connect( ctx_, SIGNAL( JavaScriptConsoleMessage( const QString&, int, const QString& ) ),
                 this, SIGNAL( javaScriptConsoleMessage( const QString&, int, const QString& ) ) );
        connect( ctx_, SIGNAL( JSContextCleared() ), this, SIGNAL( javaScriptContextCleared() ) );
    }
    Context* GetContext() { return ctx_; } //used only within the library, not exposed to JS
    //property accessors
    const QString& code() const { return ctx_->Code(); }
    void storeCode( bool on ) { ctx_->SetStoreCode( on ); }
    bool storeCode() const { return ctx_->GetStoreCode(); }
// invokable from javascript
public slots: // js interface
    void suppressQtMessages( bool on ) { ctx_->SuppressQtMessages( on ); }
    void connectSigToSlot( QObject* srcObj,
                           const QString& sigSignature,
                           QObject* targetObj,
                           const QString& slotSignature ) {
        ctx_->ConnectSigToSlot( srcObj, sigSignature, targetObj, slotSignature );
    }
    QVariant wrapQObject( QObject* qobj, bool takeOwnership = true ) { return ctx_->WrapQObject( qobj, takeOwnership ); } 
    QString signalSignature( const QString& sig ) { return ctx_->SignalSignature( sig ); }
    QString slotSignature( const QString& sig ) { return ctx_->SlotSignature( sig ); }
    void yeld() { ctx_->Yeld(); }
    int netReadTimeout() const { return ctx_->GetNetReadTimeout(); }
    void setNetReadTimeout( int timeout ) { ctx_->SetNetReadTimeout( timeout ); }
    QString uri() const { return ctx_->LastReadURI().isEmpty() ? appDir() :  ctx_->LastReadURI(); }
    bool setAllowInterrupt( bool yes ) { return ctx_->SetAllowInterrupt( yes ); }
    bool getAllowInterrupt() const { return ctx_->GetAllowInterrupt(); }
    QVariant data( const QVariant& d = QVariant(), const QString& instanceName = QString() ) const  { 
        return ctx_->Data( d, instanceName ); 
    }
    void kill() {
        ctx_->Eval( this->jsInstanceName() + "= undefined"  );
        ctx_->deleteLater();
        ctx_ = 0;
        this->deleteLater();
    }
    void addObject( QObject* obj, const QString& jsInstanceName, bool own = false ) {
        ctx_->AddQObjectToJSContext( obj, jsInstanceName, own );
    }
    void addNewObject( QObject* qobj, const QString& jsInstanceName ) {
        Object* obj = dynamic_cast< Object* >( qobj );
        if( !obj ) {
            error( "loco::Object instance required" );
            return;
        }
        if( !obj->cloneable() ) {
            error( "Object cannot be copied" );
            return;
        }
        obj = obj->Clone();
        obj->SetJSInstanceName( jsInstanceName );
        ctx_->AddObjToJSContext( obj );
    }
    void addProtocolHandler( const QString& scheme, QObject* handler ) {
        ctx_->AddNetworkRequestHandler( scheme, handler );
    }
    void setEnableCustomProtocolHandlers( bool yes ) { ctx_->SetEnableCustomNetRequestHandlers( yes ); }
    QVariant create( const QString& className, const QVariantMap& init = QVariantMap() ) {
        return ctx_->Create( className, init );
    }
    QVariant loadQtPlugin( QString filePath,
                           const QString& jsInstanceName = QString(),
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

