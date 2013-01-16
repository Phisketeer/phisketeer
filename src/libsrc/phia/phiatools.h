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
#ifndef PHIATOOLS_H
#define PHIATOOLS_H

#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QUrl>
#include <QNetworkDiskCache>
#include <QMutex>
#include <QTreeWidgetItem>
#include "ui_sourcedialog.h"
#include "ui_cookiedlg.h"
#include "phia.h"

class PHIAEXPORT PHIACookies : public QNetworkCookieJar
{
    Q_OBJECT

public:
    static PHIACookies* instance();
    virtual bool setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url );
    virtual QList<QNetworkCookie> cookiesForUrl( const QUrl &url ) const;
    bool removeCookie( const QString &domain, const QString &name );
    inline QList<QNetworkCookie> cookies() const { return allCookies(); }
    inline void clear() { setAllCookies( QList<QNetworkCookie>() ); }

protected:
    PHIACookies( QObject *parent=0 );
    virtual ~PHIACookies();

private:
    static PHIACookies* _instance;
    mutable QMutex _mutex;
};

class PHIAEXPORT PHIADiskCache : public QNetworkDiskCache
{
    Q_OBJECT

public:
    explicit PHIADiskCache( QObject *parent=0 );
    virtual ~PHIADiskCache();
};

class PHIAEXPORT PHIASourceDialog : public QDialog, private Ui::SourceDialog
{
    Q_OBJECT

public:
    explicit PHIASourceDialog( QWidget* parent=0 );
    virtual ~PHIASourceDialog();

public slots:
    void slotSetSource( const QString& );
};

class PHIAEXPORT PHIACookieDlg : public QDialog, private Ui::CookieDlg
{
    Q_OBJECT

public:
    explicit PHIACookieDlg( QWidget *parent=0 );
    virtual ~PHIACookieDlg();

protected slots:
    void on__delete_clicked();
    void on__clear_clicked();

protected:
    QTreeWidgetItem* findDomain( const QString &domain );
};

#endif // PHIATOOLS_H
