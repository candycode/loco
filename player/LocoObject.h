#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QAtomicInt>
#include <QVariantMap>
#include <QStringList>

#include "EWL.h"

namespace loco {

class Context;


class ObjectInfo : public QObject {
    Q_OBJECT
	Q_PROPERTY( QString name READ name )
	Q_PROPERTY( QStringList resourceAccess READ resourceAccess ) //persistent storage,local filesystem,network...
	Q_PROPERTY( QStringList version READ version )
	Q_PROPERTY( QStringList locoVersion READ locoVersion )
	Q_PROPERTY( QString qtVersion READ qtVersion )
	Q_PROPERTY( QStringList category READ category )
	Q_PROPERTY( QString description READ description )
	Q_PROPERTY( QString vendor READ vendor )
	Q_PROPERTY( QString vendorUrl READ vendorUrl )
	Q_PROPERTY( QString author READ author )
	Q_PROPERTY( QString license READ license )
    Q_PROPERTY( QString buildInfo READ buildInfo )
	Q_PROPERTY( QStringList locale READ locale )
	Q_PROPERTY( QStringList dependencies READ dependencies )
	Q_PROPERTY( QString url READ url )
	Q_PROPERTY( QString downloadUrl READ downloadUrl )
	Q_PROPERTY( QString docUrl READ docUrl )
	Q_PROPERTY( QString updateUrl READ updateUrl )
	Q_PROPERTY( QVariantMap vendorData READ vendorData )
public:
	const QString& name() const { return name_; }
	const QStringList& resourceAccess() const { return resourceAccess_; }
	const QStringList& version() const { return version_; }
	const QStringList& locoVersion() const { return locoVersion_; }
	QString qtVersion() const { return qVersion(); }
	const QStringList& category() const { return category_; }
	const QString& description() const { return description_; }
	const QString& vendor() const { return vendor_; }
	const QString& vendorUrl() const { return vendorUrl_; }
	const QString& author() const { return author_; }
	const QString& license() const { return license_; }
	const QString& buildInfo() const { return buildInfo_; }
	const QStringList& locale() const { return locale_; }
	const QStringList& dependencies() const { return dependencies_; }
	const QString& url() const { return url_; }
	const QString& downloadUrl() const { return downloadUrl_; }
	const QString& docUrl() const { return docUrl_; }
	const QString& updateUrl() const { return updateUrl_; }
    const QVariantMap& vendorData() const { return vendorData_; }
public:
	ObjectInfo& SetName( const QString& n ) { name_ = n; return *this; }
	ObjectInfo& SetResourceAccess( const QStringList& r ) { resourceAccess_ = r; return *this; }
	ObjectInfo& SetVersion( const QStringList& v ) { version_ = v; return *this; }
	ObjectInfo& SetLocoVersion( const QStringList& lv ) { locoVersion_ = lv; return *this; }
	ObjectInfo& SetCategory( const QStringList& c ) { category_ = c; return *this; }
	ObjectInfo& SetDescription( const QString& n ) { name_ = n; return *this; }
	ObjectInfo& SetVendor( const QString& v ) { vendor_ = v; return *this; }
	ObjectInfo& SetVendorUrl( const QString& vu ) { vendorUrl_ = vu; return *this; }
	ObjectInfo& SetAuthor( const QString& v ) { author_ = v; return *this; }
	ObjectInfo& SetLicense( const QString& l ) { license_ = l; return *this; }
	ObjectInfo& SetBuildInfo( const QString& bi ) { buildInfo_ = bi; return *this; }
	ObjectInfo& SetLocale( const QStringList& l ) { locale_ = l; return *this; }
	ObjectInfo& SetDependencies( const QStringList& d ) { dependencies_ = d; return *this; }
	ObjectInfo& SetUrl( const QString& u ) { url_ = u; return *this; }
	ObjectInfo& SetDownloadUrl( const QString& d ) { downloadUrl_ = d; return *this; }
	ObjectInfo& SetDocUrl( const QString& d ) { docUrl_ = d; return *this; }
	ObjectInfo& SetUpdateUrl( const QString& u ) { updateUrl_ = u; return *this; }
	ObjectInfo& SetVendorData( const QVariantMap& v ) { vendorData_ = v; return *this; }
public:
	ObjectInfo& AddVersion( const QString& v ) { version_ << v; return *this; }
	ObjectInfo& AddLocoVersion( const QString& v ) { locoVersion_ << v; return *this; }
	ObjectInfo& AddResAccess( const QString& s ) { resourceAccess_ << s; return *this; }
	ObjectInfo& AddCategory( const QString& s ) { category_ << s; return *this; }
	ObjectInfo& AddLocale( const QString& s ) { locale_ << s; return *this; }
	ObjectInfo& AddDep( const QString& s ) { dependencies_ << s; return *this; }
	ObjectInfo& AddVendorData( const QString& s, const QVariant& v ) { vendorData_.insert( s, v ); return *this; }
private:
	QString name_;
	QStringList resourceAccess_;
	QStringList version_;
	QStringList locoVersion_;
	QStringList category_;
	QString description_;
	QString vendor_;
	QString vendorUrl_;
	QString author_;
	QString license_;
	QString buildInfo_;
	QStringList locale_;
	QStringList dependencies_;
	QString url_;
	QString downloadUrl_;
	QString docUrl_;
	QString updateUrl_;
	QVariantMap vendorData_;

};

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
	  instanceId_( IncInstanceCount() ), module_( module ), info_( objInfo ) {
    	jsInstanceName_ = objNamePrefix_ + module_ + "_" + 
			              name() + objNameSuffix_ + QString("%1").arg( instanceId_ ); 
    }
	const QString& GetModule() const { return module_; }  
    void SetContext( Context* c ) { context_ = c; }
    Context* GetContext() const { return context_; }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    void SetName( const QString& n ) { name_ = n; }
    void SetType( const QString& t ) { type_ = t; }
    const QString& jsInstanceName() const { return jsInstanceName_; }
    void setJSInstanceName( const QString& jsi ) { jsInstanceName_ = jsi; }
    virtual ~Object() { DecInstanceCount(); if( info_ )info_->deleteLater(); }
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
	void destroy() { deleteLater(); }
	ObjectInfo* info() const { return info_; }
private:
    Context* context_;
    QString name_;
    QString type_;
    QString jsInstanceName_;
    int instanceId_;
	QString module_;
	ObjectInfo* info_;
private:
    static QAtomicInt instanceCount_;
    static QString objNamePrefix_;
    static QString objNameSuffix_;        
};

}

Q_DECLARE_INTERFACE( loco::Object, "Loco.Object:1.0" )