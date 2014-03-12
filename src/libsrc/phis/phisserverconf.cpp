/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
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
#include <QSettings>
#include <QFileDialog>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "phisserverconf.h"
#include "phiapplication.h"
#include "phislogwriter.h"
#include "phisparent.h"
#include "phinetmanager.h"
#include "phislogviewer.h"

PHISServerConf::PHISServerConf( QWidget *parent )
    : QDialog( parent, Qt::Sheet | Qt::WindowMaximizeButtonHint )
{
    setupUi( this );
    setWindowIcon( QIcon( L1( ":/phi/logo" ) ) );
    setWindowTitle( QString::fromLatin1( "PhisConf %1" ).arg( L1( PHISVERSION ) ) );
    _configCombo->insertItem( 0, PHI::defaultString() );
    _virtualTree->setIndentation( 0 );
    _tab->setCurrentIndex( 0 );
    resize( 600, 300 );
    _base=phiApp->dataPath();

    QSettings *s=phiApp->serverSettings();
    s->beginGroup( PHI::defaultString() );

#ifdef Q_OS_WIN
    int mode=s->value( L1( "StartMode" ), 3 ).toInt();
    if ( mode==2 ) _winStartupCheck->setChecked( true );
    else _winStartupCheck->setChecked( false );
#else
    _winStartupCheck->hide();
#endif

    _base=s->value( L1( "BaseDir" ), _base ).toString();
    _email->setText( s->value( L1( "Admin" ), L1( "admin@localhost" ) ).toString() );
    _index->setText( s->value( L1( "Index" ), L1( "index.phis,index.html,index.html" ) ).toString() );
    _baseDir->setText( _base );
    _mime->setText( s->value( L1( "MimeTypesFile" ), _base+QDir::separator()+L1( "mimetypes.txt" ) ).toString() );

    _port->setValue( s->value( L1( "ListenerPort" ), 8080 ).toInt() );
    _interface->setText( s->value( L1( "ListenerIF" ), L1( "Any" ) ).toString() );
    _keepAlive->setValue( s->value( L1( "KeepAlive" ), 60000 ).toInt() );

    _logDir->setText( s->value( L1( "LogDir" ), _base+QDir::separator()+QLatin1String( "log" ) ).toString() );
    qint32 filter=s->value( L1( "LogFilter" ), 0 ).value<qint32>();
    _logCritical->setChecked( filter & PHISLogWriter::Critical );
    _logWarn->setChecked( filter & PHISLogWriter::Warning );
    _logError->setChecked( filter & PHISLogWriter::Error );
    _logDebug->setChecked( filter & PHISLogWriter::Debug );
    _logTrace->setChecked( filter & PHISLogWriter::Trace );
    _logUser->setChecked( filter & PHISLogWriter::User );

    _sslBox->setChecked( s->value( L1( "SSLEnabled" ), false ).toBool() );
    _sslCert->setText( s->value( L1( "SSLCertificate" ) ).toString() );
    _sslKey->setText( s->value( L1( "SSLPrivateKey" ) ).toString() );
    _sslInterface->setText( s->value( L1( "SSLListenerIF" ), L1( "Any" ) ).toString() );
    _sslPort->setValue( s->value( L1( "SSLListenerPort" ), 443 ).toInt() );

    QStringList hosts=s->childGroups();
    QString host;
    foreach ( host, hosts ) {
        s->beginGroup( host );
        QTreeWidgetItem *it=new QTreeWidgetItem();
        it->setText( 0, host );
        it->setText( 1, s->value( L1( "DocumentRoot" ) ).toString() );
        it->setText( 2, s->value( L1( "TempDir" ) ).toString() );
        it->setFlags( Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable );
        _virtualTree->addTopLevelItem( it );
        s->endGroup();
    }
    s->endGroup(); // default string
    QSettings *ps=phiApp->settings();
    restoreGeometry( ps->value( L1( "GeometryServerconf" ) ).toByteArray() );
    QTimer::singleShot( 0, this, SLOT( slotInit() ) );
}

PHISServerConf::~PHISServerConf()
{
    QSettings *s=phiApp->settings();
    s->setValue( L1( "GeometryServerconf" ), saveGeometry() );
    qDebug( "PHISServerConf::~PHISServerConf()" );
}

void PHISServerConf::slotInit()
{
    QSettings *s=phiApp->serverSettings();
    if ( !s->isWritable() ) { // always true in Mac OS X
        QMessageBox::critical( this, tr( "Missing privileges" ), tr( "Can not write to the config file\n"
            "'%1'. Try as 'root' or 'Administrator'." ).arg( s->fileName() ), QMessageBox::Ok );
        reject();
        return;
    }
    slotCheckService();
}

void PHISServerConf::slotCheckService()
{
    int running=-1;
    QSettings *s=phiApp->serverSettings();
#ifdef PHIEMBEDEDSERVER
    s->beginGroup( PHI::defaultString() );
    int lport=s->value( L1( "ListenerPort" ), 8080 ).toInt();
    s->endGroup();
    QUrl url( L1( "http://localhost/phi.phis?i=ping" ) );
    url.setPort( lport );
    QNetworkRequest req( url );
    QNetworkReply *reply=PHINetManager::instance()->defaultNetworkAccessManager()->get( req );
    QEventLoop loop;
    connect( reply, &QNetworkReply::finished, &loop, &QEventLoop::quit );
    loop.exec( QEventLoop::ExcludeUserInputEvents );
    if ( reply->error()==QNetworkReply::NoError ) {
        if ( reply->readAll().startsWith( "alive" ) ) running=1;
    }
    reply->deleteLater();
#else
    Q_UNUSED( s )
    running=phiApp->checkPhisService();
#endif
    if ( running==1 ) {
        _stop->setEnabled( true );
        _interface->setEnabled( false );
        _port->setEnabled( false );
        _keepAlive->setEnabled( false );
        _clearCache->setEnabled( true );
        _start->setEnabled( false );
        _icon->setPixmap( QPixmap( L1( ":/gnome/ok" ) ) );
    } else {
        _stop->setEnabled( false );
        _interface->setEnabled( true );
        _port->setEnabled( true );
        _keepAlive->setEnabled( true );
        _clearCache->setEnabled( false );
        _start->setEnabled( true );
        _icon->setPixmap( QPixmap( L1( ":/gnome/del" ) ) );
    }
}

void PHISServerConf::accept()
{
    save();
    QDialog::accept();
}

void PHISServerConf::save()
{
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( PHI::defaultString() );
    s->setValue( L1( "Admin" ), _email->text() );
    s->setValue( L1( "Index" ), _index->text() );
    s->setValue( L1( "BaseDir" ), _baseDir->text() );
    s->setValue( L1( "MimeTypesFile" ), _mime->text() );
    s->setValue( L1( "ListenerPort" ), _port->value() );
    s->setValue( L1( "ListenerIF" ), _interface->text() );
    s->setValue( L1( "KeepAlive" ), _keepAlive->value() );

#ifdef Q_OS_WIN
    QSettings set( L1( "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\services\\phis" ), QSettings::NativeFormat );
    if ( _winStartupCheck->isChecked() ) {
        s->setValue( L1( "StartMode" ), 2 );
        set.setValue( L1( "Start" ), 2 );
    } else {
        s->setValue( L1( "StartMode" ), 3 );
        set.setValue( L1( "Start" ), 3 );
    }
#endif

    s->setValue( L1( "LogDir" ), _logDir->text() );
    qint32 filter=0;
    if ( _logCritical->isChecked() ) filter |= PHISLogWriter::Critical;
    if ( _logWarn->isChecked() ) filter |= PHISLogWriter::Warning;
    if ( _logError->isChecked() ) filter |= PHISLogWriter::Error;
    if ( _logTrace->isChecked() ) filter |= PHISLogWriter::Trace;
    if ( _logDebug->isChecked() ) filter |= PHISLogWriter::Debug;
    if ( _logUser->isChecked() ) filter |= PHISLogWriter::User;
    s->setValue( L1( "LogFilter" ), filter );

    s->setValue( L1( "SSLEnabled" ), _sslBox->isChecked() );
    s->setValue( L1( "SSLCertificate" ), _sslCert->text() );
    s->setValue( L1( "SSLPrivateKey" ), _sslKey->text() );
    s->setValue( L1( "SSLListenerPort" ), _sslPort->value() );
    s->setValue( L1( "SSLListenerIF" ), _sslInterface->text() );

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
        s->setValue( L1( "DocumentRoot" ), it->text( 1 ) );
        s->setValue( L1( "TempDir" ), it->text( 2 ) );
        s->endGroup();
    }
    s->endGroup();
}

void PHISServerConf::on__start_clicked()
{
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Overwrite config" ),
        tr( "This will store your current configuration." )+QLatin1Char( '\n' )
        +tr( "Are you sure you want to continue?" ),
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
    if ( res==QMessageBox::Abort ) return;
    save();
#ifdef PHIEMBEDEDSERVER
    PHISParent::instance()->startService();
#else
    qWarning() << "start clicked";
    if ( !phiApp->startPhisService() ) {
        QMessageBox::warning( this, tr( "Error" ),
            tr( "Could not start the Phis Web service." ), QMessageBox::Ok );
        return;
    }
#endif
    _start->setEnabled( false );
    _stop->setEnabled( false );
    QTimer::singleShot( 1000, this, SLOT( slotCheckService() ) );
}

void PHISServerConf::on__stop_clicked()
{
#ifdef PHIEMBEDEDSERVER
    PHISParent::instance()->stopService();
#else
    if ( !phiApp->stopPhisService() ) {
        QMessageBox::warning( this, tr( "Error" ),
            tr( "Could not stop the Phis Web service." ), QMessageBox::Ok );
        return;
    }
#endif
    _start->setEnabled( false );
    _stop->setEnabled( false );
    QTimer::singleShot( 1000, this, SLOT( slotCheckService() ) );
}

void PHISServerConf::on__clearCache_clicked()
{
#ifdef PHIEMBEDEDSERVER
    PHISParent::instance()->invalidate();
#else
    if ( !phiApp->clearPhisServiceCache() ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not clear the server cache." ),
            QMessageBox::Ok );
    }
#endif
}

void PHISServerConf::on__baseDirTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Base directory" ), _base );
    if ( file.isEmpty() ) return;
    _base=file;
    _baseDir->setText( file );
}

void PHISServerConf::on__mimeTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "Mime types file" ), _base );
    if ( file.isEmpty() ) return;
    _mime->setText( file );
}

void PHISServerConf::on__logDirTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Log directory" ), _logDir->text() );
    if ( file.isEmpty() ) return;
    _logDir->setText( file );
}

void PHISServerConf::on__rootTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Document root" ), _root->text() );
    if ( file.isEmpty() ) return;
    _root->setText( file );
}

void PHISServerConf::on__viewLog_clicked()
{
    PHISLogViewer::instance()->raise();
}

void PHISServerConf::on__clearLog_clicked()
{
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( PHI::defaultString() );
    QString logFile=s->value( L1( "LogDir" ), phiApp->dataPath()+QDir::toNativeSeparators( L1( "/log" ) ) ).toString();
    s->endGroup();
    logFile.append( QDir::toNativeSeparators( L1( "/error.log" ) ) );
    if ( !QFile::exists( logFile ) ) {
        QMessageBox::warning( this, tr( "Error" ), tr( "Could not access '%1'." ).arg( logFile ), QMessageBox::Ok );
        return;
    }
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Clear log" ),
        tr( "Do you really want to clear the content of the log file?" ), QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
    if ( res==QMessageBox::No ) return;
    QFile f( logFile );
    f.open( QIODevice::WriteOnly );
    f.close();
}

void PHISServerConf::on__tempTool_clicked()
{
    QString file=QFileDialog::getExistingDirectory( this, tr( "Temporary directory" ), _temp->text() );
    if ( file.isEmpty() ) return;
    _temp->setText( file );
}

void PHISServerConf::on__sslCertTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "SSL certificate file" ), _sslCert->text() );
    if ( file.isEmpty() ) return;
    _sslCert->setText( file );
}

void PHISServerConf::on__sslKeyTool_clicked()
{
    QString file=QFileDialog::getOpenFileName( this, tr( "SSL private key file" ), _sslKey->text() );
    if ( file.isEmpty() ) return;
    _sslKey->setText( file );
}

void PHISServerConf::on__addTool_clicked()
{
    QTreeWidgetItem *it;
    for ( int i=0; i<_virtualTree->topLevelItemCount(); i++ ) {
        it=_virtualTree->topLevelItem( i );
        if ( it->text( 0 )==_host->text() ) {
            QMessageBox::warning( this, tr( "Error"), tr( "Current virtual hostname already exists." ), QMessageBox::Ok );
            return;
        }
    }
    it=new QTreeWidgetItem();
    it->setText( 0, _host->text() );
    it->setText( 1, _root->text() );
    it->setText( 2, _temp->text() );
    it->setFlags( Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable );
    _virtualTree->addTopLevelItem( it );
}

void PHISServerConf::on__delTool_clicked()
{
    QTreeWidgetItem *it=_virtualTree->currentItem();
    if ( !it ) return;
    delete it;
}

void PHISServerConf::on__virtualTree_itemClicked( QTreeWidgetItem *it , int  )
{
    if ( !it ) return;
    _host->setText( it->text( 0 ) );
    _root->setText( it->text( 1 ) );
    _temp->setText( it->text( 2 ) );
}
