#include "DynamicQObject.h"

bool DynamicQObject::Connect( QObject *obj, const char *signal, const char *slot ) {
    QByteArray theSignal = QMetaObject::normalizedSignature(signal);
    QByteArray theSlot = QMetaObject::normalizedSignature(slot);
    if (!QMetaObject::checkConnectArgs(theSignal, theSlot))
        return false;

    int signalId = obj->metaObject()->indexOfSignal(theSignal);
    if (signalId < 0) 
        return false;

    int slotId = slotIndices.value(theSlot, -1);
    if (slotId < 0) {
        slotId = slotList.size();
        slotIndices[theSlot] = slotId;
        slotList.append(createSlot(theSlot.data()));
    }
    return QMetaObject::connect(obj, signalId, this, slotId + metaObject()->methodCount());
}

int DynamicQObject::qt_metacall(QMetaObject::Call c, int id, void **arguments) {
    id = QObject::qt_metacall(c, id, arguments);
    if (id < 0 || c != QMetaObject::InvokeMetaMethod) 
        return id;
    Q_ASSERT(id < slotList.size());
    slotList[id]->Invoke( arguments );
    return -1;
}

