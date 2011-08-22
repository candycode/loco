#pragma once
//#SRCHEADER

#include <QObject>
#include <QString>
#include <iostream>

namespace loco {

class FSystem : public QObject {
    Q_OBJECT
	// QDir::Filters
	Q_PROPERTY( int Dirs READ Dirs CONSTANT )
    Q_PROPERTY( int AllDirs READ AllDirs CONSTANT )
	Q_PROPERTY( int Files READ Files CONSTANT )
	Q_PROPERTY( int Drives READ Drives CONSTANT )
    Q_PROPERTY( int NoSymLinks READ NoSymLinks CONSTANT )
	Q_PROPERTY( int NoDotAndDotDot READ NoDotAndDotDot CONSTANT )
	Q_PROPERTY( int AllEntries READ AllEntries CONSTANT )
	Q_PROPERTY( int Readable READ Readable CONSTANT )
	Q_PROPERTY( int Writable READ Writable CONSTANT )
	Q_PROPERTY( int Executable READ Executable CONSTANT )
	Q_PROPERTY( int Modified READ Modified CONSTANT )
	Q_PROPERTY( int Hidden READ Hidden CONSTANT )
	Q_PROPERTY( int System READ System CONSTANT )
	Q_PROPERTY( int CaseSensitive READ CaseSensitive CONSTANT )
	// QDir::SortFlags
    Q_PROPERTY( int Name READ Name CONSTANT )
	Q_PROPERTY( int Time READ Time CONSTANT )
	Q_PROPERTY( int Size READ Size CONSTANT )
	Q_PROPERTY( int Type READ Type CONSTANT )
	Q_PROPERTY( int Unsorted READ Unsorted CONSTANT )
	Q_PROPERTY( int NoSort READ NoSort CONSTANT )
	Q_PROPERTY( int Unsorted READ Unsorted CONSTANT )
	Q_PROPERTY( int DirsFirst READ DirsFirst CONSTANT )
	Q_PROPERTY( int Reversed READ Reversed CONSTANT )
	Q_PROPERTY( int IgnoreCase READ IgnoreCase CONSTANT )
	Q_PROPERTY( int LocaleAware READ LocaleAware CONSTANT )

public:
	// QDir::Filters
	int Dirs() const { return int( QDir::Dirs ); }
	int AllDirs() const { return int( QDir::AllDirs ); }
	int Files() const { return int( QDir::Files ); }
	int Drives() const { return int( QDir::Drives ); }
	int NoSymLinks() const { return int( QDir::NoSymLinks ); }
	int NoDotAndDotDot() const { return int( QDir::NoDotAndDotDot ); }
	int AllEntries() const { return int( QDir::AllEntries ); }
	int Readable() const { return int( QDir::Readable ); }
	int Writable() const { return int( QDir::Writable ); }
	int Executable() const { return int( QDir::Executable ); }
	int Modified() const { return int( QDir::Modified ); }
	int Hidden() const { return int( QDir::Hidden ); }
	int System() const { return int( QDir::System ); }
	int CaseSensitive() const { return int( QDir::CaseSensitive ); }
	// QDir::SortFlags
	int Name() const { return int( QDir::Name ); }
	int Time() const { return int( QDir::Time ); }
	int Size() const { return int( QDir::Size ); }
	int Type() const { return int( QDir::Type ); }
	int Unsorted() const { return int( QDir::Unsorted ); }
	int NoSort() const { return int( QDir::NoSort ); }
	int DirsFirst() const { return int( QDir::DirsFirst ); }
	int DirsLast() const { return int( QDir::DirsLast ); }
	int Reversed() const { return int( QDir::Reversed ); }
	int IgnoreCase() const { return int( QDir::IgnoreCase ); }
	int LocaleAware() const { return int( QDir::LocaleAware ); }

public:
	FSystem() : dir_( QDir::current() ) {
	    InitDefaults();
	}
public slots:
	QString currentDir() const {
		return dir_.absolutePath();
	}
	bool setCurrentDir( const QString& d ) {
		return dir_.setPath( d );
	}
	QString homeDir() const {
		return QDir::homePath();
	}
	QString rootDir() const {
		return QDir::rootPath();
	}
	QString tmpDir() const {
		return QDir::tempPath();
	}
	QStringList ls( const QStringList& nameFilters,
		            const QList< int >& filters   = QList< int >(),
					const QList< int >& sortFlags = QList< int >() ) {
	    
		int flt = 0;
		if( filters.empty() ) flt = filtersDefault_;
		else
		    for( QList< int >::const_iterator f = filters.begin();
			    f != filters.end(); ++f ) flt |= *f;
        int sf = 0;
		if( sortFlags.empty() ) sf = sortFlagsDefault_;
		else
		    for( QList< int >::const_iterator s = sortFlags.begin();
			    s != sortFlags.end(); ++s ) sf |= *s;
        return dir_.entryList( nameFilters, flt, sf );
	}
	QStringList ls( const QStringList& nameFilters, int filters, int sortFlags ) {	          
		return dir_.entryList( nameFilters, QDir::Filters( filters ), QDir::SortFlags( sf ) );
	}
	QString canonicalPath() const { return dir_.canonicalPath(); }
	bool cd( const QString& p ) { return dir_.cd( p ); }
	bool cdUp() { return dir_.cdUp(); }
	uint count() const { return dir_.count(); }
	QString dirName() const { return dir_.dirName(); }
	bool exists( const QString& n ) const { return dir_.exists( n ); }
	void refresh() { dir_.refresh(); }
private:
	QFile file_;
	QDir  dir_;
	QDir::Filters   filtersDefault_;
	QDir::SortFlags sortFlagsDefault_;
};
}