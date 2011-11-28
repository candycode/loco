#pragma once
//#SRCHEADER

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
public:
    ContextThreadLoop() : obj_( 0, "LocoContextThreadLock", "/loco/sys/thread" ), threadId_( 0 ), exit_( false )  {}
    void run() {
		threadId_ = quint64( currentThreadId() );
		while( true ) {
    	    evalMutex_.lock();
    	    startEvaluation_.wait( &evalMutex_ );
    	    // this is to guarantee that the evalMutex has been acquired before eval() returns
    	    evaluationStarted_.wakeOne();
    	    if( exit_ ) {
    	    	evalMutex_.unlock();
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
public slots:
   void eval( const QString& code, const QStringList& filters = QStringList() ) {
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
    void startLoop() { exit_ = false; start(); }
    void stopLoop() { exit_ = true; startEvaluation_.wakeOne(); }
    void sync() {
    	evalMutex_.lock();
    	evalMutex_.unlock();
    }
private:
    void start() { QThread::start(); }
private:
    QWaitCondition startEvaluation_;
    QWaitCondition evaluationStarted_;
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
};

}
