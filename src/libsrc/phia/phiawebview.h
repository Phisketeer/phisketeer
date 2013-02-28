/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
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
#ifndef PHIAWEBVIEW_H
#define PHIAWEBVIEW_H

#include <QIODevice>
#include <QNetworkReply>
#include <QWidget>
#include <QUrl>
#include <QtScript>
#include <QPalette>
#include <QBuffer>
#include <QScriptValue>
#include <QHash>
#include "phiaabstractwebview.h"
#include "phiascene.h"
#include "phiapage.h"
#include "phiaitem.h"
#include "phia.h"

class PHIAEXPORT PHIAWebView : public PHIAAbstractWebView
{
    Q_OBJECT
    Q_DISABLE_COPY( PHIAWebView )

public:
    enum ReadingType { RTHeader=0, RTPage=1, RTPageSize=2, RTItem=3, RTItemSize=4 };
    explicit PHIAWebView( PHIAHistory*, QWidget *parent=0 );
    virtual ~PHIAWebView();

    virtual inline QIcon icon() const { return _page->icon().isNull() ? _defaultIcon : _page->icon(); }
    virtual inline QString title() const { return _page->title(); }
    virtual inline bool isLoading() const { return _isLoading; }
    virtual inline void renderDocument( QPainter *p ) { _scene->render( p ); }
    virtual inline QUrl url() const { return _reqUrl; }
    virtual inline void setUrl( const QString &url ) { _reqUrl=QUrl( url ); }
    virtual inline QScriptEngine* scriptEngine() const { return _engine; }

    inline void insertScriptValue( const QString &id, const QScriptValue &v )
        { _scriptIdHash.insert( id, v ); }
    inline QScriptValue scriptValue( const QString &id ) const { return _scriptIdHash.value( id ); }

    inline PHIAScene* scene() const { return _scene; }
    inline PHIAPage* page() const { return _page; }
    inline QByteArray phiOutput() const { return _stream; }    

    virtual void updateTabOrdering();
    virtual void setNetworkReply( QNetworkReply* );
    /** Sets @arg dev to read from a phi source (mostly a QNetworkReply). */
    virtual void setIODevice( QIODevice *dev, bool callSlotFinished=false );
    virtual void runScript( const QString &script );
    virtual QString source() const;
    virtual void throwJavaScriptError( const QScriptValue &err );
    virtual void setScrollBarsEnabled( bool enable );

public slots:
    virtual void slotNetworkRequest( const QNetworkRequest &req );
    virtual void slotBack();
    virtual void slotForward();
    virtual void slotStop();
    virtual void slotReload();
    virtual void slotZoomIn();
    virtual void slotZoomOut();
    virtual void slotZoomNormal();
    virtual void slotSetZoomFactor( qreal );
    virtual void slotPrint( QPrinter* );
    virtual void slotCreateWindow( const QString &url, const QString &target, const QString &opt );

protected slots:
    virtual void slotDataAvailable();
    virtual void slotFinished();
    virtual void slotItemSetupDone( const PHIAItem* );
    virtual void slotTitleChanged();
    virtual void slotIconChanged();
    virtual void slotLinkRequested( const QUrl &url );
    virtual void slotSubmitForm( const QString &buttonid ); // force to send form
    virtual void slotSubmitFormRequested( const QString& ); // check "phi.onsubmit()"
    virtual void slotResetForm();
    virtual void slotCheckAppMode();
    virtual void slotNetworkError( QNetworkReply::NetworkError );
    virtual void slotMetaDataChanged();
    virtual void slotSslErrors( QNetworkReply *reply, const QList<QSslError>& );
    virtual void slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth );
    virtual void slotCheckContainerItems();
    virtual void slotRequestPrint();
    virtual void slotReturnButtonPressed();
    virtual void slotRequestForImageFinished();

protected:
    /** Called as soon as a page (without elements) is loaded. */
    void setupPage();
    int selectMenu( const QStringList &items, const QPointF &pos );
    void requestBgImage();
    //void switchBuffer();

signals:
    void checkSubmit( const QString& );

protected:
    PHIAPage *_page; //, *_page1, *_page2;
    PHIAScene *_scene; //, *_scene1, *_scene2;
    PHIAGraphicsView *_view;
    QIODevice *_ioDev;
    QScriptEngine *_engine;

    bool _isLoading, _contentTypeChecked;
    ReadingType _readingType;
    int _progress;
    quint32 _itemSize;
    QUrl _reqUrl;
    QPalette _alphaPal; // palette with 'Window' set to transparent
    qreal _zoomFactor;
    QByteArray _stream;
    QHash <QString, QScriptValue> _scriptIdHash;
    QNetworkReply *_bgReply;
};

#endif // PHIAWEBVIEW_H
