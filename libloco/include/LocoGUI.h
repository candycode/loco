#pragma once
//#SRCHEADER
#include <QVariant>
#include <QDesktopWidget>
#include <QRect>
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QMessageBox>
#include <QApplication>

#include "LocoObject.h"

namespace loco {

class GUI : public Object {
    Q_OBJECT

public:
   GUI() : Object( 0, "LocoGUI", "Loco/GUI" ) {}

public slots:
    
    QVariant create( const QString& name, const QVariantMap& params = QVariantMap() ) const;

    QVariantMap desktopGeometry( int screen = -1 ) {
        const QDesktopWidget dw;
        const QRect geom = dw.availableGeometry();
        QVariantMap vm;
        vm[ "screen" ] = screen;
        vm[ "x" ] = geom.x();
        vm[ "y" ] = geom.y();
        vm[ "w" ] = geom.width();
        vm[ "h" ] = geom.height();
        vm[ "screens" ] = dw.screenCount();
        return vm;
    }

    QString saveFileDialog( const QString& caption = QString(),
    		                const QString& dir = QString(),
    		                const QString& filter = QString() ) {

        return QFileDialog::getSaveFileName ( 0,  caption, dir, filter );
    }

    QString	openFileDialog( const QString& caption = QString(),
    		                const QString& dir = QString(),
    		                const QString& filter = QString() ) {
    	return QFileDialog::getOpenFileName( 0, caption, dir, filter );
    }
    
    QStringList	openFilesDialog( const QString& caption = QString(),
    		                     const QString& dir = QString(),
    		                     const QString& filter = QString() ) {
    	return QFileDialog::getOpenFileNames( 0, caption, dir, filter );
    }

	QString selectDirDialog ( const QString& caption = QString(),
			                  const QString& dir = QString() ) {
		return QFileDialog::getExistingDirectory( 0, caption, dir );
	}

    QVariantMap colorDialog( const QVariantMap& cm = QVariantMap(),
    		                 const QString& caption = QString(),
    		                 bool alpha = false ) {
    	QColor c;
    	if( !cm.isEmpty() ) {
    		if( alpha ) c.setAlpha( cm[ "a" ].toInt() );
    		c.setRed( cm[ "r" ].toInt() );
    		c.setGreen( cm[ "g" ].toInt() );
    		c.setBlue( cm[ "b" ].toInt() );
    	}
    	c = QColorDialog::getColor( c, 0, caption );
        QVariantMap m;
        if( alpha ) m[ "a" ] = c.alpha();
        m[ "r" ] = c.red();
        m[ "g" ] = c.green();
        m[ "b" ] = c.blue();
        return m;
    }

	void criticalDialog( const QString& caption, const QString& msg ) {
		QMessageBox::critical( 0, caption, msg );
	}
	void infoDialog( const QString& caption, const QString& msg ) {
		QMessageBox::information( 0, caption, msg );
	}
	void questionDialog( const QString& caption, const QString& msg ) {
		QMessageBox::question( 0, caption, msg );
	}
	void warningDialog( const QString& caption, const QString& msg ) {
		QMessageBox::warning( 0, caption, msg );
	}

	QVariantMap dpi() const {
		QVariantMap m;
		m[ "x" ] = QApplication::desktop()->physicalDpiX();
		m[ "y" ] = QApplication::desktop()->physicalDpiY();
        return m;
	}

};

}
