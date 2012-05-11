////////////////////////////////////////////////////////////////////////////////
//Copyright (c) 2012, Ugo Varetto
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//    * Neither the name of the copyright holder nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL UGO VARETTO BE LIABLE FOR ANY
//DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include <QObject>
#include <QtPlugin>
#include <QList>
#include <QVariantList>


struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class BenchmarkPlugin : public QObject, public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public:
signals:
    void emittedSignal( const QVariantList& ) const;
public slots:
    QList< int > copyIntArray( const QList< int >& ia ) const { return ia; }
    QVariant copy( const QVariantList& vl ) const { return vl; }
    void emitSignal( int times, int arrayLength ) const {
    	QVariantList array;
        array.reserve( arrayLength );
        for( int i = 0; i != arrayLength; ++i ) array.push_back( double( i ) );
        for( ; times; --times ) emit emittedSignal( array );
    }
};

Q_EXPORT_PLUGIN2( loco_BenchmarkPlugin, BenchmarkPlugin )
