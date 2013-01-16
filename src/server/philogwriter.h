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
#ifndef PHILOGWRITER_H
#define PHILOGWRITER_H
#include <QThread>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFlag>
#include <QSemaphore>
#include "phi.h"

class PHILogWriter : public QObject
{
    Q_OBJECT

friend class PHILogWriterThread;

public:
    enum FilterFlag { None=0x00, Trace=0x01, Error=0x02, Warning=0x04, Critical=0x08,
        Debug=0x10, User=0x20, All=0xFF };
    Q_DECLARE_FLAGS( Filter, FilterFlag )
    static PHILogWriter* instance();

protected:
    explicit PHILogWriter( QObject *parent, const QString& qmgrName );
    virtual ~PHILogWriter();
    static PHILogWriter* init( const QString& );

public slots:
    void log( int type, const char* file, int line,
        const QDateTime &dt, PHIRC rc, const QString &todo );
    void setFilter( int filter );

private:
    static PHILogWriter *_instance;
    QMutex _mutex;
    QFile _file;
    QTextStream _out;
    Filter _filter;
    QString _logFile;

signals:
    void error( int type, PHIRC rc, const QDateTime &dt, const QString &todo );
};

Q_DECLARE_OPERATORS_FOR_FLAGS( PHILogWriter::Filter )

class PHILogWriterThread : public QThread
{
    Q_OBJECT

public:
    static PHILogWriterThread* instance();
    virtual ~PHILogWriterThread();
    PHIRC init( QObject* );

protected:
    PHILogWriterThread( QObject *parent=0 );
    virtual void run();

private:
    static PHILogWriterThread *_instance;
    QString _mgrName;
    PHILogWriter *_lw;
    QSemaphore _sem;
};

#endif // PHILOGWRITER_H
