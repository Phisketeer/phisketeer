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
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include "phiaabstractwebview.h"
#include "phi.h"
//#include "phiatools.h"
//#include "phiasettingsdlg.h"
//#include "phiaauthdlg.h"
//#include "phiaconfig.h"

QIcon PHIAAbstractWebView::_defaultIcon=QIcon();
QIcon PHIAAbstractWebView::_missingIcon=QIcon();

PHIAExtractWindowOpts::PHIAExtractWindowOpts( const QString &options )
{
    QStringList list=options.simplified().split( QLatin1Char(',') );
    QString key, value;
    int sep;
    foreach( key, list ) {
        sep=key.indexOf( QLatin1Char('=') );
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
    if ( !_options.contains( L1( "left" ) ) ) return -1;
    int x=_options.value( L1( "left" ) ).toInt();
    if ( x<0 ) x=0;
    int max=QApplication::desktop()->availableGeometry().width();
    if ( x>max ) x=max-100;
    return x;
}

int PHIAExtractWindowOpts::top() const
{
    if ( !_options.contains( L1( "top" ) ) ) return -1;
    int y=_options.value( L1( "top" ) ).toInt();
    if ( y<0 ) y=0;
    int max=QApplication::desktop()->availableGeometry().height();
    if ( y>max ) y=max-100;
    return y;
}

int PHIAExtractWindowOpts::height() const
{
    if ( !_options.contains( L1( "height" ) ) ) return -1;
    int h=_options.value( L1( "height" ) ).toInt();
    if ( h<100 ) h=100;
    int max=QApplication::desktop()->availableGeometry().height();
    if ( h>max ) h=max-100;
    return h;
}

int PHIAExtractWindowOpts::width() const
{
    if ( !_options.contains( L1( "width" ) ) ) return -1;
    int w=_options.value( L1( "width" ) ).toInt();
    if ( w<100 ) w=100;
    int max=QApplication::desktop()->availableGeometry().width();
    if ( w>max ) w=max-100;
    return w;
}

bool PHIAExtractWindowOpts::showStatusBar() const
{
    if ( !_options.contains( L1( "status" ) ) ) return false;
    if ( _options.value( L1( "status" ) )==L1( "yes" ) ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showLocationBar() const
{
    if ( !_options.contains( L1( "location" ) ) ) return false;
    if ( _options.value( L1( "location" ) )==L1( "yes" ) ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showMenuBar() const
{
    if ( !_options.contains( L1( "menubar" ) ) ) return false;
    if ( _options.value( L1( "menubar" ) )==L1( "yes" ) ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showToolBar() const
{
    if ( !_options.contains( L1( "toolbar" ) ) ) return false;
    if ( _options.value( L1( "toolbar" ) )==L1( "yes" ) ) return true;
    return false;
}

bool PHIAExtractWindowOpts::resizable() const
{
    if ( !_options.contains( L1( "resizable" ) ) ) return false;
    if ( _options.value( L1( "resizable" ) )==L1( "yes" ) ) return true;
    return false;
}

bool PHIAExtractWindowOpts::showScrollBars() const
{
    if ( !_options.contains( L1( "scrollbars" ) ) ) return false;
    if ( _options.value( L1( "scrollbars" ) )==L1( "yes" ) ) return true;
    return false;
}

PHIAAbstractWebView::PHIAAbstractWebView( QWidget *parent )
    : QWidget( parent )
{
    qDebug( "PHIAAbstractWebView::PHIAAbstractWebView()" );
    /*
    connect( networkAccessManager(), SIGNAL( sslErrors( QNetworkReply*,
        const QList<QSslError>& ) ), this,  SLOT( slotSslErrors( QNetworkReply*,
        const QList<QSslError>& ) ) );
    connect( networkAccessManager(), SIGNAL( authenticationRequired( QNetworkReply*, QAuthenticator* ) ),
        this, SLOT( slotAuthRequest( QNetworkReply*, QAuthenticator* ) ) );
    */
}

PHIAAbstractWebView::~PHIAAbstractWebView()
{
    qDebug( "PHIAAbstractWebView::~PHIAAbstractWebView()" );
}

bool PHIAAbstractWebView::canGoBack() const
{
    return false;
    //return _history->canGoBack();
}

bool PHIAAbstractWebView::canGoForward() const
{
    return false;
    //return _history->canGoForward();
}

/*
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
*/
