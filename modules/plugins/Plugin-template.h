#include <QObject>
#include <QtPlugin>

// for plugins to work you need to derive from an interface and
// use the Q_INTERFACES() macros

struct IDummy {};
Q_DECLARE_INTERFACE(IDummy,"dummy")

class Plugin : public ..., public IDummy {
	Q_OBJECT
	Q_INTERFACES( IDummy )
public:
signals: //automatically exposed to JavaScript
public slots: // automatically exposed to JavaScript
};

Q_EXPORT_PLUGIN2( <plugin name>, Plugin )
//                   ^              ^
//                 plugin name    name of c++ class implementing the plugin
