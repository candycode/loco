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
#include <QMutex>
#include <QWaitCondition>
#include <QMutexLocker>

#include "LocoJSContext.h"

namespace loco {

class ContextThreadLoop : public QThread {
    Q_OBJECT
    Q_PROPERTY( QVariant result READ GetResult )
    Q_PROPERTY( QVariant data READ GetData WRITE SetData )
    Q_PROPERTY( quint64 id READ GetThreadId )
    Q_PROPERTY( bool autoDestroy READ AutoDestroy WRITE SetAutoDestroy )
public:
    ContextThreadLoop() : obj_( 0, "LocoContextThreadLoop", "/loco/sys/thread" ),
        threadId_( 0 ), exit_( false ), autoDestroy_( false )  {}
    void run() {
        threadId_ = quint64( currentThreadId() );
        bool first = true;
        while( true ) {
            evalMutex_.lock();
            if( first ) {
                started_.wakeOne();
                first = false;
            }
            // this is to guarantee that the evalMutex has been acquired before eval() returns
            evaluationStarted_.wakeOne();
            startEvaluation_.wait( &evalMutex_ );
            if( exit_ ) {
                evalMutex_.unlock();
                evaluationStopped_.wakeOne();
                break;
            }

            result_ = ctx_->eval( code_, filters_ );
            evalMutex_.unlock();
        }
    }
    QString JSInstanceName() const { return obj_.jsInstanceName(); }
    QVariant GetResult() const {
        QMutexLocker ml( &evalMutex_ );
        const QVariant result = result_;
        return result;
    }
    quint64 GetThreadId() const { return threadId_; }
    void SetData( const QVariant& d ) { data_ = d; }
    QVariant GetData() const {
        QMutexLocker ml( &evalMutex_ );
        const QVariant data = data_;
        return data;
    }
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
signals:
    void notify( const QVariant& ); //connect to this signal to receive notifications from thread
public slots:
   void eval( const QString& code, const QStringList& filters = QStringList() ) {
        if( !running() ) startLoop();
        evalMutex_.lock();
        code_ = code;
        filters_ = filters;
        evalMutex_.unlock();
        QMutexLocker ml( &evalStartMutex_ );
        //start evaluation
        startEvaluation_.wakeOne();
        //wait for confirmation that evaluation has started and that
        //evalMutex_ has been acquired by worker thread
        evaluationStarted_.wait( &evalStartMutex_ );
    }
    void setContext( QObject* ctx ) {
        ctx_ = qobject_cast< JSContext* >( ctx );
    }
    void startLoop() {
        exit_ = false;
        QMutex started;
        QMutexLocker ml( &started );
        start();
        started_.wait( &started );
    }
    bool stopLoop( unsigned long maxTimeout_us = -1 ) {
        exit_ = true;
        QMutex stopped;
        QMutexLocker ml( &stopped );
        evalMutex_.lock();
        startEvaluation_.wakeOne();
        evalMutex_.unlock();
        return evaluationStopped_.wait( &stopped, maxTimeout_us );
    }
    void sync() {
        evalMutex_.lock();
        evalMutex_.unlock();
    }
    bool running() const { return isRunning(); }
    const QString& name() const { return obj_.name(); }
private:
    void start() { QThread::start(); }
private:
    QWaitCondition startEvaluation_;
    QWaitCondition evaluationStarted_;
    QWaitCondition evaluationStopped_;
    QWaitCondition started_;
    mutable QMutex evalStartMutex_;
    mutable QMutex evalMutex_;
    bool exit_;
    QPointer< JSContext > ctx_;
    QVariant result_;
    QString code_;
    QStringList filters_;
    Object obj_;
    QVariant data_;
    mutable quint64 threadId_;
    bool autoDestroy_;
};

}
