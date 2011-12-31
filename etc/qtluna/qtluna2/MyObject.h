#pragma once

#include <iostream>

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QList>
#include <QVector>

class MyObject : public QObject {
	Q_OBJECT
public slots:
	void method( const QString& msg ) {
		std::cout << msg.toStdString() << std::endl;
	}
	void emitSignal() { emit aSignal(); }
	void aSlot() { std::cout << "aSlot() called" << std::endl; }
	QString copyString( const QString& s ) { return s; }
	QVariantMap copyVariantMap( const QVariantMap& vm ) { return vm; }
	QVariantList copyVariantList( const QVariantList& vl ) { return vl; }
	QObject* createObject() { 
		MyObject* mo = new MyObject; 
		mo->setObjectName( "New Object" );
		return mo; //WARNING: NOT DESTROYED WHEN GARBAGE COLLECTED 
		           //SINCE DEFAULT IS 'QOBJ_NO_DELETE'
	}
	QList< float > copyFloatList( const QList< float >& l ) { return l; }
	QVector< float > copyFloatVector( const QVector< float >& v ) { return v; }
	QList< short > copyShortList( const QList< short >& l ) { return l; }
	QVector< short > copyShortVector( const QVector< short >& v ) { return v; }
signals:
	void aSignal();
};