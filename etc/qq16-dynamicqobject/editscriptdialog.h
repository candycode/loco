/****************************************************************************
**
** Copyright (C) 2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef EDITSCRIPTDIALOG_H
#define EDITSCRIPTDIALOG_H

#include <QDialog>

class QByteArray ;
class QWidget ;
class QTextEdit ;

class EditScriptDialog: public QDialog
{
    Q_OBJECT
public:
    inline EditScriptDialog(QWidget *parent, QByteArray *scriptToEdit);

    void init();

public slots:
    void copyResult();

private:
    QByteArray *script;
    QTextEdit *textEdit;
};

inline EditScriptDialog::EditScriptDialog(QWidget *parent, QByteArray *scriptToEdit)
: QDialog(parent), script(scriptToEdit)
{
    init();
}

#endif
