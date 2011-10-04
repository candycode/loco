#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QStringList>

namespace loco {

// errors warnings logs
class EWL : public QObject {
	Q_OBJECT
public:
    EWL( bool autoDeleteEntries = false ) : autoDelete_( autoDeleteEntries ) {}
    virtual ~EWL() {}
public slots:
    QString lastError() const { 
        if( errors_.isEmpty() ) return "";
	    QString e = errors_.back();
	    if( autoDelete_ ) errors_.removeLast();
	    return e;
	}
    QString lastWarning() const {
        if( warnings_.isEmpty() ) return ""; 
	    QString w = warnings_.back();
	    if( autoDelete_ ) warnings_.removeLast();
	    return w;
	}
    QString lastLog() const {
        if( logs_.isEmpty() ) return ""; 
	    QString l = logs_.back();
	    if( autoDelete_ ) logs_.removeLast();
	    return l;
	}
    virtual void error( const QString& emsg ) const { 
	    errors_.push_back( emsg );
	    emit onError( emsg );
	}
    virtual void warn( const QString& wmsg ) const { 
	    warnings_.push_back( wmsg );
	    emit onWarning( wmsg );
	}
    virtual void log( const QString& lmsg ) const { 
	    logs_.push_back( lmsg );
	    emit onLog( lmsg );
	}
    void clearErrors() { errors_.clear(); }
    void clearWarnings() { errors_.clear(); }
    void clearLogs() { logs_.clear(); }

    const QStringList& errors() const { return errors_; }
    const QStringList& warnings() const { return warnings_; }
    const QStringList& logs() const { return logs_; }
    
    virtual bool error() const { return !errors_.empty(); }

signals:
    void onError( const QString& ) const;
    void onWarning( const QString& ) const;
    void onLog( const QString& ) const;
private:
    typedef QStringList Errors;
	typedef QStringList Warnings;
	typedef QStringList Logs;
	mutable Errors errors_;
	mutable Warnings warnings_;
	mutable Logs logs_;
    bool autoDelete_;
};

}
