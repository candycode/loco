#pragma once
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


#include <QList>
#include "LocoObject.h"

namespace loco {

class Context;

class QObjectArray : public Object {
    Q_OBJECT
    Q_PROPERTY( int size READ GetSize )
    Q_PROPERTY( bool empty READ Empty )
public:
   QObjectArray( Context* c = 0 ) : Object( c, "LocoQObjectArray", "Loco/Context" ) {}
   int GetSize() const { return objlist_.size(); }
   bool Empty() const { return objlist_.isEmpty(); }
   const QList< QObject* >& GetList() const { return objlist_; } 
public slots:
   void pushback( QObject* obj ) { objlist_.push_back( obj ); }
   void popfront() { objlist_.pop_front(); }
   QObject* front() { return objlist_.front(); }
   QObject* back() { return objlist_.back(); }
   QObject* at( int i ) { return objlist_[ i ]; }
   void remove( int i ) { objlist_.removeAt( i ); }
   void insert( int i, QObject* obj ) { objlist_.insert( i, obj ); }
   void clear() { objlist_.clear(); }
private:
  QList< QObject* > objlist_;
};

}
