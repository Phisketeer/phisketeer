/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef PHIWEBPAGE_H
#define PHIWEBPAGE_H
#include <QUrl>
#include <QString>
#include "phi.h"

#ifdef PHIWEBKIT
#include <QWebPage>

class QWebView;

class PHIEXPORT PHIWebPage : public QWebPage
{
    Q_OBJECT

public:
    explicit PHIWebPage( QObject *parent=0 );
    inline bool isLoading() const { return _loading; }
    inline void setView( QWebView *view ){ _view=view; }
    void setContent( const QByteArray &, const QString &mimeType=QString(), const QUrl &baseUrl=QUrl() );
    void setUrl( const QUrl &url );
    QUrl url() const;

protected slots:
    void slotUpdateGeometry( const QRect &rect );
    void slotLinkClicked( const QUrl &url );
    void slotIconChanged();

protected:
    virtual QWebPage* createWindow( WebWindowType type );

private slots:
    void slotLoadingFinished() { _loading=false; }
    void slotLoadingStart() { _loading=true; }

private:
    QWebView *_view;
    bool _loading;
};

#else // don't use webkit:
#include <QObject>

class PHIEXPORT PHIWebPage : public QObject
{
    Q_OBJECT

public:
    explicit PHIWebPage( QObject *parent=0 );
    bool isLoading() const { return _loading; }
    void setContent( const QByteArray &, const QString &mimeType=QString(), const QUrl &baseUrl=QUrl() );

private slots:
    void slotLoadingFinished() { _loading=false; }
    void slotLoadingStart() { _loading=true; }

private:
    bool _loading;
};
#endif // PHIWEBKIT

#endif // PHIWEBPAGE_H
