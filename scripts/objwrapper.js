/*
class QObjectWrapper : public Object {
	Q_OBJECT
	Q_PROPERTY( QString msg READ msg WRITE setMsg )
public:
	QObjectWrapper( const QString& m ) : msg_( m ) {}
    const QString& msg() const { return msg_; }
    void setMsg( const QString& msg ) { msg_ = msg; }
public slots:
    QString text() const { return "TEXT FROM METHOD"; }
private:
	QString msg_;
};

...
class...
public slots:
    QVariant testCBack();
signals:
    void testJSCBack( QObject* );

...

QVariant <class>::testCBack() {
    QObjectWrapper* ow = new QObjectWrapper( "ciao" );
    QVariant obj = GetContext()->AddObjToJSContext( ow );
    emit testJSCBack( ow );
    return obj;
}
*/


try {
  var print = Loco.console.println;
  myobject.testJSCBack.connect( function( obj ) { 
     obj.msg = "CHANGED FROM JAVASCRIPT";
     print( obj.text() );
  } );
  var obj = mybobject.testCBack();
  print( obj.msg );
  Loco.ctx.exit( 0 ); 
  
} catch(e) {
  Loco.console.printerrln(e);
  Loco.ctx.quit( -1 );
}

