#pragma once
//#SRCHEADER
class QWebView;

typedef QWebView* QWebViewPtr;

namespace loco {

struct IWebConfig {
	virtual void Shutdown( QWebViewPtr ) = 0;
	virtual void Init( QWebViewPtr ) = 0;
	virtual ~IWebConfig() = 0 {}
};

}