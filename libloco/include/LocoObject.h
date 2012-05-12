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


#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QVariantMap>
#include <QStringList>
#include <QPluginLoader>
#include <QPointer>

#include "EWL.h"
#include "LocoObjectInfo.h"


namespace loco {

class Context;

// loco objects are mainly intended for run-time loading

class Object : public EWL {
    Q_OBJECT
    Q_PROPERTY( QString name READ name WRITE setName )
    Q_PROPERTY( QString type READ type )
    Q_PROPERTY( QString jsInstance READ jsInstanceName )
    Q_PROPERTY( bool cloneable READ cloneable )
public:    
    Object( Context* c = 0, 
            const QString& n = "LocoObject",
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
        if( info_ != 0 ) info_->setParent( this );                   
    }
    virtual Object* Clone() const { return 0; }
    QVariant JSInstance() const;
    virtual void Init( const QVariantMap& ) {}
    const QString& GetModule() const { return module_; }  
    void SetPluginLoader( QPluginLoader* pl ) { pluginLoader_ = pl; }
    QPluginLoader* GetPluginLoader() const { return pluginLoader_; }
    virtual void SetContext( Context* c ) { context_ = c; }
    Context* GetContext() const { return context_; }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    void setName( const QString& n ) { name_ = n; }
    void SetType( const QString& t ) { type_ = t; }
    const QString& jsInstanceName() const { return jsInstanceName_; }
    void SetJSInstanceName( const QString& jsi ) { jsInstanceName_ = jsi; }
    void SetDestroyable( bool d ) { destroyable_ = d; }
    bool GetDestroyable() const { return destroyable_; }
    void SetObjectInfo( ObjectInfo* oi ) {
        if( info_ != 0 && oi == info_ ) return;
        if( info_ != 0 ) info_->deleteLater();
        info_ = oi;
        info_->setParent( this );        
    }
    virtual void error( const QString& em ) const { EWL::error( FormatEWLMsg( em ) ); }
    virtual void warning( const QString& em ) const { EWL::warn( FormatEWLMsg( em ) ); }
    virtual void log( const QString& em ) const { EWL::log( FormatEWLMsg( em ) ); }
    virtual bool error() const { return EWL::error(); }  

    static int IncInstanceCount()  { 
        const int c = instanceCount_.fetchAndAddAcquire( 1 );
        return c + 1; 
    }
public:
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
    virtual bool cloneable() const { return false; }
    ObjectInfo* info() const { return info_; }
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
    // invoking this slot from Javascript allows to send messages across
    // contexts without resorting to eval:
    // /*PARENT CONTEXT*/
    // var webWindow = ...
    // webWindow.event.connect( function(e, window) {
    //                            print(e);
    //                            window.sendMessage( {"bk-color": red } );
    //                          } );
    // webWindow.addObjectToContext( "thisWindow", webWindow );
    // webWindow.setPreLoadCBack( // or add a <script> in the web page
    //   "... \
    //    thisWindow.messageReceived.connect( function( msg ) { \
    //      if( msg['bk-color'] ) changeBkColor( msg['bk-color'] ); \
    //    }" );
    // /*CHILD CONTEXT INSIDE A WEB PAGE*/
    // <a href="javascript:thisWindow.triggerEvent( "Link clicked" )>click</a>
    void sendMessage( const QVariant& data, QObject* sender ) {
        emit messageReceived( data, sender );
    }
    void triggerEvent( const QVariant& data ) {
        emit event( data, this );
    }
signals:
    void event( const QVariant&, QObject* /*sender*/ );
    void messageReceived( const QVariant&, QObject* /*sender*/ );
private:
    QString FormatEWLMsg( const QString& msg ) const {
        return type_ + " " + jsInstanceName_ + ": " + msg; 
    }
private:
    Context* context_; // ONLY EXCEPTION TO THE QPointer rule for non-owned objects;
                       // required because QPointer needs to know that Context is derived from QObject
                       // and this in turn requires the inclusion of LocoContext.h but LocoContext.h
                       // *does* need LocoObject.h.
    QString name_;
    QString type_;
    QString jsInstanceName_;
    int instanceId_;
    QString module_;
    ObjectInfo* info_;
    QPointer< QPluginLoader > pluginLoader_;
    bool destroyable_; 
private:
    static QAtomicInt instanceCount_;
    static QString objNamePrefix_;
    static QString objNameSuffix_;        
};

}

Q_DECLARE_INTERFACE( loco::Object, "Loco.Object:1.0" )
