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

#include "LocoFile.h"
#include "LocoContext.h"

namespace loco {

bool File::open( const QStringList& openMode ) {
    if( GetContext() == 0 || GetContext()->GetFileAccessManager() == 0 ) {
        error( "FileAccessManager not available, aborting open operation" );
        return false;
    }
    const FileAccessManager* fm = GetContext()->GetFileAccessManager();
    if( !fm->CheckAccess( file_.fileName(), MapOpenMode( openMode ) ) ) {
        error( "Not authorized to access file " + file_.fileName() );
        return false;
    } else if( file_.fileName().isEmpty() ) {
        error( "Empty file name" );
        return false; 
    } else if( file_.open( MapOpenMode( openMode ) ) ) {
        return true;
    } else {
        error( "File " + file_.fileName() + " open failed" );
        return false;
    }            
}

QIODevice::OpenMode File::MapOpenMode( const QStringList& mode ) {
    QIODevice::OpenMode om = 0;
    for( QStringList::const_iterator i = mode.begin(); i != mode.end(); ++i ) {
        const QString& m = *i;
        if( m == "r" ) om |= QIODevice::ReadOnly;
        else if( m == "w" ) om |= QIODevice::WriteOnly;
        else if( m == "a" ) om |= QIODevice::Append;
        else if( m == "truncate" ) om |= QIODevice::Truncate;
        else if( m == "text" ) om |= QIODevice::Text;
        else if( m == "unbuffered" ) om |= QIODevice::Unbuffered;
    }
    return om;   
} 

}
