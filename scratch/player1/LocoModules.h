#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>
#include <QMap>
#include "LocoTextFilter.h"
namespace loco {
// errors warnings logs
class EWL {
	Q_OBJECT
public slots:
    QString lastError() const { return errors_.back(); }
    QString lastWarning() const { return warnings_.back(); }
    QString lastLog() const { return logs_.back(); }
    void error( const QString& emsg ) { errors_.push_back( emsg ); }
    void warn( const QString& wmsg ) { warnings_.push_back( wmsg ); }
    void log( const QString& lmsg ) { logs_.push_back( lmsg ); }
    void clearErrors() { errors_.clear(); }
    void clearWarnings() { errors_.clear(); }
    void clearLogs() { logs_.clear(); }
    const QStringList& errors() const { return errors_; }
    const QStringList& warnings() const { return warnings_; }
    const QStringList& logs() const { return logs_; }
    //void saveErrors( const QString& fileName ) {
    //    QFile of( fileName );
    //    of.open( QIODevice
    //}
    //void saveWarnings( const QString& fileName ) {}
    //void saveLogs( const QString& fileName ) {}
	//void sendErrors( const QString& url ) {}
    //void sendWarnings( const QString& url ) {}
    //void sendLogs( const QString& url ) {}
	//void sendError( const QString& emsg ) { errors_.push_back( emsg ); }
    //void sendWarn( const QString& wmsg ) { warnings_.push_back( wmsg ); }
    //void sendLog( const QString& lmsg ) { logs_.push_back( lmsg ); }
private:
    typedef QStringList Errors;
	typedef QStringList Warnings;
	typedef QStringList Logs;
	Errors errors_;
	Warnings warnings_;
	Logs logs_;
};
}

namespace loco {

struct Filter : EWL {
    const QString& Filter( const QString& ) = 0;
	bool Error() const = 0;
	~Filter() = 0 {}
};

class ScriptFilter : public Filter {
public:
	ScriptFilter( WebFramePtr wf,
		          const QString& s,
				  const QString& jfun,
				  const QString& jerrfun = "",
				  const QString& codePlaceHolder = "",
				  const QString& type = "" ) 
		: s_( s ), wf_( wf ), jfun_( jfun ),
		  jerrfun_( jerrfun ), codePlaceHolder_( codePlaceHolder ), 
		  type_( type )
	{}
	void SetWF( WebFramePtr wf ) { wf_ = wf; }
	WebFramePtr GetWF() const { return wf_; }
	void SetScript( const QString& s ) { s_ = s; }
	const QString& GetScript() { return s_; }
	void SetType( const QString& t ) { type_ = t; }
	const QString& GetType() const { return type_; }
	void SetJFun( const QString& f ) { jfun_ = f; }
	const QString& GetJFun() const { return jfun_; }
	void SetJErrFun( const QString& f ) { jerrfun_ = f; }
	const QString& GetJErrFun() const { return jerrfun_; }
public:
	const QString& Filter( const QString& s ) {
		err_ = true;
		QVariant r;
		if( codePlaceHolder_.isEmpty() ) {
			r = wf_->evaluateJavaScript( jfun + '(' + s + ");" );
		}
		else {
			QString nj = jfun_;
			const QString& njref = nj.replace( placeHolder_, s );
			r = wf_->evaluateJavaScript( njref );
		}
		if( r.isNull() || !r.isValid() ) {
			QVariant e = wf_->evaluateJavaScript( jerrfun );
			if( !e.isNull() && e.isValid() ) error( e.toString() );
			return s;
		}
		err_ = false;
		return r.toString();
	}
	bool Error() const { return err_; }
private:
    QString s_;
	WebFramePtr wf_;
	QString jfun_;
	QString jerrfun_;
	QString codePlaceHolder_;
	QString type_;
	mutable bool err_;
};


}

namespace loco {

typedef Filter* FilterPtr;
typedef QWebFramePtr WebFramePtr;

class Environment : public QObject, public EWL {
    Q_OBJECT
public:
    Environment( WebFramePtr wf = 0,
                 bool addSelf = true,
                 const QString& jsName = "loco_env__" ) 
        : wf_( wf ), jsName_( jsName ), addSelf_( addSelf ) {
        if( wf != 0 ) Connect();
    } 
    void SetWebFrame( QWebFramePtr wf ) { wf_ = wf; if( wf_ != 0 ) Connect(); }

	WebFramePtr GetWebFrame() const { return wf_; }

	void AddObject( QObject* obj, const QString& jscriptName, bool immediateAdd = false ) {
        if( obj->parent == 0 ) obj->setParent( this );
		objMap_[ jscriptName ] = obj;
		if( immediateAdd ) wf_->addJavaScriptWindowObject( obj, jscriptName );
	}
    bool AddInitCode( const QString& code, int pos, const QStringList& filters = QStringList() ) {
		bool ok = true;
		if( !filters.empty() ) {
		    QString c = code;
			for( QStringList::iterator i = filters.begin(); i != filters.end, ++i ) {
			    c = i->Filter( c );
				if( i->Error() ) {
					error( i->getLastError() );
					ok = false;
					break;
				}
			}
			if( !ok ) return false;
			codeInitMap_[ pos ] = c;
		} else codeInitMap_[ pos ] = code;
		return true;
	}
	bool AddPreLoadCode( const QString& code, int pos, const QStringList& filters = QStringList() ) {
		bool ok = true;
		if( !filters.empty() ) {
		    QString c = code;
			for( QStringList::iterator i = filters.begin(); i != filters.end; ++i ) {
			    c = i->Filter( c );
				if( i->Error() ) {
					error( i->getLastError() );
					ok = false;
					break;
				}
			}
			if( !ok ) return false;
			codePreLoadMap_[ pos ] = c;
		} else codePreLoadMap_[ pos ] = code;
		return true;
	}
    void AddPostLoadCode( const QString& code, int pos, const QStringList& filters = QStringList() ) {
        bool ok = true;
		if( !filters.empty() ) {
		    QString c = code;
			for( QStringList::iterator i = filters.begin(); i != filters.end; ++i ) {
			    c = i->Filter( c );
				if( i->Error() ) {
					error( i->getLastError() );
					ok = false;
					break;
				}
			}
			if( !ok ) return false;
			codePostLoadMap_[ pos ] = c;
		} else codePostLoadMap_[ pos ] = code;
		return true;
    }
    void AddFilter( FilterPtr f, const QString& id ) { 
        if( f->parent() == 0 ) f->setParent( this );
        filterMap_[ id ] = f;
    }
	bool AddScriptFilter( const QString& js,
		                  const QString& id,
		                  const QString& jfun,
						  const QString& jerrfun = "",
						  const QString& codePlaceHolder = "",
						  const QStringList& filtersOnFilter = QStringList() ) {
	    if( !filtersOnFilter.empty() ) {
		    bool ok = true;
            QString c = js;
			for( QStringList::iterator i = filters.begin(); i != filters.end() ) {
			    c = i->Filter( c );
				if( i->Error() ) {
					error( i->getLastError() );
					ok = false;
					break;
				}
			}
			if( !ok ) return false;
			AddFilter( new ScriptFilter( wf_, c, jfun, jerrfun, codePlaceHolder ), id ); 
		} else AddFilter( new ScriptFilter( wf_, js ), id );
		return true;
	}
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
            lastJSEval_ = wf_->addToJavaScriptWindowObject( i->second, i->first );
        }
        for( CodeMap::const_iterator i = codeInitMap_.begin();
		     i != codeInitMap_.end(); ++i ) {
            lastJSEval_ = wf_->evaluateJavaScript( i->second );
        }   
    }
    
    QString read( const QString& path, const QStringList& filters = QStringList() ) {
        if( path.contains( ":/" ) ) return readFromUrl( path, filters );
        else readFromFile( path, filters );
    }    
    
    //void download( const QString& url, const QString& localPath, const QString& progressFun, const QString& errFun ) {}
      
    void include( const QString& path, const QStringList& filters = QStringList() ) {
        if( path.contains( "://" ) ) includeUrl( path, filters );
        else includeFile( path, filters );
    }    

    QString filter( const QString text, const QStringList& filterIds ) {
        for( QStringList::const_iterator f = filterIds.begin();
             f != filterIds.end(); ++f ) { 
            FilterMap::iterator i = filterMap_.find( *f );
            if( i == filterMap_.end() ) {
                FilterPtr fp =  filterMap_[ f ];
                text = fp->Filter( text );
                if( fp->Error() ) error( fp->lastError() );
                break;                 
            }
        }
        return text; 
    }
    void initCode() {
         for( CodeMap::const_iterator i = codeInitMap_.begin();
		     i != codeInitMap_.end(); ++i ) {
            lastJSEval_ = wf_->evaluateJavaScript( i->second );
        }    
    }    
    void preLoadCode() {
         for( CodeMap::const_iterator i = codePreLoadMap_.begin();
		     i != codePreLoadMap_.end(); ++i ) {
            lastJSEval_ = wf_->evaluateJavaScript( i->second );
        }    
    }    
    void postLoadCode( bool ok ) {
        if( !ok ) return;
        for( CodeMap::const_iterator i = codePostLoadMap_.begin();
		     i != codePostLoadMap_.end(); ++i ) {
            lastJSEval_ = wf_->evaluateJavaScript( i->second );
        } 
    }

	QVariant lastEval() const { return lastJSEval_; }

private slots:
    void includeUrlFinished( QNetworkReply* nr ) {
       if( nr->error() != QNetworkReply::NoError ) {
           error( nr->errorString() );
       } else {
           lastJSEval_ = wf_->evaluateJavaScript( filter( nr->readAll(), includeUrlFilters_ ) );
       }
       nr->close();
       includeUrlWaitCond_.wakeAll(); 
    }
    void readFromUrlFinished( QNetworkReply* nr ) {
       if( nr->error() != QNetworkReply::NoError ) {
           error( nr->errorString() );
           readFromUrlString_ = "";
       } else {
          readFromUrlString_ = filter( nr->readAll(), includeUrlFilters_ );
       }
       nr->close();
       readFromUrlWaitCond_.wakeAll(); 
    }
private:
    void Connect() {
        connect( wf_, SIGNAL( javaScriptWindowObjectCleared() ), this, SLOT( addObjectsToJScriptContext() ) );
        connect( wf_, SIGNAL( loadStarted() ), this, SLOT( preLoadCode() ) );
        connect( wf_, SIGNAL( loadFinished( bool ) ), this, SLOT( postLoadCode( bool ) ) );
        connect( &netAccessMgrInclude_, SIGNAL( finished( QNetworkRequest* ) ),
                 this, SLOT( includeUrlFinished( QNetworkRequest* ) ) );
        connect( &netAccessMgrRead_, SIGNAL( finished( QNetworkRequest* ) ),
                 this, SLOT( readFromUrlFinished( QNetworkRequest* ) ) );  
    }
    QString readFromUrl( const QString& url, const QStringList& filters ) {
        readFromUrlMutex_.lock();
        readFromUrlFilters_ = filters;
        netAccessMgr_.get( QNetworkRequest( QUrl( url ) ) );
        readFromUrlWaitCond_.wait( readFromUrlMutex_ ); 
        QString s = readFromUrlString_;  
        readFromUrlMutex_.unlock();
        return s;
    }
    QString readFromFile( const QString& fp, const QStringList& filters ) {
        QFile f( fp );
        f.open( QIODevice::ReadOnly );
        QString c = f.readAll();
        f.close();
        return filter( c, filters );
    }
    void includeUrl( const QString& url, const QStringList& filters ) {
        includeUrlMutex_.lock();
        includeUrlFilters_ = filters;
        includeUrlReply_ = netAccessMgr_.get( QNetworkRequest( QUrl( url ) ) );
        includeUrlWaitCond_.wait( includeUrlMutex_ );
        includeUrlMutex_.unlock(); 
    }
    void includeFile( const QString& fp, const QStringList& filters ) {
        QFile f( fp );
        f.open( QIODevice::ReadOnly );
        QString c = f.readAll();
        f.close();
        lastJSEval_ = wf_->evaluateJavaScript( filter( c, filters ) );
    }
    //void includeUrl( const QString& url ) {
    //   r //QNetworkRequest r( QUrl( url ) );
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
	QVariant lastJSEval_;
    QNetworkAccessManager netAccessMgrInclude_;
    mutable QStringList includeUrlFilters_;
    QNetworkAccessManager netAccessMgrRead_;
    mutable QStringList readFromUrlFilters_;
    QWaitCondition includeUrlWaitCond_;
    QMutex includeUrlMutex_;
    QWaitCondition readFromUrlWaitCond_;
    QMutex readFromUrlMutex_;
    mutable QString readFromUrlString_;
    mutable QString includeUrlString_;
     
  
};
}