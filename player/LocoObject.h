#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QAtomicInt>

#include "EWL.h"

namespace loco {

class Object : EWL {
    Q_OBJECT
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString type READ type )
public:    
    Object( const QString& n, const QString& type = "" ) 
    : name_( n ), type_( type ), instanceId_( IncInstanceCount() ) {
    	jsInstanceName_ = objNamePrefix_ + name() + objNameSuffix_ + QString("%1").arg( instanceId_ ); 
    }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    void SetName( const QString& n ) { name_ = n; }
    void SetType( const QString& t ) { type_ = t; }
    const QString& jsInstanceName() const { return jsInstanceName_; }
    void setJSInstanceName( const QString& jsi ) { jsInstanceName_ = jsi; }
    virtual ~Object() { DecInstanceCount(); }
public:
    static int IncInstaceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( 1 );
	    return c + 1; 
	}
    static void DecInstaceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( -1 );
	}
    static int  GetInstanceCount() { return instanceCount_; }
    static const QString& ObjNamePrefix() { return objNamePrefix_; }
    static const QString& ObjNameSuffix() { return objNameSuffix_; }
    static void SetObjNamePrefix( const QString& p ) { objNamePrefix_ = p; }
    static void SetObjNameSuffix( const QString& s ) { objNameSuffix_ = s; }      
public slot:
    void destroy() { deleteLater(); }    
private:
    QString name_;
    QString type_;
    QString jsInstanceName_;
private:
    static QAtomicInt instanceCount_;
    static QString objNamePrefix_;
    static QString objNameSuffix_;        
};

}