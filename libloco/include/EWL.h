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


#include <QObject>
#include <QString>
#include <QStringList>

namespace loco {

// errors warnings logs
class EWL : public QObject {
	Q_OBJECT
public:
    EWL( bool autoDeleteEntries = false ) : autoDelete_( autoDeleteEntries ) {}
    EWL( const EWL& ewl ) : autoDelete_( ewl.autoDelete_ ) {}
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
