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
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "phiwizard.h"
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QRegExp>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QProcess>
#include <QProcessEnvironment>

PHIWizardStart::PHIWizardStart( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardStart::PHIWizardStart()" );
    setPixmap( QWizard::WatermarkPixmap, QPixmap( ":/misc/watermark" ) );
    setupUi( this );
    _text->setText( tr( "This will install Phisketeer V%1 on your computer." ).arg( PHIVERSION )
        +"\n\n"+tr( "Click Next to continue, or Cancel to exit Setup." ) );
    //setTitle( "Welcome");
    //setSubTitle( "subtitle");
}

PHIWizardStart::~PHIWizardStart()
{
    qDebug( "PHIWizardStart::~PHIWizardStart()" );
}

void PHIWizardStart::initializePage()
{
}

PHIWizardLicense::PHIWizardLicense( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardLicense::PHIWizardLicense()" );
    setupUi( this );
    QString text;
    QFile f( QDir::currentPath()+"/license.txt" );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QTextStream t( &f );
        text=t.readAll();
    } else text=QDir::currentPath();
    text.replace( "\r\n", "\n" );

    _license->setText( text );
    _license->setReadOnly( true );
    registerField( "license*", _accept );
}

PHIWizardLicense::~PHIWizardLicense()
{
    qDebug( "PHIWizardLicense::~PHIWizardLicense()" );
}

void PHIWizardLicense::initializePage()
{
}

PHIWizardRoot::PHIWizardRoot( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardRoot::PHIWizardRoot()" );
    setupUi( this );
    QSettings s( QSettings::SystemScope, PHIORG, "phis" );
    s.setFallbacksEnabled( false );
    QString binDir=s.value( "BinDir", QString( "/opt/phisketeer-%1/bin" ).arg( PHIVERSION ) ).toString();
    binDir.replace( QRegExp( ".bin$" ), "" );
    if ( binDir.contains( '-' ) ) {
        binDir.replace( QRegExp( "-[^-]*$" ), "" );
        binDir.append( QString( '-' )+PHIVERSION );
    }
    _rootDir->setText( binDir );
    _rootDir->setReadOnly( true );
    registerField( "rootdir", _rootDir );
}

PHIWizardRoot::~PHIWizardRoot()
{
    qDebug( "PHIWizardRoot::~PHIWizardRoot()" );
}

void PHIWizardRoot::on__browse_clicked()
{
    QString dir=QFileDialog::getExistingDirectory( this, tr( "Please select the directory where"
        " to install Phisketeer." ), _rootDir->text() );
    if ( dir.isEmpty() ) return;
    _rootDir->setText( dir );
}

bool PHIWizardRoot::validatePage()
{
    if ( QDir::current().exists( _rootDir->text() ) ) {
        QMessageBox::Button res=QMessageBox::warning( this, tr( "Warning" ),
            tr( "The directory '%1' already exists. Do you want to overwrite it?" )
            .arg( _rootDir->text() ), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
        if ( res==QMessageBox::No ) return false;
    }
    return true;
}

PHIWizardComponents::PHIWizardComponents( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardComponents::PHIWizardComponents()" );
    setupUi( this );
    registerField( "libs", _libsCheck );
    registerField( "phis", _phisCheck );
    registerField( "apache", _apacheCheck );
    registerField( "artephis", _artephisCheck );
    registerField( "amphibia", _amphibiaCheck );
}

PHIWizardComponents::~PHIWizardComponents()
{
    qDebug( "PHIWizardComponents::~PHIWizardComponents()" );
}

int PHIWizardComponents::nextId() const
{
    if ( _phisCheck->isChecked() ) return static_cast<int>(PHIWizard::Data);
    if ( _apacheCheck->isChecked() ) return static_cast<int>(PHIWizard::Apache);
    return static_cast<int>(PHIWizard::Commit);
}

PHIWizardData::PHIWizardData( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardData::PHIWizardData()" );
    setupUi( this );
    QSettings s( QSettings::SystemScope, PHIORG, "phis" );
    s.setFallbacksEnabled( false );
    QString binDir=s.value( "RootDir", QString( "/var/phis" ) ).toString();
    _dataDir->setText( binDir );
    _dataDir->setReadOnly( true );
    registerField( "datadir", _dataDir );
}

PHIWizardData::~PHIWizardData()
{
    qDebug( "PHIWizardData::~PHIWizardData()" );
}

int PHIWizardData::nextId() const
{
    //if ( field( "apache" ).toBool() ) return static_cast<int>(PHIWizard::Apache);
    return static_cast<int>(PHIWizard::Phis);
}

void PHIWizardData::on__browse_clicked()
{
    QString dir=QFileDialog::getExistingDirectory( this, tr( "Please select the directory where"
        " to install the Phis data files." ), _dataDir->text() );
    if ( dir.isEmpty() ) return;
    _dataDir->setText( dir );
}

bool PHIWizardData::validatePage()
{
    if ( QDir::current().exists( _dataDir->text() ) ) {
        QMessageBox::Button res=QMessageBox::warning( this, tr( "Warning" ),
            tr( "The directory '%1' already exists. "
                "Do you really want to use it as the Phis data directory?" )
            .arg( _dataDir->text() ), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
        if ( res==QMessageBox::No ) return false;
    }
    return true;
}

PHIWizardPhis::PHIWizardPhis( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardPhis::PHIWizardPhis()" );
    setupUi( this );
    QSettings s( QSettings::SystemScope, PHIORG, "phis" );
    s.setFallbacksEnabled( false );
    s.beginGroup( "default" );

    registerField( "port", _port );
    registerField( "domain", _domain );
    registerField( "email", _email );

    _port->setValue( s.value( "ListenerPort", 8080 ).toInt() );
    QString domain="localhost";
    if ( s.childGroups().count() ) domain=s.childGroups().first();
    _domain->setText( domain );
    _email->setText( s.value( "Admin", QString( "admin@localhost" ) ).toString() );
}

PHIWizardPhis::~PHIWizardPhis()
{
    qDebug( "PHIWizardPhis::~PHIWizardPhis()" );
}

int PHIWizardPhis::nextId() const
{
    if ( field( "apache" ).toBool() ) return static_cast<int>(PHIWizard::Apache);
    return static_cast<int>(PHIWizard::Commit);
}

PHIWizardApache::PHIWizardApache( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardApache::PHIWizardApache()" );
    setupUi( this );
    if ( QFile::exists( "/var/lib/apache2" ) ) _dataDir->setText( "/var/lib/apache2" );
    else if ( QFile::exists( "/usr/lib/apache2" ) ) _dataDir->setText( "/usr/lib/apache2" );
    else if ( QFile::exists( "/usr/lib64/apache2" ) ) _dataDir->setText( "/usr/lib64/apache2" );
    else _dataDir->setText( "/etc/apache2" );
    _dataDir->setReadOnly( true );
    registerField( "apachedir", _dataDir );
}

PHIWizardApache::~PHIWizardApache()
{
    qDebug( "PHIWizardApache::~PHIWizardApache()" );
}

int PHIWizardApache::nextId() const
{
    return static_cast<int>(PHIWizard::Commit);
}

void PHIWizardApache::on__browse_clicked()
{
    QString dir=QFileDialog::getExistingDirectory( this, tr( "Please select the Apache"
        " root directory." ), _dataDir->text() );
    if ( dir.isEmpty() ) return;
    _dataDir->setText( dir );
}

PHIWizardCommit::PHIWizardCommit( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardCommit::PHIWizardCommit()" );
    setupUi( this );
    setCommitPage( true );
    setButtonText( QWizard::CommitButton, tr( "Install" ) );
}

PHIWizardCommit::~PHIWizardCommit()
{
    qDebug( "PHIWizardCommit::~PHIWizardCommit()" );
}

void PHIWizardCommit::initializePage()
{
    QString comp="Program libraries<br />";
    if ( field( "phis" ).toBool() ) comp+="Phis server<br />";
    if ( field( "apache" ).toBool() ) comp+="Apache module<br />";
    if ( field( "amphibia" ).toBool() ) comp+="Amphibia browser<br />";
    if ( field( "artephis" ).toBool() ) comp+="Artephis editor<br />";
    QString steps=tr( "<b>Destination location:</b><br />%1<br /><br /><b>Components:</b><br />%2" )
        .arg( field( "rootdir" ).toString() ).arg( comp );
    _steps->setHtml( steps );
}

PHIWizardInstall::PHIWizardInstall( QWidget *parent )
    : QWizardPage( parent )
{
    qDebug( "PHIWizardInstall::PHIWizardInstall()" );
    setPixmap( QWizard::WatermarkPixmap, QPixmap( ":/misc/watermark" ) );
    setupUi( this );
    setFinalPage( true );
}

PHIWizardInstall::~PHIWizardInstall()
{
    qDebug( "PHIWizardInstall::~PHIWizardInstall()" );
}

void PHIWizardInstall::initializePage()
{
    _progress->setValue( 0 );
    wizard()->button( QWizard::CancelButton )->setEnabled( false );
    _box->setEnabled( false );
    _label->setText( tr( "Creating directories" ) );
    _phis->setEnabled( field( "phis" ).toBool() );
    _daemon->setEnabled( field( "phis" ).toBool() && QFile::exists( "/etc/init.d" ) );
    _artephis->hide();
    QTimer::singleShot( 0, this, SLOT( init() ) );
}

void PHIWizardInstall::init()
{
    wizard()->button( QWizard::FinishButton )->setEnabled( false );
    install();
}

void PHIWizardInstall::finish( bool err )
{
    if ( err ) wizard()->button( QWizard::CancelButton )->setEnabled( true );
    else {
        wizard()->button( QWizard::FinishButton )->setEnabled( true );
        _box->setEnabled( true );
    }
}

bool PHIWizardInstall::validatePage()
{
    if ( _readme->isChecked() ) {
        QDialog dlg( this );
        dlg.setWindowTitle( tr( "README" ) );
        dlg.setSizeGripEnabled( true );
        dlg.resize( 400, 300 );
        QFile f( QDir::currentPath()+"/README.txt" );
        if ( f.open( QIODevice::ReadOnly ) ) {
            QTextStream t( &f );
            QString readme=t.readAll();
            readme.replace( "\r\n", "\n" );
            f.close();
            QPlainTextEdit *edit=new QPlainTextEdit( &dlg );
            edit->setPlainText( readme );
            edit->setReadOnly( true );
            QPushButton *close=new QPushButton( tr( "Close" ) );
            connect( close, SIGNAL( clicked() ), &dlg, SLOT( accept() ) );
            QVBoxLayout *vbox=new QVBoxLayout();
            vbox->addWidget( edit );
            QHBoxLayout *hbox=new QHBoxLayout();
            hbox->addStretch();
            hbox->addWidget( close );
            vbox->addLayout( hbox );
            dlg.setLayout( vbox );
            dlg.exec();
        }
    }
    QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
    QString libpath=env.value( "LD_LIBRARY_PATH" );
    libpath=field( "rootdir" ).toString()+"/lib:"+libpath;
    env.insert( "LD_LIBRARY_PATH", libpath );
    env.insert( "LD_LIBRARY_PATH_64", libpath );
    if ( _phis->isChecked() ) {
        QProcess p_phis;
        p_phis.setProcessEnvironment( env );
        p_phis.start( field( "rootdir" ).toString()+"/bin/phis -t" );
        p_phis.waitForFinished( 10000 );
        p_phis.startDetached( field( "rootdir" ).toString()+"/bin/phis" );
    }
    if ( _artephis->isChecked() ) {
        QProcess p_artephis;
        p_artephis.setProcessEnvironment( env );
        p_artephis.startDetached( field( "rootdir" ).toString()+"/bin/artephis -style cleanlooks" );
    }
    if ( _daemon->isChecked() ) {
        QFile f( "/etc/init.d/phis" );
        if ( !f.exists() ) {
            QMessageBox::warning( this, tr( "Daemon" ), tr( "Could not find the daemon"
               " script '%1'. Phis daemon will not be installed." )
               .arg( f.fileName() ), QMessageBox::Ok );
        } else {
            f.link( "/etc/rc0.d/K10phis" );
            f.link( "/etc/rc1.d/K10phis" );
            f.link( "/etc/rc2.d/S90phis" );
            f.link( "/etc/rc3.d/S90phis" );
            f.link( "/etc/rc4.d/S90phis" );
            f.link( "/etc/rc5.d/S90phis" );
            f.link( "/etc/rc6.d/K10phis" );
        }
    }
    if ( field( "apache" ).toBool() ) {
        QString adir=field( "apachedir" ).toString();
        if ( !QFile::exists( adir ) ) {
            QMessageBox::warning( this, tr( "Apache" ), tr( "The provided Apache dir '%1' "
                "is not found. The Phi module for apache could not be linked. "
                "You need to setup the module manually." ).arg( adir ), QMessageBox::Ok  );
        } else {
            QFile f( field( "rootdir" ).toString()+"/lib/libmod_phi.so.1" );
            if ( f.exists() ) {
                if ( QFile::exists( adir+"/modules" ) ) adir=adir+"/modules";
                QFile modphi( adir+"/mod_phi.so" );
                if ( modphi.exists() ) modphi.remove();
                f.link( adir+"/mod_phi.so" );
            } else {
                QMessageBox::warning( this, tr( "Apache" ), tr( "Could not find '%1' "
                    "You need to setup the Phi module manually." )
                    .arg( f.fileName() ), QMessageBox::Ok );
            }
        }
    }
    return true;
}

PHIWizard::PHIWizard( QWidget *parent )
    : QWizard( parent )
{
    qDebug( "PHIWizard::PHIWizard()" );
    setWizardStyle( QWizard::ModernStyle );
    setWindowTitle( tr( "Setup - Phisketeer" ) );
    setWindowIcon( QIcon( ":/misc/philogo" ) );
    //setPixmap( QWizard::WatermarkPixmap, QPixmap( ":/misc/watermark" ) );
    setPixmap( QWizard::LogoPixmap, QPixmap( ":/misc/philogo" ) );
    setPixmap( QWizard::BannerPixmap, QPixmap() );
    setPage( static_cast<int>(Start), new PHIWizardStart );
    setPage( static_cast<int>(License), new PHIWizardLicense );
    setPage( static_cast<int>(Root), new PHIWizardRoot );
    setPage( static_cast<int>(Components), new PHIWizardComponents );
    setPage( static_cast<int>(Data), new PHIWizardData );
    setPage( static_cast<int>(Phis), new PHIWizardPhis );
    setPage( static_cast<int>(Apache), new PHIWizardApache );
    setPage( static_cast<int>(Commit), new PHIWizardCommit );
    setPage( static_cast<int>(Install), new PHIWizardInstall );
}

PHIWizard::~PHIWizard()
{
    qDebug( "PHIWizard::~PHIWizard()" );
}

void PHIWizard::reject()
{
    if ( !button( CancelButton )->isEnabled() ) return;
    //if ( currentId()==static_cast<int>(Install) ) return;
    QMessageBox::Button res=QMessageBox::warning( this, tr( "Abort setup" ),
        tr( "Are you sure you want to abort the setup process?" ),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
    if ( res==QMessageBox::Yes ) return QWizard::reject();
}
