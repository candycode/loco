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

#ifndef SUBCLASS_H
#define SUBCLASS_H

#include "dynamicqobject.h"

#include <QByteArray>

class Calculator;

class MyDynamicQObject : public DynamicQObject
{
public:
    MyDynamicQObject(Calculator *parent);

    DynamicSlot *createSlot(char *slot);

private:
    Calculator *app;
};

class Slot : public DynamicSlot
{
public:
    Slot(MyDynamicQObject *parent, Calculator *app);

    void call(QObject *sender, void **arguments);

private:
    QByteArray nextToken();
    void execute();
    int expression();

    QByteArray runningScript;
    MyDynamicQObject *parent;
    Calculator *app;
};

#endif
