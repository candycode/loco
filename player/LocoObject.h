#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QVariantMap>
#include <QStringList>
#include <QPluginLoader>
#include <QSharedPointer>

#include "EWL.h"
#include "LocoObjectInfo.h"

//temporary
#include <iostream>


namespace loco {

class Context;

// loco objects are mainly intended for run-time loading

class Object : public EWL {
    Q_OBJECT
    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString type READ type )
    Q_PROPERTY( QString jsInstance READ jsInstanceName )
public:    
    Object( Context* c, 
		    const QString& n,
			const QString& type = "", 
			const QString& module = "",
            bool destroyable = false,
			ObjectInfo* objInfo = 0 ) // used only for creating js instance names w/ dynamic loading 
    : context_( c ), name_( n ), type_( type ),
	  instanceId_( IncInstanceCount() ), module_( module ), info_( objInfo ),
        pluginLoader_( 0 ), destroyable_( destroyable )  {
    	jsInstanceName_ = objNamePrefix_ + module_ + "_" + 
			              name() + objNameSuffix_ + QString("%1").arg( instanceId_ );
        setObjectName( jsInstanceName_ ); //choose unique name for Qt object instance                   
    }
	const QString& GetModule() const { return module_; }  
    void SetPluginLoader( QPluginLoader* pl ) { pluginLoader_ = pl; }
	QPluginLoader* GetPluginLoader() const { return pluginLoader_; }
    virtual void SetContext( Context* c ) { context_ = c; }
    Context* GetContext() const { return context_; }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    void setName( const QString& n ) { name_ = n; }
    void SetName( const QString& n ) { name_ = n; }
    void SetType( const QString& t ) { type_ = t; }
    const QString& jsInstanceName() const { return jsInstanceName_; }
    void SetJSInstanceName( const QString& jsi ) { jsInstanceName_ = jsi; }
    void SetDestroyable( bool d ) { destroyable_ = d; }
    bool GetDestroyable() const { return destroyable_; }
    virtual void error( const QString& em ) const { EWL::error( FormatEWLMsg( em ) ); }
    virtual void warning( const QString& em ) const { EWL::warn( FormatEWLMsg( em ) ); }
    virtual void log( const QString& em ) const { EWL::log( FormatEWLMsg( em ) ); }
    virtual bool error() const { return EWL::error(); }
    virtual ~Object() {        
        DecInstanceCount(); 
        if( info_ ) info_->deleteLater();
    }
  
private:
    static int IncInstanceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( 1 );
	    return c + 1; 
	}
    static void DecInstanceCount()  { 
	    const int c = instanceCount_.fetchAndAddAcquire( -1 );
	}
public:    
    static int  GetInstanceCount() { return instanceCount_; }
    static const QString& ObjNamePrefix() { return objNamePrefix_; }
    static const QString& ObjNameSuffix() { return objNameSuffix_; }
    static void SetObjNamePrefix( const QString& p ) { objNamePrefix_ = p; }
    static void SetObjNameSuffix( const QString& s ) { objNameSuffix_ = s; }      
public slots:
	ObjectInfo* info() const { return info_.data(); }
    void destroy() {
        //global objects set from Context must never be destroyed
        if( !destroyable_ ) {
            error( "Tried to destroy Loco global object");
            return;
        }
        //only destroy non-root (i.e. created with QPluginLoader::instance) objects
        if( !pluginLoader_ ) { setParent( 0 ); deleteLater(); }
        else { pluginLoader_->unload(); }
    }

private:
    QString FormatEWLMsg( const QString& msg ) const {
        return type_ + " " + jsInstanceName_ + ": " + msg; 
    }
private:
    Context* context_;
    QString name_;
    QString type_;
    QString jsInstanceName_;
    int instanceId_;
	QString module_;
	QSharedPointer< ObjectInfo > info_;
    QPluginLoader* pluginLoader_;
    bool destroyable_; 
private:
    static QAtomicInt instanceCount_;
    static QString objNamePrefix_;
    static QString objNameSuffix_;        
};

}

Q_DECLARE_INTERFACE( loco::Object, "Loco.Object:1.0" )
