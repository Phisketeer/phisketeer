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
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIAGRAPHICSSCENE_H
#define PHIAGRAPHICSSCENE_H
#include <QSslConfiguration>
#include "phigraphicsscene.h"

class QScriptEngine;
class QNetworkReply;
class QMenuBar;
class QPrinter;
class PHIAWebView;
class PHIAbstractLayoutItem;

class PHIAGraphicsScene : public PHIGraphicsScene
{
    Q_OBJECT

public:
    enum ReadingType { RTHeader=0, RTPage=1, RTPageSize=2, RTItem=3, RTItemSize=4 };
    explicit PHIAGraphicsScene( QObject *parent );
    virtual ~PHIAGraphicsScene();

    void setUrl( const QUrl &url );
    inline QUrl url() const { return _requestedUrl; }
    inline const QSslConfiguration& sslConfiguration() const { return _sslConfig; }
    QScriptEngine* scriptEngine() const;
    void abort();

public slots:
    void slotSubmitForm( const QString &buttonId );
    void slotResetForm( const QString &buttonId );
    void slotLangChangeRequested( const QString &lang );
    void slotRequestPrint( QPrinter *printer=0 );

protected:
    PHIAWebView* webView() const;
    virtual void drawBackground( QPainter *painter, const QRectF &r );
    void init();
    void updateTabOrder();
    void startAnimations();
    void dragMoveEvent( QGraphicsSceneDragDropEvent *event );
    void initMenuBar( QMenuBar *menubar );

signals:
    void titleChanged( const QString &title );
    void iconChanged( const QIcon &icon );
    void linkRequested( const QUrl &url );

private slots:
    void slotDataAvailable();
    void slotReplyFinished();
    void slotMetaDataChanged();
    void slotBgImageReady( const QImage &img );
    void slotLinkActivated( const QString &url );
    void slotOpenUrlTriggered();
    void slotPrintPreview();
    void slotPaintRequested( QPrinter *printer );
    void slotShowPageInfo();

private:
    QNetworkReply *_reply;
    QUrl _requestedUrl;
    ReadingType _readingType;
    QSslConfiguration _sslConfig;
    bool _contentTypeChecked;
    QScriptEngine *_engine;
    quint32 _itemSize;
    quint8 _version;
    QList <PHIAbstractLayoutItem*> _layouts;
    QPrinter *_printer;
};

#endif // PHIAGRAPHICSSCENE_H
