#pragma once
//#SRCHEADER
#include <QFuture>
#include <QtConcurrentRun>
#include <QVariant>
#include <QThread>

#include "LocoObject.h"
#include "LocoJSContext.h"
#include "LocoDataType.h"

///\todo remove
#include <iostream>
namespace loco {

class FutureWrapper : public Object {
	Q_OBJECT
	Q_PROPERTY( QVariant result READ result )
	Q_PROPERTY( bool finished READ isFinished )
public:
	FutureWrapper( const QFuture< QVariant >& f = QFuture< QVariant >() )
	  : Object( 0, "LocoFutureWrapper", "/loco/sys" ), future_( f ) {

	}
	bool isFinished() const { return future_.isFinished(); }
	QVariant result() const { return future_.result(); }
private:
    QFuture< QVariant > future_;
};

class Runner : public Object {
    Q_OBJECT
public:    
    Runner() : Object( 0, "LocoRunner", "/loco/sys" ) {}
public slots:
    QVariant run( const QString& code, QObject* context ) {
		context->moveToThread( QThread::currentThread() );
    	F = QtConcurrent::run( *this, &Runner::execute, code, qobject_cast< JSContext* >( context ) );
    	FutureWrapper* fw = new FutureWrapper( F );
    	return GetContext()->AddObjToJSContext( fw );
    }
signals:
	void go();
private:
    QVariant execute( const QString& code, JSContext* ctx ) {
    	emit go();
		//ctx->setParent( 0 );
    	//ctx->moveToThread( QThread::currentThread() );
        return ctx->eval( code );
    }
    QFuture< QVariant > F;
};

}

