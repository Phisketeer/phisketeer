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
#include <QFile>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QLocale>
#include <QMessageBox>
#include <QInputDialog>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QProcess>
#include "phiaconfig.h"
#include "phi.h"
#include "phia.h"
#include "phiaauthdlg.h"
#include "phianetmanager.h"
#include "phiupdateinfo.h"
#include "phisysinfo.h"

QDataStream& operator<< ( QDataStream &out, const PHIAAddress &a )
{
    out << static_cast<qint32>(a.options) << a.title << a.firstname << a.lastname << a.organisation
        << a.unit << a.street << a.state << a.city << a.zip << a.country;
    return out;
}

QDataStream& operator>> ( QDataStream &in, PHIAAddress &a )
{
    qint32 options;
    in >> options >> a.title >> a.firstname >> a.lastname >> a.organisation >> a.unit >> a.street
       >> a.state >> a.city >> a.zip >> a.country;
    a.options=static_cast<PHIAAddress::Options>(options);
    return in;
}

QDataStream& operator<< ( QDataStream &out, const PHIAContact &c )
{
    PHIAContact::Options opts=c.options;
    opts |= PHIAContact::HasUrl;
    out << static_cast<qint32>(opts) << c.name << c.email << c.mobile << c.phone << c.fax << c.url;
    return out;
}

QDataStream& operator>> ( QDataStream &in, PHIAContact &c )
{
    qint32 options;
    in >> options >> c.name >> c.email >> c.mobile >> c.phone >> c.fax;
    c.options=static_cast<PHIAContact::Options>(options);
    if ( c.options & PHIAContact::HasUrl ) in >> c.url;
    return in;
}

QDataStream& operator<< ( QDataStream &out, const PHIACreditCard &c )
{
    out << static_cast<qint32>(c.options) << c.name << c.type << c.civ << c.valid;
    if ( c.options & PHIACreditCard::FullNumber ) out << c.number;
    else out << c.number.left( 10 );
    return out;
}

QDataStream& operator>> ( QDataStream &in, PHIACreditCard &c )
{
    qint32 options;
    in >> options >> c.name >> c.type >> c.civ >> c.valid >> c.number;
    c.options=static_cast<PHIACreditCard::Options>(options);
    return in;
}

PHIADownloadEntry::PHIADownloadEntry( QObject *parent )
    : QObject( parent ), _status( PHIADownloadEntry::Init ), _reply( 0 ),
    _totalBytes( 0 ), _bytesDownloaded( 0 ), _parent( 0 )
{
    qDebug( "PHIADownloadEntry::PHIADownloadEntry()" );
}

PHIADownloadEntry::~PHIADownloadEntry()
{
    qDebug( "PHIADownloadEntry::~PHIADownloadEntry()" );
}

void PHIADownloadEntry::addDownload( QNetworkReply *reply, const QString &dir, QWidget *parent )
{
    if ( !reply ) return;
    _parent=parent;
    _reply=reply;
    _reply->setParent( this );
    _dt=QDateTime::currentDateTime();
    connect( reply, SIGNAL( finished() ), this, SLOT( slotDownloadFinished() ) );
    connect( reply, SIGNAL( readyRead() ), this, SLOT( slotReadyRead() ) );
    _status=Downloading;
    _url=reply->url();
    QFileInfo info( _url.path() );
    _location=dir+QDir::separator()+info.fileName();
    int i=2;
    while ( 1 ) {
        _file.setFileName( _location );
        if ( _file.exists() ) {
            _location=dir+QDir::separator()+info.completeBaseName()
                +QString( "_%1." ).arg( i++ )+info.suffix();
            continue;
        }
        break;
    }
    if ( !_file.open( QIODevice::WriteOnly ) ) {
        QMessageBox::warning( parent, tr( "Download" ), tr( "Could not open file '%1' for writing." )
            .arg( _location ), QMessageBox::Ok );
        _status=Error;
        _reply->deleteLater();
        _reply=0;
    }
    slotReadyRead();
    if ( _reply->isFinished() ) slotDownloadFinished();
}

void PHIADownloadEntry::slotReadyRead()
{
    if ( !_reply ) return;
    if ( !_reply->bytesAvailable() ) return;
    _file.write( _reply->readAll() );
}

void PHIADownloadEntry::slotDownloadFinished()
{
    if ( !_reply ) return;
    _reply->deleteLater();
    if ( _status==Downloading ) {
        if ( _reply->error()!=QNetworkReply::NoError ) _status=Error;
        else _status=Completed;
    }
    _file.close();
    emit save();
    if ( _status==Error ) {
        if ( _parent ) {
            QMessageBox::warning( _parent, tr( "Network error" ),
                tr( "Reply: %1" ).arg( _reply->errorString() ), QMessageBox::Ok );
        }
        emit aborted();
    } else emit finished( _file.fileName() );
    _reply=0;
}

void PHIADownloadEntry::abort()
{
    if ( !_reply ) return;
    _reply->abort();
    _reply->deleteLater();
    _reply=0;
    _status=Canceled;
    _file.close();
    _file.remove();
    emit save();
    emit aborted();
}

QDataStream& operator<<( QDataStream &out, const PHIADownloadEntry *e )
{
    out << static_cast<quint8>(e->_status) << e->_dt << e->_location
        << e->_url << e->_totalBytes << e->_bytesDownloaded;
    return out;
}

QDataStream& operator>>( QDataStream &in, PHIADownloadEntry *e )
{
    quint8 status;
    in >> status >> e->_dt >> e->_location >> e->_url >> e->_totalBytes >> e->_bytesDownloaded;
    e->_status=static_cast<PHIADownloadEntry::Status>(status);
    return in;
}

PHIAConfig* PHIAConfig::_instance=0;

PHIAConfig::PHIAConfig( QObject *parent )
    : QObject( parent ), _initDownloadDialog( true ), _updateInProgress( false ),
    _interval( PHIAConfig::INever )
{
    qDebug( "PHIAConfig::PHIAConfig()" );
    clear();
    connect( &_timer, SIGNAL( timeout() ), this, SLOT( slotPasswordTimeout() ) );
    QSettings s;
    QString cfile=QStandardPaths::writableLocation( QStandardPaths::DataLocation );
    QDir path( cfile );
    if ( !path.exists() ) path.mkpath( cfile );
    cfile+="/phia.cfg";
    _fileName=s.value( PHIA::configName( PHIA::ConfigFile ), cfile ).toString();
    _lastUpdateCheck=s.value( PHIA::configName( PHIA::LastUpdateCheck ), QDateTime() ).toDateTime();
    if ( !_lastUpdateCheck.isValid() ) {
        _lastUpdateCheck=QDateTime::currentDateTime().addDays(-2);
    }
    if ( !_fileName.isEmpty() ) load( _fileName );
    loadDownloadEntries();

    bool checkUpdate=false;
    QDate today=QDate::currentDate();
    switch ( _interval ) {
    case INever:
        qDebug( "check never" );
        return;
    case IEveryDay:
        qDebug( "check everyday" );
        if ( _lastUpdateCheck.date()<today ) checkUpdate=true;
        break;
    case IEveryWeek:
        if ( _lastUpdateCheck.date().addDays(7)<today ) checkUpdate=true;
        break;
    case IEveryTwoWeeks:
        if ( _lastUpdateCheck.date().addDays(14)<today ) checkUpdate=true;
        break;
    case IEveryMonth:
        if ( _lastUpdateCheck.date().addMonths(1)<today ) checkUpdate=true;
        break;
    }
    if ( checkUpdate ) QTimer::singleShot( 1000, this, SLOT( slotCheckForUpdate() ) );
}

PHIAConfig::~PHIAConfig()
{
    PHIADownloadEntry *e;
    foreach ( e, _downloads ) e->abort();
    slotSaveDownloadEntries();
    QSettings s;
    s.setValue( PHIA::configName( PHIA::LastUpdateCheck ), _lastUpdateCheck );
    qDebug( "PHIAConfig::~PHIAConfig()" );
}

PHIAConfig* PHIAConfig::instance( QObject *parent )
{
    if ( _instance ) return _instance;
    _instance=new PHIAConfig( parent );
    return _instance;
}

void PHIAConfig::clear()
{
    _timer.stop();
    _fileVersion=1;
    _options= OAskForApp | OAskForUnsecure | ONotifyUpdate;
    _passwdOpen=false;
    _addressList.clear();
    _contactList.clear();
    _creditCardList.clear();

    _languages.clear();
    QLocale locale;
    QString lang=locale.name();
    if ( lang.size()>2 ) {
        lang[2]='-'; // de_DE -> de-de
        lang.replace( 3, 2, lang.right( 2 ).toLower() );
    }
    _languages << lang;
    if ( lang.size()>2 ) _languages << lang.left( 2 );
    _languages << "en";

    _fileName="";
    _validProxy=true;
    _passwd.clear();
    _proxyPasswd.clear();
    _proxyUser.clear();
    _usedIds=UNone;
    _pwTimeout=30*60; // timeout after 30 min
    _server="www.phisketeer.org";
    _interval=IEveryTwoWeeks;
    _downloadDir=QStandardPaths::writableLocation( QStandardPaths::DownloadLocation );
}

PHIADownloadEntry* PHIAConfig::addDownload( QNetworkReply *reply, QWidget *parent, bool ask )
{
    if ( !reply ) return 0;
    if ( ask ) {
        QMessageBox::Button res=QMessageBox::warning( parent, tr( "Download" ),
           tr( "Do you really want to download '%1' to the local file system?" )
           .arg( QFileInfo( reply->url().path() ).fileName() ),
           QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
        if ( res==QMessageBox::Abort ) return 0;
    }
    showDownloadDialog( parent );
    qDebug( "adding download entry" );
    PHIADownloadEntry *e=new PHIADownloadEntry( this );
    if ( _downloadDir.isEmpty() ) {
        _downloadDir=QStandardPaths::writableLocation( QStandardPaths::DownloadLocation );
        QDir mkdir( _downloadDir );
        if ( !mkdir.exists() ) mkdir.mkpath( _downloadDir );
    }
    connect( e, SIGNAL( save() ), this, SLOT( slotSaveDownloadEntries() ) );
    e->addDownload( reply, _downloadDir, parent );
    PHIADownloadDlg::instance()->addDownload( e );
    _downloads.append( e );
    if ( !slotSaveDownloadEntries() ) {
        QMessageBox::warning( parent, tr( "Save error" ),
            tr( "Could not save download DB '%1'." ).arg( "downloads.db" ) );
    }
    return e;
}

bool PHIAConfig::slotSaveDownloadEntries()
{
    QSettings s;
    s.setValue( PHIA::configName( PHIA::DownloadDirectory ), _downloadDir );
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ) ).toString();
    if ( dir.isEmpty() ) QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
    QDir mkdir( dir );
    if ( !mkdir.exists() ) mkdir.mkpath( dir );
    QFile f( dir+QDir::separator()+"downloads.db" );
    if ( !f.open( QIODevice::WriteOnly ) ) return false;
    QDataStream out( &f );
    out.setVersion( PHI_DSV );
    out << static_cast<quint8>(1) << static_cast<qint32>(_downloads.count());
    for ( int i=0; i<_downloads.count(); i++ ) out << _downloads.at(i);
    return true;
}

bool PHIAConfig::loadDownloadEntries()
{
    QSettings s;
    _downloadDir=s.value( PHIA::configName( PHIA::DownloadDirectory ), QString() ).toString();
    QString dir=s.value( PHIA::configName( PHIA::CacheDirectory ), QString() ).toString();
    if ( dir.isEmpty() )QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
    QDir mkdir( dir );
    if ( !mkdir.exists() ) return false;
    QFile f( dir+QDir::separator()+"downloads.db" );
    if ( !f.open( QIODevice::ReadOnly ) ) return false;
    QDataStream in( &f );
    in.setVersion( PHI_DSV );
    quint8 version;
    qint32 counts;
    in >> version >> counts;
    for ( qint32 i=0; i<counts; i++ ) {
        PHIADownloadEntry *e=new PHIADownloadEntry( this );
        in >> e;
        _downloads.append( e );
    }
    return true;
}

void PHIAConfig::slotDownloadsClearClicked()
{
    PHIADownloadEntry *e;
    foreach ( e, _downloads ) delete e;
    _downloads.clear();
    if ( !slotSaveDownloadEntries() ) {
        // print error message
        //QMessageBox::warning( 0, tr( "Save error" ), tr( "Could not save download DB." ) );
    }
}

void PHIAConfig::slotCheckForUpdate( QWidget *parent )
{
    if ( _updateInProgress ) {
        if ( parent ) {
            QMessageBox::warning( parent, tr( "Update" ), tr( "Another update process is "
            "currently running, please wait until the process has finished." ), QMessageBox::Ok );
        }
        return;
    }
    qDebug( "Starting check for update" );
    // if parent==0 - use silent mode
    _updateWidgetParent=parent;
    QNetworkRequest req;
    QSettings *s=PHI::globalSettings();
    QString url=s->value( "PhiUpdateXML",
        QString( "http://www.phisketeer.org/downloads/phiupdate.xml" ) ).toString();
    req.setUrl( url );
    _updateReply=PHIANetManager::instance()->networkAccessManager()->get( req );
    connect( _updateReply, SIGNAL( finished() ), this, SLOT( slotGetUpdateInfoFinished() ) );
}

void PHIAConfig::slotGetUpdateInfoFinished()
{
    qDebug( "slotGetUpdateInfoFinished() parent=%p", _updateWidgetParent );
    _updateReply->deleteLater();
    if ( _updateReply->error()!=QNetworkReply::NoError ) {
        if ( _updateWidgetParent ) {
            QMessageBox::warning( _updateWidgetParent, tr( "Network reply error" ),
                tr( "Reply: %1" ).arg( _updateReply->errorString() ), QMessageBox::Ok );
            _updateWidgetParent=0;
        }
        return;
    }
    QString product="phisketeer";
/*
    QString product="phiplugin";
#ifdef Q_OS_MAC
    product="phisketeer";
#else
    QSettings *s=PHI::globalSettings();
    if ( s->contains( "Phisketeer" ) ) product="phisketeer";
    else {
        QString binDir=s->value( "BinDir", QString() ).toString();
        if ( binDir.isEmpty() ) {
            if ( _updateWidgetParent ) {
                binDir=QFileDialog::getExistingDirectory( _updateWidgetParent,
                    tr( "Please select the binary directory of the Phi installation:"),
                    PHI::compiledPrefix() );
                if ( !QFile::exists( binDir ) ) {
                    QMessageBox::warning( _updateWidgetParent, tr( "Bin directory" ),
                    tr( "The Phi binary directory could not be detected, please check "
                        "for updates manually." ), QMessageBox::Ok );
                    _updateWidgetParent=0;
                    return;
                }
                s->setValue( "BinDir", binDir );
            } else {
                qDebug( "Silent update: could not detect binary directory, aborted" );
                return;
            }
        }
        QString exe=binDir+QDir::separator()+"artephis.exe";
        QString bin=binDir+QDir::separator()+"artephis";
        if ( QFile::exists( exe ) || QFile::exists( bin ) ) product="phisketeer";
    }
#endif
*/
    _lastUpdateCheck=QDateTime::currentDateTime();
    PHIUpdateInfo info( QString::fromUtf8( _updateReply->readAll() ) );
    if ( PHI::isUpToDate( PHIA::libVersion(), info.version( product ) ) ) {
        qDebug( "Phi version of %s is up to date", qPrintable( product ) );
        if ( _updateWidgetParent ) {
            QMessageBox::information( _updateWidgetParent, tr( "Product information" ),
                tr( "Your installed Phi version is up to date."), QMessageBox::Ok );
            _updateWidgetParent=0;
        }
        return;
    }
    if ( !downloadInstall() ) {
        QMessageBox::Button res=QMessageBox::information( _updateWidgetParent,
            tr( "Product information" ), tr( "A new Phi version (%1) is available." )
            .arg( info.version( product ) )+'\n'
            +tr( "Release date: %1" ).arg( info.releaseDate( product ).toString() )+'\n'
            +info.description( product, "en" )+"\n\n"
            +tr( "Do you want to download it?" ),
            QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
        if ( res==QMessageBox::Abort ) {
            _updateWidgetParent=0;
            return;
        }
    }
    _updateInProgress=true;
    QNetworkRequest req;
    req.setUrl( info.downloadUrl( product ) );
    _updateReply=PHIANetManager::instance()->networkAccessManager()->get( req );
    if ( _updateWidgetParent ) { // visible download
        // warning: _updateReply becomes controlled by PHIADownloadEntry
        // do not delete it!!!
        PHIADownloadEntry *e=addDownload( _updateReply, _updateWidgetParent, false );
        connect( e, SIGNAL( finished( const QString& ) ), this,
            SLOT( slotUpdateDownloadFinished( const QString& ) ) );
        connect( e, SIGNAL( aborted() ), this, SLOT( slotUpdateAborted() ) );
        _updateReply=0;
        return;
    }
    // download silently
    if ( _downloadDir.isEmpty() ) {
        _downloadDir=QStandardPaths::writableLocation( QStandardPaths::DownloadLocation );
        QDir mkdir( _downloadDir );
        if ( !mkdir.exists() ) mkdir.mkpath( _downloadDir );
    }
    _updateFile=new QFile( this );
    QFileInfo finfo( _updateReply->url().path() );
    QString location=_downloadDir+QDir::separator()+finfo.fileName();
    int i=2;
    while ( 1 ) {
        _updateFile->setFileName( location );
        if ( _updateFile->exists() ) {
            location=_downloadDir+QDir::separator()+finfo.completeBaseName()
                +QString( "_%1." ).arg( i++ )+finfo.suffix();
            continue;
        }
        break;
    }
    if ( !_updateFile->open( QIODevice::WriteOnly ) ) {
        qDebug( "Could not open file %s for writing", qPrintable( _updateFile->fileName() ) );
        _updateReply->deleteLater();
        _updateReply=0;
        _updateInProgress=false;
        _updateWidgetParent=0;
        return;
    }
    connect( _updateReply, SIGNAL( readyRead() ), this, SLOT( slotUpdateNewBytesAvailable() ) );
    connect( _updateReply, SIGNAL( finished() ), this, SLOT( slotUpdateReplyFinished() ) );
    qDebug( "Silent download of update %s started", qPrintable( _updateFile->fileName() ) );
    slotUpdateNewBytesAvailable();
    if ( _updateReply->isFinished() ) slotUpdateReplyFinished();
}

void PHIAConfig::slotUpdateNewBytesAvailable()
{
    _updateFile->write( _updateReply->readAll() );
}

void PHIAConfig::slotUpdateReplyFinished()
{
    qDebug( "slotUpdateReplyFinished()" );
    _updateFile->close();;
    _updateFile->deleteLater();
    _updateReply->deleteLater();
    if ( _updateReply->error()!=QNetworkReply::NoError ) {
        if ( _updateWidgetParent ) {
            QString tmp=tr( "The download of the Phi update software (%1) was aborted." )
                .arg( QFileInfo( _updateFile->fileName() ).fileName() );
            if ( _updateReply->error()!=QNetworkReply::NoError )
                tmp+='\n'+tr( "Reply: %1" ).arg( _updateReply->errorString() );
            QMessageBox::warning( _updateWidgetParent, tr( "Update aborted" ),
                tmp, QMessageBox::Ok );
            _updateWidgetParent=0;
        }
    } else slotUpdateDownloadFinished( _updateFile->fileName() );
    _updateFile=0;
    _updateReply=0;
    _updateInProgress=false;
}

void PHIAConfig::slotUpdateAborted()
{
    // this slot is called by PHIADownloadEntry
    qDebug( "slotUpdateAborted()" );
    _updateInProgress=false;
    if ( _updateWidgetParent ) {
        QString tmp=tr( "The download of the Phi update software was aborted." );
        QMessageBox::warning( _updateWidgetParent, tr( "Update aborted" ),
            tmp, QMessageBox::Ok );
        _updateWidgetParent=0;
    }
    _updateFile=0;
    _updateReply=0;
}

void PHIAConfig::slotUpdateDownloadFinished( const QString &filename )
{
    qDebug( "slotUpdateDownloadFinished()" );
    // _updateReply is deleted by PHIADownloadEntry!
    _updateInProgress=false;
    if ( !downloadInstall() ) {
        QMessageBox::Button res=QMessageBox::information( _updateWidgetParent,
            tr( "Download completed" ),
            tr( "The download of the Phi software update package '%1' is completed. "
            "Please close all browser and application windows which are displaying "
            "Phi content immediately after clicking 'Yes'. "
            "Do you want to start the setup now?" ).arg( filename ),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes  );
        if ( res==QMessageBox::Yes ) QProcess::startDetached( filename );
        _updateWidgetParent=0;
        return;
    }
    if ( !downloadInstall() ) return;
    QProcess::startDetached( filename );
    //QDesktopServices::openUrl( "file://"+filename );
}

void PHIAConfig::load( const QString &file )
{
    QFile f( file );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QDataStream ds( &f );
        ds.setVersion( PHI_DSV );
        ds >> this;
        f.close();
        _fileName=file;
    }
}

void PHIAConfig::save( const QString &fileName, QWidget *parent )
{
    QString file=fileName;
    if ( file.isEmpty() ) {
        file=QFileDialog::getSaveFileName( parent, tr( "Save as" ), QDir::homePath()
            +QDir::separator()+"user.phicfg", tr( "Phi user config")+" (*.phicfg)" );
        if ( file.isEmpty() ) {
            QMessageBox::warning( parent, tr( "Save user config" ),
                tr( "User config is not stored!" ), QMessageBox::Ok );
            return;
        }
    }
    QFile f( file );
    if ( f.open( QIODevice::WriteOnly ) ) {
        QDataStream ds( &f );
        ds.setVersion( PHI_DSV );
        ds << this;
        f.close();
        _fileName=file;
        QSettings s;
        s.setValue( PHIA::configName( PHIA::ConfigFile ), _fileName );
    } else {
        QMessageBox::warning( parent, tr( "Save error" ),
            tr( "Could not save config to '%1'. Please check the file permissions." ).arg( file ),
            QMessageBox::Ok );
    }
}

void PHIAConfig::showDownloadDialog( QWidget *parent )
{
    if ( !_initDownloadDialog ) return PHIADownloadDlg::instance()->show();
    PHIADownloadDlg *dlg=PHIADownloadDlg::instance( parent );
    connect( dlg, SIGNAL( clearClicked() ), this, SLOT( slotDownloadsClearClicked() ) );
    for ( int i=0; i<_downloads.count(); i++ ) {
        dlg->addDownload( _downloads.at( i ) );
    }
    _initDownloadDialog=false;
    dlg->show();
}

bool PHIAConfig::showPasswdDialog( QWidget *parent )
{
    if ( !usePasswd() || passwdOpen() ) return true;
    while( 1 ) {
        QString tmp=QInputDialog::getText( parent, tr( "Password" ),
            tr( "Your user profile is password protected." ), QLineEdit::Password );
        if ( tmp.isEmpty() ) return false;
        if ( !checkPasswd( tmp ) ) {
            QMessageBox::Button res=QMessageBox::warning( parent, tr( "Wrong password" ),
                tr( "The password you entered is invalid." ), QMessageBox::Abort
                | QMessageBox::Retry | QMessageBox::Reset, QMessageBox::Retry );
            if ( res==QMessageBox::Abort ) {
                return false;
            } else if ( res==QMessageBox::Reset ) {
                clear();
                return true;
            }
        } else {
            if ( _pwTimeout > 0 ) _timer.start( _pwTimeout*1000 );
            else _timer.stop();
            return true;
        }
    }
}

bool PHIAConfig::showProxyDialog( QWidget *parent, const QNetworkProxy &proxy, QAuthenticator *auth )
{
    if ( useProxy() && _validProxy ) {
        auth->setPassword( _proxyPasswd );
        auth->setUser( _proxyUser );
        _validProxy=false;
        return true;
    }
    PHIAProxyDlg dlg( parent, proxy, auth );
    if ( dlg.exec()==QDialog::Accepted ) return true;
    return false;
}

bool PHIAConfig::showAuthDialog( QWidget *parent, QNetworkReply *reply, QAuthenticator *auth )
{
    PHIAAuthDlg dlg( parent, reply, auth );
    if ( dlg.exec()==QDialog::Rejected ) {
        reply->abort();
        return false;
    }
    return true;
}

bool PHIAConfig::showSslErrorDialog( QWidget *parent, QNetworkReply *reply,
    const QList<QSslError> &errors )
{
    QString tmp=tr( "Reply:" )+"\n'";
    QSslError err;
    foreach( err, errors ) tmp+=err.errorString()+'\n';
    tmp.chop( 1 ); // remove last newline
    tmp+="'\n"+tr( "Do you want to connect anyway?" );
    QMessageBox::Button res=QMessageBox::warning( parent, tr( "SSL error" ), tmp,
        QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort );
    if ( res==QMessageBox::Abort ) return false;
    reply->ignoreSslErrors( errors );
    return true;
}

bool PHIAConfig::checkPasswd( const QString &pwd )
{
    if ( !usePasswd() ) return true;
    if ( _passwdOpen ) return true;
    QByteArray md5=QCryptographicHash::hash( pwd.toUtf8(), QCryptographicHash::Md5 );
    if ( md5==_passwd ) {
        _passwdOpen=true;
        return true;
    }
    return false;
}

void PHIAConfig::setPasswd( const QString &pwd )
{
    _passwd=QCryptographicHash::hash( pwd.toUtf8(), QCryptographicHash::Md5 );
    _options |= OUsePasswd;
    _passwdOpen=false;
}

void PHIAConfig::slotPasswordTimeout()
{
    if ( usePasswd() ) _passwdOpen=false;
}

void PHIAConfig::showUsedDialog( QWidget *parent, PHIAConfig::UsedId id, const QString &title, const QString &text )
{
    if ( _usedIds & id ) return; // user does not want to see this dialog

    QDialog dlg( parent );
    dlg.setWindowTitle( title );
    QVBoxLayout *vbox=new QVBoxLayout();
    QLabel *l=new QLabel( text );
    l->setWordWrap( true );
    vbox->addWidget( l );
    QCheckBox *check=new QCheckBox( QObject::tr( "Do not show this message again" ) );
    vbox->addWidget( check );
    QDialogButtonBox *but=new QDialogButtonBox( QDialogButtonBox::Ok );
    connect( but, SIGNAL( accepted() ), &dlg, SLOT( accept() ) );
    vbox->addWidget( but );
    dlg.setLayout( vbox );
    dlg.exec();
    if ( check->isChecked() ) {
        _usedIds |= id;
        save( _fileName, parent );
    }
}

QDataStream& operator<< ( QDataStream &out, const PHIAConfig *c )
{
    out << c->_fileVersion << c->_addressList << c->_contactList << c->_creditCardList
        << c->_startDate << c->_endDate << static_cast<qint32>(c->_options)
        << c->_passwd << c->_languages << c->_server << c->_home
        << c->_proxyPasswd << c->_proxyUser << static_cast<qint32>(c->_usedIds)
        << c->_pwTimeout << static_cast<quint8>(c->_interval);
    return out;
}

QDataStream& operator>> ( QDataStream &in, PHIAConfig *c )
{
    qint32 opts, ids;
    quint8 interval;
    c->_downloads.clear();
    in >> c->_fileVersion >> c->_addressList >> c->_contactList >> c->_creditCardList
       >> c->_startDate >> c->_endDate >> opts >> c->_passwd >> c->_languages
       >> c->_server >> c->_home  >> c->_proxyPasswd >> c->_proxyUser >> ids
       >> c->_pwTimeout >> interval;
    c->_options=static_cast<PHIAConfig::Options>(opts);
    c->_usedIds=static_cast<PHIAConfig::UsedIds>(ids);
    c->_interval=static_cast<PHIAConfig::Interval>(interval);
    return in;
}
