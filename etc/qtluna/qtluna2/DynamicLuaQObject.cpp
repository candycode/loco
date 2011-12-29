#include "LuaContext.h"
#include "DynamicLuaQObject.h"

namespace qlua {

void PushLuaValue( LuaContext* lc, QMetaType::Type type, void* arg ) {
    switch( type ) {
    case QMetaType::Bool: lua_pushinteger( lc->LuaState(), *reinterpret_cast< bool* >( arg ) );
		                 break;
    case QMetaType::Char: lua_pushnumber( lc->LuaState(), *reinterpret_cast< char* >( arg ) );
		                  break;
    case QMetaType::UChar: lua_pushnumber( lc->LuaState(), *reinterpret_cast< unsigned char* >( arg ) );
		                   break;
	case QMetaType::UInt: lua_pushnumber( lc->LuaState(), *reinterpret_cast< unsigned* >( arg ) );
		                 break;
    case QMetaType::Int: lua_pushinteger( lc->LuaState(), *reinterpret_cast< int* >( arg ) );
		                 break;
    case QMetaType::LongLong: lua_pushnumber( lc->LuaState(), *reinterpret_cast< long long * >( arg ) );
		                      break;
    case QMetaType::ULongLong: lua_pushnumber( lc->LuaState(), *reinterpret_cast< unsigned long long * >( arg ) );
		                       break;
    case QMetaType::Float: lua_pushnumber( lc->LuaState(), *reinterpret_cast< float* >( arg ) );
		                    break;
	case QMetaType::Double: lua_pushnumber( lc->LuaState(), *reinterpret_cast< double* >( arg ) );
		                    break;
	case QMetaType::QString: lua_pushstring( lc->LuaState(), (reinterpret_cast< QString* >( arg ))->toAscii().constData() );
		                     break;
    case QMetaType::QWidgetStar: lc->AddQObject( reinterpret_cast< QObject* >( arg ) );
		                         break;
    case QMetaType::QObjectStar: lc->AddQObject( reinterpret_cast< QObject* >( arg ) );
		                         break;
	default: throw std::logic_error( "Type not supported" );
	}
}


bool DynamicLuaQObject::Connect( QObject *obj, const char *signal, const char *slot ) {
    QByteArray theSignal = QMetaObject::normalizedSignature(signal);
    QByteArray theSlot = QMetaObject::normalizedSignature(slot);
    if ( !QMetaObject::checkConnectArgs(theSignal, theSlot) ) 
        return false;

    int signalId = obj->metaObject()->indexOfSignal(theSignal);
    if (signalId < 0) 
        return false;

    int slotId = slotIndices.value(theSlot, -1);
    if (slotId < 0) {
        slotId = slotList.size();
        slotIndices[theSlot] = slotId;
        slotList.append(CreateLuaCBackSlot(theSlot.data()));
    }
    return QMetaObject::connect(obj, signalId, this, slotId + metaObject()->methodCount());
}

int DynamicLuaQObject::qt_metacall(QMetaObject::Call c, int id, void **arguments) {
    id = QObject::qt_metacall(c, id, arguments);
    if (id < 0 || c != QMetaObject::InvokeMetaMethod) 
        return id;
    Q_ASSERT(id < slotList.size());
    slotList[id]->Invoke( arguments );
    return -1;
}

LuaCBackSlot* DynamicLuaQObject::CreateLuaCBackSlot( const char *slot ) {
    return new LuaCBackSlot( lc_, luaRefMap_[ slot ].paramTypes, luaRefMap_[ slot ].luaCBackRef );
}

void LuaCBackSlot::Invoke( void **arguments ) {
	for( ParameterTypes::const_iterator i = paramTypes_.begin();
		 i != paramTypes_.end(); ++i, ++arguments ) {
	    PushLuaValue( lc_, *i, *arguments );
	}
	lua_rawgeti( lc_->LuaState(), LUA_REGISTRYINDEX, luaCBackRef_ );
	lua_pcall( lc_->LuaState(), paramTypes_.size(), 0, 0 );
}

}

//enum Type {
//        // these are merged with QVariant
//        Void = 0, Bool = 1, Int = 2, UInt = 3, LongLong = 4, ULongLong = 5,
//        Double = 6, QChar = 7, QVariantMap = 8, QVariantList = 9,
//        QString = 10, QStringList = 11, QByteArray = 12,
//        QBitArray = 13, QDate = 14, QTime = 15, QDateTime = 16, QUrl = 17,
//        QLocale = 18, QRect = 19, QRectF = 20, QSize = 21, QSizeF = 22,
//        QLine = 23, QLineF = 24, QPoint = 25, QPointF = 26, QRegExp = 27,
//        QVariantHash = 28, QEasingCurve = 29, LastCoreType = QEasingCurve,
//
//        FirstGuiType = 63 /* QColorGroup */,
//#ifdef QT3_SUPPORT
//        QColorGroup = 63,
//#endif
//        QFont = 64, QPixmap = 65, QBrush = 66, QColor = 67, QPalette = 68,
//        QIcon = 69, QImage = 70, QPolygon = 71, QRegion = 72, QBitmap = 73,
//        QCursor = 74, QSizePolicy = 75, QKeySequence = 76, QPen = 77,
//        QTextLength = 78, QTextFormat = 79, QMatrix = 80, QTransform = 81,
//        QMatrix4x4 = 82, QVector2D = 83, QVector3D = 84, QVector4D = 85,
//        QQuaternion = 86,
//        LastGuiType = QQuaternion,
//
//        FirstCoreExtType = 128 /* VoidStar */,
//        VoidStar = 128, Long = 129, Short = 130, Char = 131, ULong = 132,
//        UShort = 133, UChar = 134, Float = 135, QObjectStar = 136, QWidgetStar = 137,
//        QVariant = 138,
//        LastCoreExtType = QVariant,
//
//// This logic must match the one in qglobal.h
//#if defined(QT_COORD_TYPE)
//        QReal = 0,
//#elif defined(QT_NO_FPU) || defined(QT_ARCH_ARM) || defined(QT_ARCH_WINDOWSCE) || defined(QT_ARCH_SYMBIAN)
//        QReal = Float,
//#else
//        QReal = Double,
//#endif
//
//        User = 256
