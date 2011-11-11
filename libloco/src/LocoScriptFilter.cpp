//#SRCHEADER
#include "LocoScriptFilter.h"



namespace loco {

QString ScriptFilter::Apply( const QString& ss ) const {
   QString s = ss.trimmed() + "\n";
	s = "\"" + s.replace( "\"", "\\\"" ) + "\"";
	std::cout << s.toStdString() << std::endl;
    s.remove( "\r" );
	s.replace("\n", "\\n");
	if( !jcodeBegin_.isEmpty() ) {
		jsInterpreter_->EvaluateJavaScript( jcodeBegin_ );
	}
	QVariant r;
	// no placeholder, assume it's a function call
   if( codePlaceHolder_.isEmpty() ) {
	std::cout << jfun_.toStdString() << std::endl;
	std::cout << s.toStdString() << std::endl;
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

