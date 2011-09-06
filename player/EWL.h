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
    EWL( bool autoDeleteEntries = true ) : autoDelete_( autoDeleteEntries ) {}
public slots:
    QString lastError() const { 
	    QString e = errors_.back();
	    if( autoDelete_ ) errors_.removeLast();
	    return e;
	}
    QString lastWarning() const { 
	    QString w = warnings_.back();
	    if( autoDelete_ ) warnings_.removeLast();
	    return w;
	}
    QString lastLog() const { 
	    QString l = logs_.back();
	    if( autoDelete_ ) logs_.removeLast();
	    return l;
	}
    void error( const QString& emsg ) const { 
	    errors_.push_back( emsg );
	    emit onError( emsg );
	}
    void warn( const QString& wmsg ) { 
	    warnings_.push_back( wmsg );
	    emit onWarning( wmsg );
	}
    void log( const QString& lmsg ) { 
	    logs_.push_back( lmsg );
	    emit onLog( lmsg );
	}
    void clearErrors() { errors_.clear(); }
    void clearWarnings() { errors_.clear(); }
    void clearLogs() { logs_.clear(); }

    const QStringList& errors() const { return errors_; }
    const QStringList& warnings() const { return warnings_; }
    const QStringList& logs() const { return logs_; }
    
    bool error() const { return !errors_.empty(); }

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
