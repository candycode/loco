#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace loco {

// derivation form QObject required to keep references into QPointers

struct IAuthenticator : public QObject {
	virtual QVariantMap Credentials( const QString& /*url*/ ) const = 0;
	virtual ~IAuthenticator() {}
};

class DefaultAuthenticator : public IAuthenticator {
public:
	DefaultAuthenticator( const QString& u = QString(), const QString& p = QString() )
	: user_( u ), password_( p ) {}
    const QString& user() const { return user_; }
    void setUser( const QString& u ) { user_ = u; }
    const QString& password() const { return password_; }
    void setPassword( const QString& p ) { password_ = p; }
    QVariantMap Credentials( const QString& /*url*/ ) const {
    	QVariantMap m;
    	m[ "user" ] = user_;
    	m[ "password" ] = password_;
    	return m;
    }
private:
	QString user_;
	QString password_;
};

}
