/*
#    Copyright (C) 2010-2013  Marius B. Schumacher
#    Copyright (C) 2011-2014  Phisys AG, Switzerland
#    Copyright (C) 2012-2014  Phisketeer.org team
#
#    This C++ library is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QProcess>
#include <QTextCodec>
#include "phissystem.h"

QString PHISSystemObj::_C=QStringLiteral( "C" );
QString PHISSystemObj::_utf8=QStringLiteral( "utf-8" );
QString PHISFileObj::_utf8=QStringLiteral( "utf-8" );

QString PHISSystemObj::toLocaleFormat( const QString &s )
{
    // translate de-de -> de_DE
    if ( s.size()==2 ) return s;
    if ( s.size()==5 ) {
        QString n=s.left( 3 );
        n.replace( 2, 1, QLatin1Char( '_' ) );
        n.append( s.right( 2 ).toUpper() );
        return n;
    }
    return s.left( 2 );
}

QString PHISSystemObj::exec( const QString &name, const QStringList &args, const QString &codecName )
{
    QProcess proc;
    _lastError=QString();
    proc.start( name, args, QIODevice::ReadOnly );
    if ( !proc.waitForStarted() ) {
        QString err=tr( "Could not start process '%1' with arguments '%2': %3" )
            .arg( name ).arg( args.join( QStringLiteral( " " ) ) ).arg( proc.errorString() );
        PHIS_ERROR(err);
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
        return QString();
    }
    if ( !proc.waitForFinished() ) {
        QString err=tr( "Could not finish process '%1' with arguments '%2': %3" )
            .arg( name ).arg( args.join( QStringLiteral( " " ) ) ).arg( proc.errorString() );
        PHIS_ERROR(err);
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
        return QString();
    }
    if ( proc.exitCode() ) {
        QString err=tr( "Process '%1' exit code is '%2': %3" )
            .arg( name ).arg( proc.exitCode() )
            .arg( QString::fromUtf8( proc.readAllStandardError() ) );
        PHIS_WARN(err);
        _lastError=proc.errorString();
        _exitCode=proc.exitCode();
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    return codec->toUnicode( proc.readAllStandardOutput() );
}

PHISFileObj::~PHISFileObj()
{
    QFile *f;
    foreach( f, _files ) delete f;
    // _files.clear();
    qDebug( "PHISFileObj::~PHISFileObj()" );
}

int PHISFileObj::open( const QString &fn, int mode ) {
    QFile *f=new QFile( fn );
    f->setPermissions( QFile::ReadUser | QFile::WriteUser | QFile::ReadOwner
        | QFile::WriteOwner | QFile::ReadGroup );
    if ( !f->open( static_cast<QIODevice::OpenMode>(mode) ) ) {
        _error=f->errorString();
        PHIS_ERROR(_error);
        delete f;
        return 0;
    }
    _files.insert( ++_handleCounter, f );
    return _handleCounter;
}

QString PHISFileObj::read( int handle, const QString &codecName ) const
{
    if ( !_files.contains( handle ) ) {
        _error=tr( "File handle '%1' not found." ).arg( handle );
        PHIS_WARN(_error);
        return QString();
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    try {
        QByteArray data=_files.value( handle )->readAll();
        QString tmp=codec->toUnicode( data );
        return tmp;
    } catch ( std::bad_alloc& ) {
        _error=tr( "Could not read data for handle '%1' - out of memory." ).arg( handle );
        PHIS_CRIT(_error);
    }
    return QString();
}

bool PHISFileObj::write( int handle, const QString &data, const QString &codecName )
{
    if ( !_files.contains( handle ) ) {
        _error=tr( "File handle '%1' not found." ).arg( handle );
        PHIS_WARN(_error);
        return false;
    }
    QTextCodec *codec=QTextCodec::codecForName( codecName.toUtf8() );
    if ( !codec ) codec=QTextCodec::codecForLocale();
    try {
        QByteArray arr=codec->fromUnicode( data );
        if ( _files.value( handle )->write( arr )==-1 ) {
            _error=tr( "Could not write to '%1': %2" ).arg( _files.value( handle )->fileName() )
                .arg( _files.value( handle )->errorString() );
            PHIS_ERROR(_error )
            return false;
        }
    } catch ( std::bad_alloc& ) {
        _error=tr( "Could not write data for handle '%1' - out of memory." ).arg( handle );
        PHIS_CRIT(_error);
        return false;
    }
    return true;
}
