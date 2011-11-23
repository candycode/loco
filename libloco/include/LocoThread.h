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
public:
    void run() {
        result_ = ctx_->eval( code_, filters_ );
    }
public slots:
    void eval( const QString& code, const QStringList& filters = QStringList() ) {
    	if( this->isRunning() ) this->wait();
    	code_ = code;
    	filters_ = filters;
    	ctx_->setParent( 0 );
    	ctx_->moveToThread( this );
    	start();
    }
    void setContext( QObject* ctx ) {
    	ctx_ = qobject_cast< JSContext* >( ctx );
    }
private:
    QPointer< JSContext > ctx_;
    QVariant result_;
    QString code_;
    QStringList filters_;
};

}
