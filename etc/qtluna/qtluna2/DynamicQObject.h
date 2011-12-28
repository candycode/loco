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

#ifndef DYNAMICQOBJECT_H
#define DYNAMICQOBJECT_H

#include <QHash>
#include <QList>
#include <QMetaObject>
#include <QObject>

class DynamicSlot 
{
public:
    virtual void call(QObject *sender, void **arguments) = 0;
};

class DynamicQObject: public QObject
{
public:
    DynamicQObject(QObject *parent = 0) : QObject(parent) { }

    virtual int qt_metacall(QMetaObject::Call c, int id, void **arguments);

    bool emitDynamicSignal(char *signal, void **arguments);	
    bool connectDynamicSlot(QObject *obj, char *signal, char *slot);
    bool connectDynamicSignal(char *signal, QObject *obj, char *slot);

    virtual DynamicSlot *createSlot(char *slot) = 0;    

private:
    QHash<QByteArray, int> slotIndices;
    QList<DynamicSlot *> slotList;
    QHash<QByteArray, int> signalIndices;
};

#endif
