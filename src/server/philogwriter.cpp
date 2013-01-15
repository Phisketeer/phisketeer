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
#include <QCoreApplication>
#include <QMutexLocker>
#include <QSettings>
#include <QDir>
#include <QDesktopServices>
#include "philogwriter.h"
#include "phierror.h"
#include "phimanager.h"
#include "phiconfig.h"

PHILogWriterThread* PHILogWriterThread::_instance=0;
PHILogWriter* PHILogWriter::_instance=0;

PHILogWriterThread::PHILogWriterThread( QObject *parent )
    : QThread( parent )
{
    qDebug( "PHILogWriterThread::PHILogWriterThread()" );
    setObjectName( "LogWriterThread" );
    _sem.release( 1 );
}

PHILogWriterThread* PHILogWriterThread::instance()
{
    if ( !_instance ) _instance=new PHILogWriterThread( 0 );
    return _instance;
}

PHILogWriterThread::~PHILogWriterThread()
{
    quit();
    wait();
    if ( PHILogWriter::instance() ) delete PHILogWriter::instance();
    _instance=0;
    qDebug( "PHILogWriterThread::~PHILogWriterThread()" );
}

PHIRC PHILogWriterThread::init( QObject *mgr )
{
    if ( !mgr ) return PHIRC_MGR_NAME_ERROR;
    setParent( mgr );
    _mgrName=mgr->objectName();
    if ( _mgrName.isEmpty() ) return PHIRC_MGR_NAME_ERROR;
    _sem.acquire( 1 );
    start( QThread::LowPriority );
    _sem.acquire( 1 ); // wait for thread to init
    return PHIRC_OK;
}

void PHILogWriterThread::run()
{
    qDebug( "PHILogWriterThread::run(): thread %p", QThread::currentThread() );
    PHILogWriter::init( _mgrName );
    _sem.release( 1 );
    exec();
}

PHILogWriter::PHILogWriter( QObject *parent, const QString &mgrName )
    : QObject( parent )
{
    qDebug( "PHILogWriter::PHILogWriter()" );
    setObjectName( "LogWriter" );
    Q_UNUSED( mgrName );
    _filter=All;
    _filter&= ~(static_cast<PHILogWriter::Filter>(PHIConfig::instance()->logFilter()));
    _logFile=PHIConfig::instance()->logDir()+QDir::separator()+"error.log";
    _file.setFileName( _logFile );
    qDebug( "Using log file '%s'", qPrintable( _logFile ) );
    if ( !_file.open( QFile::WriteOnly | QFile::Append ) ){
        PHIError::instance()->print( PHIRC_LOG_INIT_ERROR,
            tr( "Check if '%1' is writable." ).arg( _logFile )
            +PHI::nl()+tr( "Disabling error log." ) );
        _file.open( stderr, QIODevice::WriteOnly );
    }
    _out.setDevice( &_file );
}

PHILogWriter::~PHILogWriter()
{
    _mutex.lock();
    _file.close();
    _instance=0;
    _mutex.unlock();
    qDebug( "PHILogWriter::~PHILogWriter()" );
}

PHILogWriter* PHILogWriter::instance()
{
    if ( !_instance ) {
        PHIError::instance()->print( PHIRC_MGR_INTERNAL_ERROR,
            tr( "Logwriter not yet initialized." ) );
        return 0;
    }
    return _instance;
}

PHILogWriter* PHILogWriter::init( const QString &fn )
{
    if ( !_instance ) _instance=new PHILogWriter( 0, fn );
    return _instance;
}

void PHILogWriter::setFilter( int filter )
{
    _mutex.lock();
    //_settings->setValue( "Logfilter", filter );
    //_settings->sync();
    _filter=All;
    _filter&=~filter;
    _mutex.unlock();
}

void PHILogWriter::log( int type, const char* file, int line,
    const QDateTime &dt, PHIRC rc, const QString &todo )
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
    QChar t( 'U' );
    switch( type ){
    case 0x02: t='E'; break;
    case 0x04: t='W'; break;
    case 0x01: t='T'; break;
    case 0x08: t='C'; break;
    case 0x10: t='D'; break;
    }
    QMutexLocker l( &_mutex );
    if ( !(type & _filter) ) return;
    _out << sep << PHI::nl() << dt.toUTC().toString( "yyyy-MM-dd hh:mm:ss.zzz" );
    _out << " UTC, " << dt.toString( "hh:mm:ss" );
    _out << ' ' << tr( "localtime" ) << " (" << t << ')' << PHI::nl();
    _out << "RC=" << rc << " (" << PHIError::instance()->shortDesc( rc );
    _out << ")" << PHI::nl() << tr( PHIError::instance()->longDesc( rc ).toLatin1() );
    _out << PHI::nl() << PHI::nl() << tr( "Details:" ) << PHI::nl() << action;
    _out << PHI::nl() << PHI::nl();
    _out.flush();
}
