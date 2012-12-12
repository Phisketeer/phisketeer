/*
#    Copyright (C) 2010-2012  Marius B. Schumacher
#    Copyright (C) 2011-2012  Phisys AG, Switzerland
#    Copyright (C) 2012  Phisketeer.org team
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QApplication>
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include "sconfig.h"
#include "phi.h"

SConfig::SConfig( QWidget *parent )
    : QDialog( parent )
{
    qDebug( "SConfig::SConfig()" );
    setupUi( this );
    setWindowTitle( qApp->applicationName()+" V"+QString( PHIVERSION ) );
    //_addTool->setIcon( QPixmap( ":/gnome/add" ) );
    //_delTool->setIcon( QPixmap( ":/gnome/cancel" ) );
    _configCombo->insertItem( 0, "default" );
    _virtualTree->setIndentation( 0 );
    _tab->setCurrentIndex( 0 );
    resize( 600, 300 );

    _base=PHI::compiledRoot()+QDir::separator()+"default";

    QSettings *s=PHI::globalSettings();
    s->beginGroup( "default" );

#ifdef Q_OS_WIN
    int mode=s->value( "StartMode", 3 ).toInt();
    if ( mode==2 ) _winStartupCheck->setChecked( true );
    else _winStartupCheck->setChecked( false );
#else
    _winStartupCheck->hide();
#endif
#ifdef PHIAPPSTORE
    // Unfortunately the detached process does not open PowerBox file dialogs in sandboxing
    // Any ideas are welcome
    _sslCertTool->hide();
    _sslKeyTool->hide();
    _baseDirTool->hide();
    _mimeTool->hide();
    _logDirTool->hide();
    _baseDirTool->hide();
    _rootTool->hide();
    _tempTool->hide();
#endif

    _base=s->value( "BaseDir", _base ).toString();
    _email->setText( s->value( "Admin", "admin@localhost" ).toString() );
    _index->setText( s->value( "Index", "index.phis,index.html,index.html" ).toString() );
    _baseDir->setText( _base );
    _mime->setText( s->value( "MimeTypesFile", _base+QDir::separator()+"mimetypes->txt" ).toString() );

    _port->setValue( s->value( "ListenerPort", 8080 ).toInt() );
    _interface->setText( s->value( "ListenerIF", "Any" ).toString() );
    _keepAlive->setValue( s->value( "KeepAlive", 60000 ).toInt() );

    _logDir->setText( s->value( "LogDir", _base+QDir::separator()+"log" ).toString() );
    qint32 filter=s->value( "LogFilter", 0 ).value<qint32>();
    _logCritical->setChecked( filter & PHI_LOG_FILTER_CRIT );
    _logWarn->setChecked( filter & PHI_LOG_FILTER_WARN );
    _logError->setChecked( filter & PHI_LOG_FILTER_ERROR );
    _logDebug->setChecked( filter & PHI_LOG_FILTER_DEBUG );
    _logTrace->setChecked( filter & PHI_LOG_FILTER_TRACE );
    _logUser->setChecked( filter & PHI_LOG_FILTER_USER );

    _sslBox->setChecked( s->value( "SSLEnabled", false ).toBool() );
    _sslCert->setText( s->value( "SSLCertificate" ).toString() );
    _sslKey->setText( s->value( "SSLPrivateKey" ).toString() );
    _sslInterface->setText( s->value( "SSLListenerIF", "Any" ).toString() );
    _sslPort->setValue( s->value( "SSLListenerPort", 443 ).toInt() );

    QStringList hosts=s->childGroups();
    QString host;
    foreach ( host, hosts ) {
        s->beginGroup( host );
        QTreeWidgetItem *it=new QTreeWidgetItem();
        it->setText( 0, host );
        it->setText( 1, s->value( "DocumentRoot" ).toString() );
        it->setText( 2, s->value( "TempDir" ).toString() );
        it->setFlags( Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable );
        _virtualTree->addTopLevelItem( it );
        s->endGroup();
    }
    s->endGroup();
    QSettings geo;
    restoreGeometry( geo.value( "GeometryPhisconf" ).toByteArray() );
    QTimer::singleShot( 0, this, SLOT( slotInit() ) );
}

SConfig::~SConfig()
{
    QSettings geo;
    geo.setValue( "GeometryPhisconf", saveGeometry() );
    qDebug( "SConfig::~SConfig()" );
}

void SConfig::slotInit()
{
    QSettings *s=PHI::globalSettings();
    if ( !s->isWritable() ) { // always true in Mac OS X
        QMessageBox::critical( this, tr( "Missing privileges" ), tr( "You must be administrator "
            "or root to run this application." ), QMessageBox::Ok );
        reject();
        return;
    }
    _bin=s->value( "BinDir", qApp->applicationDirPath() ).toString();
    if ( !QFile::exists( _bin ) ) _bin="";
    if ( _bin.isEmpty() ) {
        _bin=QFileDialog::getExistingDirectory( this,
            tr( "Please select the binary directory of the Phisketeer installation." ),
            PHI::compiledPrefix() );
        if ( _bin.isEmpty() ) return;
        s->setValue( "BinDir", _bin );
        s->sync();
    }
    slotCheckService();
}

void SConfig::slotCheckService()
{
    int res=PHI::checkService();
    qDebug( "RES=%d", res );
    if ( res==1 ) {
        _stop->setEnabled( true );
        _interface->setEnabled( false );
        _port->setEnabled( false );
        _keepAlive->setEnabled( false );
        _clearCache->setEnabled( true );
        _start->setEnabled( false );
        _icon->setPixmap( QPixmap( ":/gnome/ok" ) );
    } else {
        _stop->setEnabled( false );
        _interface->setEnabled( true );
        _port->setEnabled( true );
        _keepAlive->setEnabled( true );
        _clearCache->setEnabled( false );
        _start->setEnabled( true );
        _icon->setPixmap( QPixmap( ":/gnome/del" ) );
    }
}

void SConfig::accept()
{
    save();
    QDialog::accept();
}

void SConfig::save()
{
    QSettings *s=PHI::globalSettings();
    s->beginGroup( "default" );
    s->setValue( "Admin", _email->text() );
    s->setValue( "Index", _index->text() );
    s->setValue( "BaseDir", _baseDir->text() );
    s->setValue( "MimeTypesFile", _mime->text() );
    s->setValue( "ListenerPort", _port->value() );
    s->setValue( "ListenerIF", _interface->text() );
    s->setValue( "KeepAlive", _keepAlive->value() );

#ifdef Q_OS_WIN
    QSettings set( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis", QSettings::NativeFormat );
    if ( _winStartupCheck->isChecked() ) {
        s->setValue( "StartMode", 2 );
        set.setValue( "Start", 2 );
    } else {
        s->setValue( "StartMode", 3 );
        set.setValue( "Start", 3 );
    }
#endif

    s->setValue( "LogDir", _logDir->text() );
    qint32 filter=0;
    if ( _logCritical->isChecked() ) filter |= PHI_LOG_FILTER_CRIT;
    if ( _logWarn->isChecked() ) filter |= PHI_LOG_FILTER_WARN;
    if ( _logError->isChecked() ) filter |= PHI_LOG_FILTER_ERROR;
    if ( _logTrace->isChecked() ) filter |= PHI_LOG_FILTER_TRACE;
    if ( _logDebug->isChecked() ) filter |= PHI_LOG_FILTER_DEBUG;
    if ( _logUser->isChecked() ) filter |= PHI_LOG_FILTER_USER;
    s->setValue( "LogFilter", filter );

    s->setValue( "SSLEnabled", _sslBox->isChecked() );
    s->setValue( "SSLCertificate", _sslCert->text() );
    s->setValue( "SSLPrivateKey", _sslKey->text() );
    s->setValue( "SSLListenerPort", _sslPort->value() );
    s->setValue( "SSLListenerIF", _sslInterface->text() );

    QStringList domains=s->childGroups();
    QString domain;
    foreach ( domain, domains ) {
        s->remove( domain );
    }

    QTreeWidgetItem *it;
    for ( int i=0; i<_virtualTree->topLevelItemCount(); i++ ) {
        it=_virtualTree->topLevelItem( i );
        if ( it->text( 0 ).isEmpty() ) continue;
        s->beginGroup( it->text( 0 ) );
        s->setValue( "DocumentRoot", it->text( 1 ) );
        s->setValue( "TempDir", it->text( 2 ) );
        s->endGroup();
    }
    s->endGroup();
}

void SConfig::on__start_clicked()
{
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Overwrite config" ),
        tr( "This will store your current configuration." )+'\n'
        +tr( "Are you sure you want to continue?" ),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
    if ( res==QMessageBox::Abort ) return;
    save();
    if ( !PHI::startPhisService() ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not start the server." ),
            QMessageBox::Ok );
        return;
    }
    _start->setEnabled( false );
    _stop->setEnabled( false );
    QTimer::singleShot( 2000, this, SLOT( slotCheckService() ) );
}

void SConfig::on__stop_clicked()
{
    if ( !PHI::stopPhisService() ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not stop the server." ),
            QMessageBox::Ok );
        return;
    }
    _start->setEnabled( false );
    _stop->setEnabled( false );
    QTimer::singleShot( 2000, this, SLOT( slotCheckService() ) );
}

void SConfig::on__clearCache_clicked()
{
    if ( !PHI::clearPhisServiceCache() ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not clear the server cache." ),
            QMessageBox::Ok );
    }
}

void SConfig::on__baseDirTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Base directory" ), _base );
    if ( file.isEmpty() ) return;
    _base=file;
    _baseDir->setText( file );
}

void SConfig::on__mimeTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "Mime types file" ), _base );
    if ( file.isEmpty() ) return;
    _mime->setText( file );
}

void SConfig::on__logDirTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Log directory" ), _logDir->text() );
    if ( file.isEmpty() ) return;
    _logDir->setText( file );
}

void SConfig::on__rootTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Document root" ), _root->text() );
    if ( file.isEmpty() ) return;
    _root->setText( file );
}

void SConfig::on__tempTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Temporary directory" ), _temp->text() );
    if ( file.isEmpty() ) return;
    _temp->setText( file );
}

void SConfig::on__sslCertTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "SSL certificate file" ), _sslCert->text() );
    if ( file.isEmpty() ) return;
    _sslCert->setText( file );
}

void SConfig::on__sslKeyTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "SSL private key file" ), _sslKey->text() );
    if ( file.isEmpty() ) return;
    _sslKey->setText( file );
}

void SConfig::on__addTool_clicked()
{
    QTreeWidgetItem *it=new QTreeWidgetItem();
    it->setText( 0, _host->text() );
    it->setText( 1, _root->text() );
    it->setText( 2, _temp->text() );
    it->setFlags( Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    _virtualTree->addTopLevelItem( it );
}

void SConfig::on__delTool_clicked()
{
    QTreeWidgetItem *it=_virtualTree->currentItem();
    if ( !it ) return;
    delete it;
}

void SConfig::on__virtualTree_itemClicked( QTreeWidgetItem *it , int  )
{
    if ( !it ) return;
    _host->setText( it->text( 0 ) );
    _root->setText( it->text( 1 ) );
    _temp->setText( it->text( 2 ) );
}
