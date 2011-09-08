#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QVariantMap>
#include <QStringList>
#include <QPluginLoader>

#include "EWL.h"
#include "LocoObjectInfo.h"

//temporary
#include <iostream>


namespace loco {

class Context;

// loco objects are mainly intended for run-time loading

class Object : public EWL {
    Q_OBJECT
    Q_PROPERTY( QString name READ name )
    Q_PROPERTY( QString type READ type )
public:    
    Object( Context* c, 
		    const QString& n,
			const QString& type = "", 
			const QString& module = "",
			ObjectInfo* objInfo = 0 ) // used only for creating js instance names w/ dynamic loading 
    : context_( c ), name_( n ), type_( type ),
	  instanceId_( IncInstanceCount() ), module_( module ), info_( objInfo ),
      contextPtrToThis_( 0 ), pluginLoader_( 0 )  {
    	jsInstanceName_ = objNamePrefix_ + module_ + "_" + 
			              name() + objNameSuffix_ + QString("%1").arg( instanceId_ ); 
    }
	const QString& GetModule() const { return module_; }  
    void SetContextPointerToThis( Object** ptrToThis ) { contextPtrToThis_ = ptrToThis; }
    void SetPluginLoader( QPluginLoader* pl ) { pluginLoader_ = pl; }
    void SetContext( Context* c ) { context_ = c; }
    Context* GetContext() const { return context_; }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    void SetName( const QString& n ) { name_ = n; }
    void SetType( const QString& t ) { type_ = t; }
    const QString& jsInstanceName() const { return jsInstanceName_; }
    void setJSInstanceName( const QString& jsi ) { jsInstanceName_ = jsi; }
    virtual ~Object() { 
std::cout << "~Object()" << std::endl;        
        DecInstanceCount(); 
        if( info_ ) info_->deleteLater();
    }
    void Error( const QString& msg ) { 
        error( msg );
        emit RiseError( this );
    }
 
signals:
    void RiseError( Object* );
    
public:
    static int IncInstanceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( 1 );
	    return c + 1; 
	}
    static void DecInstanceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( -1 );
	}
    static int  GetInstanceCount() { return instanceCount_; }
    static const QString& ObjNamePrefix() { return objNamePrefix_; }
    static const QString& ObjNameSuffix() { return objNameSuffix_; }
    static void SetObjNamePrefix( const QString& p ) { objNamePrefix_ = p; }
    static void SetObjNameSuffix( const QString& s ) { objNameSuffix_ = s; }      
public slots:
	ObjectInfo* info() const { return info_; }
    void destroy() {
        //only destroy non-root (i.e. created with QPluginLoader::instance) objects
        if( !pluginLoader_ ) deleteLater();
    }
public slots:
    void OnDestroy( QObject* obj ) {
        //set the pointer to this object in context to NULL
        if( contextPtrToThis_ && *contextPtrToThis_ ) {
             *contextPtrToThis_ = 0;
        }
    }
private:
    Context* context_;
    QString name_;
    QString type_;
    QString jsInstanceName_;
    int instanceId_;
	QString module_;
	ObjectInfo* info_;
    Object** contextPtrToThis_;
    QPluginLoader* pluginLoader_;
private:
    static QAtomicInt instanceCount_;
    static QString objNamePrefix_;
    static QString objNameSuffix_;        
};

}

Q_DECLARE_INTERFACE( loco::Object, "Loco.Object:1.0" )