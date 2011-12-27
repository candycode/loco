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

#include "calculator.h"
#include "subclass.h"
#include "editscriptdialog.h"

Calculator::Calculator(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    initConnections();
}

void Calculator::initConnections()
{
    dynamo = new MyDynamicQObject(this);

    connect(ui.plus, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editScript()));
    connect(ui.minus, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editScript()));
    connect(ui.multiply, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editScript()));
    connect(ui.divide, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(editScript()));

    ui.plus->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.minus->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.multiply->setContextMenuPolicy(Qt::CustomContextMenu);
    ui.divide->setContextMenuPolicy(Qt::CustomContextMenu);

    // When clicking a button, we perform its script
    dynamo->connectDynamicSlot(ui.plus, "clicked(bool)", "runScript()");
    dynamo->connectDynamicSlot(ui.minus, "clicked(bool)", "runScript()");
    dynamo->connectDynamicSlot(ui.multiply, "clicked(bool)", "runScript()");
    dynamo->connectDynamicSlot(ui.divide, "clicked(bool)", "runScript()");

    // When setting the result in a script, the dynamic signal setResult is emitted
    dynamo->connectDynamicSignal("setResult(QString)", ui.result, "setText(QString)");

    scripts[ui.plus] = "A + B";
    scripts[ui.minus] = "A - B";
    scripts[ui.multiply] = "A * B";
    scripts[ui.divide] = "A / B";
}

int Calculator::getVal(bool *ok, const QByteArray &identifier)
{
    if (identifier == "A") {
        *ok = true;
        return ui.inputA->value();
    } else if (identifier == "B") {
        *ok = true;
        return ui.inputB->value();
    }

    *ok = false;
    return 0;
}

QByteArray Calculator::getScriptForObject(QObject *obj)
{
    return scripts.value(obj, "");
}

void Calculator::editScript()
{
    QObject *obj = sender();
    QByteArray script = getScriptForObject(obj);

    EditScriptDialog d(this, &script);
    d.exec();

    scripts[obj] = script;
}
