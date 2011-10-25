#pragma once
//#SRCHEADER

namespace loco {

struct IAuthenticator : public QObject {
	virtual QVariantMap Check( const QString& /*url*/ ) const = 0;
	virtual ~IAuthenticator() {}
};

class ConstAuthenticator : public IAuthenticator {
	Q_OBJECT
	Q_PROPERTY( QString user READ user WRITE setUser )
	Q_PROPERTY( QString password READ password WRITE setPassword )
public:
	ConstAuthenticator( const QString& u, const QString& p ) : user_( u ), password_( p ) {}
    const QString& user() const { return user_; }
    void setUser( const QString& u ) { user_ = u; }
    const QString& password() const { return password; }
    void setPassword( const QString& p ) { password_ = p; }
    QVariantMap Credentials( const QString& ) {
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
