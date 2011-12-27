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

#include "subclass.h"
#include "calculator.h"

#include <QByteArray>
#include <QVariant>
#include <QList>
#include <QVector>
#include <QMessageBox>

MyDynamicQObject::MyDynamicQObject(Calculator *parent) : DynamicQObject(parent), app(parent)
{ Q_ASSERT(app != 0); }

DynamicSlot *MyDynamicQObject::createSlot(char *)
{
    return new Slot(this, app);
}

Slot::Slot(MyDynamicQObject *_parent, Calculator *_app) : parent(_parent), app(_app)
{  Q_ASSERT(parent != 0); Q_ASSERT(app != 0); }

void Slot::call(QObject *sender, void **) 
{    
    runningScript = app->getScriptForObject(sender);

    execute();
}

void Slot::execute()
{
    int result = expression();

    QString strResult = QString::number(result);
    QVector<void *> args(2, 0);
    args[0] = 0;
    args[1] = &strResult;    

    parent->emitDynamicSignal("setResult(QString)", args.data());
}

int Slot::expression()
{
    QByteArray first = nextToken();
    bool ok;

    int value = first.toInt(&ok);
    if (!ok)
        value = app->getVal(&ok, first);

    if (!ok) {
        QMessageBox::warning(app, "Unknown identifier", 
            QString("Identifier must be either A, B or an integer: '%1'").arg(QString(first)));
        return 0;
    }
        
    QByteArray op = nextToken();

    // Expression is just one identifier
    if (op.length() == 0)
        return value;    

    // Or it's <identifier> <op> <expression>
    int rightSide = expression();

    if (op == "+")
        return value + rightSide;
    else if (op == "-")
        return value - rightSide;
    else if (op == "*")
        return value * rightSide;
    else if (op == "/")
        return rightSide != 0 ? value / rightSide : 0;
    else 
        QMessageBox::warning(app, "Unknown operator", QString("Operator must be +,-,* or /: '%1'").arg(QString(op)));

    return 0;
}

QByteArray Slot::nextToken()
{
    runningScript = runningScript.trimmed();
    QByteArray token;
    
    int pos = runningScript.indexOf(' ');
    if (pos < 0) {
        token = runningScript;
        runningScript = "";
    } else {
        token = runningScript.left(pos);
        runningScript.remove(0, pos);
    }       

    return token;
}
