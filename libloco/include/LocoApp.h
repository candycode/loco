#pragma once
//SRCHEADER

#include <stdexcept>

#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>
#include <QPair>
#include <QListIterator>
#include <QPointer>
#include <QRegExp>
#ifdef LOCO_GUI
#include <QMessageBox>
#endif

#include "LocoObject.h"
#include "LocoContext.h"
#include "LocoNetworkAccessManager.h"
#include "LocoFileAccessManager.h"     
#include "LocoObjectInfo.h" //reusing this for app info
#include "LocoQtApp.h"

namespace loco {

typedef QMap< QString, Object* > ModuleMap;
typedef QStringList CMDLine;
typedef QList< QPair< QRegExp, QString > > DocHandlers;

class App : public QObject {
    Q_OBJECT
private:
    enum RuleType { NetDeny, NetAllow, FileDeny, FileAllow };
public:
    App( LocoQtApp& app, int argc, char** argv, ObjectInfo* oi ); 

    void AddResources( const QString& path ) {
        QResource::registerResource( path );
    }

    void MapToFilters( const QRegExp& rx, const QStringList& filterIds ) {
        ctx_.AddNameFilterMapping( rx, filterIds );
    }

    bool GetEventLoopEnable() const { return startEventLoop_; }

    void SetEventLoopEnable( bool yes ) { startEventLoop_ = yes; }

    void SetInterpreter( IJSInterpreter* i );
    
    void SetMapFiltersToFileNames( bool on ) { ctx_.SetAutoMapFilters( on ); }

    void AddModuleToJS( Object* obj ) {
        ctx_.AddJSStdObject( obj ); // context owns module
    }

    void AddContextToJS() { //adds context to the list of objects accessible from javascript
        ctx_.AddContextToJS();
    }
    
    const QVariant& GetResult() const { return execResult_; }
    
    void SetAllowNetAccess( bool na ) { netAccess_.SetAllowNetAccess( na ); }

    void SetFilterNetRequests( bool fr ) { netAccess_.SetFilterRequests( fr ); }

    void SetNetRuleFormat( QRegExp::PatternSyntax ps ) { netAccess_.SetRxPatternSyntax( ps ); }

    void AddAllowNetRule( const QRegExp& rx ) { netAccess_.AddAllowRule( rx ); }

    void AddDenyNetRule( const QRegExp& rx ) { netAccess_.AddDenyRule( rx ); }

    void AddNetUrlMapping( const QRegExp& rx, const QString& url ) { netAccess_.AddRedirMapping( rx, url ); }

    void SetAllowFileAccess( bool fa ) { fileAccess_.SetAllowFileAccess( fa ); }

    void SetFilterFileAccess( bool fr ) { fileAccess_.SetFilterAccess( fr ); }

    void SetFileRuleFormat( QRegExp::PatternSyntax ps ) { fileAccess_.SetRxPatternSyntax( ps ); }

    void AddAllowFileRule( const QRegExp& rx, QIODevice::OpenMode mode = QIODevice::ReadWrite ) { fileAccess_.AddAllowRule( rx, mode ); }

    void AddDenyFileRule( const QRegExp& rx, QIODevice::OpenMode mode = QIODevice::ReadWrite ) { fileAccess_.AddDenyRule( rx, mode ); }

    void SetSignalNetAccessDenied( bool yes ) { netAccess_.SetSignalAccessDenied( yes ); }

    void SetDefaultNetUrl( const QString& durl ) { netAccess_.SetDefaultUrl( durl ); }

    void SetDefaultScript( const QString& ds ) { defaultScript_ = ds; }

    void ParseCommandLine();

    void InitContext() {
        ctx_.Init( jsInterpreter_, &app_, cmdLine_ );
    }
    void SetScriptFileNameMatchingExpression( const QRegExp& rx )  { scriptNameRX_ = rx; }
    int Execute( bool forceDefault = false );
    int Execute( const QString& code, const QStringList& filters = QStringList() );
    void ConfigNetAccessFromFile( const QString& deny,
                                  const QString& allow ) {
        ReadRules( deny, NetDeny );
        ReadRules( allow, NetAllow );
    }
    void ConfigFileAccessFromFile( const QString& deny,
                                   const QString& allow ) {
        ReadRules( deny, FileDeny );
        ReadRules( allow, FileAllow );
    }
    void AddNameFilterMapping( const QRegExp& rx, const QStringList& filterIds ) {
        ctx_.AddNameFilterMapping( rx, filterIds );
    }
    void RemoveNameFilterMapping( const QRegExp& rx ) {
        ctx_.RemoveNameFilterMapping( rx );
    }
    void PreloadFilter( const QString& id, const QString& uri ) {
        ctx_.LoadFilter( id, uri );
    }
    void PreloadScriptFilter( const QString& id,
                              const QString& uri,
                              const QString& jfun,
                              const QString& jcode = "",
                              const QString& jerrfun = "",
                              const QString& codePlaceHolder = "" ) {
        ctx_.LoadScriptFilter( id, uri, jfun, jcode, jerrfun, codePlaceHolder );
    }
    void PreloadObject( const QString& uri ) {
        const bool PERSISTENT = true;
        ctx_.LoadObject( uri, PERSISTENT );
    }
    void PreloadQtPlugin( QString filePath,
                          QString jsInstanceName,
                          const QString& initMethodName = "Init",
                          const QVariantMap& params = QVariantMap() ) {
        ctx_.LoadQtPlugin( filePath, jsInstanceName, initMethodName, params );
    }
    void SetDocHandler( const QRegExp& rx, const QString& scriptURI ) {
        docHandlers_.push_back( qMakePair( rx, scriptURI ) );
    }
signals:
    void OnException( const QString& );
private slots:
    void OnJavaScriptConsoleMessage( const QString& t, int l, const QString& s );
private:
    void ReadRules( const QString& fname, RuleType target );

private:
    CMDLine cmdLine_;
    LocoQtApp& app_;
    IJSInterpreter* jsInterpreter_;
    Context ctx_;
    QString defaultScript_;
    mutable QString scriptFileName_; 
    QPointer< ObjectInfo > info_;
    QString helpText_;
    QVariant execResult_;
    NetworkAccessManager netAccess_;
    FileAccessManager fileAccess_;
    bool startEventLoop_;
    QRegExp scriptNameRX_;
    DocHandlers docHandlers_;
};
}
