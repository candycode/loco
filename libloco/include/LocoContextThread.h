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
public:
    ContextThread() : obj_( 0, "LocoContextThread", "/loco/sys/thread" ), threadId_( 0 )  {}
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
public slots:
    void eval( const QString& code, const QStringList& filters = QStringList() ) {
    	code_ = code;
    	filters_ = filters;
    	start();
    }
    void setContext( QObject* ctx ) {
    	ctx_ = qobject_cast< JSContext* >( ctx );
    }
    void execute( const QString& code, const QStringList& filters = QStringList() ) {
    	eval( code, filters );
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
};

}
