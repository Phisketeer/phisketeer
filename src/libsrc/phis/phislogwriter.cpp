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
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include "phislogwriter.h"
#include "phierror.h"
#include "phiapplication.h"

PHISLogWriterThread* PHISLogWriterThread::_instance=0;
PHISLogWriter* PHISLogWriter::_instance=0;

PHISLogWriterThread::PHISLogWriterThread( QObject *parent )
    : QThread( parent )
{
    qDebug( "PHISLogWriterThread::PHISLogWriterThread()" );
    setObjectName( SL( "LogWriterThread" ) );
    _sem.release( 1 );
}

PHISLogWriterThread* PHISLogWriterThread::instance()
{
    if ( !_instance ) _instance=new PHISLogWriterThread( 0 );
    return _instance;
}

PHISLogWriterThread::~PHISLogWriterThread()
{
    quit();
    wait();
    delete PHISLogWriter::instance();
    _instance=0;
    qDebug( "PHISLogWriterThread::~PHISLogWriterThread()" );
}

void PHISLogWriterThread::init( QObject *parent, const QString &name )
{
    Q_ASSERT( parent );
    setParent( parent );
    _name=name;
    _sem.acquire( 1 );
    start( QThread::LowPriority );
    _sem.acquire( 1 ); // wait for thread to init
}

void PHISLogWriterThread::run()
{
    qDebug( "PHISLogWriterThread::run(): thread %p", QThread::currentThread() );
    PHISLogWriter::init( _name );
    _sem.release( 1 );
    exec();
}

PHISLogWriter::PHISLogWriter( QObject *parent, const QString &name )
    : QObject( parent )
{
    qDebug( "PHISLogWriter::PHISLogWriter()" );
    setObjectName( SL( "LogWriter" ) );
    QSettings *s=phiApp->serverSettings();
    s->beginGroup( name );
    quint32 filterFlags=s->value( SL( "LogFilter" ), 0 ).value<quint32>();
    QString logDir=s->value( SL( "LogDir" ), phiApp->dataPath()+QDir::toNativeSeparators( L1( "/log" ) ) ).toString();
    s->endGroup();
    QDir dir( logDir );
    if ( !dir.exists() ) dir.mkpath( logDir );
    _filter=All;
    _filter&= ~(static_cast<PHISLogWriter::Filter>(filterFlags));
    _logFile=logDir+L1( "/error.log" );
    _file.setFileName( _logFile );
    if ( !_file.open( QFile::WriteOnly | QFile::Append ) ){
        PHIError::instance()->print( PHIRC_LOG_INIT_ERROR,
            tr( "Check if '%1' is writable." ).arg( _logFile )
            +QLatin1Char( ' ' )+tr( "Disabling error log file." ) );
        _file.open( stderr, QIODevice::WriteOnly );
    } else qWarning( "Using log file: '%s'", qPrintable( _logFile ) );
    _out.setDevice( &_file );
}

PHISLogWriter::~PHISLogWriter()
{
    _mutex.lock();
    _file.close();
    _instance=0;
    _mutex.unlock();
    qDebug( "PHISLogWriter::~PHISLogWriter()" );
}

PHISLogWriter* PHISLogWriter::instance()
{
    if ( !_instance ) {
        PHIError::instance()->print( PHIRC_MGR_INTERNAL_ERROR,
            tr( "Logwriter not yet initialized." ) );
        return 0;
    }
    return _instance;
}

PHISLogWriter* PHISLogWriter::init( const QString &name )
{
    if ( !_instance ) _instance=new PHISLogWriter( 0, name );
    return _instance;
}

void PHISLogWriter::setFilter( int filter )
{
    _mutex.lock();
    _filter=All;
    _filter&=~filter;
    _mutex.unlock();
}

void PHISLogWriter::log( int type, const char* file, int line, const QDateTime &dt, PHIRC rc, const QString &todo )
{
    QString action( todo );
    QByteArray sep( 75, '-' );
    QByteArray tmp( QByteArray( "< " )+file );
    tmp.append( " : "+QByteArray::number( line )+" >" );
    sep.replace( 3, tmp.size(), tmp );
    if ( action.isEmpty() ) action=tr( "No interaction required." );
    /* Serialize output: _out could be invoked simultaniously
     * by more than one thread if called directly, however
     * calling by a SLOT queues this request (but we are safe to use a _mutex) */
    QChar t( QLatin1Char( 'U' ) );
    switch( type ){
    case 0x02: t=QLatin1Char( 'E' ); break;
    case 0x04: t=QLatin1Char( 'W' ); break;
    case 0x01: t=QLatin1Char( 'T' ); break;
    case 0x08: t=QLatin1Char( 'C' ); break;
    case 0x10: t=QLatin1Char( 'D' ); break;
    }
    QMutexLocker l( &_mutex );
    if ( !(type & _filter) ) return;
    _out << sep << PHI::nl()
         << dt.toUTC().toString( QStringLiteral( "yyyy-MM-dd hh:mm:ss.zzz" ) )
         << " UTC, " << dt.toString( QStringLiteral( "hh:mm:ss" ) )
         << ' ' << tr( "localtime" ) << " (" << t << ')' << PHI::nl()
         << "RC=" << rc << " (" << PHIError::instance()->shortDesc( rc )
         << ")" << PHI::nl() << tr( PHIError::instance()->longDesc( rc ).toLatin1().constData() )
         << PHI::nl() << PHI::nl() << tr( "Details:" ) << PHI::nl() << action
         << PHI::nl() << PHI::nl();
    _out.flush();
}
