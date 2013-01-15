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
#include <QSettings>
#include <QFile>
#include <QDataStream>
#include <QList>
#include <QDir>
#include <QApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QNetworkCookie>
#include "phiatools.h"
#include "phia.h"
#include "phi.h"

PHIACookies* PHIACookies::_instance=0;

PHIACookies* PHIACookies::instance()
{
    if ( _instance ) return _instance;
    _instance=new PHIACookies( qApp );
    return _instance;
}

PHIACookies::PHIACookies( QObject *parent )
    : QNetworkCookieJar( parent )
{
    qDebug( "PHIACookies::PHIACookies()" );
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ), QString() ).toString();
    if ( dir.isEmpty() ) dir=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::browserName();
    QDir mkdir( dir );
    if ( !mkdir.exists() ) mkdir.mkpath( dir );
    QFile f( dir+QDir::separator()+"cookies.db" );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QList<QNetworkCookie> list;
        QDataStream ds( &f );
        QByteArray arr;
        qint32 count;
        ds >> count;
        for ( qint32 i=0; i<count; i++ ) {
            ds >> arr;
            QList<QNetworkCookie> cookies=QNetworkCookie::parseCookies( arr );
            list.append( cookies.first() );
        }
        f.close();
        setAllCookies( list );
    }
}

PHIACookies::~PHIACookies()
{
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ), QString() ).toString();
    if ( dir.isEmpty() ) dir=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::browserName();
    QDir mkdir( dir );
    if ( !mkdir.exists() ) mkdir.mkpath( dir );
    QFile f( dir+QDir::separator()+"cookies.db" );
    if ( f.open( QIODevice::WriteOnly ) ) {
        QList<QNetworkCookie> list=allCookies();
        QDataStream ds( &f );
        ds << static_cast<qint32>(list.count());
        QNetworkCookie cookie;
        foreach ( cookie, list ) ds << cookie.toRawForm();
        f.close();
    }
    qDebug( "PHIACookies::~PHIACookies()" );
}

bool PHIACookies::setCookiesFromUrl( const QList<QNetworkCookie> &cookieList, const QUrl &url )
{
    QMutexLocker lock( &_mutex );
    QNetworkCookie cookie;
    foreach ( cookie, cookieList ) {
        qDebug( "set cookie name<%s> path<%s> domain<%s> for url<%s>", cookie.name().data(), qPrintable( cookie.path() ),
            qPrintable( cookie.domain() ), qPrintable( url.toString() ) );
    }
    return QNetworkCookieJar::setCookiesFromUrl( cookieList, url );
}

QList<QNetworkCookie> PHIACookies::cookiesForUrl( const QUrl &url ) const
{
    QMutexLocker lock( &_mutex );
    return QNetworkCookieJar::cookiesForUrl( url );
}

bool PHIACookies::removeCookie( const QString &domain, const QString &name )
{
    QList<QNetworkCookie> list=allCookies();
    QNetworkCookie cookie;
    int row=-1;
    for ( int i=0; i<list.count(); i++ ) {
        cookie=list.at( i );
        if ( cookie.domain()==domain && cookie.name()==name.toUtf8() ) {
            row=i;
            break;
        }
    }
    if ( row>-1 ) list.removeAt( row );
    setAllCookies( list );
    if ( row>-1 ) return true;
    return false;
}

PHIADiskCache::PHIADiskCache( QObject *parent )
    : QNetworkDiskCache( parent )
{
    qDebug( "PHIADiskCache::PHIADiskCache()" );
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ), QString() ).toString();
    if ( dir.isEmpty() ) dir=QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
        +QDir::separator()+PHIA::browserName();
    QDir mkdir( dir );
    if ( !mkdir.exists() ) mkdir.mkpath( dir );
    setCacheDirectory( dir );
    setMaximumCacheSize( s.value( PHIA::configName( PHIA::MaxCacheSize ),
        100*1024*1024 ).toLongLong() );
}

PHIADiskCache::~PHIADiskCache()
{
    qDebug( "PHIADiskCache::~PHIADiskCache()" );
}

PHIASourceDialog::PHIASourceDialog( QWidget *parent )
    : QDialog( parent )
{
    setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );
}

PHIASourceDialog::~PHIASourceDialog()
{
}

void PHIASourceDialog::slotSetSource( const QString &s )
{
    _code->setPlainText( s );
}

PHIACookieDlg::PHIACookieDlg( QWidget *parent )
    : QDialog( parent )
{
    qDebug( "PHIACookieDlg::PHIACookieDlg()" );
    setupUi( this );
    setWindowTitle( tr( "Cookies" ) );
    _cookieTree->setSortingEnabled( true );

    PHIACookies *cookies=PHIACookies::instance();
    QList<QNetworkCookie> list=cookies->cookies();
    QNetworkCookie cookie;
    foreach ( cookie, list ) {
        QTreeWidgetItem *par=findDomain( cookie.domain() );
        if ( !par ) {
            par=new QTreeWidgetItem();
            par->setText( 0, cookie.domain() );
            par->setFlags( Qt::ItemIsEnabled );
            _cookieTree->addTopLevelItem( par );
        }
        QTreeWidgetItem *it=new QTreeWidgetItem( par );
        it->setFlags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
        it->setText( 1, QString::fromUtf8( cookie.name() ) );
        it->setText( 2, QString::fromUtf8( cookie.value() ) );
        it->setText( 3, cookie.path() );
        it->setData( 0, Qt::UserRole, cookie.domain() );
    }
    _cookieTree->sortByColumn( 0, Qt::AscendingOrder );
    _buttonBox->setFocus();
}

PHIACookieDlg::~PHIACookieDlg()
{
    qDebug( "PHIACookieDlg::~PHIACookieDlg()" );
}

void PHIACookieDlg::on__clear_clicked()
{
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Clear" ),
        tr( "Are you sure you want to clear all cookies?"),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
    if ( res==QMessageBox::Abort ) return;
    PHIACookies::instance()->clear();
}

void PHIACookieDlg::on__delete_clicked()
{
    QTreeWidgetItem *it=_cookieTree->currentItem();
    if ( !it ) return;
    PHIACookies *cookies=PHIACookies::instance();
    cookies->removeCookie( it->data( 0, Qt::UserRole ).toString(), it->text( 1 ) );
    delete it;
}

QTreeWidgetItem* PHIACookieDlg::findDomain( const QString &domain )
{
    QTreeWidgetItem *it;
    for ( int i=0; i<_cookieTree->topLevelItemCount(); i++ ) {
        it=_cookieTree->topLevelItem( i );
        if ( it->text( 0 )==domain ) return it;
    }
    return 0;
}
