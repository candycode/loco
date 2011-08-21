#pragma once
#ifndef LOCO_FSYSTEM_
#define LOCO_FSYSTEM_
//#SRCHEADER
#include <QObject>
#include <QVariantMap>
#include <QString>
class LocoFSystem : public QObject {
    Q_OBJECT
public slots:
	bool copy ( const QString & fileName, const QString & newName ) {
		return QFile::copy( finelName, newName );
	}
    QString	decodeName ( const QByteArray & localFileName ) {
    	return QFile::decodeName( localFileName );
    }
    QString	decodeName ( const char * localFileName )
    QByteArray	encodeName ( const QString & fileName )
    bool	exists ( const QString & fileName )
    bool	link ( const QString & fileName, const QString & linkName )
    Permissions	permissions ( const QString & fileName )
    bool	remove ( const QString & fileName )
    bool	rename ( const QString & oldName, const QString & newName )
    bool	resize ( const QString & fileName, qint64 sz )
    void	setDecodingFunction ( DecoderFn function )
    void	setEncodingFunction ( EncoderFn function )
    bool	setPermissions ( const QString & fileName, Permissions permissions )
    QString	symLinkTarget ( const QString & fileName )
private:
    QFile f_;                                                                  	
};
#endif