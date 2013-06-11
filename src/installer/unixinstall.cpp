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
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include "phiwizard.h"

void PHIWizardInstall::install()
{
    QString file;
    const QString root=field( "rootdir" ).toString();
    QDir rd( root );
    if ( !rd.exists() ) {
        if ( !rd.mkpath( root ) ) {
            QMessageBox::warning( this, tr( "Permission denied" ),
                tr( "Could not create root directory '%1'." ).arg( root ), QMessageBox::Ok );
            return finish( true );
        }
    }
    rd.mkpath( root+"/lib" );
    rd.mkpath( root+"/bin" );
    rd.mkpath( root+"/plugins/sqldrivers" );
    rd.mkpath( root+"/plugins/accessible" );
    rd.mkpath( root+"/plugins/imageformats" );
    rd.mkpath( root+"/plugins/modules" );
    rd.mkpath( root+"/plugins/iconengines" );
    rd.mkpath( root+"/plugins/printsupport" );
    rd.mkpath( root+"/plugins/platforms" );
    rd.mkpath( root+"/ts" );
    if ( field( "phis" ).toBool() ) {
        file=field( "datadir" ).toString();
        if ( !rd.mkpath( file+"/default" ) ) {
            QMessageBox::warning( this, tr( "Permission denied" ),
                tr( "Could not create Phis data directory '%1'." ).arg( file ), QMessageBox::Ok );
        } else {
            rd.mkpath( file+"/default/"+field( "domain" ).toString() );
            rd.mkpath( file+"/default/log" );
            rd.mkpath( file+"/default/temp" );
        }
    }
    _progress->setValue( 5 );
    _label->setText( tr( "Copying files" ) );

    QDir lib( QDir::currentPath()+"/lib" );
    QDir bin( QDir::currentPath()+"/bin" );
    QDir sqldrivers( QDir::currentPath()+"/plugins/sqldrivers" );
    QDir accessible( QDir::currentPath()+"/plugins/accessible" );
    QDir imageformats( QDir::currentPath()+"/plugins/imageformats" );
    QDir modules( QDir::currentPath()+"/plugins/modules" );
    QDir iconengines( QDir::currentPath()+"/plugins/iconengines" );
    QDir printsupport( QDir::currentPath()+"/plugins/printsupport" );
    QDir platforms( QDir::currentPath()+"/plugins/platforms" );
    QDir ts( QDir::currentPath()+"/ts" );
    QStringList files=lib.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( lib.absolutePath()+'/'+file, root+"/lib/"+file );
        _progress->setValue( _progress->value()+1 );
    }
    files=bin.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( bin.absolutePath()+'/'+file, root+"/bin/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    QFile::remove( root+"/bin/phiinstaller" );
    files=sqldrivers.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( sqldrivers.absolutePath()+'/'+file, root+"/plugins/sqldrivers/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=accessible.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( accessible.absolutePath()+'/'+file, root+"/plugins/accessible/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=imageformats.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( imageformats.absolutePath()+'/'+file, root+"/plugins/imageformats/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=modules.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( modules.absolutePath()+'/'+file, root+"/plugins/modules/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=printsupport.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( printsupport.absolutePath()+'/'+file, root+"/plugins/printsupport/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=iconengines.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( iconengines.absolutePath()+'/'+file, root+"/plugins/iconengines/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    files=platforms.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( platforms.absolutePath()+'/'+file, root+"/plugins/platforms/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }

    files=ts.entryList( QDir::Files );
    foreach ( file, files ) {
        QFile::copy( ts.absolutePath()+'/'+file, root+"/ts/"+file );
        _progress->setValue( _progress->value()+1 );
        qApp->processEvents();
    }
    QFile readme( QDir::currentPath()+"/README.txt" );
    if ( readme.open( QIODevice::ReadOnly ) ) {
        QTextStream t( &readme );
        QString f=t.readAll();
        f.replace( "\r\n", "\n" );
        QFile dest( root+"/README.txt" );
        if ( dest.open( QIODevice::WriteOnly ) ) {
            dest.write( f.toUtf8() );
            dest.close();
        }
        readme.close();
    }
    QFile license( QDir::currentPath()+"/license.txt" );
    if ( license.open( QIODevice::ReadOnly ) ) {
        QTextStream t( &license );
        QString f=t.readAll();
        f.replace( "\r\n", "\n" );
        QFile dest( root+"/license.txt" );
        if ( dest.open( QIODevice::WriteOnly ) ) {
            dest.write( f.toUtf8() );
            dest.close();
        }
        license.close();
    }
    QFile phis_sh( QDir::currentPath()+"/phis.sh" );
    if ( field( "phis" ).toBool() && phis_sh.open( QIODevice::ReadOnly ) ) {
        QTextStream t( &phis_sh );
        QString f=t.readAll();
        f.replace( "%phibinpath%", root+"/bin" );
        f.replace( "%philibpath%", root+"/lib" );
        f.replace( "%phipluginpath%", root+"/plugins" );
        if ( QFile::exists( "/etc/init.d" ) ) {
            QFile dest( "/etc/init.d/phis" );
            if ( dest.open( QIODevice::WriteOnly ) ) {
                dest.write( f.toUtf8() );
                dest.close();
                QFile::Permissions perm=dest.permissions();
                dest.setPermissions( QFile::ExeUser | QFile::ExeGroup | perm );
            }
        }
        phis_sh.close();
    }
    QFile uninstall( QDir::currentPath()+"/linuxuninstall.sh" );
    if ( uninstall.open( QIODevice::ReadOnly ) ) {
        QTextStream t( &uninstall );
        QString f=t.readAll();
        f.replace( "%phiroot%", root );
        f.replace( "%phiversion%", PHIVERSION );
        QFile dest( root+"/bin/phiuninstall.sh" );
        if ( dest.open( QIODevice::WriteOnly ) ) {
            dest.write( f.toUtf8() );
            dest.close();
            QFile::Permissions perm=dest.permissions();
            dest.setPermissions( QFile::ExeUser | perm );
        }
        uninstall.close();
    }

    _label->setText( tr( "Writing config" ) );
    _progress->setValue( 90 );
    qApp->processEvents();

    QSettings sg( "/etc/phi/phis.conf", QSettings::IniFormat );
    sg.setFallbacksEnabled( false );
    sg.setValue( "BinDir", root+"/bin" );
    sg.setValue( "Phisketeer", "1" );
    sg.setValue( "PluginsPath", root+"/plugins" );

    QSettings sa( "/etc/phi/phiapp.conf", QSettings::IniFormat );
    sa.setFallbacksEnabled( false );
    sa.setValue( "Path", root+"/bin/phiapp" );

    if ( field( "phis" ).toBool() ) {
        QSettings s( "/etc/phi/phis.conf", QSettings::IniFormat );
        s.setFallbacksEnabled( false );
        s.setValue( "RootDir", field( "datadir" ) );
        QString datadir=s.value( "RootDir" ).toString();
        s.beginGroup( "default" );
        s.setValue( "ListenerPort", field( "port" ).toUInt() );
        s.setValue( "Admin", field( "email" ) );
        s.setValue( "BaseDir", QString( datadir+"/default" ) );
        s.setValue( "Index", s.value( "Index", QString( "index.phis,index.html,index.htm" ) ) );
        s.setValue( "KeepAlive", s.value( "KeepAlive", 60000 ).toUInt() );
        s.setValue( "ListenerIF", s.value( "ListenerIF", QString( "Any" ) ) );
        s.setValue( "LogDir", s.value( "LogDir", QString( datadir+"/default/log" ) ) );
        s.setValue( "LogFilter", s.value( "LogFilter", 16 ).toUInt() );
        s.setValue( "MimeTypesFile", s.value( "MimeTypesFile", QString( datadir+"/mimetypes.txt" ) ) );
        s.setValue( "SSLCertificate", s.value( "SSLCertificate", QString( datadir+"/default/cert.crt" ) ) );
        s.setValue( "SSLEnabled", s.value( "SSLEnabled", false ) );
        s.setValue( "SSLListenerIF", s.value( "SSLListenerIF", QString( "Any" ) ) );
        s.setValue( "SSLListenerPort", s.value( "SSLListenerPort", 443 ).toUInt() );
        s.setValue( "SSLPrivateKey", s.value( "SSLPrivateKey", QString( datadir+"/default/ssl.key" ) ) );
        s.beginGroup( field( "domain" ).toString() );
        s.setValue( "DocumentRoot", s.value( "DocumentRoot",
            QString( datadir+"/default/"+field( "domain" ).toString() ) ) );
        s.setValue( "TempDir", s.value( "TempDir",
            QString( datadir+"/default/temp" ) ) );
        s.endGroup();
        s.endGroup();
    }
    _label->setText( tr( "Cleaning up" ) );
    _progress->setValue( 95 );
    qApp->processEvents();
    if ( !field( "phis" ).toBool() ) {
        QFile::remove( root+"/bin/phis" );
        QFile::remove( "/etc/init.d/phis" );
    }
    if ( !field( "artephis" ).toBool() ) QFile::remove( root+"/bin/artephis" );
    if ( !field( "amphibia" ).toBool() ) QFile::remove( root+"/bin/amphibia" );
    if ( field( "phis" ).toBool() || field( "apache" ).toBool() ) {
        QString docroot=field( "datadir" ).toString()+"/default/"+field( "domain" ).toString();
        QFile phitest( QDir::currentPath()+"/phitest.phis" );
        QString phitestold=docroot+"/phitest.phis";
        if ( QFile::exists( phitestold ) ) QFile::remove( phitestold );
        phitest.copy( phitestold );

        QFile phitestpost( QDir::currentPath()+"/phitestpost.phis" );
        QString phitestpostold=docroot+"/phitestpost.phis";
        if ( QFile::exists( phitestpostold ) ) QFile::remove( phitestpostold );
        phitestpost.copy( phitestpostold );
        QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
        QString user=env.value( "SUDO_USER", "root" );
        QProcess proc;
        proc.execute( "/bin/chown "+user+" "+docroot );
    }
    _label->setText( tr( "Installation completed" ) );
    _progress->setValue( 100 );

    finish();
}
