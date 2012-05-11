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


#include <QString>
#include <QMap>
#include <QRegExp>
#include <QPointer>

#include "LocoContext.h"

namespace loco {

class DefaultJSInit : public IJavaScriptInit {
public:
    DefaultJSInit( Context* ctx ) : ctx_( ctx ) {
        dictionary_[ "Context"    ] = "ctx";
        dictionary_[ "FileSystem" ] = "fs";
        dictionary_[ "System"     ] = "sys";
        dictionary_[ "Network"    ] = "net";
        dictionary_[ "GUI"        ] = "gui";
    }
    void SetContext( Context* ctx ) { ctx_ = ctx; }    
    QString GenerateCode( bool append = false ) const {
		if( !cache_.isEmpty() ) return cache_;
        const QString& GL = ctx_->GetJSGlobalNameForContext();
        QStringList sl;
		if( append ) { // append objects to pre-existing object
		    const JScriptObjCtxInstances& stdObjs = ctx_->GetStdJSObjects();
            for( JScriptObjCtxInstances::const_iterator i = stdObjs.begin();
                 i != stdObjs.end(); ++i ) {
                sl << GL + "." + ConvertNameToJS( (*i)->name() )  + 
					  " = " + (*i)->jsInstanceName() + ";";
			}
		} else { // create new object
		    sl << "var " + GL + " = {";    
            const JScriptObjCtxInstances& stdObjs = ctx_->GetStdJSObjects();
            for( JScriptObjCtxInstances::const_iterator i = stdObjs.begin();
                 i != stdObjs.end(); ++i ) {
                QString entry = ConvertNameToJS( (*i)->name() ) + ": " + (*i)->jsInstanceName();
                if( i != ( --stdObjs.end() ) ) entry += ",";  
                sl << entry;
			}
            sl << "};";
        }

#ifndef LOCO_WKIT
		sl << "var console = " + ctx_->GetJSGlobalNameForContext() + ".console;";
#endif
        cache_ = sl.join( "\n" );
		return cache_;
    }
    void ResetMap() { dictionary_.clear(); }
    void Map( const QString& key, const QString& value ) {
        dictionary_[ key ] = value;
    }
	void ResetCache() const { cache_ = QString(); }
private:
    typedef QMap< QString, QString > Dictionary;
    QString ConvertNameToJS( const QString& n ) const {
    	QRegExp r( "Loco(.+)" );
        if( r.indexIn( n ) > -1  ) {
            QString cap = r.cap( 1 );
            Dictionary::const_iterator i = dictionary_.find( cap );
            if( i != dictionary_.end() ) return i.value();
            else {
            	cap[ 0 ] = cap[ 0 ].toLower();
            	return cap;
            }
        }
        else return n; 
    }
private:
    QPointer< Context >  ctx_;
    Dictionary dictionary_;
	mutable QString cache_;

};	


}
