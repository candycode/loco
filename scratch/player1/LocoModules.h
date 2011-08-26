#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>
#include <QMap>
#include "LocoTextFilter.h"

namespace loco {

typedef TextFilter* TextFilterPtr;

class Environment : public QObject {
    Q_OBJECT
public:
    Environment( WebFramePtr wf = 0,
                 bool addSelf = true,
                 const QString& jsName = "loco_env__" ) 
        : wf_( wf ), jsName_( jsName ), addSelf_( addSelf ) {
        if( wf != 0 ) Connect();
    } 
    void SetWebFrame( QWebFrame* wf ) { wf_ = wf; if( wf_ != 0 ) Connect(); }
    const WebFramePtr GetWebFrame( wf }
	void AddObject( QObject* obj, const QString& jscriptName ) {
        objMap_[ jscriptName ] = obj;
	}
    void InitCode( const QString& code, int pos, const QString& filterId = "" ) {
		codeInitMap_[ pos ] = code;
	}
	void PreLoadCode( const QString& code, int pos, const QString& filterId = "" ) {
		codePreLoadMap_[ pos ] = filter( code, filterId );
	}
    void PostLoadCode( const QString& code, int pos, const Qstring& filterId = "" ) {
        codePostLoadMap_[ pos ] = filter( code, filterId );
    }
    void SetFilter( TextFilterPtr f, const QString& id ) { filterMap_[ id ] = f; }
public slots:
// bool createObject( const QString& dllUrl,
//	                  const QString& ifaceName,
//                    const QString& jscriptName,
//		              bool shared ); // when true if created simply assign the name to the same object in jscript
// bool objectLoaded( const QString& ifaceName );
// bool releaseObject( jscriptName );
	void addObjectsToJScriptContext() {
        
        if( addSelf_ ) wf_->addToJavaScriptWindowObject( this, jsName_ );
	    for( ObjectMap::const_iterator i = objMap_.begin();
		     i != objMap_.end(); ++i ) {
            wf_->addToJavaScriptWindowObject( i->second, i->first );
        }
        for( CodeMap::const_iterator i = codeInitMap_.begin();
		     i != codeInitMap_.end(); ++i ) {
            wf_->evaluateJavaScript( i->second );
        }    
           
    }

    QString getLastError() const { return errors_.back(); }
    QString getLastWarning() const { return warnings_.back(); }
    QString getLastLog() const { return logs_.back(); }
    void error( const QString& emsg ) { errors_.push_back( emsg ); }
    void warn( const QString& wmsg ) { warnings_.push_back( wmsg ); }
    void log( const QString& lmsg ) { logs_.push_back( lmsg ); }
    void clearErrors() { errors_.clear(); }
    void clearWarnings() { errors_.clear(); }
    void clearLogs() { logs_.clear(); }
    //void saveErrors( const QString& fileName ) {}
    //void saveWarnings( const QString& fileName ) {}
    //void saveLogs( const QString& fileName ) {}            
        

    void include( const QString& path ) {
        if( path.contains( "://" ) ) includeUrl( path );
        else includeFile( path );
    }    

    QString filter( const QString& text, const QString& filterId ) {
        FilterMap::iterator i = filterMap_.find( filterId );
        if( i == filterMap_.end() ) return "";
        return filterMap_[ filterId ].Filter( text );
    }
    void preLoadCode() {
         for( CodeMap::const_iterator i = codePreLoadMap_.begin();
		     i != codePreLoadMap_.end(); ++i ) {
            wf_->evaluateJavaScript( i->second );
        }    
    }    
    void postLoadCode( bool ok ) {
        if( !ok ) return;
        for( CodeMap::const_iterator i = codePostLoadMap_.begin();
		     i != codePostLoadMap_.end(); ++i ) {
            wf_->evaluateJavaScript( i->second );
        } 
    }    
private:
    void Connect() {
        connect( wf_.data(), SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addObjectsToJScriptContext() ) );
        connect( wf_.data(), SIGNAL( loadStarted() ), this, SLOT( preLoadCode() ) );
        connect( wf_.data(), SIGNAL( loadFinished( bool ) ), this, SLOT( postLoadCode( bool ) ) ); 
    }
    void includeUrl( const QString& ) {}
    void includeFile( const QString& fp ) {
        QFile f( fp );
        f.open( QIODevice::ReadOnly );
        QString s = f.readAll();
        f.close();
        wf_->evaluateJavaScript( s );
    }
    //void includeUrl( const QString& url ) {
    //    //QNetworkRequest r( QUrl( url ) );
    //    //connect(reply_, SIGNAL( finished( QNetworkReply ), this, SLOT( finished.... 
    //    //reply_ = nam_->get();
    //}
private:
	typedef QMap< int, QString > CodeMap;
	typedef QMap< QString, QObject* > ObjectMap;
    typedef QMap< QString, TextFilterPtr > FilterMap;
	WebFramePtr wf_;
    const QString jsName_;
    bool addSelf_;
    CodeMap codePreLoadMap_;
    CodeMap codePostLoadMap_;
    CodeMap codeInitMap_;
    FilterMap filterMap_; 
	ObjectMap objMap_;
};
}