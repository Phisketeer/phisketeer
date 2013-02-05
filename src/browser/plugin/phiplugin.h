/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This plug-in is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIPLUGIN_H
#define PHIPLUGIN_H

#include <QIODevice>
#include <QObject>
#include <QMetaClassInfo>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <qtbrowserplugin.h>
#include "phiawebview.h"

#ifdef QAXSERVER
#include <ActiveQt/QAxBindable>
#include <ActiveQt/QAxFactory>
#include <qt_windows.h>
#endif

// CLSID Safe for scripting: 7DD95801-9882-11CF-9FA9-00AA006C42C4
// CLSID Safe for initializing: 7DD95802-9882-11CF-9FA9-00AA006C42C4
// CLSID PhiPlugIn: F4463E56-F3CA-45e4-93FE-B8F007376088
// HKEY_LOCAL_MACHINE/SOFTWARE/Classes/{F446...6088}/Implemented Categories/
// CLSID old: 042D01BD-62BA-4D7E-B25A-B56E41DB1E22

#ifdef PHIDEBUG
#include <QPlainTextEdit>

class AMMsg : public QObject
{
    Q_OBJECT

public:
    static AMMsg* instance();
    void sendMsg( const QString& );

signals:
    void error( const QString& );

private:
    AMMsg( QObject *parent=0 );
    virtual ~AMMsg();
    static AMMsg* _instance;
};
#endif //PHIDEBUG

class PhiPlugIn : public QWidget, public QtNPBindable
#ifdef QAXSERVER
    , public QAxBindable
#endif
{
    Q_OBJECT

    Q_PROPERTY( QString src READ source WRITE setSource )
    Q_PROPERTY( qint32 phi READ phi WRITE setPhi )

    Q_CLASSINFO( "ClassID", "{F4463E56-F3CA-45E4-93FE-B8F007376088}" )
    Q_CLASSINFO( "InterfaceID", "{D0CFB976-1069-4548-8B88-41B060DC23D5}" )
    Q_CLASSINFO( "EventsID", "{B9C87539-4ED0-48C6-805F-7D95190DC95D}" )
    Q_CLASSINFO( "Version", "1.3" )
    Q_CLASSINFO( "ToSuperClass", "PhiPlugIn" )
    Q_CLASSINFO( "MIME", "application/x-phi:phi:Phi Plug-In" )
    Q_CLASSINFO( "DefaultProperty", "phi" )
    Q_CLASSINFO( "Creatable", "yes" )
    Q_CLASSINFO( "Description", "Phi plug-in for displaying Phi based content" )

public:
    PhiPlugIn( QWidget *parent=0 );
    virtual ~PhiPlugIn();

public slots:
    inline QString source() const { return _src; }
    void setSource( const QString &src );
    void setPhi( qint32 );
    inline qint32 phi() const { return _phi; }

protected slots:
    void slotLoading( bool );
    void slotError( const QString& );
    void slotUnsupportedContent( PHIAAbstractWebView*, QNetworkReply* );
    void slotJavaScriptConsoleMessage( const QString &msg, int lineNumber, const QString& sourceId );
    void slotProxyRequest( const QNetworkProxy &proxy, QAuthenticator *auth );
    void slotTitleChanged( PHIAAbstractWebView* );
    void slotIconChanged( PHIAAbstractWebView* );
    void slotGeometryChanged( const QRect &r );
    void slotUrlChanged( const QUrl &url );
    void slotLinkHovered( const QString&, const QString&, const QString& );

signals:
    void titleChanged( const QString& );
    void iconPathChanged( const QString& );
    void phiChanged( qint32 );
    void geometryChanged( qint32, qint32, qint32, qint32 );
    void contentChanged( const QString& );
    void linkHovered( const QString& );

protected:
    virtual bool readData( QIODevice *source, const QString &format );

private:
    QString _src;
    qint32 _phi;
    PHIAWebView *_view;

#ifdef PHIDEBUG
    QPlainTextEdit *_edit;
#endif
};

#endif // AMPLUGIN_H
