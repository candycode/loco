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

#include <QVariant>
#include <QtGui/QDesktopWidget>
#include <QRect>
#include <QtGui/QFileDialog>
#include <QtGui/QColorDialog>
#include <QtGui/QColor>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include "LocoObject.h"

namespace loco {

class GUI : public Object {
    Q_OBJECT

public:
   GUI() : Object( 0, "LocoGUI", "Loco/GUI" ) {}
   GUI* Clone() const { return new GUI; }
public slots:
    bool cloneable() const { return true; }
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
                            const QString& filePath = QString(),
                            const QString& filter = QString() ) {

        return QFileDialog::getSaveFileName ( 0,  caption, filePath, filter );
    }
    QString openFileDialog( const QString& caption = QString(),
                            const QString& filePath = QString(),
                            const QString& filter = QString() ) {
        return QFileDialog::getOpenFileName( 0, caption, filePath, filter );
    }    
    QStringList openFilesDialog( const QString& caption = QString(),
                                 const QString& filePath = QString(),
                                 const QString& filter = QString() ) {
        return QFileDialog::getOpenFileNames( 0, caption, filePath, filter );
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
