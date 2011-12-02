#pragma once
//#SRCHEADER

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
