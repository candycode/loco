#pragma once
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


#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace loco {

// derivation form QObject required to keep references into QPointers

struct IAuthenticator : public QObject {
	virtual QVariantMap Credentials( const QString& /*url*/ ) const = 0;
	virtual ~IAuthenticator() {}
};

class DefaultAuthenticator : public IAuthenticator {
public:
	DefaultAuthenticator( const QString& u = QString(), const QString& p = QString() )
	: user_( u ), password_( p ) {}
    const QString& user() const { return user_; }
    void setUser( const QString& u ) { user_ = u; }
    const QString& password() const { return password_; }
    void setPassword( const QString& p ) { password_ = p; }
    QVariantMap Credentials( const QString& /*url*/ ) const {
    	QVariantMap m;
    	m[ "user" ] = user_;
    	m[ "password" ] = password_;
    	return m;
    }
private:
	QString user_;
	QString password_;
};

}
