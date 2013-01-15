/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
#ifndef AMPWEBVIEW_H
#define AMPWEBVIEW_H

#include <QtWebKit>
#include <QPrinter>
#include <QSslConfiguration>
#include <QWebView>
#include "phiaabstractwebview.h"
#include "ampwebpage.h"
#include "phiahistory.h"

class AMPWebView : public PHIAAbstractWebView
{
    Q_OBJECT

public:
    AMPWebView( PHIAHistory*, QWidget *parent=0 );
    virtual ~AMPWebView();

    virtual inline QIcon icon() const {
        return _webPage->mainFrame()->icon().isNull() ? _defaultIcon : _webPage->mainFrame()->icon(); }
    virtual inline QString title() const { return _webPage->mainFrame()->title(); }
    virtual inline QString source() const { return _webPage->currentFrame()->toHtml(); }
    virtual inline bool isLoading() const { return _isLoading; }
    virtual inline void renderDocument( QPainter *p ) { _webPage->currentFrame()->render( p ); }
    virtual inline AMPWebPage* webPage() const { return _webPage; }
    virtual inline QUrl url() const { return _webPage->mainFrame()->url(); }
    virtual inline void setUrl( const QString& ) { ; }
    virtual void setNetworkReply( QNetworkReply* );

public slots:
    virtual void slotNetworkRequest( const QNetworkRequest &req );
    virtual void slotBack();
    virtual void slotForward();
    virtual void slotReload();
    virtual void slotStop();
    virtual void slotZoomIn();
    virtual void slotZoomOut();
    virtual void slotZoomNormal();
    virtual void slotSetZoomFactor( qreal );
    virtual void slotPrint( QPrinter* );

protected slots:
    void slotTitleChanged( const QString& );
    void slotIconChanged();
    void slotUrlChanged( const QUrl& );
    void slotLoadFinished( bool );
    void slotLoadStarted();
    void slotLinkHovered( const QString &link, const QString &title, const QString &content );
    void slotStatusBarMessage( const QString& );
    void slotUnsupportedContent( QNetworkReply* );
    void slotReplyFinished();
    void slotReplyDownloadProgress( qint64, qint64 );
    void slotUpdateGeometry( const QRect& );
    void slotReplyFinished( QNetworkReply *reply );
    void slotSslErrors( QNetworkReply *reply, const QList<QSslError> &errors );
    void slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth );
    void slotPrintRequest( QWebFrame* );
    void slotPrintFrame( QPrinter* );

protected:
/*
    virtual QSize sizeHint() const;
    virtual bool event( QEvent* );
    virtual void resizeEvent( QResizeEvent* );
    virtual void paintEvent( QPaintEvent* );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseDoubleClickEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void contextMenuEvent( QContextMenuEvent *e );
    virtual void wheelEvent( QWheelEvent *e );
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void focusOutEvent( QFocusEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void inputMethodEvent( QInputMethodEvent *e );
    virtual void changeEvent( QEvent *e );
    virtual bool focusNextPrevChild( bool next );
    virtual QVariant inputMethodQuery( Qt::InputMethodQuery property ) const;
    void setCursor( const QCursor& newCursor );
*/
protected:
    AMPWebPage *_webPage;
    QWebView *_webView;
    bool _isLoading;
    QNetworkReply *_reply;
    QWebFrame *_printFrame;
//    bool _cursorSetByWebCore;
//    bool _usesWebCoreCursor;
//    QCursor _webCoreCursor;
};

#endif // AMPWEBVIEW_H
