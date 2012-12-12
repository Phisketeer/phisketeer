/***************************************************************************
 *   $Id: omqconfig.cpp,v 1.3 2007/02/06 19:14:40 linuxprofi Exp $
 *   Copyright (c) 2008-2009 Phi Technologies, Bernd Schumacher
 ***************************************************************************/
#ifndef AMPWEBPAGE_H
#define AMPWEBPAGE_H

#include <QWebPage>
#include <QWebFrame>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>

class AMPWebPage : public QWebPage
{
    Q_OBJECT

public:
    AMPWebPage( QObject *parent=0 );
    virtual ~AMPWebPage();
    virtual QUrl requestedUrl() const { return _reqUrl; }

protected:
    virtual bool acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, NavigationType type );
    virtual void javaScriptConsoleMessage( const QString &msg, int lineNumber, const QString &sourceId );
    virtual QWebPage* createWindow( QWebPage::WebWindowType type );

signals:
    void javaScriptMessage( const QString&, int, const QString& );

protected:
    QUrl _reqUrl;
};

#endif // AMPWEBPAGE_H
