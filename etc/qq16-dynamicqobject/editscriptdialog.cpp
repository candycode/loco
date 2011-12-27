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

#include "editscriptdialog.h"

#include <QByteArray>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>

void EditScriptDialog::init()
{
    textEdit = new QTextEdit;
    QPushButton *ok = new QPushButton("Done");

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(textEdit);
    layout->addWidget(ok);

    connect(ok, SIGNAL(clicked(bool)), this, SLOT(accept()));
    connect(this, SIGNAL(finished(int)), this, SLOT(copyResult()));

    this->setLayout(layout);

    textEdit->setPlainText(QLatin1String(script->data()));

   setWindowTitle("Edit script");
}

void EditScriptDialog::copyResult()
{
    *script = textEdit->toPlainText().toLatin1();
}
