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


#include <QThread>
#include <QString>
#include <QVariant>
#include <QStringList>

#include "LocoJSContext.h"

namespace loco {

class ContextThread : public QThread {
    Q_OBJECT
    Q_PROPERTY( QVariant result READ GetResult )
    Q_PROPERTY( QVariant data READ GetData WRITE SetData )
    Q_PROPERTY( QVariant input READ GetInput WRITE SetInput )
    Q_PROPERTY( quint64 id READ GetThreadId )
    Q_PROPERTY( QObject* context READ GetContext WRITE SetContext )
    Q_PROPERTY( bool autoDestroy READ AutoDestroy WRITE SetAutoDestroy )
public:
    ContextThread() : obj_( 0, "LocoContextThread", "/loco/sys/thread" ),
        threadId_( 0 ), autoDestroy_( false ) {}
    void SetAutoDestroy( bool on ) {
    	if( on ) {
    		connect( this, SIGNAL( finished() ), this, SLOT( deleteLater() ) );
    		autoDestroy_ = true;
    	} else {
    		disconnect( this, SIGNAL( finished() ), this, SLOT( deleteLater() ) );
    		autoDestroy_ = false;
    	}
    }
    bool AutoDestroy() const { return autoDestroy_; }
    void run() {
    	threadId_ = quint64( currentThreadId() );
        result_ = ctx_->eval( code_, filters_ );
    }
    QString JSInstanceName() const { return obj_.jsInstanceName(); }
    const QVariant& GetResult() const { return result_; }
    quint64 GetThreadId() const { return threadId_; }
    void SetData( const QVariant& d ) { data_ = d; }
    const QVariant& GetData() const { return data_; }
    const QVariant& GetInput() const { return input_; }
    void SetInput( const QVariant& i ) { input_ = i; }
    void SetContext( QObject* ctx ) { ctx_ = qobject_cast< JSContext* >( ctx ); }
    QObject* GetContext() const { return ctx_; }
signals:
	void notify( const QVariant& ); //connect to this signal to receive notifications from thread
public slots:
    void eval( const QString& code, const QStringList& filters = QStringList() ) {
    	code_ = code;
    	filters_ = filters;
    	start();
    }
    void setContext( QObject* ctx ) {
    	ctx_ = qobject_cast< JSContext* >( ctx );
    }
    void execute( const QString& f ) {
    	eval( "(" + f + ")()" );
    }
    bool sync( unsigned long usTimeout = -1) { return wait( usTimeout ); }
private:
    QPointer< JSContext > ctx_;
    QVariant input_;
    QVariant result_;
    QString code_;
    QStringList filters_;
    Object obj_;
    QVariant data_;
    mutable quint64 threadId_;
    bool autoDestroy_;
};

}
