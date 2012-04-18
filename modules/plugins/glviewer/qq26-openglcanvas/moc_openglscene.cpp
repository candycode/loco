/****************************************************************************
** Meta object code from reading C++ file 'openglscene.h'
**
** Created: Wed Apr 18 15:28:22 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "openglscene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'openglscene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OpenGLScene[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   13,   12,   12, 0x0a,
      43,   13,   12,   12, 0x0a,
      63,   12,   12,   12, 0x0a,
      79,   12,   12,   12, 0x0a,
     100,   12,   12,   12, 0x0a,
     121,  112,   12,   12, 0x0a,
     140,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_OpenGLScene[] = {
    "OpenGLScene\0\0enabled\0enableWireframe(bool)\0"
    "enableNormals(bool)\0setModelColor()\0"
    "setBackgroundColor()\0loadModel()\0"
    "filePath\0loadModel(QString)\0modelLoaded()\0"
};

void OpenGLScene::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OpenGLScene *_t = static_cast<OpenGLScene *>(_o);
        switch (_id) {
        case 0: _t->enableWireframe((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->enableNormals((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setModelColor(); break;
        case 3: _t->setBackgroundColor(); break;
        case 4: _t->loadModel(); break;
        case 5: _t->loadModel((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->modelLoaded(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OpenGLScene::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OpenGLScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_OpenGLScene,
      qt_meta_data_OpenGLScene, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OpenGLScene::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OpenGLScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OpenGLScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OpenGLScene))
        return static_cast<void*>(const_cast< OpenGLScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int OpenGLScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
