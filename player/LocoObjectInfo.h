#pragma once
//#SRCHEADER

#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <Qstring>
#include <QStringList>
#include <QVariantMap>


namespace loco {

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
    QVariantMap ToVariantMap() const {
        const QMetaObject* mo = this->metaObject();
        QVariantMap vm;
        for( int p = 0; p != mo->propertyCount(); ++p ) {
            QMetaProperty mp = mo->property( p );
            vm.insert( mp.name(), mp.read( this ) );
        } 
        return vm;
    }
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

}