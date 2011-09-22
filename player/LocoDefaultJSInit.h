#pragma once
//#SRCHEADER

#include <QString>
#include <QMap>
#include <QRegExp>

#include "LocoContext.h"

namespace loco {

class DefaultJSInit : public IJavaScriptInit {
public:
    DefaultJSInit( Context* ctx ) : ctx_( ctx ) {
        dictionary_[ "Context"    ] = "ctx";
        dictionary_[ "FileSystem"] = "fs";
        dictionary_[ "System"     ] = "sys";  
    }
    void SetContext( Context* ctx ) { ctx_ = ctx; }    
    QString GenerateCode() const {
        const QString& GL = ctx_->GetJSGlobalNameForContext();
        QStringList sl;
        sl << "var " + GL + " = {";    
        const JScriptObjCtxInstances& stdObjs = ctx_->GetStdJSObjects();
        for( JScriptObjCtxInstances::const_iterator i = stdObjs.begin();
             i != stdObjs.end(); ++i ) {
            QString entry = ConvertNameToJS( (*i)->name() ) + ": " + (*i)->jsInstanceName();
            if( i != ( --stdObjs.end() ) ) entry += ",";  
            sl << entry;        
        }
        sl << "};";
        return sl.join( "\n" );
    }
    void ResetMap() { dictionary_.clear(); }
    void Map( const QString& key, const QString& value ) {
        dictionary_[ key ] = value;
    }
private:
    typedef QMap< QString, QString > Dictionary;
    QString ConvertNameToJS( const QString& n ) const {
        QRegExp r( "Loco(.+)" );
        if( r.indexIn( n ) > -1  ) {
            QString cap = r.cap( 1 );
            Dictionary::const_iterator i = dictionary_.find( cap );
            if( i != dictionary_.end() ) return i.value();
            else return cap.toLower(); 
        }
        else return n; 
    }
private:
    Context*   ctx_;
    Dictionary dictionary_;        

};	


}