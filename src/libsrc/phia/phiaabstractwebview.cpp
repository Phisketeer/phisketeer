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
#include <QMessageBox>
#include <QDesktopWidget>
#include <QApplication>
#include <QNetworkCookie>
#include "phiaabstractwebview.h"
#include "phiatools.h"
#include "phiasettingsdlg.h"
#include "phiaauthdlg.h"
#include "phiaconfig.h"

QIcon PHIAAbstractWebView::_defaultIcon=QIcon();
QIcon PHIAAbstractWebView::_missingIcon=QIcon();
QNetworkAccessManager* PHIAAbstractWebView::_networkAccessManager=0;

PHIAExtractWindowOpts::PHIAExtractWindowOpts( const QString &options )
{
    QStringList list=options.simplified().split( ',' );
    QString key, value;
    int sep;
    foreach( key, list ) {
        sep=key.indexOf( '=' );
        if ( sep==-1 ) continue;
        value=key.mid( sep+1 ).simplified().toLower();
        key=key.mid( 0, sep ).simplified().toLower();
        _options.insert( key, value );
        qDebug( "inserting <%s> <%s>", qPrintable( key ), qPrintable( value ) );
    }
}

PHIAExtractWindowOpts::~PHIAExtractWindowOpts()
{
}

bool PHIAExtractWindowOpts::contains( const QString &key ) const
{
    if ( _options.contains( key.toLower() ) ) return true;
    return false;
}

int PHIAExtractWindowOpts::left() const
{
    if ( !_options.contains( "left" ) ) return -1;
    int x=_options.value( "left" ).toInt();
    if ( x<0 ) x=0;
    int max=QApplication::desktop()->availableGeometry().width();
    if ( x>max ) x=max-100;
    return x;
}

int PHIAExtractWindowOpts::top() const
{
    if ( !_options.contains( "top" ) ) return -1;
    int y=_options.value( "top" ).toInt();
    if ( y<0 ) y=0;
    int max=QApplication::desktop()->availableGeometry().height();
    if ( y>max ) y=max-100;
    return y;
}

int PHIAExtractWindowOpts::height() const
{
    if ( !_options.contains( "height" ) ) return -1;
    int h=_options.value( "height" ).toInt();
    if ( h<100 ) h=100;
    int max=QApplication::desktop()->availableGeometry().height();
    if ( h>max ) h=max-100;
    return h;
}

int PHIAExtractWindowOpts::width() const
{
    if ( !_options.contains( "width" ) ) return -1;
    int w=_options.value( "width" ).toInt();
    if ( w<100 ) w=100;
    int max=QApplication::desktop()->availableGeometry().width();
    if ( w>max ) w=max-100;
    return w;
}

bool PHIAExtractWindowOpts::showStatusBar() const
{
    if ( !_options.contains( "status" ) ) return false;
    if ( _options.value( "status" )=="yes" ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showLocationBar() const
{
    if ( !_options.contains( "location" ) ) return false;
    if ( _options.value( "location" )=="yes" ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showMenuBar() const
{
    if ( !_options.contains( "menubar" ) ) return false;
    if ( _options.value( "menubar" )=="yes" ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showToolBar() const
{
    if ( !_options.contains( "toolbar" ) ) return false;
    if ( _options.value( "toolbar" )=="yes" ) return true;
    return false;
}

bool PHIAExtractWindowOpts::resizable() const
{
    if ( !_options.contains( "resizable" ) ) return false;
    if ( _options.value( "resizable" )=="yes" ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showScrollBars() const
{
    if ( !_options.contains( "scrollbars" ) ) return false;
    if ( _options.value( "scrollbars" )=="yes" ) return true;
    return false;
}

PHIAAbstractWebView::PHIAAbstractWebView( PHIAHistory *history, QWidget *parent )
    : QWidget( parent ), _history( history )
{
    qDebug( "PHIAAbstractWebView::PHIAAbstractWebView()" );
    Q_ASSERT( _networkAccessManager );
    if ( !history ) _history=new PHIAHistory( qApp );
    connect( networkAccessManager(), SIGNAL( sslErrors( QNetworkReply*,
        const QList<QSslError>& ) ), this,  SLOT( slotSslErrors( QNetworkReply*,
        const QList<QSslError>& ) ) );
    connect( networkAccessManager(), SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
        this, SLOT( slotAuthRequest( QNetworkReply*, QAuthenticator* ) ) );
}

PHIAAbstractWebView::~PHIAAbstractWebView()
{
    _history=0;
    qDebug( "PHIAAbstractWebView::~PHIAAbstractWebView()" );
}

bool PHIAAbstractWebView::canGoBack() const
{
    return _history->canGoBack();
}

bool PHIAAbstractWebView::canGoForward() const
{
    return _history->canGoForward();
}

void PHIAAbstractWebView::openSslDlg()
{
    if ( _sslConfig.peerCertificate().isNull() ) {
        QMessageBox::information( this, tr( "SSL" ), tr( "You are not securely connected." ),
            QMessageBox::Ok );
        return;
    }
    QSslCertificate c=_sslConfig.peerCertificate();
    QString tmp=tr( "Issuer:" )+' '+c.issuerInfo( QSslCertificate::Organization ).first()+'\n';
    tmp+=tr( "Organisation (O):" )+' '+c.subjectInfo( QSslCertificate::Organization ).first()+'\n';
    tmp+=tr( "Common name (CN):" )+' '+c.subjectInfo( QSslCertificate::CommonName ).first()+'\n';
    tmp+=tr( "Expiry date:" )+' '+c.expiryDate().toString( "yyyy-MM-dd" )+'\n';
    tmp+=tr( "Country (C):" )+' '+c.subjectInfo( QSslCertificate::CountryName ).first();
    QMessageBox::information( this, tr( "Host SSL certificate" ), tmp, QMessageBox::Ok );
}

void PHIAAbstractWebView::showJavaScriptClicked( const QString &script )
{
    PHIASourceDialog *dlg=new PHIASourceDialog( this );
    dlg->slotSetSource( script );
    dlg->setWindowTitle( title() );
    dlg->show();
}

void PHIAAbstractWebView::settingsClicked()
{
    PHIASettingsDlg dlg( this );
    dlg.exec();
}

void PHIAAbstractWebView::slotSslErrors( QNetworkReply *reply, const QList<QSslError> &errors )
{
    PHIAConfig::instance()->showSslErrorDialog( this, reply, errors );
}

void PHIAAbstractWebView::slotAuthRequest( QNetworkReply *reply, QAuthenticator *auth )
{
    PHIAConfig::instance()->showAuthDialog( this, reply, auth );
}
