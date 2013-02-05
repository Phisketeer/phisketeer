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
#include <QSettings>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QDesktopServices>
#include <QTimer>
#include <QWebSettings>
#include "phiasettingsdlg.h"
#include "phiaconfig.h"
#include "phiaaddressdlg.h"
#include "phi.h"
#include "phia.h"
#include "phianetmanager.h"

PHIASettingsDlg::PHIASettingsDlg( QWidget *parent )
    : QDialog( parent ), _passwdChanged( false )
{
    qDebug( "PHIASettingsDlg::PHIASettingsDlg()" );
    setupUi( this );
    setWindowTitle( tr( "Phi user profile" ) );

    QString tmp;
    PHIAConfig *c=PHIAConfig::instance();
    if ( !c->showPasswdDialog( parent ) ) {
        QTimer::singleShot( 0, this, SLOT( reject() ) );
    }

    _lang->setValidator( new QRegExpValidator( QRegExp( "[a-z][a-z]-[a-z][a-z]" ), _lang ) );
    _downloadServer->insertItem( 0, "www.phisketeer.org" );
    _downloadServer->insertItem( 1, "www.phisys.com" );
    _version->setText( PHIA::libVersion() );
    qDebug( "LOCALE: %s", qPrintable( QLocale().name() ) );
    _lastCheck->setText( c->lastUpdateCheck().toString( Qt::DefaultLocaleShortDate ) );

    QSettings s;
    _w_autoLoadImages->setChecked( s.value( PHIA::configName( PHIA::AutoLoadImages ), true ).toBool() );
    _w_javaScript->setChecked( s.value( PHIA::configName( PHIA::EnableJavascript ), true ).toBool() );
    _w_java->setChecked( s.value( PHIA::configName( PHIA::EnableJava ), true ).toBool() );
    _w_printBackgrounds->setChecked( s.value( PHIA::configName( PHIA::PrintBackgrounds ), true ).toBool() );
    _w_openWindows->setChecked( s.value( PHIA::configName( PHIA::JSOpenWindow ), true ).toBool() );
    _w_plugins->setChecked( s.value( PHIA::configName( PHIA::EnablePlugins ), true ).toBool() );
    _cacheDir->setText( s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString() );
    _cacheDir->setReadOnly( true );
    _downloadDir->setText( s.value( PHIA::configName( PHIA::DownloadDirectory ) ).toString() );
    _downloadDir->setReadOnly( true );
    if ( _downloadDir->text().isEmpty() ) {
        QString dload=QStandardPaths::writableLocation( QStandardPaths::DownloadLocation );
        _downloadDir->setText( dload );
    }
    _languages->insertItems( 0, c->languages() );

    _usePasswd->setChecked( c->usePasswd() );
    _askUnsecure->setChecked( c->askForUnsecure() );
    _askAppMode->setChecked( c->askForApp() );
    _downloadInstall->setChecked( c->downloadInstall() );
    //_notify->setChecked( c->notifyUpdate() );
    _home->setText( c->home() );
    _useProxy->setChecked( c->useProxy() );
    _proxyPasswd->setText( c->proxyPasswd() );
    _proxyUser->setText( c->proxyUser() );
    _proxyHost->setText( s.value( PHIA::configName( PHIA::ProxyHost ) ).toString() );
    _proxyPort->setValue( s.value( PHIA::configName( PHIA::ProxyPort ), 1080 ).toInt() );

    QTreeWidgetItem *it;
    QList <PHIAAddress> addresses=c->addresses();
    PHIAAddress a;
    foreach( a, addresses ) {
        it=new QTreeWidgetItem( _addressTree );
        setupItem( it, a );
    }
    QList <PHIACreditCard> cards=c->creditCards();
    PHIACreditCard card;
    foreach( card, cards ) {
        it=new QTreeWidgetItem( _cardTree );
        setupItem( it, card );
    }
    QList <PHIAContact> contacts=c->contacts();
    PHIAContact contact;
    foreach( contact, contacts ) {
        it=new QTreeWidgetItem( _contactTree );
        setupItem( it, contact );
    }    
    _intervalCombo->insertItems( 0, c->intervalNames() );
    _intervalCombo->setCurrentIndex( c->interval() );
}

PHIASettingsDlg::~PHIASettingsDlg()
{
    qDebug( "PHIASettingsDlg::~PHIASettingsDlg()" );
}

void PHIASettingsDlg::on__addAddress_clicked()
{
    PHIAAddressDlg dlg( this );
    if ( dlg.exec()==QDialog::Rejected ) return;
    QTreeWidgetItem *it=new QTreeWidgetItem( _addressTree );
    setupItem( it, dlg.address() );
}

void PHIASettingsDlg::on__chgAddress_clicked()
{
    QTreeWidgetItem *it=_addressTree->currentItem();
    if ( !it ) return;
    PHIAAddressDlg dlg( this );
    dlg.setAddress( itemToAddress( it ) );
    if ( dlg.exec()==QDialog::Rejected ) return;
    setupItem( it, dlg.address() );
}

void PHIASettingsDlg::on__delAddress_clicked()
{
    QTreeWidgetItem *it=_addressTree->currentItem();
    if ( it ) delete it;
}

void PHIASettingsDlg::on__addContact_clicked()
{
    PHIAContactDlg dlg( this );
    if ( dlg.exec()==QDialog::Rejected ) return;
    QTreeWidgetItem *it=new QTreeWidgetItem( _contactTree );
    setupItem( it, dlg.contact() );
}

void PHIASettingsDlg::on__chgContact_clicked()
{
    QTreeWidgetItem *it=_contactTree->currentItem();
    if ( !it ) return;
    PHIAContactDlg dlg( this );
    dlg.setContact( itemToContact( it ) );
    if ( dlg.exec()==QDialog::Rejected ) return;
    setupItem( it, dlg.contact() );
}

void PHIASettingsDlg::on__delContact_clicked()
{
    QTreeWidgetItem *it=_contactTree->currentItem();
    if ( it ) delete it;
}

void PHIASettingsDlg::on__addCard_clicked()
{
    PHIACreditCardDlg dlg( this );
    if ( dlg.exec()==QDialog::Rejected ) return;
    QTreeWidgetItem *it=new QTreeWidgetItem( _cardTree );
    setupItem( it, dlg.creditCard() );
}

void PHIASettingsDlg::on__chgCard_clicked()
{
    QTreeWidgetItem *it=_cardTree->currentItem();
    if ( !it ) return;
    PHIACreditCardDlg dlg( this );
    dlg.setCreditCard( itemToCreditCard( it ) );
    if ( dlg.exec()==QDialog::Rejected ) return;
    setupItem( it, dlg.creditCard() );
}

void PHIASettingsDlg::on__delCard_clicked()
{
    QTreeWidgetItem *it=_cardTree->currentItem();
    if ( it ) delete it;
}

void PHIASettingsDlg::on__checkNow_clicked()
{
    PHIAConfig *c=PHIAConfig::instance();
    c->setDownloadInstall( _downloadInstall->isChecked() );
    PHIAConfig::instance()->slotCheckForUpdate( this );
}

void PHIASettingsDlg::setupItem( QTreeWidgetItem *it, const PHIAAddress &a )
{
    it->setText( 0, a.title );
    it->setText( 1, a.firstname );
    it->setText( 2, a.lastname );
    it->setText( 3, a.organisation );
    it->setText( 4, a.unit );
    it->setText( 5, a.street );
    it->setText( 6, a.state );
    it->setText( 7, a.city );
    it->setText( 8, a.zip );
    it->setText( 9, a.country );
    it->setData( 0, Qt::UserRole, static_cast<qint32>(a.options) );
}

void PHIASettingsDlg::setupItem( QTreeWidgetItem *it, const PHIAContact &c )
{
    it->setText( 0, c.name );
    it->setText( 1, c.email );
    it->setText( 2, c.mobile );
    it->setText( 3, c.phone );
    it->setText( 4, c.fax );
    it->setText( 5, c.url );
    it->setData( 0, Qt::UserRole, static_cast<qint32>(c.options) );
}

void PHIASettingsDlg::setupItem( QTreeWidgetItem *it, const PHIACreditCard &c )
{
    it->setText( 0, c.type );
    it->setText( 1, c.name );
    it->setText( 2, c.number );
    it->setText( 3, c.valid.toString( "MM yyyy" ) );
    it->setText( 4, c.civ );
    it->setData( 0, Qt::UserRole, static_cast<qint32>(c.options) );
}

PHIAAddress PHIASettingsDlg::itemToAddress( const QTreeWidgetItem *it )
{
    PHIAAddress a;
    a.title=it->text( 0 );
    a.firstname=it->text( 1 );
    a.lastname=it->text( 2 );
    a.organisation=it->text( 3 );
    a.unit=it->text( 4 );
    a.street=it->text( 5 );
    a.state=it->text( 6 );
    a.city=it->text( 7 );
    a.zip=it->text( 8 );
    a.country=it->text( 9 );
    a.options=static_cast<PHIAAddress::Options>(it->data( 0, Qt::UserRole ).value<qint32>());
    return a;
}

PHIACreditCard PHIASettingsDlg::itemToCreditCard( const QTreeWidgetItem *it )
{
    PHIACreditCard c;
    c.type=it->text( 0 );
    c.name=it->text( 1 );
    c.number=it->text( 2 );
    c.valid=QDate::fromString( it->text( 3 ), "MM yyyy" );
    c.civ=it->text( 4 );
    c.options=static_cast<PHIACreditCard::Options>(it->data( 0, Qt::UserRole ).value<qint32>());
    return c;
}

PHIAContact PHIASettingsDlg::itemToContact( const QTreeWidgetItem *it )
{
    PHIAContact c;
    c.name=it->text( 0 );
    c.email=it->text( 1 );
    c.mobile=it->text( 2 );
    c.phone=it->text( 3 );
    c.fax=it->text( 4 );
    c.url=it->text( 5 );
    c.options=static_cast<PHIAContact::Options>(it->data( 0, Qt::UserRole ).value<qint32>());
    return c;
}

void PHIASettingsDlg::accept()
{
    PHIAConfig *c=PHIAConfig::instance();
    if ( _usePasswd->isChecked() && _passwdChanged ) {
        if ( _verify->text()!=_passwd->text() ) {
            QMessageBox::warning( this, tr( "Error" ), tr( "Passwords don't match." ), QMessageBox::Ok );
            return;
        }
        c->setPasswd( _passwd->text() );
    }

    QString fileName=c->fileName();
    if ( fileName.isEmpty() ) {
        fileName=QFileDialog::getSaveFileName( this, tr( "Save settings" ),
            QDir::homePath()+QDir::separator()+"user.phicfg", tr( "Phi user config" )+" (*.phicfg)" );
        if ( fileName.isEmpty() ) return;
    }

    c->setUsePasswd( _usePasswd->isChecked() );
    c->setAskForApp( _askAppMode->isChecked() );
    c->setAskForUnsecure( _askUnsecure->isChecked() );
    //c->setNotifyUpdate( _notify->isChecked() );
    c->setDownloadInstall( _downloadInstall->isChecked() );
    c->setServer( _downloadServer->currentText() );
    c->setInterval( _intervalCombo->currentIndex() );
    c->setHome( _home->text() );
    c->setUseProxy( _useProxy->isChecked() );
    c->setProxyPasswd( _proxyPasswd->text() );
    c->setProxyUser( _proxyUser->text() );

    QStringList langs;
    for ( int i=0; i<_languages->count(); i++ ) {
        langs << _languages->item( i )->text();
    }
    c->setLanguages( langs );

    QSettings s;
    s.setValue( PHIA::configName( PHIA::AutoLoadImages ), _w_autoLoadImages->isChecked() );
    s.setValue( PHIA::configName( PHIA::EnableJava ), _w_java->isChecked() );
    s.setValue( PHIA::configName( PHIA::EnableJavascript ), _w_javaScript->isChecked() );
    s.setValue( PHIA::configName( PHIA::JSOpenWindow ), _w_openWindows->isChecked() );
    s.setValue( PHIA::configName( PHIA::PrintBackgrounds ), _w_printBackgrounds->isChecked() );
    s.setValue( PHIA::configName( PHIA::EnablePlugins ), _w_plugins->isChecked() );
    if ( _useProxy->isChecked() ) {
        s.setValue( PHIA::configName( PHIA::ProxyHost ), _proxyHost->text() );
        s.setValue( PHIA::configName( PHIA::ProxyPort ), _proxyPort->value() );
    } else {
        s.setValue( PHIA::configName( PHIA::ProxyHost ), QString() );
    }
    PHIANetManager::instance()->updateProxy();
    s.setValue( PHIA::configName( PHIA::DownloadDirectory ), _downloadDir->text() );

    QWebSettings *ws=QWebSettings::globalSettings();
    ws->setOfflineStoragePath( _downloadDir->text() );
    ws->setIconDatabasePath( _downloadDir->text() );

    ws->setAttribute( QWebSettings::AutoLoadImages, _w_autoLoadImages->isChecked() );
    ws->setAttribute( QWebSettings::JavascriptEnabled, _w_javaScript->isChecked() );
    ws->setAttribute( QWebSettings::JavascriptCanOpenWindows, _w_openWindows->isChecked() );
    ws->setAttribute( QWebSettings::JavaEnabled, _w_java->isChecked() );
    ws->setAttribute( QWebSettings::PrintElementBackgrounds, _w_printBackgrounds->isChecked() );
    ws->setAttribute( QWebSettings::PluginsEnabled, _w_plugins->isChecked() );

    QTreeWidgetItem *it;
    QList <PHIAAddress> addresses;
    for ( int i=0; i<_addressTree->topLevelItemCount(); i++ ) {
        it=_addressTree->topLevelItem( i );
        addresses.append( itemToAddress( it ) );
    }
    c->setAddresses( addresses );

    QList <PHIACreditCard> cards;
    for ( int i=0; i<_cardTree->topLevelItemCount(); i++ ) {
        it=_cardTree->topLevelItem( i );
        cards.append( itemToCreditCard( it ) );
    }
    c->setCreditCards( cards );

    QList <PHIAContact> contacts;
    for ( int i=0; i<_contactTree->topLevelItemCount(); i++ ) {
        it=_contactTree->topLevelItem( i );
        contacts.append( itemToContact( it ) );
    }
    c->setContacts( contacts );

    c->save( fileName );
    QDialog::accept();
}

void PHIASettingsDlg::on__loadCacheDir_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Select cache directory" ),
        QDir::homePath() );
    if ( file.isEmpty() ) return;
    _cacheDir->setText( file );
}

void PHIASettingsDlg::on__loadDownloadDir_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Select downloads directory" ),
        QDir::homePath() );
    if ( file.isEmpty() ) return;
    _downloadDir->setText( file );
}

void PHIASettingsDlg::on__clearCache_clicked()
{
    QSettings s;
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ), QString() ).toString();
    if ( dir.isEmpty() ) return;
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Clear cache" ),
        tr( "Do you really want to remove the files in '%1'?" ).arg( dir ),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
    if ( res==QMessageBox::Abort ) return;
    QDir cacheDir( dir );
    QStringList entries=cacheDir.entryList( QDir::Files );
    QString entry;
    foreach ( entry, entries ) {
        entry=cacheDir.absolutePath()+QDir::separator()+entry;
        QFile::remove( entry );
        qDebug( "Removing: %s", qPrintable( entry ) );
    }
}

void PHIASettingsDlg::on__addLang_clicked()
{
    if ( _lang->text().isEmpty() ) return;
    _languages->addItem( new QListWidgetItem( _lang->text(), _languages ) );
}

void PHIASettingsDlg::on__delLang_clicked()
{
    QListWidgetItem *it=_languages->item( _languages->currentRow() );
    if ( !it ) return;
    delete it;
}

void PHIASettingsDlg::on__verify_textEdited( const QString& )
{
    _passwdChanged=true;
}

void PHIASettingsDlg::on__passwd_textEdited( const QString& )
{
    _passwdChanged=true;
}
