/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIASCRIPTOBJECTS_H
#define PHIASCRIPTOBJECTS_H

#include <QObject>
#include <QScriptEngine>
#include <QHash>
#include <QString>
#include <QTimerEvent>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QUrl>
#include <QNetworkReply>
#include <QTextCodec>
#include "phiawebview.h"

#define PHIASCRIPTEXTENSION QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject |\
    QScriptEngine::ExcludeSuperClassContents | QScriptEngine::ExcludeDeleteLater

class PHIAScriptWindowObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptWindowObj )

public:
    explicit PHIAScriptWindowObj( PHIAWebView *parent );
    virtual ~PHIAScriptWindowObj();

public slots:
    void alert( const QString& ) const;
    QString prompt( const QString &text, const QString &value=QString() ) const;
    bool confirm( const QString &text );
    void close();
    void back();
    void forward();
    void home();
    void stop();
    qint32 setTimeout( const QScriptValue&, qint32 );
    void clearTimeout( qint32 );
    qint32 setInterval( const QScriptValue&, qint32 );
    void clearInterval( qint32 );
    void resizeTo( qint32, qint32 );
    void resizeBy( qint32, qint32 );
    void open( const QString &url, const QString &target=QString( "_self" ),
        const QString &options=QString() );
    void print();

protected:
    virtual void timerEvent( QTimerEvent* );

signals:
    void windowCloseRequested();
    void homeRequested();
    void changeGeometry( const QRect &rect );
    void createWindow( const QString &url, const QString &target, const QString &options );
    void requestPrint();
/*
    void blur() const;
    void captureEvents();
    void disableExternalCapture();
    void enableExternalCapture();
    void find();
    void focus();
    void handleEvent();
    void moveBy();
    void moveTo();
    void releaseEvents();
    void routeEvent();
*/

private:
    QScriptEngine *_engine;
    PHIAWebView *_view;
    QHash <qint32, QScriptValue> _timeoutFunctions;
    QHash <qint32, QScriptValue> _intervalFunctions;
};

class PHIAScriptAjaxObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptAjaxObj )

    Q_PROPERTY( quint8 readyState READ readyState )
    Q_PROPERTY( qint32 status READ status )
    Q_PROPERTY( QString statusText READ statusText )
    Q_PROPERTY( QString responseText READ responseText )
    Q_PROPERTY( QString responseXML READ responseXML )

public:
    explicit PHIAScriptAjaxObj( PHIAWebView *parent );
    virtual ~PHIAScriptAjaxObj();
    enum Status { UNSENT=0, OPENED=1, HEADERS_RECEIVED=2, LOADING=3, DONE=4 };

public slots:
    inline quint8 readyState() const { return _readyState; }
    inline qint32 status() const { return _status; }
    inline QString statusText() const { return _statusText; }
    inline QString responseText() const { return _responseText; }
    inline QString responseXML() const { return _responseXML; }

    void setRequestHeader( const QString &key, const QString &value );
    void open( const QString &method, const QString &url, bool async=true,
        const QString &user=QString(), const QString &passwd=QString() );
    void send( const QString &body=QString() );
    QString getAllResponseHeaders() const;
    QString getResponseHeader( const QString &header ) const;
    void abort();

protected slots:
    void slotMetaDataChanged();
    void slotNetworkError( QNetworkReply::NetworkError err );
    void slotFinished();
    void slotNewBytesAvailable();

protected:
    void throwError( const QString &err );

private:
    QScriptEngine *_engine;
    PHIAWebView *_view;
    quint8 _readyState;
    qint32 _status;
    QString _statusText, _responseText, _responseXML, _method;
    QNetworkReply *_reply;
    bool _async, _running, _hasError;
    QUrl _url;
    QHash <QString, QString> _requestHeaders, _responseHeaders;
    //QScriptValue _ajax;
    QTextCodec *_codec;
};

Q_DECLARE_METATYPE( PHIAScriptAjaxObj* )
QScriptValue ajaxObjToScriptValue( QScriptEngine*, PHIAScriptAjaxObj* const &in );
void ajaxObjFromScriptValue( const QScriptValue&, PHIAScriptAjaxObj* &out );

class PHIAScriptLocationObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptLocationObj )

    Q_PROPERTY( QString hash READ host )
    Q_PROPERTY( QString host READ host )
    Q_PROPERTY( QString hostname READ hostname )
    Q_PROPERTY( QString href READ href WRITE setHref )
    Q_PROPERTY( QString pathname READ pathname )
    Q_PROPERTY( QString port READ port )
    Q_PROPERTY( QString protocol READ protocol )
    Q_PROPERTY( QString search READ search )

public:
    explicit PHIAScriptLocationObj( PHIAWebView *parent );
    virtual ~PHIAScriptLocationObj();

public slots:
    void setHref( const QString &h );
    inline QString hash() const { return _url.fragment(); }
    inline QString host() const { return _url.port( 80 )==80 ? _url.host() :
        _url.host()+':'+QString::number( _url.port() ); }
    inline QString hostname() const { return _url.host(); }
    inline QString href() const { return _url.toString(); }
    inline QString pathname() const { return '/'+_url.path(); }
    inline QString port() const { return _url.port( 80 )==80 ? QString() :
        QString::number( _url.port() ); }
    inline QString protocol() const { return _url.scheme(); }
    inline QString search() const { QUrlQuery q( _url ); return QString('?')+
        q.toString(); }
    void reload();

signals:
    void hrefRequested( const QUrl &url );

private:
    QUrl _url;
};

class PHIAScriptFormsObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptFormsObj )

    Q_PROPERTY( QString action READ action WRITE setAction )
    Q_PROPERTY( QString encoding READ encoding )
    Q_PROPERTY( QString method READ method )

public:
    explicit PHIAScriptFormsObj( PHIAWebView *parent );
    virtual ~PHIAScriptFormsObj();

public slots:
    inline QString action() const { return _view->page()->action(); }
    inline void setAction( const QString &a ) { _view->page()->setAction( a ); }
    inline QString encoding() const { return QString( "multipart/form-data" ); }
    inline QString method() const { return QString( "POST" ); }

    void reset();
    void submit( const QString &buttonid=QString() );

signals:
    void resetRequest();
    void submitRequest( const QString& );

private:
    PHIAWebView *_view;
};

class PHIAScriptNavigatorObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptNavigatorObj )

    Q_PROPERTY( QString appCodeName READ appCodeName )
    Q_PROPERTY( QString appName READ appName )
    Q_PROPERTY( QString appVersion READ appVersion )
    Q_PROPERTY( bool cookieEnabled READ cookieEnabled )
    Q_PROPERTY( QString language READ language )
    Q_PROPERTY( QString platform READ platform )
    Q_PROPERTY( QString userAgent READ userAgent )
    Q_PROPERTY( bool javaEnabled READ javaEnabled )

public:
    explicit PHIAScriptNavigatorObj( PHIAWebView *parent );
    virtual ~PHIAScriptNavigatorObj();

public slots:
    QString appCodeName() const;
    QString appName() const;
    QString appVersion() const;
    inline bool cookieEnabled() const { return true; }
    QString language() const;
    QString platform() const;
    QString userAgent() const;
    inline bool javaEnabled() const { return false; }

private:
    PHIAWebView *_view;
};

class PHIAScriptPhiObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptPhiObj )

    Q_PROPERTY( QString session READ session )
    Q_PROPERTY( QString lang READ lang )
    Q_PROPERTY( quint8 xAxis READ xAxis )
    Q_PROPERTY( quint8 yAxis READ yAxis )
    Q_PROPERTY( quint8 zAxis READ zAxis )

public:
    explicit PHIAScriptPhiObj( PHIAWebView *parent );
    virtual ~PHIAScriptPhiObj();

public slots:
    PHIBaseItem* getElementById( const QString & id );

    inline bool isNative() const { return true; }
    inline bool isHtml() const { return false; }
    inline QString session() const { return _view->page()->session(); }
    inline QString lang() const { return _view->page()->lang(); }
    inline quint8 xAxis() const { return 0x1; }
    inline quint8 yAxis() const { return 0x2; }
    inline quint8 zAxis() const { return 0x4; }
    inline PHIAScriptAjaxObj* createAjax() const { return new PHIAScriptAjaxObj( _view ); }
    void href( const QString &url );

    /** @deprecated */
    inline bool isXhtml() const { return false; }
    /** @deprecated */
    void setVisible( const QString &id, bool visible );
    //void toggle( const QString &id );
    /** @deprecated */
    void setOpacity( const QString &id, qreal opac );
    /** @deprecated */
    void alert( const QString &text ) const;
    /** @deprecated */
    void fadeIn( const QString &id, qint32 start=0, qint32 duration=1000, qreal maxOpac=1.,
        const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    void fadeOut( const QString &id, qint32 start=0, qint32 duration=1000, qreal minOpac=0,
        const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    void moveTo( const QString &id, qint32 left, qint32 top, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    inline void move( const QString &id, qint32 left, qint32 top, qint32 start=0, qint32 duration=1000 )
        { moveTo( id, left, top, start, duration, QString( "linear" ) ); }
    //void moveBy( const QString &id, qint32 x, qint32 y, qint32 w=0, qint32 h=0, qint32 start=0,
    //    qint32 duration=1000, const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    void rotateIn( const QString &id, quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    void rotateOut( const QString &id, quint8 axis=0x2, qint32 start=0, qint32 duration=1000,
        const QString &ease=PHI::defaultEasingCurve() );
    /** @deprecated */
    void rotate( const QString &id, quint8 axis=0x4, qreal step=1., const QString &ease="linear" );
    /** @deprecated */
    void setData( const QString &id, const QString &data, const QString &delimiter=QString( "\n" ) );

signals:
    void hrefRequested( const QUrl &url );

private:
    PHIAWebView *_view;
};

class PHIAScriptMenuObj : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAScriptMenuObj )

public:
    explicit PHIAScriptMenuObj( PHIAWebView *parent, QMenuBar *menubar );
    virtual ~PHIAScriptMenuObj();

public slots:
    void setEnabled( const QString &id, bool enable );
    bool isEnabled( const QString &id );
    void setChecked( const QString &id, bool checked );
    bool isChecked( const QString &id );
    void setText( const QString &id, const QString &text );
    QString text( const QString &id );
    void setShortcut( const QString &id, const QString &sc );
    QString shortcut( const QString &id );
    void addMenu( const QString &id, const QString &text, const QString &parentId=QString() );
    void addMenuItem( const QString &menuId, const QString &id, const QString &text,
        const QString &sc=QString() );
    void addCheckableMenuItem( const QString &menuId, const QString &id, const QString &text,
        bool isChecked=false, const QString &sc=QString() );
    void removeMenuItem( const QString &id );
    //void removeMenu( const QString &id );
    bool isCheckable( const QString &id );

protected slots:
    void slotMenuTriggered( QAction *act );

protected:
    void throwError( const QString &err );

private:
    QMenuBar *_menubar;
    QHash <QString, QMenu*> _menus;
    QHash <QString, QAction*> _actions;
    QScriptEngine *_engine;
    PHIAWebView *_view;
};

#endif // PHIASCRIPTOBJECTS_H
