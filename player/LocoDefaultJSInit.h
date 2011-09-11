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
        dictionary_[ "FileSystem "] = "fs";
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
           // sl << "." << ConvertNameToJS( (i*)->name() ) << ": " << (*i)->jsInstanceName() << ",";         
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
    QString ConvertNameToJS( const QString& n ) {
        QRegExp r( "Loco(.+)" );
        if( r.indexIn( n ) == QString( "Loco" ).length() ) {
            QString cap = r.cap( 1 ).toLower();
            Dictionary::const_iterator i = dictionary_.find( cap );
            if( i != dictionary_.end() ) return i.value();
            else return cap; 
        }
        else return n; 
    }
private:
    Context*   ctx_;
    Dictionary dictionary_;        

};	


}