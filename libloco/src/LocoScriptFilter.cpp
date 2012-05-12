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

#include "LocoScriptFilter.h"

namespace loco {

QString ScriptFilter::Apply( const QString& ss ) const {
   QString s = ss.trimmed() + "\n";
    s = "\"" + s.replace( "\"", "\\\"" ) + "\"";
    s.remove( "\r" );
    s.replace("\n", "\\n");
    if( !jcodeBegin_.isEmpty() ) {
        jsInterpreter_->EvaluateJavaScript( jcodeBegin_ );
    }
    QVariant r;
    // no placeholder, assume it's a function call
    if( codePlaceHolder_.isEmpty() ) {
       r = jsInterpreter_->EvaluateJavaScript( jfun_ + "(" +   s  + ");" );
   // placeholder, replace with translated code
    } else {
       QString nj = jfun_;
       const QString& njref = nj.replace( codePlaceHolder_, s );
       r = jsInterpreter_->EvaluateJavaScript( njref );
   }
   if( r.isNull() || !r.isValid() ) {
       QVariant e = jsInterpreter_->EvaluateJavaScript( jerrfun_ );
       if( !e.isNull() && e.isValid() ) error( e.toString() );
       else error( "Error parsing code" );
       return s;
   }
   if( r.isValid() ) {
    if( !jcodeEnd_.isEmpty() ) jsInterpreter_->EvaluateJavaScript( jcodeEnd_ );
    return r.toString();
   }
   else return "";
}

}

