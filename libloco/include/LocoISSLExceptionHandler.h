#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <QStringList>


namespace loco {

// derivation form QObject required to keep references into QPointers

struct ISSLExceptionHandler : public QObject {
	virtual bool Check( const QString& /*url*/, const QStringList& /*errors*/ ) const = 0;
	virtual bool AcceptCertificates( const QStringList& ) const = 0;
	virtual ~ISSLExceptionHandler() {}
};

class DefaultSSLExceptionHandler : public ISSLExceptionHandler {
public:
	bool Check( const QString& /*url*/, const QStringList& /*errors*/ ) const { return true; }
	bool AcceptCertificates( const QStringList& ) const { return true; }
};

}
