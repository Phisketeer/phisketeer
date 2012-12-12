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
#ifndef PHIAABSTRACTWEBVIEW_H
#define PHIAABSTRACTWEBVIEW_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QIcon>
#include <QUrl>
#include <QPainter>
#include <QPrinter>
#include <QSslConfiguration>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QScriptEngine>
#include "phiahistory.h"
#include "phia.h"

class PHIAEXPORT PHIAExtractWindowOpts
{
public:
    PHIAExtractWindowOpts( const QString &options );
    ~PHIAExtractWindowOpts();
    int width() const;
    int height() const;
    int left() const;
    int top() const;
    bool showStatusBar() const;
    bool showLocationBar() const;
    bool showMenuBar() const;
    bool showToolBar() const;
    bool showScrollBars() const;
    bool resizable() const;
    bool contains( const QString &key ) const;

private:
    QHash <QString, QString> _options;
};

class PHIAEXPORT PHIAAbstractWebView : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAAbstractWebView )

public:
    PHIAAbstractWebView( PHIAHistory*, QWidget *parent=0 );
    virtual ~PHIAAbstractWebView();
    inline PHIAHistory* history() const { return _history; }
    virtual inline QIcon icon() const { return _defaultIcon; }
    virtual inline QScriptEngine* scriptEngine() const { return 0; }
    virtual inline void setScrollBarsEnabled( bool ) {;}

    virtual bool canGoBack() const;
    virtual bool canGoForward() const;
    virtual void openSslDlg();
    virtual QSslConfiguration sslConfiguration() const { return _sslConfig; }
    virtual void settingsClicked();
    virtual void showJavaScriptClicked( const QString &script );

    virtual QString title() const=0;
    virtual QString source() const=0;
    virtual bool isLoading() const=0;
    virtual void renderDocument( QPainter* )=0;
    virtual void setNetworkReply( QNetworkReply* )=0;
    virtual QUrl url() const=0;
    virtual void setUrl( const QString &url )=0;

    static inline void setDefaultIcon( const QIcon &icon ) { _defaultIcon=icon; }
    static inline void setMissingIcon( const QIcon &icon ) { _missingIcon=icon; }
    static inline void setNetworkAccessManager( QNetworkAccessManager *am ) { _networkAccessManager=am; }
    static inline QNetworkAccessManager* networkAccessManager() { return _networkAccessManager; }

public slots:
    virtual void slotNetworkRequest( const QNetworkRequest &req )=0;
    virtual void slotBack()=0;
    virtual void slotForward()=0;
    virtual void slotStop()=0;
    virtual void slotReload()=0;
    virtual void slotZoomIn()=0;
    virtual void slotZoomOut()=0;
    virtual void slotZoomNormal()=0;
    virtual void slotSetZoomFactor( qreal )=0;
    virtual void slotPrint( QPrinter* )=0;

protected slots:
    virtual void slotSslErrors( QNetworkReply *reply, const QList<QSslError>& );
    virtual void slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth );

signals:
    void iconChanged( PHIAAbstractWebView* );
    void titleChanged( PHIAAbstractWebView* );
    void unsupportedContent( PHIAAbstractWebView*, QNetworkReply* );
    void urlChanged( const QUrl& );
    void statusBarMessage( const QString&, int timout=0 );
    void loadProgress( int );
    void progressMaximum( int );
    void windowCloseRequested();
    void loading( bool );
    void linkHovered( const QString &link, const QString &title, const QString &textContent );
    void backAvailable( bool );
    void forwardAvailable( bool );
    void homeRequested();
    void menuBarVisibilityChangeRequested( bool visible );
    void microFocusChanged();
    void scrollRequested( int dx, int dy, const QRect &rectToScroll );
    void selectionChanged();
    void statusBarVisibilityChangeRequested( bool visible );
    void toolBarVisibilityChangeRequested( bool visible );
    void scrollBarVisibilityChangeRequested( bool visible );
    void javaScriptConsoleMessage( const QString &msg, int lineNumber, const QString& sourceId );
    void geometryChangeRequested( const QRect &geom );
    void resizableChangeRequested( bool resizable );
    void locationBarVisibilityChangeRequested( bool visible );

protected:
    PHIAHistory *_history;
    QSslConfiguration _sslConfig;
    static QIcon _defaultIcon;
    static QIcon _missingIcon;
    static QNetworkAccessManager *_networkAccessManager;
};

#endif // PHIAABSTRACTWEBVIEW_H
