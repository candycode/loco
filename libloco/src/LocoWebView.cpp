//#SRCHEADER
#include <QPrinter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

#include "LocoWebView.h"


namespace loco {

namespace {

static QMap< QString, QPrinter::PaperSize > paperTypeStringToQtG;
static QMap< QString, QPrinter::Unit > unitStringToQtG;


void InitPaperTypeMap() {
#ifdef LOCO_PAPERSIZE_MAP
  #error "LOCO_PAPERSIZE_MAP already defined"
#endif
#define LOCO_PAPERSIZE_MAP( P ) paperTypeStringToQtG[ #P ] = QPrinter::P
    LOCO_PAPERSIZE_MAP( A0 );LOCO_PAPERSIZE_MAP( B6 );
    LOCO_PAPERSIZE_MAP( A1 );LOCO_PAPERSIZE_MAP( B7 );
    LOCO_PAPERSIZE_MAP( A2 );LOCO_PAPERSIZE_MAP( B8 );
    LOCO_PAPERSIZE_MAP( A3 );LOCO_PAPERSIZE_MAP( B9 );
    LOCO_PAPERSIZE_MAP( A4 );LOCO_PAPERSIZE_MAP( B10 );
    LOCO_PAPERSIZE_MAP( A5 );LOCO_PAPERSIZE_MAP( C5E );
    LOCO_PAPERSIZE_MAP( A6 );LOCO_PAPERSIZE_MAP( Comm10E );
    LOCO_PAPERSIZE_MAP( A7 );LOCO_PAPERSIZE_MAP( DLE );
    LOCO_PAPERSIZE_MAP( A8 );LOCO_PAPERSIZE_MAP( Folio );
    LOCO_PAPERSIZE_MAP( A9 );LOCO_PAPERSIZE_MAP( Ledger );
    LOCO_PAPERSIZE_MAP( B0 );LOCO_PAPERSIZE_MAP( Tabloid );
    LOCO_PAPERSIZE_MAP( B1 );LOCO_PAPERSIZE_MAP( Custom );
    LOCO_PAPERSIZE_MAP( B2 );LOCO_PAPERSIZE_MAP( Legal );
    LOCO_PAPERSIZE_MAP( B3 );LOCO_PAPERSIZE_MAP( Letter );
    LOCO_PAPERSIZE_MAP( B4 );LOCO_PAPERSIZE_MAP( B5 );

#undef LOCO_PAPERSIZE_MAP
}
void InitUnitMap() {
#ifdef LOCO_UNIT_MAP
  #error "LOCO_UNIT_MAP already defined"
#endif
#define LOCO_UNIT_MAP( U ) unitStringToQtG[ #U ] = QPrinter::U
	LOCO_UNIT_MAP( Millimeter );
	LOCO_UNIT_MAP( Point );
	LOCO_UNIT_MAP( Inch );
	LOCO_UNIT_MAP( Pica );
	LOCO_UNIT_MAP( Didot );
	LOCO_UNIT_MAP( Cicero );
	LOCO_UNIT_MAP( DevicePixel );
}

}

QString WebView::SavePDF( const QString& filePath, const QVariantMap& options ) const {
    if( paperTypeStringToQtG.size() == 0 ) {
    	InitPaperTypeMap();
    	InitUnitMap();
    }
	QPrinter p;
	p.setOutputFileName( filePath );
	p.setOutputFormat( QPrinter::PdfFormat );
	for( QVariantMap::const_iterator i = options.begin(); i != options.end(); ++i ) {
		const QString& op = i.key();
		if( op == "orientation" ) {
			if( i.value().toString() == "portrait" ) p.setOrientation( QPrinter::Portrait );
			else if( i.value().toString() == "landscape" ) p.setOrientation( QPrinter::Landscape );
			else return "Error: unknown orientation - " + i.value().toString();
		} else if( op == "colormode" ) {
			if( i.value().toString() == "color" ) p.setColorMode( QPrinter::Color );
			else if( i.value().toString() == "grayscale" || i.value() == "greyscale" ) p.setColorMode( QPrinter::GrayScale );
			else return "Error: unknown colormode - " + i.value().toString();
		} else if( op == "paperType" ) {
			if( !paperTypeStringToQtG.contains( i.value().toString() ) ) return "Error: unknown paper type - " + i.value().toString();
			p.setPaperSize( paperTypeStringToQtG[ i.value().toString() ] );
		} else if( op == "paperSize" ) {
            QVariantMap s = i.value().toMap();
            if( !s.isEmpty() ||
            	!s.contains( "w" ) ||
            	!s.contains( "h" ) ||
            	!s.contains( "unit" ) ) return "Error: invalid paperSize format";
            if( !unitStringToQtG.contains( s[ "unit" ].toString() ) ) return "Error: invalid unit - " + s[ "unit" ].toString();
            p.setPaperSize( QSizeF( s[ "w" ].toReal(), s[ "h" ].toReal() ), unitStringToQtG[ s[ "unit" ].toString() ] );
		}
	}
	page()->mainFrame()->print( &p );
	return "";
}

bool WebView::SaveUrl( const QString& url, const QString& filename, int timeout ) {
      QNetworkAccessManager* nam = this->page()->networkAccessManager();
      QFile out( filename );
      if( !out.open( QFile::WriteOnly )  ) {
    	  return false;
      }
      QNetworkReply* reply = nam->get( QNetworkRequest( QUrl( url ) ) );
      QEventLoop loop;
      QObject::connect( nam, SIGNAL( finished( QNetworkReply* ) ), &loop, SLOT( quit() ) );
      // soft real-time guarantee: kill network request if the total time is >= timeout
      QTimer::singleShot( timeout, &loop, SLOT( quit() ) );
      // Execute the event loop here, now we will wait here until readyRead() signal is emitted
      // which in turn will trigger event loop quit.
      loop.exec();
      if( reply->isRunning() ) {
          reply->close();
          return false;
      }
      else {
    	  while( reply->bytesAvailable() > 0 ) out.write( reply->read( reply->bytesAvailable() ) );
    	  out.close();
    	  reply->close();
    	  return true;
      }
  }


}


/*
QPrinter::A0	5	841 x 1189 mm
QPrinter::A1	6	594 x 841 mm
QPrinter::A2	7	420 x 594 mm
QPrinter::A3	8	297 x 420 mm
QPrinter::A4	0	210 x 297 mm, 8.26 x 11.69 inches
QPrinter::A5	9	148 x 210 mm
QPrinter::A6	10	105 x 148 mm
QPrinter::A7	11	74 x 105 mm
QPrinter::A8	12	52 x 74 mm
QPrinter::A9	13	37 x 52 mm
QPrinter::B0	14	1000 x 1414 mm
QPrinter::B1	15	707 x 1000 mm
QPrinter::B2	17	500 x 707 mm
QPrinter::B3	18	353 x 500 mm
QPrinter::B4	19	250 x 353 mm
QPrinter::B5	1	176 x 250 mm, 6.93 x 9.84 inches
QPrinter::B6	20	125 x 176 mm
QPrinter::B7	21	88 x 125 mm
QPrinter::B8	22	62 x 88 mm
QPrinter::B9	23	33 x 62 mm
QPrinter::B10	16	31 x 44 mm
QPrinter::C5E	24	163 x 229 mm
QPrinter::Comm10E	25	105 x 241 mm, U.S. Common 10 Envelope
QPrinter::DLE	26	110 x 220 mm
QPrinter::Executive	4	7.5 x 10 inches, 190.5 x 254 mm
QPrinter::Folio	27	210 x 330 mm
QPrinter::Ledger	28	431.8 x 279.4 mm
QPrinter::Legal	3	8.5 x 14 inches, 215.9 x 355.6 mm
QPrinter::Letter	2	8.5 x 11 inches, 215.9 x 279.4 mm
QPrinter::Tabloid	29	279.4 x 431.8 mm
QPrinter::Custom	30	Unknown, or a user defined size.
*/



