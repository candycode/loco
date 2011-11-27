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
		dataMutex_.lock();
    	threadId_ = reinterpret_cast< quint64 >( currentThreadId() );
		QMutex evalMutex;
		while( true ) {
    	    evalMutex.lock();
    	    startEvaluation_.wait( &evalMutex );
    	    if( exit_ ) {
    	    	evalMutex.unlock();
    	    	break;
    	    }
    	    dataMutex_.tryLock();
    	    result_ = ctx_->eval( code_, filters_ );
    	    dataMutex_.unlock();
    	    evalMutex.unlock();
    	}
    }
    QString JSInstanceName() const { return obj_.jsInstanceName(); }
    QVariant GetResult() const {
    	QMutexLocker ml( &dataMutex_ );
    	const QVariant result = result_;
    	return result;
    }
    quint64 GetThreadId() const { return threadId_; }
    void SetData( const QVariant& d ) { data_ = d; }
    QVariant GetData() const {
    	QMutexLocker ml( &dataMutex_ );
    	const QVariant data = data_;
    	return data;
    }
public slots:
   void eval( const QString& code, const QStringList& filters = QStringList() ) {
	    code_ = code;
    	filters_ = filters;
    	startEvaluation_.wakeOne();
    }
    void setContext( QObject* ctx ) {
    	ctx_ = qobject_cast< JSContext* >( ctx );
    }
    void startLoop() { exit_ = false; start(); }
    void stopLoop() { exit_ = true; startEvaluation_.wakeOne(); }
    void sync() {
    	dataMutex_.lock();
    	dataMutex_.unlock();
    }
private:
    void start() { QThread::start(); }
private:
    QWaitCondition startEvaluation_;
    mutable QMutex dataMutex_;
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
