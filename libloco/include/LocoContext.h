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


#include <cstdlib> //for dupenv (win), putenv, getenv
#include <stdexcept>

#include <QCoreApplication>

#include <QString>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include <QPluginLoader>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QtNetwork/QNetworkAccessManager>
#include <QSet>
#include <QUrl>
#include <QRegExp>
#include <QPointer>
#include <QMap>
#include <QResource>

#include <algorithm>
#include <cstdlib>

#include "EWL.h"
#include "LocoFilter.h"
#include "LocoObject.h"
#include "LocoConsole.h"
#include "LocoScriptFilter.h"
#include "LocoFileSystem.h"
#include "LocoSystem.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"
#include "LocoObjectInfo.h"
#include "LocoIJSInterpreter.h"
#include "LocoQtApp.h"
#include "LocoNetworkAccessManager.h"


namespace loco {

typedef QMap< QString, Filter* > Filters;
typedef QList< Object* > JScriptObjCtxInstances; 

typedef QMap< QString, Object* > URIObjectMap;
typedef QMap< QString, Filter* > URIFilterMap;

typedef QList< QPair< QRegExp, QStringList > > NameFilterMap;


struct IJavaScriptInit : QObject {
    virtual void SetContext( Context*  ) = 0;
    virtual QString GenerateCode( bool = false ) const = 0; // bool = true append, false generate
    virtual ~IJavaScriptInit() {}
};

typedef QMap< QString, Object* > NamePointerMap;
typedef QMap< Object*, QString > PointerNameMap;


class JSContext;

class Context : public EWL {
    Q_OBJECT  

friend class App; //loco contexts are embedded into an application

public:
    Context( Context* parent = 0 );
    Context( IJSInterpreter* jsi, LocoQtApp* app, const QStringList& cmdLine,
             Context* parent = 0 );
    void Init( IJSInterpreter* jsi, LocoQtApp* app = 0,
               const QStringList& cmdLine = QStringList(), Context* parent = 0 );
// called from C++
public:
    void LoadFilter( const QString& id, Filter* f ) {
        filters_[ id ] = f;
    }
    void ConnectSigToSlot( QObject* srcObj,
                           const QString& sigSignature,
                           QObject* targetObj,
                           const QString& slotSignature ) {
        QObject::connect( srcObj, SignalSignature( sigSignature ).toAscii().constData(),
                          targetObj, SlotSignature( slotSignature ).toAscii().constData() );
    }
    void ConnectSigToSig( QObject* srcObj,
                          const QString& sig1Signature,
                          QObject* targetObj,
                          const QString& sig2Signature ) {
            QObject::connect( srcObj, SignalSignature( sig1Signature ).toAscii().constData(),
                              targetObj, SignalSignature( sig2Signature ).toAscii().constData() );
    }
    void Connect( QObject* srcObj,
                  const QString& sigSignature,
                  QObject* targetObj,
                  const QString& sigOrSlotSignature ) {
                QObject::connect( srcObj, sigSignature.toAscii().constData(),
                                  targetObj, sigOrSlotSignature.toAscii().constData() );
    }
    QString SignalSignature( const QString& sig ) { //returns mangled signature
        QString placeHolder = SIGNAL( () );
        return placeHolder.replace( "()", sig );
    }
    QString SlotSignature( const QString& sig ) { //returns mangled signature
        QString placeHolder = SLOT( () );
        return placeHolder.replace( "()", sig );
    }
    void Yeld() { app_->processEvents(); }
    const QString& LastReadURI() const { return lastReadURI_; }
    bool SetAllowInterrupt( bool yes ) { return jsInterpreter_->SetAllowInterrupt( yes ); }
    bool GetAllowInterrupt() const { return jsInterpreter_->GetAllowInterrupt(); }
    QVariant Data( const QVariant& data, const QString& instanceName = QString() );
    const QString& Code() const { return code_; }
    void SetStoreCode( bool on ) { storeCode_ = on; }
    bool GetStoreCode() const { return storeCode_; }
    void AddNetworkRequestHandler( const QString& scheme, QObject* handler );
    void SetEnableCustomNetRequestHandlers( bool yes ) {
        netAccessMgr_->SetEnableCustomRequestHandlers( yes );
    }
    QVariant Create( const QString& className, const QVariantMap& init = QVariantMap() );
    bool RegisterResources( const QString& path, const QString& rootPath = QString() ) {
        return QResource::registerResource( path, rootPath );
    }
    bool UnRegisterResources( const QString& path, const QString& rootPath = QString() ) {
        return QResource::unregisterResource( path, rootPath );
    }
    void SetAddObjectsFromParentContext( bool yes ) { addParentObjs_ = yes; }
    void SetParentContext( Context* pc ) { parent_ = pc; }
    QString GetJSInitCode() const { return jsInitGenerator_->GenerateCode(); }
    void AddNameFilterMapping( const QRegExp& rx, const QStringList& filterIds ) {
        nameFilterMap_.push_back( qMakePair( rx, filterIds ) );
    }
    void RemoveNameFilterMapping( const QRegExp& rx );
    void ResetNameFilterMap() { nameFilterMap_.clear(); }
    void SetAppInfo( ObjectInfo* ai ) { appInfo_ = ai; }
    // object ownership is transferred to context     
    void AddContextToJS(); 

    void AddJSStdObject( Object* obj, bool immediateAdd = false );    
    QVariant AddObjToJSContext( Object* obj, bool ownedByJavascript = true );
    // this method is intended to insert objects from other contexts
    void AddQObjectToJSContext( QObject* obj, const QString& name, bool ownedByJavascript = false );
    void AddFilter( const QString& id, Filter* f ) { 
        if( f->GetPluginLoader() == 0 && f->parent() == 0 ) f->setParent( this );
        filters_[ id ] = f;
    }
    void SetJSGlobalNameForContext( const QString& n ) {
        globalContextJSName_ = n;
    }
    const QString& GetJSGlobalNameForContext() const {
        return globalContextJSName_;
    }
    void SetJSErrCBack( const QString& code,
                        const QStringList& filterIds = QStringList() ) {
        jsErrCBack_ = ApplyFilter( code, filterIds );
    }
    const QString& GetJSErrCBack() const { return jsErrCBack_; }
    void SetJSInitGenerator( IJavaScriptInit* jsi ) {
        if( jsi == jsInitGenerator_ ) return;
        if( jsInitGenerator_ != 0 ) jsInitGenerator_->deleteLater();
        jsInitGenerator_ = jsi;
        jsInitGenerator_->setParent( this ); 
    }
    IJavaScriptInit* GetJSInitGenerator() const { return jsInitGenerator_; }
    const JScriptObjCtxInstances& GetStdJSObjects() const { return jscriptStdObjects_; }

    void SetNetworkAccessManager( NetworkAccessManager* nam );
    QNetworkAccessManager* GetNetworkAccessManager() const { return netAccessMgr_; }
    void SetFileAccessManager( FileAccessManager* fm ) { fileAccessMgr_ = fm; }
    const FileAccessManager* GetFileAccessManager() const { return fileAccessMgr_; }
    void SetMaxRedirections( int r ) { maxNetRedirections_ = r; }
    int GetMaxRedirection() const { return maxNetRedirections_; }
    void SetNetReadTimeout( int ms ) { readNetworkTimeout_ = ms; }
    int GetNetReadTimeout() const { return readNetworkTimeout_; }
    void Exit( int r ) { app_->exit( r ); exit( r ); }
    void SetJSContextName( const QString& n ); 
    void Quit() { app_->quit(); }
    QString JSInterpreterName() const { return jsInterpreter_->Name(); }
    //adds both a hidden QPluginLoader wrapper and an object created through QPluginLoader::instance()
    //the object created through ::instance() method is set to be the child of the QPluginLoader instance.
    //The QPluginLoader lifetime is handled by the JS interpreter i.e. it dies when the JS wrapper dies.
    //If the object has a QString Init(QVariantMap) method then such method is invoked with the parameters
    //stored in 'params' before the object is added to the javascript interpreter.
    QVariant LoadQtPlugin( QString filePath,
                           QString jsInstanceName = QString(),
                           const QString& initMethodName = "Init",
                           const QVariantMap& params = QVariantMap() );
    QVariant WrapQObject( QObject* obj, bool takeOwnership = true );
 // attched to internal signals            
private slots:
   
    void RemoveStdObject( QObject* o );
    void OnUnauthorizedNetworkAccess() {
        error( "Unauthorized network access attempted" );
    }    
    void OnNetworkRequestDenied( QString url ) {
        error( "Unauthorized access to " + url + " attempted" ); 
    } 
    void InitJScript();

    /// Add javascript objects that need to be available at initialization time
    void AddJavaScriptObjects() {
        if( addParentObjs_ && parent_ != 0  ) {
            parent_->AddJSStdObjects( jsInterpreter_ );
        }
        AddJSStdObjects( jsInterpreter_ );
    }
    void RemoveStdObjects();
    void RemoveFilters();   
    void OnJSContextCleared();
signals:
    void JSContextCleared();
    void JavaScriptConsoleMessage( const QString&, int, const QString& );
public slots:
    // loco::Objects should be connected to this slot to have errors handled by the context
    // and indirectly by javascript
    // an object RaiseError signal must be connected to one and only one context instance
    void OnObjectError( const QString& err ) {
        error( err );
    }
    void OnFilterError( const QString& err ) {
        error( err );
    }
    // connect to this from e.g. loco::App
    void OnExternalError( const QString& err ) {
        error( err );
    }
    // allow to add objects to other contexts
    void AddJSStdObjects( IJSInterpreter* jsi );

private slots:
    void OnSelfError( const QString& err ) {
        if( !jsInterpreter_.isNull() ) jsInterpreter_->EvaluateJavaScript( jsErrCBack_ );
        else throw std::runtime_error( err.toStdString() );
    }

private:
    
    void ConnectErrCBack( Object* obj );

friend class JSContext;

public: 
    // Eval needs to be accessible for starting script execution
    QVariant Eval( QString code, const QStringList& filters = QStringList() );
    ObjectInfo* GetAppInfo() const { return appInfo_; }
    // DataT = QByteArray OR QString
    template < typename DataT >
    DataT ApplyFilter( DataT data, const QStringList& filterIds = QStringList() ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            Filters::iterator i = filters_.find( *f );
            if( i != filters_.end() ) {
                Filter* fp =  i.value();
                data = fp->Apply( data );
                if( fp->error() ) break;                 
            }
            else {
                error( "filter id " + *f + " not found" );
                break;
            }
        }
        return error() ? DataT() : data; 
    }

private:

    void AddToIncludePath( const QString& p ) { includePath_ << p; }    
    void SetIncludePath( const QStringList& sl ) { includePath_ = sl; }
    const QStringList& GetIncludePath() const { return includePath_; }
    QVariant LoadObject( const QString& uri,  //used as a regular file/resource path for now
                         bool persistent = false );
#ifdef LOCO_GUI
    int Exec() { return app_->exec(); }
#endif
    void LoadFilter( const QString& id, const QString& uri );    
    bool HasFilter( const QString& id ) { return filters_.find( id ) != filters_.end(); }
    void AddScriptFilter( const QString& id,
                          const QString& jfun,
                          const QString& jcode = "",
                          const QString& jerrfun = "",
                          const QString& codePlaceHolder = "" ) {
        Filter* lf = new ScriptFilter( jsInterpreter_.data(), jfun, jcode, jerrfun, codePlaceHolder );
        connect( lf, SIGNAL( onError( const QString& ) ), 
                                      this, SLOT( OnFilterError( const QString& ) ) );
        filters_[ id ] = lf;
    }
    void LoadScriptFilter( const QString& id,
                           const QString& uri,
                           const QString& jfun,
                           const QString& jcode = "",
                           const QString& jerrfun = "",
                           const QString& codePlaceHolder = "" );
    QStringList CmdLine() const { return cmdLine_; }
    void RegisterJSErrCBack( const QString& code, const QStringList& filters = QStringList() ) { 
        jsErrCBack_ = ApplyFilter( code, filters );
    }
    QByteArray Read( const QString& uri, const QStringList& filters = QStringList() ) {
        // resources: ':/' -> file; 'qrc://' -> url
        QByteArray ret = uri.contains( "://" ) ? ReadUrl( uri ) : ReadFile( uri );
        if( !ret.isEmpty() ) return ApplyFilter( ret, filters );
        return ret;
    }
    QVariant Insert( const QString& uri, const QStringList& filters = QStringList() );

    QVariant Include( QString uri, const QStringList& filters = QStringList() ) {
        if( includeSet_.find( uri ) != includeSet_.end() ) return QVariant();
        includeSet_.insert( uri );
        return Insert( uri, filters );
    } 
    void SetAutoMapFilters( bool on ) { autoMapFilters_ = on; }
    void SuppressQtMessages( bool on ) {
        if( on ) qInstallMsgHandler( NullQtMessageHandler );
        else qInstallMsgHandler( 0 );
    }
    
private:
    static void NullQtMessageHandler( QtMsgType , const char* ) {} 
    QByteArray ReadUrl( const QString& url, QSet< QUrl > redirects = QSet< QUrl >() );
    QByteArray ReadFile( QString f );
    void jsErr() { jsInterpreter_->EvaluateJavaScript( jsErrCBack_ ); }

private:
    JSContext* jsContext_;
    QPointer< IJSInterpreter > jsInterpreter_;
    QPointer< LocoQtApp > app_;
    QPointer< Context > parent_;
    QStringList cmdLine_;
   
private:
    Filters filters_;
    JScriptObjCtxInstances jscriptStdObjects_;
    QString jsErrCBack_;
    URIObjectMap uriObjectMap_;
    URIFilterMap uriFilterMap_;
    QString globalContextJSName_;
    NameFilterMap nameFilterMap_;
    bool autoMapFilters_;
    bool addParentObjs_;
private: 
    IJavaScriptInit* jsInitGenerator_; 
    QPointer< NetworkAccessManager > netAccessMgr_;
    QPointer< FileAccessManager > fileAccessMgr_;
private:
    QSet< QString > includeSet_;
private:
    int readNetworkTimeout_;
    int maxNetRedirections_;
    QPointer< ObjectInfo > appInfo_;
    QStringList includePath_;
    QString code_;
    bool storeCode_;
    QString lastReadURI_; // identifier of last read file or URL
};

}
