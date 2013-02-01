/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2013  Phisys AG, Switzerland
#    Copyright (C) 2012-2013  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
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
